/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  16.03.2011 13:38:15
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef IMAGESEGMENTER_TTAZQSZT
#define IMAGESEGMENTER_TTAZQSZT

#include <QImage>
#include <QPoint>
#include <QRect>
#include <QSize>
#include <QTransform>
#include <QVector>
#include "ImageFilter.h"

namespace FaceDetect {

/**
 * \brief Segmentácia bitmapy
 *
 * Táto trieda je zodpovedná za segmentáciu bitmapy na menšie bitmapy používané
 * pre detekciu tváre. Obraz rozdeľovaný na segmenty môže byť transformovaný
 * transformačnou maticou. V takomto prípade nebudú vrátené segmenty, ktoré sa
 * nachádzajú mimo aktívnej oblasti bitmapy.
 */
class ImageSegmenter
{
public:
	explicit ImageSegmenter(const QImage &image);
	~ImageSegmenter();
	QSize segmentSize() const;
	void setSegmentSize(const QSize &size);
	QTransform transform() const;
	void setTransform(const QTransform &transform);
	int xStep();
	void setXStep(int step);
	int yStep();
	void setYStep(int step);
	void setStep(int xStep, int yStep);
	bool grayscaleFilter() const;
	void setGrayscaleFilter(bool filter);
	QRect boundingRect() const;
	int segmentCount() const;
	QRect segmentRect(int segment) const;
	QImage segmentImage(int segment) const;

private:
	void recalcSegments() const;
	double calcIntersect(const QPointF &p1, const QPointF &p2, double yPos, bool *ok = 0) const;

private:
	/// Zdrojový obrázok.
	QImage m_sourceImage;
	/// Obrázok po transformácii.
	mutable QImage m_transformedImage;
	/// Veľkosť segmentu.
	QSize m_segmentSize;
	/// Transformácia bitmapy
	QTransform m_transform;
	/// Počty segmentov v riadku segmentov
	mutable QVector<int> m_lineCounts;
	/// Začiatky riadkov segmentov
	mutable QVector<int> m_lineStarts;
	/// Oblasť, v ktorej je transformovaná bitmapa
	mutable QRect m_boundingRect;
	/// x-ový posun segmentu
	int m_xStep;
	/// y-ový posun segmentu
	int m_yStep;
	/// Ak je \e true segmenty musia byť znovu prepočítané
	mutable bool m_dirty;
	/// Počet segmentov v obrázku
	mutable int m_segmentCount;
	/// Filter pre prevod do odtieňov šedej
	FaceDetect::ImageFilter m_grayscaleFilter;
	/// Maximálne rozlíšenie transformovaného obrázku
	static const long MaxImageResolution = 4096l * 4096l;
}; /* -----  end of class ImageSegmenter  ----- */

} /* end of namespace FaceDetect */

#endif /* end of include guard: IMAGESEGMENTER_TTAZQSZT */

