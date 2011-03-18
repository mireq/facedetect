/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  16.03.2011 13:38:15
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <QImage>
#include <QPoint>
#include <QRect>
#include <QSize>
#include <QTransform>
#include <QVector>

namespace FaceDetect {

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
	QRect boundingRect() const;
	int segmentCount() const;
	QRect segmentRect(int segment) const;
	QImage segmentImage(int segment) const;
	QImage trans() const;

private:
	void recalcRegions() const;
	double calcIntersect(const QPointF &p1, const QPointF &p2, double yPos, bool *ok = 0) const;

private:
	QImage m_sourceImage;
	mutable QImage m_transformedImage;
	QSize m_segmentSize;
	QTransform m_transform;
	mutable QVector<int> m_lineCounts;
	mutable QVector<int> m_lineStarts;
	mutable QRect m_boundingRect;
	int m_xStep;
	int m_yStep;
	mutable bool m_dirty;
	mutable int m_segmentCount;
}; /* -----  end of class ImageSegmenter  ----- */

} /* end of namespace FaceDetect */

