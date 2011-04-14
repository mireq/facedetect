/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  09.04.2011 20:09:14
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <QList>
#include <QMutexLocker>
#include <QPair>
#include <limits>
#include "BPNeuralNet.h"
#include "NeuralNet.h"
#include "TrainingDataReader.h"
#include "NetTrainer.h"

#include <QDebug>

using std::numeric_limits;
using std::ostringstream;
using std::istringstream;
using std::string;

namespace FaceDetect {

/**
 * Vytvorenie novej inštancie trénera siete.
 */
NetTrainer::NetTrainer(QObject *parent):
	QThread(parent),
	m_reader(0),
	m_stop(false),
	m_numEpoch(100),
	m_net(0),
	m_trainingSetSize(0)
{
}

NetTrainer::~NetTrainer()
{
}

int NetTrainer::numEpoch() const
{
	return m_numEpoch;
}

void NetTrainer::setNumEpoch(int numEpoch)
{
	m_numEpoch = numEpoch;
}

std::size_t NetTrainer::trainingSetSize() const
{
	return m_trainingSetSize;
}

void NetTrainer::setTrainingSetSize(std::size_t size)
{
	m_trainingSetSize = size;
}

/**
 * Vráti objekt čítajúci tréningové dáta siete.
 */
TrainingDataReader *NetTrainer::trainingDataReader() const
{
	return m_reader;
}

/**
 * Nastavenie objektu čítajúceho tréningové dáta siete.
 */
void NetTrainer::setTrainingDataReader(TrainingDataReader *reader)
{
	m_reader = reader;
}

/**
 * V prípade, že práve beží tréning volanie tejto metódy zastaví tréning.
 */
void NetTrainer::stop()
{
	{
		QMutexLocker lock(&m_stopMutex);
		m_stop = true;
	}
	wait();
}

/**
 * Natrénovanie siete \a net.
 */
void NetTrainer::trainNet(FaceDetect::NeuralNet *net)
{
	stop();
	{
		QMutexLocker lock(&m_stopMutex);
		m_stop = false;
	}
	m_net = net;
	start();
}

/**
 * Spustenie tréningu. Pri tréningu sa dáta načítavajú pomocou objektu typu
 * TrainingDataReader.
 * \sa setTrainingDataReader, sampleFinished, epochFinished
 */
void NetTrainer::run()
{
	if (m_reader == 0) {
		return;
	}
	m_net->setInputVectorSize(m_reader->inputVectorSize());
	m_net->setOutputVectorSize(m_reader->outputVectorSize());
	{
		QMutexLocker lock(&m_stopMutex);
		if (m_stop == true) {
			m_stop = false;
			return;
		}
		m_stop = false;
	}
	// Inicializácia váh
	m_net->initializeTraining();
	// Doteraz najlepšie nájdené MSE
	double bestMse = numeric_limits<double>::max();
	// Doteraz najlepšie nájdené hodnoty neurónovej siete
	string bestNet = m_net->saveText();
	// V každej trénovacej epoche sa vyšle signál epochFinished
	for (int epoch = 0; epoch < m_numEpoch; ++epoch) {
		// Pre každú vzorku sa volá trainSample
		std::size_t trainingSetSize = qMin(m_reader->trainingSetSize(), m_trainingSetSize);
		if (trainingSetSize == 0) {
			m_trainingSetSize = m_reader->trainingSetSize();
		}
		for (std::size_t sample = 0; sample < trainingSetSize; ++sample) {
			{
				QMutexLocker lock(&m_stopMutex);
				if (m_stop) {
					m_stop = false;
					return;
				}
			}
			m_net->trainSample(m_reader->inputVector(sample), m_reader->outputVector(sample));
			// Každých 16 položiek sa vyšle signál sampleFinished
			if (sample % 16 == 0) {
				emit sampleFinished(sample + 1, epoch);
			}
		}
		emit sampleFinished(m_reader->trainingSetSize(), epoch);
		double msea = calcMse(0, trainingSetSize);
		double msee = msea;
		double thresholda = 0;
		double msebina = calcMse(0, trainingSetSize, true, &thresholda);
		double thresholde = thresholda;
		double msebine = msebina;
		if (trainingSetSize != m_reader->trainingSetSize()) {
			msee = calcMse(trainingSetSize, m_reader->trainingSetSize());
			msebine = calcMse(trainingSetSize, m_reader->trainingSetSize(), true, &thresholde);
		}
		if (msebine < bestMse) {
			bestMse = msee;
			bestNet = m_net->saveText();
		}
		emit epochFinished(epoch, msea, msee, msebina, msebine, thresholda, thresholde);
	}
	m_net->restoreText(bestNet);
	{
		QMutexLocker lock(&m_stopMutex);
		m_stop = false;
	}
}

/**
 * Výpočet MSE neurónovej siete.
 * \sa calcOutput
 */
double NetTrainer::calcMse(std::size_t from, std::size_t to, bool binary, double *thresholdOut)
{
	double errorSum = 0;
	typedef QPair<double,double> OutT;
	QList<OutT> outData;
	for (std::size_t sample = from; sample < to; ++sample) {
		double out = m_net->calcOutput(m_reader->inputVector(sample))(0);
		double expected = m_reader->outputVector(sample)(0);
		if (binary) {
			outData << QPair<double,double>(out, expected);
		}
		else {
			double diff = out - expected;
			errorSum += diff * diff;
		}
	}
	if (binary) {
		long badSum = 0;
		qSort(outData.begin(), outData.end(), [](OutT x, OutT y) { return x.first < y.first; });
		double threshold = 0;
		for (auto sample = outData.begin(); sample != outData.end(); ++sample) {
			double expected = sample->second;
			double out = 1.0; // Pre hranicu 0
			if (expected != out) {
				badSum++;
			}
		}
		long best = badSum;
		long bestThres = threshold;
		for (auto sample = outData.begin(); sample != outData.end(); ++sample) {
			threshold = sample->first;
			if (sample->second == 1) {
				badSum++;
			}
			else {
				badSum--;
			}
			if (badSum < best) {
				best = badSum;
				bestThres = threshold;
			}
		}
		errorSum = best;
		if (thresholdOut != 0) {
			*thresholdOut = threshold;
		}
	}
	return errorSum / static_cast<double>(to - from);
}

} /* end of namespace FaceDetect */

