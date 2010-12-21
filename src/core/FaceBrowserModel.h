/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  07.12.2010 08:55:12
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef FACEBROWSERMODEL_XWF44CQJ
#define FACEBROWSERMODEL_XWF44CQJ

#include <QAbstractListModel>
#include <QVector>
#include "libfacedetect/Align.h"
#include "libfacedetect/FaceFileReader.h"

class FaceBrowserModel: public QAbstractListModel
{
Q_OBJECT
public:
	explicit FaceBrowserModel(QObject *parent = 0);
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QImage getStatisticsImage() const;
	QTransform getTransform(const FaceDetect::FaceFileReader::FaceData &data) const;

public slots:
	void addDefinitionFile(const QString &fileName);

private:
	enum DataRole {
		ImageRole = Qt::UserRole + 1,
		FaceDataRole,
		DefinitionFileRole,
		TransformRotateRole,
		TransformScaleRole,
		TransformTranslateXRole,
		TransformTranslateYRole
	};

	QVector<QString> m_files;
	FaceDetect::Align m_aligner;
}; /* -----  end of class FaceBrowserModel  ----- */

#endif /* end of include guard: FACEBROWSERMODEL_XWF44CQJ */

