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

/**
 * \brief Trieda pre výpočet hodnôt transformačnej matice na zarovnanie tvárí
 */
class Align: public QObject
{
Q_OBJECT
public:
	explicit Align(QObject *parent = 0);
	virtual ~Align();
	void setCollectStatistics(bool statistics);
	void setImageSize(int imageSize);
	void addImage(const FaceFileScanner::ImageInfo &info);
	std::size_t faceCount() const;
	QTransform getTransform(const FaceFileScanner::FaceData &face) const;
	QImage getStatisticsImage() const;

private:
	bool checkControlPoints(const FaceFileScanner::FaceData &data) const;
	LaVectorDouble getControlPointsVector(const FaceFileScanner::FaceData &data) const;
	LaVectorDouble getTransformVector(const LaGenMatDouble &aMatrix, bool normalized = true) const;
	LaVectorDouble transform(const LaVectorDouble &input, const LaVectorDouble &transVector) const;
	LaGenMatDouble fillFeaturesMatrix(const LaVectorDouble &input) const;
	void calcAvg() const;
	void normalize() const;
	bool checkPointRange(const QPoint &point) const;

private:
	/// Súčet kontrolných bodov (x1, y1, x2 ...) používaný pre výpočet priemeru.
	LaVectorDouble m_faceFeaturesSum;
	/// Priemerné hodnoty kontrolných bodov, vypočítajú sa volaním calcAvg().
	mutable LaVectorDouble m_avgFaceFeatures;
	/// Hodnoty kontrolných bodov po normalizácii, vypočítajú sa volaním normalize().
	mutable LaVectorDouble m_normalizedFaceFeatures;
	/// Celkový počet korektných tvárí.
	std::size_t m_faceCount;
	/// Pomocná premenná aby sa pri volaní normalize() neprepočítavali hodnoty znovu.
	mutable bool m_normalized;
	/// Záznam štatistík.
	bool m_collectStatistics;
	/// Cieľová veľkosť hrany obrázku po transformácií.
	int m_imageSize;
	/// Dáta používané pri generovaní štatistík.
	QVector<FaceFileScanner::FaceData> m_faceData;

	/// Počet kontrolných bodov.
	static const int FaceFeaturesCount = 4;
}; /* -----  end of class Align  ----- */

} /* end of namespace FaceDetect */

#endif /* end of include guard: ALIGN_L4LZ6THZ */

