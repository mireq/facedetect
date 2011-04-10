/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  05.02.2011 09:00:36
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include "NeuralNet.h"

namespace FaceDetect {

/**
 * Vytvorenie inštancie neurónovej siete.
 */
NeuralNet::NeuralNet(QObject *parent):
	QObject(parent)
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

/**
 * Nastavenie veľkosti vstupného vektoru.
 */
void NeuralNet::setInputVectorSize(int size)
{
	m_inputVectorSize = size;
}

/**
 * Nastavenie veľkosti výstupného vektoru.
 */
void NeuralNet::setOutputVectorSize(int size)
{
	m_outputVectorSize = size;
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

} /* end of namespace FaceDetect */

