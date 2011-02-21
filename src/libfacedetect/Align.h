/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  14.12.2010 11:43:15
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef ALIGN_L4LZ6THZ
#define ALIGN_L4LZ6THZ

#include <QObject>
#include <QString>
#include <QTransform>
#include <QVector>
#include <lapackpp/gmd.h>
#include <lapackpp/lavd.h>
#include <cstdlib>
#include "FaceFileScanner.h"

namespace FaceDetect {

class Align: public QObject
{
Q_OBJECT
public:
	explicit Align(QObject *parent = 0);
	virtual ~Align();
	void setCollectStatistics(bool statistics);
	void setImageSize(int imageSize);
	void addImage(const FaceFileScanner::ImageInfo &info);
	std::size_t imgCount() const;
	QTransform getTransform(const FaceFileScanner::FaceData &face) const;
	QImage getStatisticsImage() const;

private:
	bool checkControlPoints(const FaceFileScanner::FaceData &data) const;
	LaVectorDouble getControlPointsVector(const FaceFileScanner::FaceData &data) const;
	LaVectorDouble getTransformVector(const LaGenMatDouble &aMatrix, bool normalized = true) const;
	LaVectorDouble transform(const LaVectorDouble &input, const LaVectorDouble &transVector) const;
	void fillFeaturesMatrix(const LaVectorDouble &input, LaGenMatDouble &aMatrix) const;
	void calcAvg() const;
	void normalize() const;
	bool checkPointRange(const QPoint &point) const;

private:
	LaVectorDouble m_faceFeaturesSum;
	mutable LaVectorDouble m_avgFaceFeatures;
	mutable LaVectorDouble m_normalizedFaceFeatures;
	mutable LaVectorDouble m_topLine;
	std::size_t m_imgCount;
	mutable bool m_avgDirty;
	mutable bool m_normalized;
	bool m_collectStatistics;
	int m_imageSize;
	QVector<FaceFileScanner::FaceData> m_faceData;

	static const int sm_faceFeaturesCount = 4;
}; /* -----  end of class Align  ----- */

} /* end of namespace FaceDetect */

#endif /* end of include guard: ALIGN_L4LZ6THZ */

