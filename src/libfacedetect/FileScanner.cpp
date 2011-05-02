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
#include <QMutexLocker>
#include "FileScanner.h"

#include <QDebug>
#include <unistd.h>

FileScanner::FileScanner(QObject *parent):
	QThread(parent),
	m_scanning(false)
{
	clearState();
	connect(this, SIGNAL(finished()), SLOT(onFinished()));
	connect(this, SIGNAL(terminated()), SLOT(onFinished()));
	connect(this, SIGNAL(started()), SLOT(onStarted()));
}

FileScanner::~FileScanner()
{
}

void FileScanner::setScanPath(const QString &path)
{
	stop();
	clearState();
	if (m_scanPath != path) {
		m_scanPath = path;
		emit scanPathChanged(m_scanPath);
	}
}

QString FileScanner::scanPath() const
{
	return m_scanPath;
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
	m_stopMutex.lock();
	m_stop = true;
	m_stopMutex.unlock();
	wait();
}

void FileScanner::run()
{
	{
		QMutexLocker lock(&m_stopMutex);
		m_stop = false;
	}
	if (m_scanStack.isEmpty()) {
		clearState();
		ScanStackItem item;
		item.directory = m_scanPath;
		item.progressMin = 0;
		item.progressMax = 1;
		item.dirCount = 0;
		item.fileCount = 0;
		item.scanned = false;
		m_scanStack.append(item);
	}
	scan();
	{
		QMutexLocker lock(&m_stopMutex);
		m_stop = false;
	}
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

void FileScanner::scan()
{
	{
		QMutexLocker lock(&m_stopMutex);
		if (m_stop) {
			return;
		}
	}

	if (m_scanStack.isEmpty() || (m_scanStack.first().files.isEmpty() && m_scanStack.first().scanned)) {
		return;
	}

	while (!m_scanStack.empty()) {
		{
			QMutexLocker lock(&m_stopMutex);
			if (m_stop) {
				return;
			}
		}
		auto stackItem = m_scanStack.begin();

		QList<QFileInfo> dirs;
		if (!stackItem->scanned) {
			// Vytvorenie zoznamu adresárov a súborov
			QDir dir(stackItem->directory);
			dirs = dir.entryInfoList(QDir::Dirs | QDir::Readable | QDir::NoDotAndDotDot, QDir::Name);
			stackItem->files = dir.entryInfoList(QDir::Files | QDir::Readable | QDir::NoDotAndDotDot, QDir::Name);
			stackItem->scanned = true;
			stackItem->dirCount = dirs.count();
			stackItem->fileCount = stackItem->files.count();

			// Aktualizácia celkového počtu súborov a adresárov
			m_totalDirs += dirs.count();
			m_totalFiles += stackItem->files.count();
		}

		// Výpočet zmen priebehu pre každú položku v zozname súborov
		double progressStep = (stackItem->progressMax - stackItem->progressMin) / (stackItem->dirCount + stackItem->fileCount);

		double progressNext = m_progress;
		// Rekurzívne prehľadanie adresárov
		foreach (QFileInfo dirInfo, dirs) {
			ScanStackItem item;
			item.directory = dirInfo.absoluteFilePath();
			item.progressMin = progressNext;
			progressNext += progressStep;
			item.progressMax = progressNext;
			item.scanned = false;
			item.dirCount = 0;
			item.fileCount = 0;
			m_scanStack.append(item);
		}
		stackItem = m_scanStack.begin();

		// Spracovanie súborov v aktuálnom adresári
		while (!stackItem->files.isEmpty()) {
			{
				QMutexLocker lock(&m_stopMutex);
				if (m_stop) {
					return;
				}
			}
			QFileInfo fileInfo = stackItem->files.takeFirst();
			scanFile(fileInfo.absoluteFilePath());
			m_progress += progressStep;
			m_scannedFiles++;
			emit progressChanged(m_progress);
		}

		m_scannedDirs++;
		m_scanStack.removeFirst();
		usleep(1000);
	}

	if (m_scanStack.isEmpty()) {
		ScanStackItem item;
		item.directory = m_scanPath;
		item.progressMin = 0;
		item.progressMax = 1;
		item.dirCount = 0;
		item.fileCount = 0;
		item.scanned = true;
		m_scanStack.append(item);
	}
}

void FileScanner::clearState()
{
	m_progress = 0;
	m_scannedDirs = 0;
	m_scannedFiles = 0;
	m_totalDirs = 1;
	m_totalFiles = 0;
	m_scanStack.clear();
	QMutexLocker lock(&m_stopMutex);
	m_stop = false;
	emit progressChanged(m_progress);
}

