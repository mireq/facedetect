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
#include <string>
#include <boost/serialization/access.hpp>
#include <cmath>
#include <lapackpp/gmd.h>
#include <lapackpp/lavd.h>

namespace FaceDetect {
class NetTrainer;

/**
 * \brief Všeobecná neurónová sieť.
 */
class NeuralNet: public QObject
{
Q_OBJECT
/**
 * Rýchlosť učenia neurónovej siete. Táto hodnota obyčajne nastavuje parameter
 * eta konkrétnej implementácie neurónovej siete.
 */
Q_PROPERTY(double learningSpeed READ learningSpeed WRITE setLearningSpeed);
/**
 * Hodnota, po ktorú v prípade binárneho výstupu vráthi \e false nad ktorou
 * vráti \e true. V prípade rovnosti vráti tiež \e true.
 */
Q_PROPERTY(double binaryThreshold READ binaryThreshold WRITE setBinaryThreshold);
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
	 * Vráti hodnotu hranice pre binárny výstup.
	 */
	double binaryThreshold() const;
	/**
	 * Nastavenie hranice pre binárny výstup.
	 */
	void setBinaryThreshold(double threshold);
	/**
	 * Výpočet výstupu pre vstup \a input. Výstup je návratovou hodnotou.
	 */
	virtual LaVectorDouble calcOutput(const LaVectorDouble &input) = 0;
	/**
	 * Výpočet binárneho výstupu pomocou binaryThreshold.
	 */
	bool calcBinaryOutput(const LaVectorDouble &input);
	/**
	 * Vráti veľkosť vstupného vektoru siete.
	 */
	int inputVectorSize() const { return m_inputVectorSize; };
	/**
	 * Vráti veľkosť výstupného vektoru siete.
	 */
	int outputVectorSize() const { return m_outputVectorSize; };
	void setInputVectorSize(int size);
	void setOutputVectorSize(int size);
	/**
	 * Serializácia dát neurónovej siete.
	 */
	template<class Archive> void serialize(Archive &ar, const unsigned int version) {
		Q_UNUSED(version);
		ar & m_learningSpeed;
		ar & m_inputVectorSize;
		ar & m_outputVectorSize;
		ar & m_binaryThreshold;
	};
	static NeuralNet *create(const std::string &type, QObject *parent = 0);

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
	/**
	 * Sigmoidná funkcia pre hodnotu \a potencial.
	 */
	double sigmoid(const double &potencial) const { return 1.0 / (1.0 + std::exp(-potencial)); }
	/**
	 * Derivácia sigmoidnej funkci pre hodnotu \a potencial.
	 */
	double derivSigmoid(const double &potencial) const {
		double pom = exp(-potencial);
		double pom1 = 1.0 + pom;
		return pom/(pom1 * pom1);
	};

private:
	/// Rýchlosť učenia neurónovej siete - eta.
	double m_learningSpeed;
	/// Veľkosť vstupného vektoru.
	int m_inputVectorSize;
	/// Veľkosť výstupného vektoru.
	int m_outputVectorSize;
	/// Hranica pre prevod do binárnych hodnôt.
	double m_binaryThreshold;

friend class boost::serialization::access;
friend class FaceDetect::NetTrainer;
}; /* -----  end of class NeuralNet  ----- */

} /* end of namespace FaceDetect */

#endif /* end of include guard: NEURALNET_9RUG4BT8 */

