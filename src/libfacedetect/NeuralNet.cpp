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

NeuralNet::NeuralNet(QObject *parent):
	QObject(parent),
	m_stop(false),
	m_learningSpeed(1),
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

TrainingDataReader *NeuralNet::trainingDataReader() const
{
	return m_reader.data();
}

void NeuralNet::setTrainingDataReader(TrainingDataReader *reader)
{
	m_reader = QSharedPointer<TrainingDataReader>(reader);
	if (m_reader != 0) {
		m_reader->setParent(this);
	}
}

std::size_t NeuralNet::inputVectorSize() const
{
	if (m_reader == 0) {
		return 0;
	}
	return m_reader->inputVectorSize();
}

std::size_t NeuralNet::outputVectorSize() const
{
	if (m_reader == 0) {
		return 0;
	}
	return m_reader->outputVectorSize();
}

void NeuralNet::train()
{
	if (m_reader == 0) {
		return;
	}
	m_stop = false;
	for (int epoch = 0; epoch < m_numEpoch; ++epoch) {
		for (std::size_t sample = 0; sample < m_reader->trainingSetSize(); ++sample) {
			if (m_stop) {
				m_stop = false;
				return;
			}
			trainSample(m_reader->inputVector(sample), m_reader->outputVector(sample));
		}
	}
	m_stop = false;
}

void NeuralNet::stop()
{
	m_stop = true;
	wait();
}

} /* end of namespace FaceDetect */

