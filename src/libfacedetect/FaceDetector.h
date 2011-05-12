/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  26.03.2011 11:53:25
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef FACEDETECTOR_YYFAKUAG
#define FACEDETECTOR_YYFAKUAG

#include <QImage>
#include <QMutex>
#include <QPolygon>
#include <QSharedPointer>
#include <QThread>
#include <QVariant>
#include <valarray>
#include "ImageFilter.h"
#include "ImageSegmenter.h"
#include "NeuralNet.h"

namespace FaceDetect {

/**
 * \brief Detektor tváre skenujúci vstupný obraz.
 */
class FaceDetector: public QThread
{
Q_OBJECT
/**
 * Stav detekcie tváre, ak detekcia beží má hodnotu \e true.
 */
Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged);
/**
 * Aktuálna úroveň priebehu.
 */
Q_PROPERTY(double progress READ progress NOTIFY progressChanged);
public:
	struct DetectionWindow {
		double value;
		QPolygon polygon;
	};
	FaceDetector(NeuralNet *neuralNet, QObject *parent = 0);
	~FaceDetector();
	/**
	 * Vráti \e true, ak beží detekcia tváre.
	 */
	bool isRunning() const;
	/**
	 * Vráti aktuálnu úroveň priebehu skenovania.
	 */
	double progress() const;
	QImage image() const;
	void setImage(const QImage &image);
	void setupSegmenter(const ImageSegmenter::Settings &settings);
	void scanImage();
	void setLocalFilter(const ImageFilter &localFilter);
	void setGlobalFilter(const ImageFilter &globalFilter);
	QImage scanResultImage() const;
	QVector<FaceDetector::DetectionWindow> scanResult() const;
	void stop();
	void setScanImageEnabled(bool enabled);

signals:
	void scanResultReturned(FaceDetect::FaceDetector::DetectionWindow window);
	/**
	 * Signál sa emituje v prípade zmeny stavu detekcie tváre.
	 */
	void runningChanged(bool running);
	/**
	 * Signál sa vyšle pri zmene priebehu.
	 */
	void progressChanged(double progress, const QPolygon &rect);

protected:
	virtual void run();

private slots:
	void onStarted();
	void onFinished();

private:
	/// Neurónová sieť používaná na detekciu.
	NeuralNet *m_neuralNet;
	/// Trieda pre rozklad obrazu na fragmenty.
	QSharedPointer<ImageSegmenter> m_segmenter;
	/// Nastavenie rozkladu obrazu.
	ImageSegmenter::Settings m_settings;
	/// Štatistiky pre obraz.
	std::valarray<float> m_statistics;
	/// Lokálny filter.
	ImageFilter m_localFilter;
	/// Výsledok skenovania súboru.
	QImage m_scanResultImage;
	/// Výsledky ako súbor polygónov.
	QVector<DetectionWindow> m_scanResult;
	/// Ak je táto premenná \e true deteckia sa ukončí hneď po ukončení vzorky.
	bool m_stop;
	/// Ochrana premennej stop.
	QMutex m_stopMutex;
	/// Stavová premenná, ak vlákno beží má hodnotou \e true.
	bool m_running;
	/// Aktuálna úroveň priebehu detekcie.
	double m_progress;
	/// Ak sa má po detekcii vyrenderovať obrázok s výsledkami má hodnotu \e true.
	bool m_scanImageEnabled;
	/// Maximálny počet položiek vo výsledkoch skenovania;
	static const int MaxScanResult = 4096;
}; /* -----  end of class FaceDetector  ----- */

} /* end of namespace FaceDetect */

#endif /* end of include guard: FACEDETECTOR_YYFAKUAG */

