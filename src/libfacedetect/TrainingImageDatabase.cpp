/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  21.02.2011 08:24:20
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <algorithm>
#include <QCryptographicHash>
#include <QDir>
#include <QFileInfo>
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
	m_imageFilter.setFilters(FaceDetect::ImageFilter::GrayscaleFilter | FaceDetect::ImageFilter::IlluminationFilter);
	//m_imageFilter.setFilters(FaceDetect::ImageFilter::GrayscaleFilter);
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
			if (sample.info->isValid()) {
				m_samples.append(sample);
			}
		}
	}
	else if (image.faceEnd() - image.faceBegin() == 1) {
		TrainingSample sample;
		sample.info = QSharedPointer<FaceFileScanner::ImageInfo>(new FaceFileScanner::ImageInfo(image));
		if (sample.info->isValid()) {
			m_samples.append(sample);
		}
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
 * Nastavenie adresára, v ktorom sa budú budú ukladať cachované spracované
 * fotografie.
 */
void TrainingImageDatabase::setCacheDir(const QString &dir)
{
	m_cacheDir = dir;
	if (!m_cacheDir.isEmpty()) {
		QFileInfo info(m_cacheDir);
		if (!info.exists()) {
			QDir directory(dir);
			if (!directory.mkpath(m_cacheDir)) {
				m_cacheDir = QString();
				return;
			}
		}
		else if (!info.isDir() || !info.isReadable() || !info.isWritable()) {
			m_cacheDir = QString();
			return;
		}
	}
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
		QByteArray cacheFileName;
		bool loaded = false;
		if (!m_cacheDir.isEmpty()) {
			cacheFileName = m_cacheDir.toUtf8() + "/" + QCryptographicHash::hash(imageInfo.url().toLocalFile().toAscii(), QCryptographicHash::Md4).toHex();
			QImage loadedImg(cacheFileName.constData(), "PNG");
			if (!loadedImg.isNull()) {
				m_workingImage = loadedImg;
				loaded = true;
			}
		}
		if (!loaded) {
			QTransform transform = m_aligner->getTransform(*imageInfo.faceBegin());
			QPainter painter(&m_workingImage);
			painter.setTransform(transform);
			painter.drawImage(QPoint(0, 0), imageInfo.getImage());
			painter.end();
			if (!m_cacheDir.isEmpty()) {
				m_workingImage.save(cacheFileName.constData(), "PNG");
			}
		}
	}
	else {
		m_workingImage = imageInfo.getImage().scaled(QSize(ImageWidth, ImageHeight));
	}

	m_samples[sample].info.clear();
	m_samples[sample].input = m_imageFilter.filterVector(m_workingImage.scaled(QSize(ImageWidth, ImageHeight), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
	m_samples[sample].output = LaVectorDouble(1);
	m_samples[sample].output(0) = hasFace;

	static int i = 0;
	i++;
	m_imageFilter.filterImage(m_workingImage.scaled(QSize(ImageWidth, ImageHeight), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)).save(QString("/dev/shm/pict%1.png").arg(i), "PNG");
}

} /* end of namespace FaceDetect */

