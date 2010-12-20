/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  07.12.2010 08:54:55
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <QUrl>
#include "FaceBrowserModel.h"

FaceBrowserModel::FaceBrowserModel(QObject *parent):
	QAbstractListModel(parent)
{
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
		case Qt::DisplayRole:
			return QVariant(QUrl("image://faceimage/" + m_files[index.row()]));
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

