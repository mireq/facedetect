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
#include "TrainingDataReader.h"
#include "BPNeuralNet.h"

namespace FaceDetect {

/**
 * \econstruct
 */
BPNeuralNet::BPNeuralNet(QObject *parent):
	NeuralNet(parent)
{
	m_stred = LaGenMatDouble(m_stredNeuronov, 1);
	m_delta = LaGenMatDouble(m_stredNeuronov, 1);
}

BPNeuralNet::~BPNeuralNet()
{
}

LaVectorDouble BPNeuralNet::calcOutput(const LaVectorDouble &input)
{
	// Výstupný vektor
	LaGenMatDouble ret(trainingDataReader()->outputVectorSize(), 1);
	// Násobenie matice a vektoru $\vect{u}^{stred} = \matr{W} \times \vect{x}$
	Blas_Mat_Mat_Mult(m_w, input, m_stred, false, false);
	// Výpočet $o_j = \Psi(u_j^{stred})$
	for (int row = 0; row < m_stred.rows(); ++row) {
		m_stred(row) = aktivFunkcia(m_stred(row));
	}
	// Výpočet $\hat{y} = \vect{x}^T \times \vect{v}$
	Blas_Mat_Mat_Mult(m_stred, m_v, ret, true);
	ret(0, 0) = aktivFunkcia(ret(0, 0));
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
	double delta = chyba * derivAktivFunkcia(out);
	// Adaptácia váh $\vect{v} = \vect{v} + \vect{o}\eta\Delta$
	Blas_Add_Mult(m_v, (n * delta), m_stred);

	// Adaptácia váh stredných neurónov
	// Výpočet $\Psi^\prime(u_j)$
	// Ako dočasné úložište sa používa hodnota stredných neurónov
	for (int row = 0; row < m_stred.rows(); ++row) {
		m_delta(row) = derivAktivFunkcia(m_stred(row));
	}
	// Výpočet $\Delta_j=\Psi^\prime(u_j)v_j^T\Delta$
	Blas_Mat_Mat_Trans_Mult(m_delta, m_v, m_delta);
	m_delta *= delta;
	// Výpočet $\matr{W} = \matr{W}+\Delta\vect{x}^T\eta$
	Blas_Mat_Mat_Mult(m_delta, input, m_w, false, true, n, 1.0);
}

void BPNeuralNet::initializeTraining()
{
	m_w = LaGenMatDouble(m_stredNeuronov, trainingDataReader()->inputVectorSize());
	m_v = LaGenMatDouble(m_stredNeuronov, 1);
	initializeMatrix(m_w, -0.5, 0.5);
	initializeMatrix(m_v, -0.5, 0.5);
}

/**
 * Vypočíta aktivačnú funkciu (výstupnú hodnotu) pre potenciál \a potencial.
 */
inline double BPNeuralNet::aktivFunkcia(const double &potencial) const
{
	return 1.0 / (1.0 + exp(-potencial));
}

/**
 * Vypočíta deriváciu aktivačnej funkcie pre potenciál \a potencial.
 */
inline double BPNeuralNet::derivAktivFunkcia(const double &potencial) const
{
	double pom = exp(-potencial);
	double pom1 = 1.0 + pom;
	return pom/(pom1 * pom1);
}

} /* end of namespace FaceDetect */

