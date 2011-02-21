/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  21.02.2011 08:24:20
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <QDebug>
#include <QPainter>
#include <QPixmap>
#include <QTransform>
#include "TrainingImageDatabase.h"

namespace FaceDetect {

TrainingImageDatabase::TrainingImageDatabase(QObject *parent):
	TrainingDataReader(parent),
	m_aligner(new Align(this))
{
	m_aligner->setImageSize(128);
	m_aligner->setCollectStatistics(false);
}

TrainingImageDatabase::~TrainingImageDatabase()
{
}

std::size_t TrainingImageDatabase::inputVectorSize() const
{
	return sm_inputVectorSize;
}

std::size_t TrainingImageDatabase::outputVectorSize() const
{
	return 1;
}

std::size_t TrainingImageDatabase::trainingSetSize() const
{
	return m_samples.size();
}

LaVectorDouble TrainingImageDatabase::inputVector(std::size_t sample) const
{
	if (m_samples[sample].info != 0) {
		calcVectors(sample);
	}
	return m_samples[sample].input;
}

LaVectorDouble TrainingImageDatabase::outputVector(std::size_t sample) const
{
	if (m_samples[sample].info != 0) {
		calcVectors(sample);
	}
	return m_samples[sample].output;
}

void TrainingImageDatabase::addImage(const FaceDetect::FaceFileScanner::ImageInfo &image)
{
	if (!image.isValid()) {
		return;
	}
	m_aligner->addImage(image);
	if (image.faceEnd() - image.faceBegin() >= 2) {
		auto images = image.splitFaces();
		foreach (const FaceFileScanner::ImageInfo &img, images) {
			TrainingSample sample;
			sample.info = QSharedPointer<FaceFileScanner::ImageInfo>(new FaceFileScanner::ImageInfo(img));
			m_samples.append(sample);
		}
	}
	else {
		TrainingSample sample;
		sample.info = QSharedPointer<FaceFileScanner::ImageInfo>(new FaceFileScanner::ImageInfo(image));
		m_samples.append(sample);
	}
}

inline void TrainingImageDatabase::calcVectors(std::size_t sample) const
{
	if (m_workingImage.isNull()) {
		m_workingImage = QImage(QSize(128, 128), QImage::Format_RGB888);
		QVector<QRgb> colorTable(256);
		for (int i = 0; i < 256; ++i) {
			colorTable[i] = qRgb(i, i, i);
		}
		m_colorTable = colorTable;
	}


	FaceFileScanner::ImageInfo imageInfo = *(m_samples[sample].info);
	bool hasFace = false;
	if (imageInfo.faceEnd() - imageInfo.faceBegin() == 1) {
		hasFace = true;
		QTransform transform = m_aligner->getTransform(*imageInfo.faceBegin());
		QPainter painter(&m_workingImage);
		painter.setTransform(transform);
		painter.drawImage(QPoint(0, 0), imageInfo.getImage());
	}
	else {
		m_workingImage = imageInfo.getImage().scaled(QSize(sm_imageWidth, sm_imageHeight));
	}

	m_samples[sample].input = LaVectorDouble(sm_inputVectorSize);
	m_samples[sample].output = LaVectorDouble(1);
	m_samples[sample].info.clear();

	QImage smallImage = m_workingImage.scaled(QSize(sm_imageWidth, sm_imageHeight), Qt::IgnoreAspectRatio, Qt::SmoothTransformation).convertToFormat(QImage::Format_Indexed8, m_colorTable);
	for (std::size_t pixel = 0; pixel < sm_inputVectorSize; ++pixel) {
		m_samples[sample].input(pixel) = static_cast<double>(smallImage.pixelIndex(pixel % sm_imageWidth, pixel / sm_imageWidth)) / 256;
	}
	m_samples[sample].output(0) = hasFace;
}

} /* end of namespace FaceDetect */

