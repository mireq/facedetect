/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  06.02.2011 14:20:38
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
#include "TrainingDataReader.h"
#include "BPNeuralNet.h"

using std::istringstream;
using std::ostringstream;

namespace FaceDetect {

/**
 * \econstruct
 */
BPNeuralNet::BPNeuralNet(QObject *parent):
	NeuralNet(parent),
	m_stredNeuronov(0),
	m_uOut(0)
{
	setStredNeuronov(80);
}

BPNeuralNet::~BPNeuralNet()
{
}

LaVectorDouble BPNeuralNet::calcOutput(const LaVectorDouble &input)
{
	// Násobenie matice a vektoru $\vect{u}^{stred} = \matr{W} \times \vect{x}$
	Blas_Mat_Mat_Mult(m_w, input, m_uStred, false, false);
	// Výpočet $o_j = \Psi(u_j^{stred})$
	for (int row = 0; row < m_stred.rows(); ++row) {
		m_stred(row) = sigmoid(m_uStred(row));
	}
	// Výstupný vektor
	LaGenMatDouble ret(outputVectorSize(), 1);
	// Výpočet $\hat{y} = \vect{x}^T \times \vect{v}$
	Blas_Mat_Mat_Mult(m_stred, m_v, ret, true);
	m_uOut = ret(0, 0);
	ret(0, 0) = sigmoid(ret(0, 0));
	return ret;
}

void BPNeuralNet::trainSample(const LaVectorDouble &input, const LaVectorDouble &expectedOutput)
{
	// Rýchlosť učenia ($\eta$)
	double n = learningSpeed();
	// Výpočet $\hat{y}$
	double out = calcOutput(input)(0);
	// Výpočet chyby $e = y - \hat{y}$
	double chyba = expectedOutput(0) - out;
	// Výpočet delta výstupného neurónu $\Delta = \Psi^\prime(u)$
	double delta = chyba * derivSigmoid(m_uOut);
	// Adaptácia váh $\vect{v} = \vect{v} + \vect{o}\eta\Delta$
	Blas_Add_Mult(m_v, (n * delta), m_stred);

	// Adaptácia váh stredných neurónov
	// Výpočet $\Psi^\prime(u_j)$
	for (int row = 0; row < m_stred.rows(); ++row) {
		m_delta(row) = derivSigmoid(m_uStred(row));
	}
	// Výpočet $\Delta_j=\Psi^\prime(u_j)v_j^T\Delta$
	Blas_Mat_Mat_Trans_Mult(m_delta, m_v, m_delta);
	m_delta *= delta;
	// Výpočet $\matr{W} = \matr{W}+\Delta\vect{x}^T\eta$
	Blas_Mat_Mat_Mult(m_delta, input, m_w, false, true, n, 1.0);
}

void BPNeuralNet::initializeTraining()
{
	m_w = LaGenMatDouble(m_stredNeuronov, inputVectorSize());
	m_v = LaGenMatDouble(m_stredNeuronov, 1);
	initializeMatrix(m_w, -0.5, 0.5);
	initializeMatrix(m_v, -0.5, 0.5);
}

int BPNeuralNet::stredNeuronov() const
{
	return m_stredNeuronov;
}

void BPNeuralNet::setStredNeuronov(int neuronov)
{
	if (neuronov != m_stredNeuronov) {
		m_stredNeuronov = neuronov;
		m_stred = LaGenMatDouble(m_stredNeuronov, 1);
		m_uStred = LaGenMatDouble(m_stredNeuronov, 1);
		m_delta = LaGenMatDouble(m_stredNeuronov, 1);
		stredNeuronovChanged(m_stredNeuronov);
	}
}

} /* end of namespace FaceDetect */

BOOST_CLASS_EXPORT_GUID(FaceDetect::BPNeuralNet, "FaceDetect::BPNeuralNet")

