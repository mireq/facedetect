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

#include <QFileInfo>
#include <QList>
#include <QMutex>
#include <QThread>

/**
 * \brief Rekurzívny skener súborov.
 */
class FileScanner: public QThread
{
Q_OBJECT
/**
 * Cesta k adresáru, ktorý sa má skenovať.
 * \sa setScanPath()
 */
Q_PROPERTY(QString scanPath READ scanPath WRITE setScanPath NOTIFY scanPathChanged)
/**
 * Približný odhad priebehu skenovania v rozsahu 0 - 1.
 * \sa scanPathChanged()
 */
Q_PROPERTY(double progress READ progress NOTIFY progressChanged)
/**
 * Stav skeneru súborov. Ak skenovanie prebieha má hodnotu \e true, inak \e
 * false.
 * \sa scanningChanged()
 */
Q_PROPERTY(bool scanning READ isScanning NOTIFY scanningChanged)
/**
 * Počet preskenovaných adresárov.
 * \sa progressChanged()
 */
Q_PROPERTY(long scannedDirs READ scannedDirs NOTIFY progressChanged)
/**
 * Počet preskenovaných súborov.
 * \sa progressChanged()
 */
Q_PROPERTY(long scannedFiles READ scannedFiles NOTIFY progressChanged)
/**
 * Celkový počet adresárov, o ktorých má skener zatiaľ informáciu (v priebehu
 * skenovania sa tento počet mení).
 * \sa progressChanged()
 */
Q_PROPERTY(long totalDirs READ totalDirs NOTIFY progressChanged)
/**
 * Celkový počet súborov, o ktorých má skener zatiaľ informáciu (v priebehu
 * skenovania sa tento počet mení).
 * \sa progressChanged()
 */
Q_PROPERTY(long totalFiles READ totalFiles NOTIFY progressChanged)
public:
/**
 * Vytvorenie inštanice skenera.
 */
	explicit FileScanner(QObject *parent = 0);
/**
 * Zrušenie inštancie skenera.
 */
	~FileScanner();
/**
 * Nastavenie adresára, ktorý sa má skenovať.
 * \sa scanPath
 */
	void setScanPath(const QString &path);
/**
 * Vráti cestu k adresáru, ktorý sa má skenovať.
 * \sa scanPath
 */
	QString scanPath() const;
/**
 * Vráti aktuálny priebeh skenovania.
 * \sa progress
 */
	double progress() const;
/**
 * Vráti aktuálny stav skeneru.
 * \sa scanning
 */
	bool isScanning() const;
/**
 * Vráti počet preskenovaných adresárov.
 * \sa scannedDirs
 */
	long scannedDirs() const;
/**
 * Vráti počet preskenovaných súborov.
 * \sa scannedFiles
 */
	long scannedFiles() const;
/**
 * Vráti celkový počet adresárov.
 * \sa totalDirs
 */
	long totalDirs() const;
/**
 * Vráti celkový počet súborov.
 * \sa totalFiles
 */
	long totalFiles() const;

protected:
/**
 * Spracovanie súboru \a fileName. Túto metódu implementujú potomkovia tejto
 * triedy. Metóda sa volá pri spustení skenovania pre každý súbor v adresári
 * scanPath.
 */
	virtual void scanFile(const QString &fileName) = 0;

public slots:
/**
 * Zastavenie skenovania a čakanie na ukončenie činnosti vlákna.
 */
	void stop();

signals:
/**
 * Signál sa vyšle pri zmene priebehu. Pri vyslaní tohto signálu sa môžu okrem
 * priebehu meniť aj ďalšie stavová vlastnosti ako scannedDirs, scannedFiles,
 * totalDirs a totalFiles
 * \sa scannedDirs, scannedFiles, totalDirs a totalFiles
 */
	void progressChanged(double progress);
/**
 * Signál sa vyšle pri zmene stavu skenera.
 */
	void scanningChanged(bool scanning);
/**
 * Signál sa vyšle pri zmene adresára, ktorý sa má skenovať.
 */
	void scanPathChanged(const QString &newPath);

protected:
/**
 * Spustenie samotného skenovania. Táto metóda je reimplementáciou rovnakej
 * metódy triedy QThread. Volá sa automaticky po zavolaní start().
 */
	void run();

private slots:
/**
 * Metóda sa vykoná po úspešnom ukončení činnosti vlákna, alebo po tvrdom
 * ukončení vlákna.
 */
	void onFinished();
/**
 * Metóda sa vykoná po spustení skenovania.
 */
	void onStarted();

private:
/**
 * Položka zásobnika skenovania.
 */
	struct ScanStackItem {
		QString directory;
		double progressMin;
		double progressMax;
		bool scanned;
		int dirCount;
		int fileCount;
		QList<QFileInfo> files;
	};
/**
 * Spustenie skenovania.
 */
	void scan();
/**
 * Vyčistenie stavových premenných.
 */
	void clearState();

private:
	/// Adresár, ktorý sa má skenovať.
	QString m_scanPath;
	/// Aktuálny priebeh skenovania v rozhahu 0 - 1.
	double m_progress;
	/// Počet skenovaných adresárov.
	long m_scannedDirs;
	/// Počet skenovaných súborov.
	long m_scannedFiles;
	/// Celkový počet adresárov.
	long m_totalDirs;
	/// Celkový počet súborov.
	long m_totalFiles;
	/// Ochrana premennej stop.
	QMutex m_stopMutex;
	/// Indikátor zastavenia (pri najbližšom skenovanom súbore sa skenovanie zastaví).
	bool m_stop;
	/// Indikátor činnosti - ak práve prebieha skenovanie má hodnotu \e true.
	bool m_scanning;
	/// Zásobnik skenovania.
	QList<ScanStackItem> m_scanStack;

	Q_DISABLE_COPY(FileScanner)
}; /* -----  end of class FileScanner  ----- */

#endif /* end of include guard: FILESCANNER_BRQV7MA7 */

