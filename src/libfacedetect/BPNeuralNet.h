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
	virtual LaColVectorDouble calcOutput(const LaColVectorDouble &input);
	virtual void trainSample(const LaColVectorDouble &input, const LaColVectorDouble &expectedOutput);
	virtual void initializeTraining();

private:
}; /* -----  end of class BPNeuralNet  ----- */

} /* end of namespace FaceDetect */

#endif   /* ----- #ifndef BPNEURALNET_H  ----- */


