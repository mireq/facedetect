/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  26.03.2011 11:53:17
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include "FaceDetector.h"

namespace FaceDetect {

FaceDetector::FaceDetector(NeuralNet *neuralNet):
	m_neuralNet(neuralNet)
{
}

FaceDetector::~FaceDetector()
{
}

} /* end of namespace FaceDetect */

