/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  19.12.2010 14:09:22
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <QDir>
#include <QFile>
#include "FileScanner.h"

#include <QDebug>

FileScanner::FileScanner(QObject *parent):
	QThread(parent),
	m_scanning(false)
{
	clearState();
	connect(this, SIGNAL(finished()), SLOT(onFinished()));
	connect(this, SIGNAL(terminated()), SLOT(onFinished()));
	connect(this, SIGNAL(started()), SLOT(onStarted()));
}

void FileScanner::setScanPath(const QString &path)
{
	m_scanPath = path;
}

double FileScanner::progress() const
{
	return m_progress;
}

bool FileScanner::isScanning() const
{
	return m_scanning;
}

long FileScanner::scannedDirs() const
{
	return m_scannedDirs;
}

long FileScanner::scannedFiles() const
{
	return m_scannedFiles;
}

long FileScanner::totalDirs() const
{
	return m_totalDirs;
}

long FileScanner::totalFiles() const
{
	return m_totalFiles;
}

void FileScanner::stop()
{
	if (isRunning()) {
		m_stop = true;
		wait();
	}
}

void FileScanner::run()
{
	scanDirectory(m_scanPath, 0.0, 1.0);
}

void FileScanner::onFinished()
{
	if (m_scanning) {
		m_scanning = false;
		emit scanningChanged(m_scanning);
	}
}

void FileScanner::onStarted()
{
	if (!m_scanning) {
		m_scanning = true;
		emit scanningChanged(m_scanning);
	}
}

void FileScanner::scanDirectory(const QString &directory, double progressFrom, double progressTo)
{
	if (m_stop) {
		return;
	}
	QDir dir(directory);
	QList<QFileInfo> dirs = dir.entryInfoList(QDir::Dirs | QDir::Readable | QDir::NoDotAndDotDot, QDir::Name);
	QList<QFileInfo> files = dir.entryInfoList(QDir::Files | QDir::Readable | QDir::NoDotAndDotDot, QDir::Name);

	// Aktualizácia celkového počtu súborov a adresárov
	m_totalDirs += dirs.count();
	m_totalDirs += files.count();
	double progressStep = (progressTo - progressFrom) / (dirs.count() + files.count());

	// Rekurzívne prehľadanie adresárov
	foreach (QFileInfo dirInfo, dirs) {
		scanDirectory(dirInfo.absoluteFilePath(), m_progress, m_progress + progressStep);
		m_scannedDirs++;
	}

	// Spracovanie súborov v aktuálnom adresári
	foreach (QFileInfo fileInfo, files) {
		if (m_stop) {
			return;
		}
		scanFile(fileInfo.absoluteFilePath());
		m_progress += progressStep;
		m_scannedFiles++;
		emit progressChanged(m_progress);
	}
	m_progress = progressTo;
	emit progressChanged(m_progress);
}

void FileScanner::clearState()
{
	m_progress = 0;
	m_scannedDirs = 0;
	m_scannedFiles = 0;
	m_totalDirs = 0;
	m_totalFiles = 0;
	m_stop = false;
}

