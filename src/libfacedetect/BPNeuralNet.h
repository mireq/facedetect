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

#include "NeuralNet.h"

namespace FaceDetect {

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

private:
	static const int m_stredNeuronov = 20;

	LaGenMatDouble m_w;
	LaVectorDouble m_v;
	LaVectorDouble m_stred;
}; /* -----  end of class BPNeuralNet  ----- */

} /* end of namespace FaceDetect */

#endif   /* ----- #ifndef BPNEURALNET_H  ----- */


