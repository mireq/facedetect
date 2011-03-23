/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  21.02.2011 08:24:20
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <algorithm>
#include <QPainter>
#include <QPixmap>
#include <QTransform>
#include "TrainingImageDatabase.h"

namespace FaceDetect {

/**
 * Vytvorenie trénovacej databázy.
 */
TrainingImageDatabase::TrainingImageDatabase(QObject *parent):
	TrainingDataReader(parent),
	m_aligner(new Align(this))
{
	m_aligner->setImageSize(128);
	m_aligner->setCollectStatistics(false);
	m_imageFilter.setFilters(FaceDetect::ImageFilter::GrayscaleFilter);
}

TrainingImageDatabase::~TrainingImageDatabase()
{
}

int TrainingImageDatabase::inputVectorSize() const
{
	return InputVecotrSize;
}

int TrainingImageDatabase::outputVectorSize() const
{
	return 1;
}

std::size_t TrainingImageDatabase::trainingSetSize() const
{
	return m_samples.size();
}

LaVectorDouble TrainingImageDatabase::inputVector(std::size_t sample) const
{
	if (!m_workingImage.isNull()) {
		m_workingImage = QImage();
	}
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

/**
 * Pridanie obrázku \a image do množiny trénovacích vzorov.
 */
void TrainingImageDatabase::addImage(const FaceDetect::FaceFileScanner::ImageInfo &image)
{
	if (!image.isValid()) {
		return;
	}

	m_aligner->addImage(image);

	// Viacej než 1 tvár v súbore
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

/**
 * Pridanie trénovacieho vzoru priamo ako dvojicu: vstupný vektor, očakávaný
 * výstup.
 */
void TrainingImageDatabase::addImage(const LaVectorDouble &input, const LaVectorDouble &output)
{
	TrainingSample sample;
	sample.input = input;
	sample.output = output;
	m_samples.append(sample);
}

/**
 * Premiešanie vstupov.
 */
void TrainingImageDatabase::shuffle()
{
	std::random_shuffle(m_samples.begin(), m_samples.end());
}

/**
 * Prevod obrázku vo vzorke \a sample na vektor. Výstup sa zapisuje do premennej
 * m_samples.
 */
inline void TrainingImageDatabase::calcVectors(std::size_t sample) const
{
	if (m_workingImage.isNull()) {
		m_workingImage = QImage(QSize(128, 128), QImage::Format_RGB888);
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
		m_workingImage = imageInfo.getImage().scaled(QSize(ImageWidth, ImageHeight));
	}

	m_samples[sample].input = LaVectorDouble(InputVecotrSize);
	m_samples[sample].output = LaVectorDouble(1);
	m_samples[sample].info.clear();

	QImage smallImage = m_imageFilter.filterImage(m_workingImage.scaled(QSize(ImageWidth, ImageHeight), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
	for (int pixel = 0; pixel < InputVecotrSize; ++pixel) {
		m_samples[sample].input(pixel) = static_cast<double>(smallImage.pixelIndex(pixel % ImageWidth, pixel / ImageWidth)) / 256;
	}
	m_samples[sample].output(0) = hasFace;
}

} /* end of namespace FaceDetect */

