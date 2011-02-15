/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  06.02.2011 10:02:36
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <lapackpp/lacvd.h>
#include <QObject>

namespace FaceDetect {

class TrainingDataReader: public QObject
{
Q_OBJECT
public:
	explicit TrainingDataReader(QObject *parent = 0);
	virtual ~TrainingDataReader();
	virtual std::size_t inputVectorSize() const = 0;
	virtual std::size_t outputVectorSize() const = 0;
	virtual std::size_t trainingSetSize() const = 0;
	virtual LaColVectorDouble inputVector(std::size_t sample) const = 0;
	virtual LaColVectorDouble outputVector(std::size_t sample) const = 0;

private:
}; /* -----  end of class TrainingDataReader  ----- */

} /* end of namespace FaceDetect */

