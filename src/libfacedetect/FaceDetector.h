/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  26.03.2011 11:53:25
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef FACEDETECTOR_YYFAKUAG
#define FACEDETECTOR_YYFAKUAG

#include <QImage>
#include "ImageSegmenter.h"
#include "NeuralNet.h"

namespace FaceDetect {

class FaceDetector
{
public:
	FaceDetector(NeuralNet *neuralNet);
	~FaceDetector();
	void scanImage(const QImage &image);

private:
	NeuralNet *m_neuralNet;
	QImage m_sourceImage;
}; /* -----  end of class FaceDetector  ----- */

} /* end of namespace FaceDetect */

#endif /* end of include guard: FACEDETECTOR_YYFAKUAG */

