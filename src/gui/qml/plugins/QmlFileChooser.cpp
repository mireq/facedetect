/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  01.05.2011 17:47:09
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <QApplication>
#include <QFileDialog>
#include "QmlFileChooser.h"

QmlFileChooser::QmlFileChooser(QObject *parent):
	QObject(parent),
	m_acceptMode(AcceptOpen),
	m_fileMode(AnyFile)
{
}

QmlFileChooser::~QmlFileChooser()
{
}

bool QmlFileChooser::selectFile()
{
	QFileDialog dialog(qApp->activeWindow());
	dialog.setModal(true);
	QFileDialog::AcceptMode am = QFileDialog::AcceptOpen;
	switch (m_acceptMode) {
		case AcceptOpen:
			am = QFileDialog::AcceptOpen;
			break;
		case AcceptSave:
			am = QFileDialog::AcceptSave;
			break;
	}
	QFileDialog::FileMode fm = QFileDialog::AnyFile;
	switch (m_fileMode) {
		case AnyFile:
			fm = QFileDialog::AnyFile;
			break;
		case ExistingFile:
			fm = QFileDialog::ExistingFile;
			break;
		case Directory:
			fm = QFileDialog::Directory;
			break;
	}
	dialog.setAcceptMode(am);
	dialog.setFileMode(fm);
	dialog.setWindowTitle(m_windowTitle);
	dialog.selectFile(m_selectedFile);
	int ret = dialog.exec();
	if (ret == QDialog::Accepted && dialog.selectedFiles().count() > 0) {
		setSelectedFile(dialog.selectedFiles().first());
	}
	if (ret == QDialog::Accepted) {
		return true;
	}
	else {
		return false;
	}
}

QmlFileChooser::AcceptMode QmlFileChooser::acceptMode() const
{
	return m_acceptMode;
}

void QmlFileChooser::setAcceptMode(QmlFileChooser::AcceptMode acceptMode)
{
	m_acceptMode = acceptMode;
}

QmlFileChooser::FileMode QmlFileChooser::fileMode() const
{
	return m_fileMode;
}

void QmlFileChooser::setFileMode(QmlFileChooser::FileMode fileMode)
{
	m_fileMode = fileMode;
}

QString QmlFileChooser::windowTitle() const
{
	return m_windowTitle;
}

void QmlFileChooser::setWindowTitle(const QString &windowTitle)
{
	m_windowTitle = windowTitle;
}

QString QmlFileChooser::selectedFile() const
{
	return m_selectedFile;
}

void QmlFileChooser::setSelectedFile(const QString &selectedFile)
{
	if (selectedFile != m_selectedFile) {
		m_selectedFile = selectedFile;
		emit selectedFileChanged(m_selectedFile);
	}
}

