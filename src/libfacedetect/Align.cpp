/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  14.12.2010 11:43:12
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <lapackpp/blas3pp.h>
#include <lapackpp/laslv.h>
#include <lapackpp/lavli.h>

#include "Align.h"

#include <iostream>
#include <QDebug>

namespace FaceDetect {

Align::Align():
	m_faceFeaturesSum(sm_faceFeaturesCount * 2),
	m_avgFaceFeatures(sm_faceFeaturesCount * 2),
	m_imgCount(0),
	m_avgDirty(false)
{
	for (int i = 0; i < m_faceFeaturesSum.rows(); ++i) {
		m_faceFeaturesSum(i) = 0;
		m_avgFaceFeatures(i) = 0;
	}
}

Align::~Align()
{
}

void Align::scanImage(const QString &definitionFile)
{
	FaceFileReader reader;
	if (!reader.readFile(definitionFile)) {
		return;
	}

	QVector<FaceFileReader::FaceData> faces = reader.faceData();
	foreach (const FaceFileReader::FaceData &face, faces) {
		// Kointrola, či sú všetky požadované body zadané
		if (!checkControlPoints(face)) {
			continue;
		}
		m_imgCount++;
		if (m_imgCount == 1) {
			m_avgFaceFeatures = getControlPointsVector(face);
			m_faceFeaturesSum = m_avgFaceFeatures;
		}
		else {
			m_avgDirty = true;
			LaGenMatDouble aMatrix(sm_faceFeaturesCount * 2, 4);
			LaColVectorDouble featVector = getControlPointsVector(face);
			for (int row = 0; row < sm_faceFeaturesCount * 2; ++row) {
				aMatrix(row, 0) = featVector(row);
				aMatrix(row, 1) = ((row & 0x01) == 0) ? (featVector(row + 1)) : (-featVector(row - 1));
				aMatrix(row, 2) = ((row & 0x01) == 0) ? 1 : 0;
				aMatrix(row, 3) = ((row & 0x01) == 0) ? 0 : 1;
			}
			LaGenMatDouble nMatrix(4, 4);
			Blas_Mat_Mat_Mult(aMatrix, aMatrix, nMatrix, true);

			LaVectorLongInt pivots(4);
			LUFactorizeIP(nMatrix, pivots);
			LaLUInverseIP(nMatrix, pivots);

			LaGenMatDouble varCovMatrix(4, 1);
			Blas_Mat_Mat_Mult(aMatrix, m_avgFaceFeatures, varCovMatrix, true);
			LaColVectorDouble tVector(4);
			Blas_Mat_Mat_Mult(nMatrix, varCovMatrix, tVector);

			LaColVectorDouble newFeatures(sm_faceFeaturesCount * 2);
			Blas_Mat_Mat_Mult(aMatrix, tVector, newFeatures);
			for (int i = 0; i < m_faceFeaturesSum.rows(); ++i) {
				m_faceFeaturesSum(i) += newFeatures(i);
			}
		}
	}
}

bool Align::checkControlPoints(const FaceFileReader::FaceData &data)
{
	if (data.leftEye != QPoint() && data.rightEye != QPoint() && data.nose != QPoint() && data.mouth != QPoint()) {
		return true;
	}
	else {
		return false;
	}
}

LaColVectorDouble Align::getControlPointsVector(const FaceFileReader::FaceData &data)
{
	LaColVectorDouble ret(sm_faceFeaturesCount * 2);
	ret(0) = data.leftEye.x();
	ret(1) = data.leftEye.y();
	ret(2) = data.rightEye.x();
	ret(3) = data.rightEye.y();
	ret(4) = data.nose.x();
	ret(5) = data.nose.y();
	ret(6) = data.mouth.x();
	ret(7) = data.mouth.y();
	return ret;
}

void Align::calcAvg()
{
	if (m_avgDirty) {
		for (int i = 0; i < m_faceFeaturesSum.rows(); ++i) {
			m_avgFaceFeatures(i) = m_faceFeaturesSum(i) / double(m_imgCount);
		}
		m_avgDirty = false;
	}
}

} /* end of namespace FaceDetect */

