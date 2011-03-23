/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  05.02.2011 09:00:36
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include "NeuralNet.h"
#include "TrainingDataReader.h"

namespace FaceDetect {

/**
 * Vytvorenie inštancie neurónovej siete.
 */
NeuralNet::NeuralNet(QObject *parent):
	QThread(parent),
	m_stop(false),
	m_learningSpeed(0.0001),
	m_numEpoch(100),
	m_reader(0)
{
}

NeuralNet::~NeuralNet()
{
}

double NeuralNet::learningSpeed() const
{
	return m_learningSpeed;
}

void NeuralNet::setLearningSpeed(double learningSpeed)
{
	m_learningSpeed = learningSpeed;
}

int NeuralNet::numEpoch() const
{
	return m_numEpoch;
}

void NeuralNet::setNumEpoch(int numEpoch)
{
	m_numEpoch = numEpoch;
}


/**
 * Vráti objekt čítajúci tréningové dáta siete.
 */
TrainingDataReader *NeuralNet::trainingDataReader() const
{
	return m_reader;
}

/**
 * Nastavenie objektu čítajúceho tréningové dáta siete.
 */
void NeuralNet::setTrainingDataReader(TrainingDataReader *reader)
{
	m_reader = reader;
}

/**
 * Vráti počet vstupov (veľkosť vstupného vektoru).
 */
int NeuralNet::inputVectorSize() const
{
	if (m_reader == 0) {
		return 0;
	}
	return m_reader->inputVectorSize();
}

/**
 * Vráti počet výstupov (veľkosť výstupného vektoru).
 */
int NeuralNet::outputVectorSize() const
{
	if (m_reader == 0) {
		return 0;
	}
	return m_reader->outputVectorSize();
}

/**
 * V prípade, že práve beží tréning volanie tejto metódy zastaví tréning.
 */
void NeuralNet::stop()
{
	m_stop = true;
	wait();
}

/**
 * Inicializácia matice \a matrix náhodnými hodnotami v rozsahu (min, max>.
 */
void NeuralNet::initializeMatrix(LaGenMatDouble &matrix, double min, double max) const
{
	for (int col = 0; col < matrix.cols(); ++col) {
		for (int row = 0; row < matrix.rows(); ++row) {
			matrix(row, col) = (static_cast<double>(rand()) / RAND_MAX) * (max - min) + min;
		}
	}
}

/**
 * Spustenie tréningu. Pri tréningu sa dáta načítavajú pomocou objektu typu
 * TrainingDataReader.
 * \sa setTrainingDataReader, sampleFinished, epochFinished
 */
void NeuralNet::run()
{
	if (m_reader == 0) {
		return;
	}
	if (m_stop == true) {
		m_stop = false;
		return;
	}

	m_stop = false;
	// Inicializácia váh
	initializeTraining();
	// V každej trénovacej epoche sa vyšle signál epochFinished
	for (int epoch = 0; epoch < m_numEpoch; ++epoch) {
		// Pre každú vzorku sa volá trainSample
		for (std::size_t sample = 0; sample < m_reader->trainingSetSize(); ++sample) {
			if (m_stop) {
				m_stop = false;
				return;
			}
			trainSample(m_reader->inputVector(sample), m_reader->outputVector(sample));
			// Každých 16 položiek sa vyšle signál sampleFinished
			if (sample % 16 == 0) {
				emit sampleFinished(sample + 1, epoch);
			}
		}
		emit sampleFinished(m_reader->trainingSetSize(), epoch);
		double mse = calcMse();
		emit epochFinished(epoch, mse);
	}
	m_stop = false;
}

/**
 * Výpočet MSE neurónovej siete.
 * \sa calcOutput
 */
double NeuralNet::calcMse()
{
	double errorSum = 0;
	for (std::size_t sample = 0; sample < m_reader->trainingSetSize(); ++sample) {
		double out = calcOutput(m_reader->inputVector(sample))(0);
		double expected = m_reader->outputVector(sample)(0);
		double diff = out - expected;
		errorSum += diff * diff;
	}
	return errorSum / static_cast<double>(m_reader->trainingSetSize());
}

} /* end of namespace FaceDetect */

