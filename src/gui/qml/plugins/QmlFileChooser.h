/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  01.05.2011 17:47:11
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef QMLFILECHOOSER_AGL7EE6Y
#define QMLFILECHOOSER_AGL7EE6Y

#include <QObject>

class QmlFileChooser: public QObject
{
Q_OBJECT
Q_PROPERTY(AcceptMode acceptMode READ acceptMode WRITE setAcceptMode);
Q_PROPERTY(FileMode fileMode READ fileMode WRITE setFileMode);
Q_PROPERTY(QString windowTitle READ windowTitle WRITE setWindowTitle);
Q_PROPERTY(QString selectedFile READ selectedFile WRITE setSelectedFile NOTIFY selectedFileChanged);
public:
	explicit QmlFileChooser(QObject *parent = 0);
	virtual ~QmlFileChooser();
	Q_INVOKABLE void selectFile();

	enum AcceptMode {
		AcceptOpen,
		AcceptSave
	};
	enum FileMode {
		AnyFile,
		ExistingFile,
		Directory
	};
	Q_ENUMS(AcceptMode);
	Q_ENUMS(FileMode);

	QmlFileChooser::AcceptMode acceptMode() const;
	void setAcceptMode(QmlFileChooser::AcceptMode acceptMode);
	QmlFileChooser::FileMode fileMode() const;
	void setFileMode(QmlFileChooser::FileMode fileMode);
	QString windowTitle() const;
	void setWindowTitle(const QString &windowTitle);
	QString selectedFile() const;
	void setSelectedFile(const QString &selectedFile);

signals:
	void selectedFileChanged(const QString &selectedFile);

private:
	AcceptMode m_acceptMode;
	FileMode m_fileMode;
	QString m_windowTitle;
	QString m_selectedFile;
}; /* -----  end of class QmlFileChooser  ----- */

#endif /* end of include guard: QMLFILECHOOSER_AGL7EE6Y */

