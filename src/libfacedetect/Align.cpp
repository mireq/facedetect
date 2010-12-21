/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  14.12.2010 11:43:12
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <QPainter>
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
	m_normalizedFaceFeatures(sm_faceFeaturesCount * 2),
	m_topLine(4),
	m_imgCount(0),
	m_avgDirty(false),
	m_normalized(false),
	m_collectStatistics(false)
{
	for (int i = 0; i < m_faceFeaturesSum.rows(); ++i) {
		m_faceFeaturesSum(i) = 0;
		m_avgFaceFeatures(i) = 0;
		m_normalizedFaceFeatures(i) = 0;
	}
}

void Align::setCollectStatistics(bool statistics)
{
	m_collectStatistics = statistics;
}

void Align::scanImage(const QString &definitionFile)
{
	FaceFileReader reader;
	if (!reader.readFile(definitionFile)) {
		return;
	}
	scanImage(reader);
}

void Align::scanImage(const FaceFileReader &reader)
{
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
			m_normalized = false;
			LaColVectorDouble featVector = getControlPointsVector(face);
			LaGenMatDouble aMatrix(sm_faceFeaturesCount * 2, 4);
			fillFeaturesMatrix(featVector, aMatrix);
			LaColVectorDouble tVector = getTransformVector(aMatrix, false);
			LaColVectorDouble newFeatures(sm_faceFeaturesCount * 2);
			Blas_Mat_Mat_Mult(aMatrix, tVector, newFeatures);
			for (int i = 0; i < m_faceFeaturesSum.rows(); ++i) {
				m_faceFeaturesSum(i) += newFeatures(i);
			}
			calcAvg();
		}
		if (m_collectStatistics) {
			m_faceData.append(face);
		}
	}
}

std::size_t Align::imgCount() const
{
	return m_imgCount;
}

QTransform Align::getTransform(const FaceFileReader::FaceData &face) const
{
	calcAvg();
	// Kointrola, či sú všetky požadované body zadané
	if (!checkControlPoints(face)) {
		return QTransform();
	}
	LaColVectorDouble featVector = getControlPointsVector(face);
	LaGenMatDouble aMatrix(sm_faceFeaturesCount * 2, 4);
	fillFeaturesMatrix(featVector, aMatrix);
	LaColVectorDouble tVector = getTransformVector(aMatrix, true);
	LaColVectorDouble newFeatures(sm_faceFeaturesCount * 2);
	Blas_Mat_Mat_Mult(aMatrix, tVector, newFeatures);
	return QTransform(tVector(0), tVector(1), -tVector(1), tVector(0), tVector(2), tVector(3));
}

QImage Align::getStatisticsImage() const
{
	normalize();
	LaGenMatDouble imgMatrix(128, 128);
	for (int x = 0; x < 128; ++x) {
		for (int y = 0; y < 128; ++y) {
			imgMatrix(y, x) = 0;
		}
	}
	foreach (const FaceFileReader::FaceData &data, m_faceData) {
		QTransform transform = getTransform(data);
		QPoint point;
		point = transform.map(data.leftEye);
		if (checkPointRange(point)) {
			imgMatrix(point.y(), point.x())++;
		}
		point = transform.map(data.rightEye);
		if (checkPointRange(point)) {
			imgMatrix(point.y(), point.x())++;
		}
		point = transform.map(data.nose);
		if (checkPointRange(point)) {
			imgMatrix(point.y(), point.x())++;
		}
		point = transform.map(data.mouth);
		if (checkPointRange(point)) {
			imgMatrix(point.y(), point.x())++;
		}
	}
	double max = 0;
	for (int x = 0; x < 128; ++x) {
		for (int y = 0; y < 128; ++y) {
			if (imgMatrix(y, x) > max) {
				max = imgMatrix(y, x);
			}
		}
	}
	if (max != 0) {
		for (int x = 0; x < 128; ++x) {
			for (int y = 0; y < 128; ++y) {
				imgMatrix(y, x) = (imgMatrix(y, x) / max) * 256.0;
			}
		}
	}

	QImage image(QSize(128, 128), QImage::Format_ARGB32);
	QPainter painter(&image);
	painter.setPen(Qt::NoPen);
	painter.fillRect(QRect(0, 0, 128, 128), Qt::white);
	painter.end();
	for (int x = 0; x < 128; ++x) {
		for (int y = 0; y < 128; ++y) {
			int value = 255 - qMin(int(imgMatrix(y, x)), 255);
			image.setPixel(x, y, qRgb(value, value, value));
		}
	}
	return image;
}

bool Align::checkControlPoints(const FaceFileReader::FaceData &data) const
{
	if (data.leftEye != QPoint() && data.rightEye != QPoint() && data.nose != QPoint() && data.mouth != QPoint()) {
		return true;
	}
	else {
		return false;
	}
}

LaColVectorDouble Align::getControlPointsVector(const FaceFileReader::FaceData &data) const
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

LaColVectorDouble Align::getTransformVector(const LaGenMatDouble &aMatrix, bool normalized) const
{
	if (normalized) {
		normalize();
	}
	LaGenMatDouble nMatrix(4, 4);
	Blas_Mat_Mat_Mult(aMatrix, aMatrix, nMatrix, true);

	LaVectorLongInt pivots(4);
	LUFactorizeIP(nMatrix, pivots);
	LaLUInverseIP(nMatrix, pivots);

	LaGenMatDouble varCovMatrix(4, 1);
	if (normalized) {
		Blas_Mat_Mat_Mult(aMatrix, m_normalizedFaceFeatures, varCovMatrix, true);
	}
	else {
		Blas_Mat_Mat_Mult(aMatrix, m_avgFaceFeatures, varCovMatrix, true);
	}
	LaColVectorDouble tVector(4);
	Blas_Mat_Mat_Mult(nMatrix, varCovMatrix, tVector);
	return tVector;
}

LaColVectorDouble Align::transform(const LaColVectorDouble &input, const LaColVectorDouble &transVector) const
{
	LaColVectorDouble ret(input.rows());
	LaGenMatDouble aMatrix(input.rows(), 4);
	fillFeaturesMatrix(input, aMatrix);
	Blas_Mat_Mat_Mult(aMatrix, transVector, ret);
	return ret;
}

void Align::fillFeaturesMatrix(const LaColVectorDouble &input, LaGenMatDouble &aMatrix) const
{
	for (int row = 0; row < input.rows(); ++row) {
		aMatrix(row, 0) = input(row);
		aMatrix(row, 1) = ((row & 0x01) == 0) ? (-input(row + 1)) : (input(row - 1));
		aMatrix(row, 2) = ((row & 0x01) == 0) ? 1 : 0;
		aMatrix(row, 3) = ((row & 0x01) == 0) ? 0 : 1;
	}
}

void Align::calcAvg() const
{
	if (m_avgDirty) {
		for (int i = 0; i < m_faceFeaturesSum.rows(); ++i) {
			m_avgFaceFeatures(i) = m_faceFeaturesSum(i) / double(m_imgCount);
		}
		m_avgDirty = false;
	}
}

void Align::normalize() const
{
	if (!m_normalized) {
		// Výpočet vrchnej čiary pre orezávanie fotografií
		double tX = 0;
		double tY = 0;
		for (int i = 0; i < 4; ++i) {
			tX += m_avgFaceFeatures(i*2);
			tY += m_avgFaceFeatures(i*2 + 1);
		}
		tX /= 4.0;
		tY /= 4.0;
		double diffX = 0;
		double diffY = 0;
		diffX = (m_avgFaceFeatures(0) - tX) * 1.5;
		diffY = (m_avgFaceFeatures(1) - tY) * 1.5;
		m_topLine(0) = tX + diffX;
		m_topLine(1) = tY + diffY;
		diffX = (m_avgFaceFeatures(2) - tX) * 1.5;
		diffY = (m_avgFaceFeatures(3) - tY) * 1.5;
		m_topLine(2) = tX + diffX;
		m_topLine(3) = tY + diffY;

		// Prepočet vlastností po normalizácii
		LaColVectorDouble transformedLine(4);
		transformedLine(0) = sm_lineSize;
		transformedLine(1) = 0;
		transformedLine(2) = 0;
		transformedLine(3) = 0;
		LaGenMatDouble aMatrix(4, 4);
		fillFeaturesMatrix(m_topLine, aMatrix);
		LaColVectorDouble tVector(4);
		LaLinearSolve(aMatrix, tVector, transformedLine);
		m_normalizedFaceFeatures = transform(m_avgFaceFeatures, tVector);

		m_normalized = true;
	}
}

bool Align::checkPointRange(const QPoint &point) const
{
	if (point.x() < 0 || point.y() < 0) {
		return false;
	}
	if (point.x() >= 128 || point.y() >= 128) {
		return false;
	}
	return true;
}

} /* end of namespace FaceDetect */

