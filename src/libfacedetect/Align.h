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

#include <QString>
#include <QTransform>
#include <QVector>
#include <lapackpp/gmd.h>
#include <lapackpp/lacvd.h>
#include <cstdlib>
#include "FaceFileReader.h"

namespace FaceDetect {

class Align
{
public:
	Align();
	void setCollectStatistics(bool statistics);
	void scanImage(const QString &definitionFile);
	void scanImage(const FaceFileReader &reader);
	std::size_t imgCount() const;
	QTransform getTransform(const FaceFileReader::FaceData &face) const;
	QImage getStatisticsImage() const;

private:
	bool checkControlPoints(const FaceFileReader::FaceData &data) const;
	LaColVectorDouble getControlPointsVector(const FaceFileReader::FaceData &data) const;
	LaColVectorDouble getTransformVector(const LaGenMatDouble &aMatrix, bool normalized = true) const;
	LaColVectorDouble transform(const LaColVectorDouble &input, const LaColVectorDouble &transVector) const;
	void fillFeaturesMatrix(const LaColVectorDouble &input, LaGenMatDouble &aMatrix) const;
	void calcAvg() const;
	void normalize() const;
	bool checkPointRange(const QPoint &point) const;

private:
	LaColVectorDouble m_faceFeaturesSum;
	mutable LaColVectorDouble m_avgFaceFeatures;
	mutable LaColVectorDouble m_normalizedFaceFeatures;
	mutable LaColVectorDouble m_topLine;
	std::size_t m_imgCount;
	mutable bool m_avgDirty;
	mutable bool m_normalized;
	bool m_collectStatistics;
	QVector<FaceFileReader::FaceData> m_faceData;

	static const int sm_faceFeaturesCount = 4;
	static const int sm_lineSize = 128;
}; /* -----  end of class Align  ----- */

} /* end of namespace FaceDetect */

#endif /* end of include guard: ALIGN_L4LZ6THZ */

