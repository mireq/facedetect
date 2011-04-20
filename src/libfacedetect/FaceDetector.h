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
#include <valarray>
#include "ImageFilter.h"
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
	void setLocalFilter(const ImageFilter &localFilter);
	void setGlobalFilter(const ImageFilter &globalFilter);

private:
	NeuralNet *m_neuralNet;
	QSharedPointer<ImageSegmenter> m_segmenter;
	ImageSegmenter::Settings m_settings;
	/// Štatistiky pre obraz.
	std::valarray<float> m_statistics;
	/// Lokálny filter.
	ImageFilter m_localFilter;
}; /* -----  end of class FaceDetector  ----- */

} /* end of namespace FaceDetect */

#endif /* end of include guard: FACEDETECTOR_YYFAKUAG */

