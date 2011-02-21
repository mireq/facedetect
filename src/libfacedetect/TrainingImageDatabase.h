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
#include <QLabel>
#include <QSharedPointer>
#include <QVector>
#include "Align.h"
#include "FaceFileScanner.h"
#include "TrainingDataReader.h"

namespace FaceDetect {

class TrainingImageDatabase: public TrainingDataReader
{
Q_OBJECT
public:
	explicit TrainingImageDatabase(QObject *parent = 0);
	virtual ~TrainingImageDatabase();
	virtual std::size_t inputVectorSize() const;
	virtual std::size_t outputVectorSize() const;
	virtual std::size_t trainingSetSize() const;
	virtual LaColVectorDouble inputVector(std::size_t sample) const;
	virtual LaColVectorDouble outputVector(std::size_t sample) const;
	void addImage(const FaceDetect::FaceFileScanner::ImageInfo &image);

private:
	struct TrainingSample {
		mutable QSharedPointer<FaceFileScanner::ImageInfo> info;
		mutable LaColVectorDouble input;
		mutable LaColVectorDouble output;
	};

	void calcVectors(std::size_t sample) const;

	static const std::size_t sm_imageWidth = 20;
	static const std::size_t sm_imageHeight = 20;
	static const std::size_t sm_inputVectorSize = sm_imageWidth * sm_imageHeight;
	Align *m_aligner;
	QVector<TrainingSample> m_samples;
	mutable QImage m_workingImage;
	mutable QVector<QRgb> m_colorTable;
	QLabel *m_label;
}; /* -----  end of class TrainingImageDatabase  ----- */

} /* end of namespace FaceDetect */

#endif /* end of include guard: TRAININGIMAGEDATABASE_20EM4P61 */

