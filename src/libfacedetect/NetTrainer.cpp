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
#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <sstream>
#include "NeuralNet.h"
#include "TrainingDataReader.h"
#include "NetTrainer.h"

using std::numeric_limits;
using std::ostringstream;
using std::istringstream;
using std::string;
#include <QDebug>

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
	m_trainingSetSize(0),
	m_falsePositiveHandicap(50),
	m_falseNegativeHandicap(1),
	m_mseSampleCount(0),
	m_running(false),
	m_epochProgress(0),
	m_epochSteps(0)
{
	qRegisterMetaType<EpochStats>();
	connect(this, SIGNAL(finished()), SLOT(onFinished()));
	connect(this, SIGNAL(terminated()), SLOT(onFinished()));
	connect(this, SIGNAL(started()), SLOT(onStarted()));
}

NetTrainer::~NetTrainer()
{
	stop();
}

int NetTrainer::numEpoch() const
{
	return m_numEpoch;
}

void NetTrainer::setNumEpoch(int numEpoch)
{
	if (numEpoch != m_numEpoch) {
		stop();
		m_numEpoch = numEpoch;
		numEpochChanged(m_numEpoch);
	}
}

std::size_t NetTrainer::trainingSetSize() const
{
	return m_trainingSetSize;
}

void NetTrainer::setTrainingSetSize(std::size_t size)
{
	m_trainingSetSize = size;
}

int NetTrainer::falsePositiveHandicap() const
{
	return m_falsePositiveHandicap;
}

void NetTrainer::setFalsePositiveHandicap(int handicap)
{
	m_falsePositiveHandicap = handicap;
}

int NetTrainer::falseNegativeHandicap() const
{
	return m_falseNegativeHandicap;
}

void NetTrainer::setFalseNegativeHandicap(int handicap)
{
	m_falseNegativeHandicap = handicap;
}

bool NetTrainer::isRunning() const
{
	return m_running;
}

double NetTrainer::epochProgress() const
{
	return m_epochProgress;
}

int NetTrainer::epoch() const
{
	return m_epoch;
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
 * Vráti štatistiky pre najlepšiu tréningovú epochu.
 */
NetTrainer::EpochStats NetTrainer::bestEpochStats() const
{
	return m_bestEpochStats;
}

/**
 * Metóda sa zavolá po spustení tréningu. Vysiela signál runningChanged.
 */
void NetTrainer::onStarted()
{
	if (!m_running) {
		m_running = true;
		emit runningChanged(m_running);
	}
}

/**
 * Metóda sa zavolá pri ukončení tréninguj (vrátane prerušenia). Vysiela signál
 * runningChanged.
 */
void NetTrainer::onFinished()
{
	if (m_running) {
		m_running = false;
		emit runningChanged(m_running);
	}
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
	m_reader->shuffle();
	m_net->setInputVectorSize(m_reader->inputVectorSize());
	m_net->setOutputVectorSize(m_reader->outputVectorSize());
	{
		QMutexLocker lock(&m_stopMutex);
		if (m_stop) {
			m_stop = false;
			return;
		}
	}
	// Inicializácia váh
	m_net->initializeTraining();
	// Doteraz najlepšie nájdené MSE
	double bestMse = numeric_limits<double>::max();
	// Doteraz najlepšie nájdené hodnoty neurónovej siete
	string bestNet;
	saveNet(bestNet);
	// Doteraz najlepšie dosiahnuté štatistiky siete
	EpochStats bestEpoch;
	std::size_t trainingSetSize = qMin(m_reader->trainingSetSize(), m_trainingSetSize);
	if (trainingSetSize == 0) {
		m_trainingSetSize = m_reader->trainingSetSize();
	}
	m_matchStatA.resize(m_trainingSetSize);
	m_matchStatE.resize(m_reader->trainingSetSize() - m_trainingSetSize);
	m_bestMatchStatA = m_matchStatA;
	m_bestMatchStatE = m_matchStatE;
	// Výpis len niektorých položiek
	int printEvery = 4;
	if (trainingSetSize > 4000) {
		printEvery = trainingSetSize / 4000;
	}
	m_epochSteps = trainingSetSize * 2 + m_reader->trainingSetSize();
	bool stop = false;
	// V každej tréningovej epoche sa vyšle signál epochFinished
	for (m_epoch = 1; m_epoch <= m_numEpoch; ++m_epoch) {
		m_epochProgress = 0;
		emit epochProgressChanged(m_epochProgress);
		emit epochChanged(m_epoch);
		// Pre každú vzorku sa volá trainSample
		for (std::size_t sample = 0; sample < trainingSetSize; ++sample) {
			{
				QMutexLocker lock(&m_stopMutex);
				if (m_stop) {
					stop = true;
					break;
				}
			}
			m_net->trainSample(m_reader->inputVector(sample), m_reader->outputVector(sample));
			// Každých niekoľko položiek sa vyšle signál sampleFinished
			if (sample % printEvery == 0) {
				emit sampleFinished(sample + 1, m_epoch);
				m_epochProgress = static_cast<double>(sample * 2) / m_epochSteps;
				emit epochProgressChanged(m_epochProgress);
			}
		}
		if (stop) {
			break;
		}
		emit sampleFinished(trainingSetSize, m_epoch);
		calcMseForEpoch(bestNet, bestMse, bestEpoch);
	}
	m_epoch = 0;
	emit epochChanged(m_epoch);
	m_mseSampleCount = 0;
	// Obnovenie najlepších nastavení
	restoreNet(bestNet);
	m_bestEpochStats = bestEpoch;
	m_bestEpochStats.matchStatA = m_bestMatchStatA;
	m_bestEpochStats.matchStatE = m_bestMatchStatE;
	{
		QMutexLocker lock(&m_stopMutex);
		m_stop = false;
	}
}

/**
 * Výpočet MSE neurónovej siete.
 * \sa calcOutput
 */
double NetTrainer::calcMse(std::size_t from, std::size_t to, bool binary, double *thresholdOut, QVector<MatchStat> *matchStat, long *fNeg, long *fPos, double *mse, long *faces)
{
	double binErrorSum = 0;
	double errorSum = 0;
	double partErrorSum = 0;
	std::size_t sampleCount = 0;
	std::size_t partSampleCount = 0;
	long faceCount = 0;
	typedef QPair<double,double> OutT;
	QList<OutT> outData;
	for (std::size_t sample = from; sample < to; ++sample) {
		double out = m_net->calcOutput(m_reader->inputVector(sample))(0);
		double expected = m_reader->outputVector(sample)(0);
		if (expected == 1) {
			faceCount++;
		}
		if (binary) {
			outData << QPair<double,double>(out, expected);
		}
		double diff = out - expected;
		partErrorSum += diff * diff;
		partSampleCount++;
		if (partSampleCount % 128 == 0) {
			exportMseStats(partSampleCount, partErrorSum);
			errorSum += partErrorSum;
			sampleCount += partSampleCount;
			partErrorSum = 0;
			partSampleCount = 0;

			// Výpočet priebehu epochy
			m_epochProgress = static_cast<double>(qMin(m_reader->trainingSetSize(), m_trainingSetSize) * 2 + m_mseSampleCount) / m_epochSteps;
			emit epochProgressChanged(m_epochProgress);

			QMutexLocker lock(&m_stopMutex);
			if (m_stop) {
				return 0;
			}
		}
	}

	if (faces != 0) {
		*faces = faceCount;
	}

	if (partSampleCount != 0) {
		exportMseStats(partSampleCount, partErrorSum);
		errorSum += partErrorSum;
		sampleCount += partSampleCount;
	}
	if (sampleCount == 0) {
		sampleCount = 1;
	}
	double msError = errorSum / static_cast<double>(sampleCount);
	if (mse != 0) {
		*mse = msError;
	}

	if (binary) {
		long badSum = 0;
		long falsePositive = 0;
		long falseNegative = 0;
		qSort(outData.begin(), outData.end(), [](OutT x, OutT y) { return x.first < y.first; });
		double threshold = 0;
		double prevThresh = 0;
		for (auto sample = outData.begin(); sample != outData.end(); ++sample) {
			double expected = sample->second;
			double out = 1.0; // Pre hranicu 0
			if (expected != out) {
				falsePositive++;
				badSum++;
			}
		}
		long best = m_falsePositiveHandicap * falsePositive + m_falseNegativeHandicap * falseNegative;
		double bestThres = threshold;
		int sampleIndex = 0;
		if (fNeg != 0) {
			*fNeg = falseNegative;
		}
		if (fPos != 0) {
			*fPos = falsePositive;
		}
		for (auto sample = outData.begin(); sample != outData.end(); ++sample) {
			threshold = sample->first;
			if (sample->second == 1) {
				falseNegative++;
				badSum++;
			}
			else {
				falsePositive--;
				badSum--;
			}
			long badVazene = m_falsePositiveHandicap * falsePositive + m_falseNegativeHandicap * falseNegative;
			if (badVazene < best) {
				best = badVazene;
				bestThres = (threshold + prevThresh) / 2.0;
				if (fNeg != 0) {
					*fNeg = falseNegative;
				}
				if (fPos != 0) {
					*fPos = falsePositive;
				}
			}
			if (matchStat != 0) {
				(*matchStat)[sampleIndex].falsePositive = falsePositive;
				(*matchStat)[sampleIndex].falseNegative = falseNegative;
				(*matchStat)[sampleIndex].threshold = (threshold + prevThresh) / 2.0;
			}
			prevThresh = threshold;
			++sampleIndex;
		}
		binErrorSum = best;
		if (thresholdOut != 0) {
			*thresholdOut = bestThres;
		}
		return binErrorSum / static_cast<double>(sampleCount);
	}
	else {
		return msError;
	}
}

/**
 * Emitovanie signálov pri ukončení epochy a vrátenie doteraz najlepšej siete.
 */
void NetTrainer::calcMseForEpoch(std::string &bestNet, double &bestMse, EpochStats &bestEpoch)
{
	m_mseSampleCount = 0;
	long fNegA = 0;
	long fPosA = 0;
	long fNegE = 0;
	long fPosE = 0;
	double msea = 0;
	double thresholda = 0;
	double msebina = calcMse(0, m_trainingSetSize, true, &thresholda, &m_matchStatA, &fNegA, &fPosA, &msea, &bestEpoch.facesA);
	double thresholde = thresholda;
	double msee = msea;
	double msebine = msebina;
	if (m_trainingSetSize != m_reader->trainingSetSize()) {
		msebine = calcMse(m_trainingSetSize, m_reader->trainingSetSize(), true, &thresholde, &m_matchStatE, &fNegE, &fPosE, &msee, &bestEpoch.facesE);
		QMutexLocker lock(&m_stopMutex);
		if (m_stop) {
			return;
		}
	}
	{
		QMutexLocker lock(&m_stopMutex);
		if (m_stop) {
			return;
		}
	}

	if (m_trainingSetSize == m_reader->trainingSetSize()) {
		msee = msea;
		msebine = msebina;
		thresholde = thresholda;
	}

	m_net->setBinaryThreshold(thresholde);
	EpochStats stats = bestEpoch;
	stats.epoch = m_epoch;
	stats.mseA = msea;
	stats.mseE = msee;
	stats.mseBinA = msebina;
	stats.mseBinE = msebine;
	stats.thresholdA = thresholda;
	stats.thresholdE = thresholde;
	stats.sizeA = m_trainingSetSize;
	stats.sizeE = m_reader->trainingSetSize() - m_trainingSetSize;
	stats.falseNegativeA = fNegA;
	stats.falsePositiveA = fPosA;
	stats.falseNegativeE = fNegE;
	stats.falsePositiveE = fPosE;
	//double mseCombined = qMax(thresholde, 1.0 - thresholde) * msebine;
	double mseCombined = msee * msebine;
	if (mseCombined <= bestMse) {
	//if (msebine <= bestMse) {
		bestMse = mseCombined;
		//bestMse = msebine;
		bestEpoch = stats;
		saveNet(bestNet);
		m_bestMatchStatA = m_matchStatA;
		m_bestMatchStatE = m_matchStatE;
	}
	emit epochFinished(stats);
}

/**
 * Uloženie neurónovej siete do reťazca \a net.
 */
void NetTrainer::saveNet(std::string &net)
{
	ostringstream saveStream;
	{
		boost::archive::text_oarchive oa(saveStream);
		oa << *const_cast<const FaceDetect::NeuralNet *>(m_net);
	}
	net = saveStream.str();
}

/**
 * Obnovenie neurónovej siete z reťazca \a net.
 */
void NetTrainer::restoreNet(const std::string &net)
{
	istringstream restoreStream(net);
	boost::archive::text_iarchive ia(restoreStream);
	ia >> *m_net;
}

/**
 * Výstup hodnôt validácie pre zobrazenie priebehu.
 */
void NetTrainer::exportMseStats(std::size_t count, double errorSum)
{
	if (count == 0) {
		return;
	}
	m_mseSampleCount += count;
	emit errorCalculated(m_mseSampleCount, count, errorSum);
}

} /* end of namespace FaceDetect */

