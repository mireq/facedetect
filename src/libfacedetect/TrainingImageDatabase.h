/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  21.02.2011 08:24:22
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef TRAININGIMAGEDATABASE_20EM4P61
#define TRAININGIMAGEDATABASE_20EM4P61

#include <QImage>
#include <QSharedPointer>
#include <QVector>
#include "Align.h"
#include "FaceFileScanner.h"
#include "ImageFilter.h"
#include "TrainingDataReader.h"

namespace FaceDetect {

/**
 * \brief Načítavanie trénovacích vzorov databázy tvárí a obrázkov
 */
class TrainingImageDatabase: public TrainingDataReader
{
Q_OBJECT
public:
	explicit TrainingImageDatabase(QObject *parent = 0);
	virtual ~TrainingImageDatabase();
	virtual int inputVectorSize() const;
	virtual int outputVectorSize() const;
	virtual std::size_t trainingSetSize() const;
	virtual LaVectorDouble inputVector(std::size_t sample) const;
	virtual LaVectorDouble outputVector(std::size_t sample) const;
	void addImage(const FaceDetect::FaceFileScanner::ImageInfo &image);
	void addImage(const LaVectorDouble &input, const LaVectorDouble &output);
	void setCacheDir(const QString &dir);
	void shuffle();
	void setLocalFilter(const ImageFilter &filter);
	void setGlobalFilter(const ImageFilter &filter);
	void clear();

private:
	/**
	 * \brief Položka tréningovej množiny.
	 */
	struct TrainingSample {
		mutable QSharedPointer<FaceFileScanner::ImageInfo> info; /**< Informácie o obrázku (ak je nastavený vstupný a výstupný vektor má hodnotu 0) */
		mutable LaVectorDouble input;                            /**< Vstupný vektor.                                                               */
		mutable LaVectorDouble output;                           /**< Očakávaný výstup.                                                             */
	};

	void calcVectors(std::size_t sample) const;

	/// Šírka obrázku.
	static const int ImageWidth = 20;
	/// Výška obrázku.
	static const int ImageHeight = 20;
	/// Veľkosť vstupného vektoru.
	int m_inputVectorSize;
	/// Objekt na zarovnávanie fotografií.
	Align *m_aligner;
	/// Zoznam trénovacích vzorov.
	QVector<TrainingSample> m_samples;
	/// Adresár s cachovanými fotografiami
	QString m_cacheDir;
	/// Globálne filtre.
	ImageFilter m_globalFilter;
	/// Lokálne filtre vzťahujúce sa na detekčné okno.
	ImageFilter m_localFilter;
	/// Pomocný obrázok používaný pred škálovaním.
	mutable QImage m_workingImage;
}; /* -----  end of class TrainingImageDatabase  ----- */

} /* end of namespace FaceDetect */

#endif /* end of include guard: TRAININGIMAGEDATABASE_20EM4P61 */

