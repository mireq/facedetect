/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  16.04.2011 12:38:58
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <lapackpp/blas3pp.h>
#include <lapackpp/blas1pp.h>
#include <lapackpp/blaspp.h>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <sstream>
#include "FaceStructuredNet.h"

using std::istringstream;
using std::ostringstream;

namespace FaceDetect {

FaceStructuredNet::FaceStructuredNet(QObject *parent):
	NeuralNet(parent),
	m_uOut(0)
{
	// Inicializácia výstupov stredných neurónov
	m_stred1.resize(PartCount);
	m_uStred1.resize(PartCount);
	m_delta1.resize(PartCount);
	for (int part = 0; part < PartCount; ++part) {
		m_stred1[part] = LaVectorDouble(m_1Neuronov);
		m_uStred1[part] = LaVectorDouble(m_1Neuronov);
		m_delta1[part] = LaVectorDouble(m_1Neuronov);
	}
	m_stred2 = LaVectorDouble(PartCount);
	m_uStred2 = LaVectorDouble(PartCount);
	m_delta2 = LaVectorDouble(PartCount);
}

FaceStructuredNet::~FaceStructuredNet()
{
}

LaVectorDouble FaceStructuredNet::calcOutput(const LaVectorDouble &input)
{
	// Výstupný vektor
	LaGenMatDouble ret(outputVectorSize(), 1);
	int inputSize = inputVectorSize();
	int partSize = inputVectorSize() / PartCount;
	int partId = 0;
	if (m_inputs.size() != uint(PartCount)) {
		m_inputs.resize(PartCount);
	}
	if (m_inputs[0].size() != partSize) {
		for (int part = 0; part < PartCount; ++part) {
			m_inputs[part] = LaVectorDouble(partSize);
		}
	}

	for (int partStart = 0; partStart < inputSize; partStart += partSize) {
		// Naplnenie vstupného vektoru časti
		for (int inputId = 0; inputId < partSize; ++inputId) {
			m_inputs[partId](inputId) = input(partStart + inputId);
		}
		Blas_Mat_Mat_Mult(m_w1[partId], m_inputs[partId], m_uStred1[partId], false, false);
		int rows = m_stred1[partId].rows();
		for (int row = 0; row < rows; ++row) {
			m_stred1[partId](row) = sigmoid(m_uStred1[partId](row));
		}
		Blas_Mat_Mat_Mult(m_stred1[partId], m_w2[partId], ret, true);
		m_uStred2(partId) = ret(0, 0);
		m_stred2(partId) = sigmoid(m_uStred2(partId));
		++partId;
	}

	Blas_Mat_Mat_Mult(m_stred2, m_w3, ret, true);
	m_uOut = ret(0, 0);
	ret(0, 0) = sigmoid(m_uOut);
	//ret(0, 0) = m_uOut / static_cast<double>(PartCount);
	return ret;
}

void FaceStructuredNet::trainSample(const LaVectorDouble &input, const LaVectorDouble &expectedOutput)
{
	// Rýchlosť učenia ($\eta$)
	double n = learningSpeed();
	// Výpočet $\hat{y}$
	double out = calcOutput(input)(0);
	// Výpočet chyby $e = y - \hat{y}$
	double chyba = expectedOutput(0) - out;
	// Výpočet delta výstupného neurónu $\Delta = \Psi^\prime(u)$
	double delta3 = chyba * derivSigmoid(m_uOut);
	//double delta3 = chyba / static_cast<double>(PartCount);

	for (int part = 0; part < PartCount; ++part) {
		m_delta2(part) = m_w3(part) * delta3 * derivSigmoid(m_uStred2(part));
	}
	for (int part = 0; part < PartCount; ++part) {
		int rows = m_uStred1[part].rows();
		for (int row = 0; row < rows; ++row) {
			m_delta1[part](row) = m_w2[part](row) * m_delta2(part) * derivSigmoid(m_uStred1[part](row));
		}
	}

	// Adaptácia váh $\vect{v} = \vect{v} + \vect{o}\eta\Delta$
	Blas_Add_Mult(m_w3, (n * delta3), m_stred2);
	for (int part = 0; part < PartCount; ++part) {
		Blas_Add_Mult(m_w2[part], (n * m_delta2(part)), m_stred1[part]);
	}
	for (int part = 0; part < PartCount; ++part) {
		Blas_Mat_Mat_Mult(m_delta1[part], m_inputs[part], m_w1[part], false, true, n, 1.0);
	}
}

void FaceStructuredNet::initializeTraining()
{
	// Počet vstupných častí
	static double matrixInitMin = -1.0;
	static double matrixInitMax = 1.0;
	int partSize = inputVectorSize() / PartCount;

	// Inicializácia váh prvej vrsvy
	m_w1.resize(PartCount);
	for (int part = 0; part < PartCount; ++part) {
		m_w1[part] = LaGenMatDouble(m_1Neuronov, partSize);
		initializeMatrix(m_w1[part], matrixInitMin, matrixInitMax);
	}
	// Inicializácia váh druhej vrstvy
	m_w2.resize(PartCount);
	for (int part = 0; part < PartCount; ++part) {
		m_w2[part] = LaGenMatDouble(m_1Neuronov, 1);
		initializeMatrix(m_w2[part], matrixInitMin, matrixInitMax);
	}
	m_w3 = LaVectorDouble(PartCount);
	initializeMatrix(m_w3, matrixInitMin, matrixInitMax);
}

std::string FaceStructuredNet::saveText() const
{
	ostringstream saveStream;
	{
		boost::archive::text_oarchive oa(saveStream);
		oa << *this;
	}
	saveStream.flush();
	return saveStream.str();
}

void FaceStructuredNet::restoreText(const std::string &data)
{
	istringstream restoreStream(data);
	boost::archive::text_iarchive ia(restoreStream);
	ia >> *this;
}

} /* end of namespace FaceDetectt */

