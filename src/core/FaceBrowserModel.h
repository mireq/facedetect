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
#include "libfacedetect/FaceFileScanner.h"

/*
 * \brief Model pre prechádzanie obrázkov tvárí.
 */
class FaceBrowserModel: public QAbstractListModel
{
Q_OBJECT
public:
	explicit FaceBrowserModel(FaceDetect::Align *aligner, QObject *parent = 0);
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QImage getStatisticsImage() const;
	QTransform getTransform(const FaceDetect::FaceFileScanner::FaceData &data) const;
	void addDefinitionFile(const FaceDetect::FaceFileScanner::ImageInfo &image);

private:
	/*! \cond PRIVATE */
	enum DataRole {
		ImageRole = Qt::UserRole + 1,
		FaceDataRole,
		DefinitionFileRole,
		TransformRotateRole,
		TransformScaleRole,
		TransformTranslateXRole,
		TransformTranslateYRole
	};

	QVector<FaceDetect::FaceFileScanner::ImageInfo> m_files;
	FaceDetect::Align *m_aligner;
	/* \endcond */
}; /* -----  end of class FaceBrowserModel  ----- */

#endif /* end of include guard: FACEBROWSERMODEL_XWF44CQJ */

