/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  20.02.2011 15:27:45
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <cstdio>
#include <QCoreApplication>
#include <QDebug>
#include <QSettings>
#include "ConsoleInterface.h"

ConsoleInterface::ConsoleInterface(QObject *parent):
	QObject(parent),
	m_cout(stdout)
{
}

ConsoleInterface::~ConsoleInterface()
{
}

void ConsoleInterface::run()
{
	QMetaObject::invokeMethod(this, "scanFaces", Qt::QueuedConnection);
	QSettings settings;
	settings.beginGroup("paths");
	m_facesPath = settings.value("faces").toString();
	settings.endGroup();
}

void ConsoleInterface::scanFaces()
{
	m_cout << "Scanning face database " << m_facesPath << "\n";
	m_cout.flush();
	m_faceScanner = QSharedPointer<FaceDetect::FaceFileScanner>(new FaceDetect::FaceFileScanner());
	m_faceScanner->setBasePath(m_facesPath);
	connect(m_faceScanner.data(), SIGNAL(progressChanged(double)), this, SLOT(updateProgress(double)));
	connect(m_faceScanner.data(), SIGNAL(scanningChanged(bool)), this, SLOT(faceScanningStatusChanged(bool)));
	m_faceScanner->start();
}

void ConsoleInterface::faceScanningStatusChanged(bool scanning)
{
	if (!scanning) {
		m_faceScanner = QSharedPointer<FaceDetect::FaceFileScanner>(0);
		qApp->quit();
	}
}

void ConsoleInterface::updateProgress(double progress)
{
	m_cout << "\r" << QString("%1%").arg(progress * 100, 5, 'f', 1);
	m_cout.flush();
}

