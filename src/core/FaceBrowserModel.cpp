/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  07.12.2010 08:54:55
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <QFile>
#include <QUrl>
#include <qmath.h>
#include "FaceBrowserModel.h"

FaceBrowserModel::FaceBrowserModel(FaceDetect::Align *aligner, QObject *parent):
	QAbstractListModel(parent),
	m_aligner(aligner)
{
	QHash<int, QByteArray> roles = roleNames();
	roles.insert(ImageRole, QByteArray("image"));
	roles.insert(FaceDataRole, QByteArray("faceData"));
	roles.insert(DefinitionFileRole, QByteArray("definitionFile"));
	roles.insert(TransformRotateRole, QByteArray("transformRotate"));
	roles.insert(TransformScaleRole, QByteArray("transformScale"));
	roles.insert(TransformTranslateXRole, QByteArray("transformTranslateX"));
	roles.insert(TransformTranslateYRole, QByteArray("transformTranslateY"));
	setRoleNames(roles);
}

int FaceBrowserModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid()) {
		return 0;
	}
	else {
		return m_files.count();
	}
}

QVariant FaceBrowserModel::data(const QModelIndex &index, int role) const
{
	if (index.parent().isValid()) {
		return QVariant();
	}
	if (index.row() < 0 || index.row() >= m_files.count()) {
		return QVariant();
	}
	FaceDetect::FaceFileScanner::ImageInfo img = m_files[index.row()];
	switch (role) {
		case ImageRole:
			return QVariant(m_files[index.row()].definitionUrl().toLocalFile());
		case FaceDataRole: {
			QList<QVariant> ret;
			for (auto face = img.faceBegin(); face != img.faceEnd(); ++face) {
				QMap<QString, QVariant> faceData;
				faceData["leftEyeX"] = face->leftEye.x();
				faceData["rightEyeX"] = face->rightEye.x();
				faceData["noseX"] = face->nose.x();
				faceData["mouthX"] = face->mouth.x();
				faceData["leftEyeY"] = face->leftEye.y();
				faceData["rightEyeY"] = face->rightEye.y();
				faceData["noseY"] = face->nose.y();
				faceData["mouthY"] = face->mouth.y();
				ret.append(faceData);
			}
			return QVariant(ret);
		}
		case DefinitionFileRole: {
			QFile defFile(m_files[index.row()].definitionUrl().toLocalFile());
			defFile.open(QIODevice::ReadOnly);
			return QString::fromUtf8(defFile.readAll());
		}
		case TransformRotateRole: {
			if (img.faceEnd() - img.faceBegin() == 1) {
				QTransform transform = getTransform(*img.faceBegin());
				QLineF line(transform.map(QPointF(0, 0)), transform.map(QPointF(1, 0)));
				return -line.angle();
			}
			break;
		}
		case TransformScaleRole: {
			if (img.faceEnd() - img.faceBegin() == 1) {
				QTransform transform = getTransform(*img.faceBegin());
				QLineF line(transform.map(QPointF(0, 0)), transform.map(QPointF(1, 0)));
				return line.length();
			}
			break;
		}
		case TransformTranslateXRole:
			if (img.faceEnd() - img.faceBegin() == 1) {
				QTransform transform = getTransform(*img.faceBegin());
				return transform.dx();
			}
			break;
		case TransformTranslateYRole:
			if (img.faceEnd() - img.faceBegin() == 1) {
				QTransform transform = getTransform(*img.faceBegin());
				return transform.dy();
			}
			break;
		default:
			return QVariant();
	}
	return QVariant();
}

QTransform FaceBrowserModel::getTransform(const FaceDetect::FaceFileScanner::FaceData &data) const
{
	return m_aligner->getTransform(data);
}

void FaceBrowserModel::addDefinitionFile(const FaceDetect::FaceFileScanner::ImageInfo &image)
{
	beginInsertRows(QModelIndex(), m_files.count(), m_files.count());
	m_files.append(image);
	endInsertRows();
}

