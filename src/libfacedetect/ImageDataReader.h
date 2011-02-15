/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  06.02.2011 17:20:31
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef IMAGEDATAREADER_9ZTGRW3T
#define IMAGEDATAREADER_9ZTGRW3T

#include "TrainingDataReader.h"

namespace FaceDetect {

class ImageDataReader: public TrainingDataReader
{
Q_OBJECT
public:
	explicit ImageDataReader(QObject *parent = 0);
	virtual ~ImageDataReader();
	virtual std::size_t inputVectorSize() const;
	virtual std::size_t outputVectorSize() const;
	virtual std::size_t trainingSetSize() const;
	virtual LaColVectorDouble inputVector(std::size_t sample) const;
	virtual LaColVectorDouble outputVector(std::size_t sample) const;

private:
}; /* -----  end of class ImageDataReader  ----- */

} /* end of namespace FaceDetect */

#endif /* end of include guard: IMAGEDATAREADER_9ZTGRW3T */

