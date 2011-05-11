/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  05.02.2011 09:00:36
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <boost/serialization/export.hpp>
#include "BPNeuralNet.h"
#include "FaceStructuredNet.h"
#include "NeuralNet.h"

namespace FaceDetect {

/**
 * Vytvorenie inštancie neurónovej siete.
 */
NeuralNet::NeuralNet(QObject *parent):
	QObject(parent),
	m_binaryThreshold(0.5)
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

double NeuralNet::binaryThreshold() const
{
	return m_binaryThreshold;
}

void NeuralNet::setBinaryThreshold(double threshold)
{
	m_binaryThreshold = threshold;
}

bool NeuralNet::calcBinaryOutput(const LaVectorDouble &input)
{
	LaVectorDouble out = calcOutput(input);
	if (out.size() != 1) {
		return false;
	}
	else {
		return out(0) >= m_binaryThreshold;
	}
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
 * Vytvorenie inštancie neurónovej siete typu \a type. Ak neurónová sieť nie je
 * podporovaná vráti 0.
 */
NeuralNet *NeuralNet::create(const std::string &type, QObject *parent)
{
	NeuralNet *net = 0;
	if (type == "bp") {
		net = new BPNeuralNet(parent);
	}
	else if (type == "fs") {
		net = new FaceStructuredNet(parent);
	}
	return net;
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

BOOST_CLASS_EXPORT_GUID(FaceDetect::NeuralNet, "FaceDetect::NeuralNet")

