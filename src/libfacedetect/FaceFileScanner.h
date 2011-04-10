/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  19.12.2010 14:38:23
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef FACEFILESCANNER_YXFS5ITD
#define FACEFILESCANNER_YXFS5ITD

#include <QImage>
#include <QMetaType>
#include <QPoint>
#include <QVector>
#include <QUrl>
#include "FileScanner.h"

namespace FaceDetect {

/**
 * \brief Skener pre vyhľadávanie a čítanie údajov z fotografií tvárí.
 */
class FaceFileScanner: public FileScanner
{
Q_OBJECT
public:
/**
 * \brief Kontrolné body tváre.
 */
	struct FaceData {
		QPoint leftEye;  /**< Pozícia ľavého oka.  */
		QPoint rightEye; /**< Pozícia pravého oka. */
		QPoint nose;     /**< Pozícia nosa.        */
		QPoint mouth;    /**< Pozícia úst.         */
		QString pose;    /**< Póza.                */
		bool isFrontal() const; /**< Vráti \e true, ak je osoba fotografovaná zpredu */
	};

/**
 * Iterátor pre iteráciu medzi dátami tváre.
 */
	typedef QVector<FaceData>::const_iterator FaceDataIterator;

	/**
	 * \brief Informácie o fotografii.
	 */
	class ImageInfo
	{
	public:
		ImageInfo();
		~ImageInfo();
		bool isValid() const;
		QUrl url() const;
		QUrl definitionUrl() const;
		FaceDataIterator faceBegin() const;
		FaceDataIterator faceEnd() const;
		QImage getImage() const;
		QVector<ImageInfo> splitFaces() const;

	private:
		void setUrl(const QUrl &url);
		void setDefinitionUrl(const QUrl &url);
		void setFaceData(const QVector<FaceData> &faceData);

	private:
		/// Čitateľnosť súboru s fotografiou (ak je čitateľný má hodnotu \e true)
		bool m_valid;
		/// URL súboru s fotografiou
		QUrl m_url;
		/// URL súboru s informácii o fotografii
		QUrl m_definitionUrl;
		/// Dáta s informáciami o tvárach na fotografii
		QVector<FaceData> m_faceData;
		friend class FaceFileScanner;
	}; /* -----  end of class ImageInfo  ----- */

	explicit FaceFileScanner(QObject *parent = 0);
	~FaceFileScanner();
	QUrl basePath() const;
	void setBasePath(const QUrl &url);
	bool filterFrontal() const;
	void setFilterFrontal(bool filter);
	virtual void scanFile(const QString &fileName);
	ImageInfo readFile(const QString &fileName);

signals:
/**
 * Signál sa vyšle po úspešnom preskenovaní fotografie. Všetky fotografie, u
 * ktorých došlo k akejkoľvek chybe pri skenovaní budú preskočené.
 */
	void imageScanned(const FaceDetect::FaceFileScanner::ImageInfo &image);

private:
	/**
	 * Stav čítania XML súboru.
	 */
	enum ReadState {
		NoState,          /**< Stav pred začiatkom čítania */
		RecordingsState,  /**< Čítanie záznamov            */
		RecordingState,   /**< Čítanie konkrétneho záznamu */
		URLState,         /**< Čítanie URL súboru          */
		SubjectState,     /**< Čítanie záznamu o subjekte  */
		ApplicationState, /**< Čítanie záznamu o dátach    */
		FaceState         /**< Čítanie dát tváre           */
	};
	/// URL základného adresára.
	QUrl m_basePath;
	/// Filtrovanie fotografií zpredu.
	bool m_filterFrontal;
}; /* -----  end of class FaceFileScanner  ----- */

} /* end of namespace FaceDetect */

Q_DECLARE_METATYPE(FaceDetect::FaceFileScanner::ImageInfo);

#endif /* end of include guard: FACEFILESCANNER_YXFS5ITD */

