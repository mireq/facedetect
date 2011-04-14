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
 * eta (η) konkrétnej implementácie neurónovej siete.
 */
Q_PROPERTY(double learningSpeed READ learningSpeed WRITE setLearningSpeed);
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
	 * Výpočet výstupu pre vstup \a input. Výstup je návratovou hodnotou.
	 */
	virtual LaVectorDouble calcOutput(const LaVectorDouble &input) = 0;
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
	};
	/**
	 * Serializácia do reťazca.
	 */
	virtual std::string saveText() const = 0;
	/**
	 * Deserializácia z reťazca.
	 */
	virtual void restoreText(const std::string &data) = 0;

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
	/// Rýchlosť učenia neurónovej siete - eta (η).
	double m_learningSpeed;
	/// Veľkosť vstupného vektoru.
	int m_inputVectorSize;
	/// Veľkosť výstupného vektoru.
	int m_outputVectorSize;

friend class boost::serialization::access;
friend class FaceDetect::NetTrainer;
}; /* -----  end of class NeuralNet  ----- */

} /* end of namespace FaceDetect */

#endif /* end of include guard: NEURALNET_9RUG4BT8 */

