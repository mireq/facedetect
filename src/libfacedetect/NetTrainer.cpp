/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  09.04.2011 20:09:14
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <QMutexLocker>
#include "NeuralNet.h"
#include "TrainingDataReader.h"
#include "NetTrainer.h"

namespace FaceDetect {

/**
 * Vytvorenie novej inštancie trénera siete.
 */
NetTrainer::NetTrainer(QObject *parent):
	QThread(parent),
	m_reader(0),
	m_stop(false),
	m_numEpoch(100),
	m_net(0)
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
	// V každej trénovacej epoche sa vyšle signál epochFinished
	for (int epoch = 0; epoch < m_numEpoch; ++epoch) {
		// Pre každú vzorku sa volá trainSample
		for (std::size_t sample = 0; sample < m_reader->trainingSetSize(); ++sample) {
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
		double mse = calcMse();
		emit epochFinished(epoch, mse, mse);
	}
	{
		QMutexLocker lock(&m_stopMutex);
		m_stop = false;
	}
}

/**
 * Výpočet MSE neurónovej siete.
 * \sa calcOutput
 */
double NetTrainer::calcMse()
{
	double errorSum = 0;
	for (std::size_t sample = 0; sample < m_reader->trainingSetSize(); ++sample) {
		double out = m_net->calcOutput(m_reader->inputVector(sample))(0);
		double expected = m_reader->outputVector(sample)(0);
		double diff = out - expected;
		errorSum += diff * diff;
	}
	return errorSum / static_cast<double>(m_reader->trainingSetSize());
}

} /* end of namespace FaceDetect */

