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

#include <QObject>
#include <QSharedPointer>
#include <lapackpp/lavd.h>

namespace FaceDetect {

class TrainingDataReader;

class NeuralNet: public QObject
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
	void train();
	void stop();
	virtual LaVectorDouble calcOutput(const LaVectorDouble &input) = 0;

protected:
	virtual void trainSample(const LaVectorDouble &input, const LaVectorDouble &expectedOutput) = 0;
	virtual void initializeTraining() = 0;

private:
	bool m_stop;
	double m_learningSpeed;
	int m_numEpoch;
	QSharedPointer<TrainingDataReader> m_reader;
}; /* -----  end of class NeuralNet  ----- */

} /* end of namespace FaceDetect */

#endif /* end of include guard: NEURALNET_9RUG4BT8 */

