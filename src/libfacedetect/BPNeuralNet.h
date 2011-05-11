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
#include <boost/serialization/nvp.hpp>
#include "utils/LapackSerialization.h"
#include "NeuralNet.h"

namespace FaceDetect {

/**
 * \brief Neurónová sieť s jednou skrytou vrstvou a spätným šírením signálov.
 */
class BPNeuralNet: public NeuralNet
{
Q_OBJECT
/**
 * Počet neurónov skrytej vrstvy.
 */
Q_PROPERTY(int stredNeuronov READ stredNeuronov WRITE setStredNeuronov NOTIFY stredNeuronovChanged);
public:
	BPNeuralNet(QObject *parent = 0);
	~BPNeuralNet();
	virtual LaVectorDouble calcOutput(const LaVectorDouble &input);
	virtual void trainSample(const LaVectorDouble &input, const LaVectorDouble &expectedOutput);
	virtual void initializeTraining();
	/**
	 * Vráti počet neurónov v strednej vrstve.
	 */
	int stredNeuronov() const;
	/**
	 * Nastavenie počtu neurónov v strednej vrstve.
	 */
	void setStredNeuronov(int neuronov);
	virtual QString netType() const;

signals:
	void stredNeuronovChanged(int stredNeuronov);

private:
	/**
	 * Serializácia a deserializácia siete.
	 */
	template<class Archive> void serialize(Archive &ar, const unsigned int version) {
		Q_UNUSED(version);
		ar & boost::serialization::make_nvp("base", boost::serialization::base_object<NeuralNet>(*this));
		ar & boost::serialization::make_nvp("w", m_w);
		ar & boost::serialization::make_nvp("v", m_v);
		int neur = m_stredNeuronov;
		ar & neur;
		setStredNeuronov(neur);
	};

	/// Počet stredných neurónov
	int m_stredNeuronov;
	/// Váhy medzi vstupnou vrstvou a skrytou vrstvou.
	LaGenMatDouble m_w;
	/// Váhy medzi skrytou vrstvou a výstupnou vrstvou.
	LaVectorDouble m_v;
	/// Potenciály v strednej vrstve.
	LaVectorDouble m_uStred;
	/// Potenciál výstupnej vrstvy.
	double m_uOut;
	/// Výstup strednej vrstvy
	LaVectorDouble m_stred;
	/// Vektor delta.
	LaVectorDouble m_delta;

	Q_DISABLE_COPY(BPNeuralNet)

friend class boost::serialization::access;
}; /* -----  end of class BPNeuralNet  ----- */

} /* end of namespace FaceDetect */

#endif   /* ----- #ifndef BPNEURALNET_H  ----- */


