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

/**
 * \brief Všeobecná neurónová sieť.
 */
class NeuralNet: public QThread
{
Q_OBJECT
/**
 * Rýchlosť učenia neurónovej siete. Táto hodnota obyčajne nastavuje parameter
 * eta (η) konkrétnej implementácie neurónovej siete.
 */
Q_PROPERTY(double learningSpeed READ learningSpeed WRITE setLearningSpeed);
/**
 * Počet epoch učenia neurónovej siete.
 */
Q_PROPERTY(int numEpoch READ numEpoch WRITE setNumEpoch);
public:
	explicit NeuralNet(QObject *parent = 0);
	virtual ~NeuralNet();
	/**
	 * Vráti rýchlosť učenia.
	 */
	double learningSpeed() const;
	/**
	 * Nastavenie rýchlosti učenia na hodnotu \a learningSpeed.
	 */
	void setLearningSpeed(double learningSpeed);
	/**
	 * Vráti počet epoch učenia.
	 */
	int numEpoch() const;
	/**
	 * Nastavenie počtu epochu učenia na hodnotu \a numEpoch.
	 */
	void setNumEpoch(int numEpoch);
	TrainingDataReader *trainingDataReader() const;
	void setTrainingDataReader(TrainingDataReader *reader);
	int inputVectorSize() const;
	int outputVectorSize() const;
	void stop();
	/**
	 * Výpočet výstupu pre vstup \a input. Výstup je návratovou hodnotou.
	 */
	virtual LaVectorDouble calcOutput(const LaVectorDouble &input) = 0;
	/**
	 * Serializácia dát neurónovej siete.
	 */
	template<class Archive> void serialize(Archive &ar, const unsigned int version) {
		Q_UNUSED(version);
		ar & m_learningSpeed;
		ar & m_numEpoch;
	}

signals:
	/**
	 * Signál o dokončení tréningu pre vzorku slúži len pre informatívne účely.
	 * Vyšle sa štandardne raz za niekoľko vzoriek. Argumentom \a sample je číslo
	 * vzorky a \a epoch je číslo aktuálnej epochy.
	 */
	void sampleFinished(std::size_t sample, int epoch);
	/**
	 * Signál sa vyšle po ukončení tréningovej epochy. Argument \a epoch je číslom
	 * epochy a \a mse je chyba v tejto epoche.
	 */
	void epochFinished(int epoch, double mse);

protected:
	/**
	 * Tréning pre vzorku so vstupom \a input a očakávaným výstupom \a
	 * expectedOutput.
	 */
	virtual void trainSample(const LaVectorDouble &input, const LaVectorDouble &expectedOutput) = 0;
	/**
	 * Táto metóda sa volá pred začiatkom tréningu. Slúži predovšetkým na
	 * inicializáciu váh.
	 */
	virtual void initializeTraining() = 0;
	void initializeMatrix(LaGenMatDouble &matrix, double min, double max) const;

private:
	void run();
	double calcMse();

private:
	/// Ak je táto premenná \e true tréning sa ukončí hneď po dokončení vzorky.
	bool m_stop;
	/// Rýchlosť učenia neurónovej siete - eta (η).
	double m_learningSpeed;
	/// Počet tréningových epoch.
	int m_numEpoch;
	/// Objekt čítajúci tréningové vzorky.
	TrainingDataReader *m_reader;
	Q_DISABLE_COPY(NeuralNet)

friend class boost::serialization::access;
}; /* -----  end of class NeuralNet  ----- */

} /* end of namespace FaceDetect */

#endif /* end of include guard: NEURALNET_9RUG4BT8 */

