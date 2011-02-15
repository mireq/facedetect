/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  06.02.2011 14:20:38
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include "BPNeuralNet.h"

namespace FaceDetect {

BPNeuralNet::BPNeuralNet(QObject *parent):
	NeuralNet(parent)
{
}

BPNeuralNet::~BPNeuralNet()
{
}

LaColVectorDouble BPNeuralNet::calcOutput(const LaColVectorDouble &input)
{
	return input;
}

void BPNeuralNet::trainSample(const LaColVectorDouble &input, const LaColVectorDouble &expectedOutput)
{
	Q_UNUSED(input);
	Q_UNUSED(expectedOutput);
}

void BPNeuralNet::initializeTraining()
{
}

} /* end of namespace FaceDetect */

