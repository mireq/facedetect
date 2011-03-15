/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  19.12.2010 14:09:24
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef FILESCANNER_BRQV7MA7
#define FILESCANNER_BRQV7MA7

#include <QThread>

class FileScanner: public QThread
{
Q_OBJECT
Q_PROPERTY(QString scanPath READ scanPath WRITE setScanPath NOTIFY scanPathChanged)
Q_PROPERTY(double progress READ progress NOTIFY progressChanged)
Q_PROPERTY(bool scanning READ isScanning NOTIFY scanningChanged)
Q_PROPERTY(long scannedDirs READ scannedDirs NOTIFY progressChanged)
Q_PROPERTY(long scannedFiles READ scannedFiles NOTIFY progressChanged)
Q_PROPERTY(long totalDirs READ totalDirs NOTIFY progressChanged)
Q_PROPERTY(long totalFiles READ totalDirs NOTIFY progressChanged)
public:
	explicit FileScanner(QObject *parent = 0);
	void setScanPath(const QString &path);
	QString scanPath() const;
	double progress() const;
	bool isScanning() const;
	long scannedDirs() const;
	long scannedFiles() const;
	long totalDirs() const;
	long totalFiles() const;
	virtual void scanFile(const QString &fileName) = 0;

public slots:
	void stop();

signals:
	void progressChanged(double progress);
	void scanningChanged(bool scanning);
	void scanPathChanged(const QString &newPath);

protected:
	void run();

private slots:
	void onFinished();
	void onStarted();

private:
	void scanDirectory(const QString &directory, double progressMin, double progressMax);
	void clearState();

private:
	Q_DISABLE_COPY(FileScanner);
	QString m_scanPath;
	double m_progress;
	long m_scannedDirs;
	long m_scannedFiles;
	long m_totalDirs;
	long m_totalFiles;
	bool m_stop;
	bool m_scanning;
}; /* -----  end of class FileScanner  ----- */

#endif /* end of include guard: FILESCANNER_BRQV7MA7 */

