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

/**
 * \brief Detektor tváre skenujúci vstupný obraz.
 */
class FaceDetector: public QThread
{
Q_OBJECT
public:
	struct DetectionWindow {
		double value;
		QPolygon polygon;
	};
	FaceDetector(NeuralNet *neuralNet, QObject *parent = 0);
	~FaceDetector();
	QImage image() const;
	void setImage(const QImage &image);
	void setupSegmenter(const ImageSegmenter::Settings &settings);
	void scanImage();
	void setLocalFilter(const ImageFilter &localFilter);
	void setGlobalFilter(const ImageFilter &globalFilter);
	QImage scanResultImage() const;
	QVector<FaceDetector::DetectionWindow> scanResult() const;

private:
	/// Neurónová sieť používaná na detekciu.
	NeuralNet *m_neuralNet;
	/// Trieda pre rozklad obrazu na fragmenty.
	QSharedPointer<ImageSegmenter> m_segmenter;
	/// Nastavenie rozkladu obrazu.
	ImageSegmenter::Settings m_settings;
	/// Štatistiky pre obraz.
	std::valarray<float> m_statistics;
	/// Lokálny filter.
	ImageFilter m_localFilter;
	/// Výsledok skenovania súboru.
	QImage m_scanResultImage;
	/// Výsledky ako súbor polygónov.
	QVector<DetectionWindow> m_scanResult;
}; /* -----  end of class FaceDetector  ----- */

} /* end of namespace FaceDetect */

#endif /* end of include guard: FACEDETECTOR_YYFAKUAG */

