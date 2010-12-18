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

class FaceBrowserModel: public QAbstractListModel
{
Q_OBJECT
public:
	FaceBrowserModel(QObject *parent = 0);
	~FaceBrowserModel();
	void addDefinitionFile(const QString &fileName);
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

private:
	QVector<QString> m_files;
}; /* -----  end of class FaceBrowserModel  ----- */

#endif /* end of include guard: FACEBROWSERMODEL_XWF44CQJ */

