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
#include <boost/serialization/export.hpp>
#include <sstream>
#include "FaceStructuredNet.h"

using std::istringstream;
using std::ostringstream;

namespace FaceDetect {

FaceStructuredNet::FaceStructuredNet(QObject *parent):
	NeuralNet(parent),
	m_1Neuronov(0),
	m_uOut(0)
{
	setS1Neuronov(10);
}

FaceStructuredNet::~FaceStructuredNet()
{
}

LaVectorDouble FaceStructuredNet::calcOutput(const LaVectorDouble &input)
{
	int inputSize = inputVectorSize();
	int partSize = inputVectorSize() / PartCount;

	// Úprava veľkosti vstupov
	if (m_inputs.size() != uint(PartCount)) {
		m_inputs.resize(PartCount);
	}
	if (m_inputs[0].size() != partSize) {
		for (int part = 0; part < PartCount; ++part) {
			m_inputs[part] = LaVectorDouble(partSize);
		}
	}

	// Výstupný vektor
	LaGenMatDouble ret(outputVectorSize(), 1);
	// Poradové číslo časti obrazu
	int partId = 0;
	for (int partStart = 0; partStart < inputSize; partStart += partSize) {
		// Naplnenie vstupného vektoru časti
		for (int inputId = 0; inputId < partSize; ++inputId) {
			m_inputs[partId](inputId) = input(partStart + inputId);
		}
		// $\vect{u}^1_k = \vect{w}^1_k \times \vect{x}^1_k$
		Blas_Mat_Mat_Mult(m_w1[partId], m_inputs[partId], m_uStred1[partId], false, false);
		// $o_{jk}^1 = $\Psi_1(u_{jk}^1)$
		int rows = m_stred1[partId].rows();
		for (int row = 0; row < rows; ++row) {
			m_stred1[partId](row) = sigmoid(m_uStred1[partId](row));
		}
		// $u^2_k = \vect{o}^1_k \times \vect{w}^2$
		Blas_Mat_Mat_Mult(m_stred1[partId], m_w2[partId], ret, true);
		m_uStred2(partId) = ret(0, 0);
		// $o_k^2 = $\Psi_2(u_k^2)$
		m_stred2(partId) = sigmoid(m_uStred2(partId));
		++partId;
	}

	// $u^3 = \vect{o}^2 \times \vect{w}^3$
	Blas_Mat_Mat_Mult(m_stred2, m_w3, ret, true);
	m_uOut = ret(0, 0);
	// $ \hat{y} = \Psi_3(u^3)$
	ret(0, 0) = sigmoid(m_uOut);
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
	// Výpočet delta výstupného neurónu $\Delta^3 = \Psi_3^\prime(u_3)$
	double delta3 = chyba * derivSigmoid(m_uOut);

	// Výpočet chybového termu delta pre neuróny druhej sktytej vrstvy
	// $\Delta^2_k = \Delta^3 w^3_k \Psi_2^\prime(u^2_k)$
	for (int part = 0; part < PartCount; ++part) {
		m_delta2(part) = m_w3(part) * delta3 * derivSigmoid(m_uStred2(part));
	}
	// $\Delta^1_jk = \Delta^2_k w^2_jk \Psi_1^\prime(u^1_jk)$
	for (int part = 0; part < PartCount; ++part) {
		int rows = m_uStred1[part].rows();
		for (int row = 0; row < rows; ++row) {
			m_delta1[part](row) = m_w2[part](row) * m_delta2(part) * derivSigmoid(m_uStred1[part](row));
		}
	}

	// Adaptácia váh $\vect{w}^3 = \vect{w}^3 + \vect{o}^2\eta\Delta^3$
	Blas_Add_Mult(m_w3, (n * delta3), m_stred2);
	// Adaptácia váh $\vect{w}^2_k = \vect{w}^2_k + \vect{o}^1\eta\vect{\Delta}^2$
	for (int part = 0; part < PartCount; ++part) {
		Blas_Add_Mult(m_w2[part], (n * m_delta2(part)), m_stred1[part]);
	}
	// Adaptácia váh $\vect{w}^1_k = \vect{w}^1_k + \vect{x}_k\eta\vect{\Delta}^1_k$
	for (int part = 0; part < PartCount; ++part) {
		Blas_Mat_Mat_Mult(m_delta1[part], m_inputs[part], m_w1[part], false, true, n, 1.0);
	}
}

void FaceStructuredNet::initializeTraining()
{
	// Počiatočné hodnoty váh
	static double matrixInitMin = -1.0;
	static double matrixInitMax = 1.0;
	// Počet hodnôt jednej časti
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
	// Inicializácia váh tretej vrstvy
	m_w3 = LaVectorDouble(PartCount);
	initializeMatrix(m_w3, matrixInitMin, matrixInitMax);
}

int FaceStructuredNet::s1Neuronov() const
{
	return m_1Neuronov;
}

void FaceStructuredNet::setS1Neuronov(int neuronov)
{
	if (m_1Neuronov != neuronov) {
		m_1Neuronov = neuronov;
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
}

} /* end of namespace FaceDetectt */

BOOST_CLASS_EXPORT_GUID(FaceDetect::FaceStructuredNet, "FaceDetect::FaceStructuredNet")

