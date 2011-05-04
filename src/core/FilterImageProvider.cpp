/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  04.05.2011 16:10:02
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <QBuffer>
#include <QDataStream>
#include <QVariant>
#include <boost/math/constants/constants.hpp>
#include "FilterImageProvider.h"

#include <QDebug>
FilterImageProvider::FilterImageProvider():
	QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
{
}

FilterImageProvider::~FilterImageProvider()
{
}

QImage FilterImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
	Q_UNUSED(size);
	Q_UNUSED(requestedSize);
	int separatorPos = id.indexOf("|");
	if (separatorPos < 0) {
		return QImage(":/qml/img/photo.png", "PNG");
	}

	// Rozdelenie na časť filtra a cestu
	QString filter = id.left(separatorPos);
	QString path = id.right(id.length() - separatorPos - 1);
	if (filter.isEmpty()) {
		return QImage(":/qml/img/photo.png", "PNG");
	}

	QByteArray filterData = QByteArray::fromBase64(filter.toLatin1());
	QVariantMap filters;
	{
		QBuffer buffer(&filterData);
		buffer.open(QIODevice::ReadOnly);
		QDataStream stream(&buffer);
		stream >> filters;
	}

	// Prevod do odtieňov šedej
	{
		QVariantMap settings = filters["grayscale"].value<QVariantMap>();
		if (settings["enabled"].value<bool>()) {
			m_filter.enableGrayscaleFilter();
		}
		else {
			m_filter.disableGrayscaleFilter();
		}
	}
	// Korekcia osvetlenia
	{
		QVariantMap settings = filters["illumination"].value<QVariantMap>();
		if (settings["enabled"].value<bool>()) {
			m_filter.enableIlluminationFilter();
			m_filter.setIlluminationPlaneOnly(settings["illuminationPlaneOnly"].value<bool>());
			m_filter.setIlluminationCorrectHistogram(settings["illuminationCorrectHistogram"].value<bool>());
		}
		else {
			m_filter.disableIlluminationFilter();
		}
	}
	// Sobelov filter
	{
		QVariantMap settings = filters["sobel"].value<QVariantMap>();
		if (settings["enabled"].value<bool>()) {
			m_filter.enableSobelFilter();
		}
		else {
			m_filter.disableSobelFilter();
		}
	}
	// Gaborov filter
	{
		QVariantMap settings = filters["gabor"].value<QVariantMap>();
		if (settings["enabled"].value<bool>()) {
			m_filter.enableGaborFilter();
			QVariantList gaborFilters = settings["filters"].value<QVariantList>();
			QList<FaceDetect::GaborFilter::GaborParameters> parameters;
			foreach (const QVariant &gaborFilterVar, gaborFilters) {
				QVariantMap gaborFilter = gaborFilterVar.value<QVariantMap>();
				FaceDetect::GaborFilter::GaborParameters gaborParameters;
				gaborParameters.lambda = gaborFilter["lambda"].value<double>();
				gaborParameters.theta = gaborFilter["theta"].value<double>() / 180.0 * boost::math::constants::pi<double>();
				gaborParameters.psi = gaborFilter["psi"].value<double>();
				gaborParameters.sigma = gaborFilter["sigma"].value<double>();
				gaborParameters.gamma = gaborFilter["gamma"].value<double>();
				gaborParameters.lum = gaborFilter["lum"].value<double>();
				parameters << gaborParameters;
			}
			m_filter.setGaborParameters(parameters);
		}
		else {
			m_filter.disableGaborFilter();
		}
	}

	QImage image(path);
	if (image.isNull()) {
		return QImage(":/qml/img/photo.png", "PNG");
	}

	m_filter.filter(image);
	return image;
}

