/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  06.02.2011 14:20:45
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef  BPNEURALNET_H
#define  BPNEURALNET_H

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include "utils/LapackSerialization.h"
#include "NeuralNet.h"

namespace FaceDetect {

/**
 * \brief Neurónová sieť so spätným šírením signálov.
 */
class BPNeuralNet: public NeuralNet
{
Q_OBJECT
public:
	BPNeuralNet(QObject *parent = 0);
	~BPNeuralNet();
	virtual LaVectorDouble calcOutput(const LaVectorDouble &input);
	virtual void trainSample(const LaVectorDouble &input, const LaVectorDouble &expectedOutput);
	virtual void initializeTraining();
	double aktivFunkcia(const double &potencial) const;
	double derivAktivFunkcia(const double &potencial) const;
	template<class Archive> void serialize(Archive &ar, const unsigned int version) {
		Q_UNUSED(version);
		ar & boost::serialization::base_object<NeuralNet>(*this);
		ar & m_w;
		ar & m_v;
	}

private:
	/// Počet stredných neurónov
	static const int m_stredNeuronov = 200;

	/// Váhy medzi vstupnou vrstvou a skrytou vrstvou.
	LaGenMatDouble m_w;
	/// Váhy medzi skrytou vrstvou a výstupnou vrstvou.
	LaVectorDouble m_v;
	/// Potenciály v strednej vrstve.
	LaVectorDouble m_stred;
	/// Vektor delta.
	LaVectorDouble m_delta;

	Q_DISABLE_COPY(BPNeuralNet)

friend class boost::serialization::access;
}; /* -----  end of class BPNeuralNet  ----- */

} /* end of namespace FaceDetect */

#endif   /* ----- #ifndef BPNEURALNET_H  ----- */


