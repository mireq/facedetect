/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  16.04.2011 13:40:17
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef FACESTRUCTUREDNET_433V3TA2
#define FACESTRUCTUREDNET_433V3TA2

#include "utils/LapackSerialization.h"
#include <lapackpp/lavd.h>
#include <lapackpp/gmd.h>
#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/vector.hpp>
#include "NeuralNet.h"

namespace FaceDetect {

class FaceStructuredNet: public NeuralNet
{
Q_OBJECT
Q_PROPERTY(int s1Neuronov READ s1Neuronov WRITE setS1Neuronov);
public:
	FaceStructuredNet(QObject *parent = 0);
	~FaceStructuredNet();
	virtual LaVectorDouble calcOutput(const LaVectorDouble &input);
	virtual void trainSample(const LaVectorDouble &input, const LaVectorDouble &expectedOutput);
	virtual void initializeTraining();
	int s1Neuronov() const;
	void setS1Neuronov(int neuronov);

private:
	template<class Archive> void serialize(Archive &ar, const unsigned int version) {
		Q_UNUSED(version);
		ar & boost::serialization::base_object<NeuralNet>(*this);
		ar & m_w1;
		ar & m_w2;
		ar & m_w3;
		int neuronov = m_1Neuronov;
		ar & neuronov;
		setS1Neuronov(neuronov);
	};
	/// Počet neurónov prvej vrstvy
	int m_1Neuronov;
	/// Pošet častí obrazu
	static const int PartCount = 5;
	/// Vstup rozdelený na samostatné časti
	std::vector<LaVectorDouble> m_inputs;
	/// Váhy medzi vstupnou vrstvou a prvou skrytou vrstvou
	std::vector<LaGenMatDouble> m_w1;
	/// Vstupy neurónov prvej skrytej vrstvy
	std::vector<LaVectorDouble> m_uStred1;
	/// Výstupy neurónov prvej skrytej vrstvy
	std::vector<LaVectorDouble> m_stred1;
	/// Váhy medzi vstupnou vrstvou a druhou skrytou vrstvou
	std::vector<LaVectorDouble> m_w2;
	/// Vstupy neurónov druhej skrytej vrstvy
	LaVectorDouble m_uStred2;
	/// Vystupy neurónov druhej skrytej vrstvy
	LaVectorDouble m_stred2;
	/// Váhy medzi druhou skrytou vrstvou a výstupom
	LaVectorDouble m_w3;
	/// Delta pre prvú skrytú vrstvu
	std::vector<LaVectorDouble> m_delta1;
	/// Delta pre druhú skrytú vrstvu
	LaVectorDouble m_delta2;
	/// Výstup siete pred aktivačnou funkciou
	double m_uOut;

	Q_DISABLE_COPY(FaceStructuredNet)

friend class boost::serialization::access;
}; /* -----  end of class FaceStructuredNet  ----- */

} /* end of namespace FaceDetect */

#endif /* end of include guard: FACESTRUCTUREDNET_433V3TA2 */

