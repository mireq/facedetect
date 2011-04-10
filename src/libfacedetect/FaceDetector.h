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
#include <QSharedPointer>
#include <QThread>
#include <lapackpp/gmf.h>
#include "ImageSegmenter.h"
#include "NeuralNet.h"

namespace FaceDetect {

class FaceDetector: public QThread
{
Q_OBJECT
public:
	FaceDetector(NeuralNet *neuralNet, QObject *parent = 0);
	~FaceDetector();
	QImage image() const;
	void setImage(const QImage &image);
	void setupSegmenter(const ImageSegmenter::Settings &settings);
	void scanImage();

private:
	NeuralNet *m_neuralNet;
	QSharedPointer<ImageSegmenter> m_segmenter;
	ImageSegmenter::Settings m_settings;
	/// Matica maximálnych nájdených hodnôt.
	/// \warining Riadky a stĺpse sú prehodené.
	LaGenMatFloat m_statistics;
}; /* -----  end of class FaceDetector  ----- */

} /* end of namespace FaceDetect */

#endif /* end of include guard: FACEDETECTOR_YYFAKUAG */

