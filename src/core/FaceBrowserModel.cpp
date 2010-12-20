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
#include "FaceBrowserModel.h"

#include <QDebug>

using FaceDetect::FaceFileReader;

FaceBrowserModel::FaceBrowserModel(QObject *parent):
	QAbstractListModel(parent)
{
	QHash<int, QByteArray> roles = roleNames();
	roles.insert(ImageRole, QByteArray("image"));
	roles.insert(FaceDataRole, QByteArray("faceData"));
	roles.insert(DefinitionFileRole, QByteArray("definitionFile"));
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
	switch (role) {
		case ImageRole:
			return QVariant(QUrl("image://faceimage/" + m_files[index.row()]));
		case FaceDataRole: {
			FaceFileReader reader;
			reader.readFile(m_files[index.row()]);
			QList<QVariant> ret;
			QVector<FaceFileReader::FaceData> data = reader.faceData();
			foreach (const FaceFileReader::FaceData &face, data) {
				QMap<QString, QVariant> faceData;
				faceData["leftEyeX"] = face.leftEye.x();
				faceData["rightEyeX"] = face.rightEye.x();
				faceData["noseX"] = face.nose.x();
				faceData["mouthX"] = face.mouth.x();
				faceData["leftEyeY"] = face.leftEye.y();
				faceData["rightEyeY"] = face.rightEye.y();
				faceData["noseY"] = face.nose.y();
				faceData["mouthY"] = face.mouth.y();
				ret.append(faceData);
			}
			return QVariant(ret);
		}
		case DefinitionFileRole: {
			QFile defFile(m_files[index.row()]);
			defFile.open(QIODevice::ReadOnly);
			return QString::fromUtf8(defFile.readAll());
		}
		default:
			return QVariant();
	}
}

void FaceBrowserModel::addDefinitionFile(const QString &fileName)
{
	beginInsertRows(QModelIndex(), m_files.count(), m_files.count());
	m_files.append(fileName);
	endInsertRows();
}

