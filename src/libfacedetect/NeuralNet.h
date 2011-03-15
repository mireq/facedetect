/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  05.02.2011 09:00:41
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef NEURALNET_9RUG4BT8
#define NEURALNET_9RUG4BT8

#include <QSharedPointer>
#include <QThread>
#include <boost/serialization/access.hpp>
#include <lapackpp/gmd.h>
#include <lapackpp/lavd.h>

namespace FaceDetect {

class TrainingDataReader;

class NeuralNet: public QThread
{
Q_OBJECT
Q_PROPERTY(double learningSpeed READ learningSpeed WRITE setLearningSpeed);
Q_PROPERTY(int numEpoch READ numEpoch WRITE setNumEpoch);
public:
	NeuralNet(QObject *parent = 0);
	virtual ~NeuralNet();
	double learningSpeed() const;
	void setLearningSpeed(double learningSpeed);
	int numEpoch() const;
	void setNumEpoch(int numEpoch);
	TrainingDataReader *trainingDataReader() const;
	void setTrainingDataReader(TrainingDataReader *reader);
	std::size_t inputVectorSize() const;
	std::size_t outputVectorSize() const;
	void run();
	void stop();
	virtual LaVectorDouble calcOutput(const LaVectorDouble &input) = 0;
	template<class Archive> void serialize(Archive &ar, const unsigned int version) {
		Q_UNUSED(version);
		ar & m_learningSpeed;
		ar & m_numEpoch;
	}

signals:
	void sampleFinished(std::size_t sample, int epoch);
	void epochFinished(int epoch, double mse);

protected:
	virtual void trainSample(const LaVectorDouble &input, const LaVectorDouble &expectedOutput) = 0;
	virtual void initializeTraining() = 0;
	void initializeMatrix(LaGenMatDouble &matrix, double min, double max) const;

private:
	double calcMse();

private:
	Q_DISABLE_COPY(NeuralNet);
	bool m_stop;
	double m_learningSpeed;
	int m_numEpoch;
	TrainingDataReader *m_reader;

friend class boost::serialization::access;
}; /* -----  end of class NeuralNet  ----- */

} /* end of namespace FaceDetect */

#endif /* end of include guard: NEURALNET_9RUG4BT8 */

