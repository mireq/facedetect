/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  09.04.2011 20:09:29
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <QThread>
#include <QMutex>

namespace FaceDetect {
class NeuralNet;
class TrainingDataReader;

/**
 * \brief Trieda trénujúca neurónovú sieť.
 */
class NetTrainer: public QThread
{
Q_OBJECT
/**
 * Počet epoch učenia neurónovej siete.
 */
Q_PROPERTY(int numEpoch READ numEpoch WRITE setNumEpoch);
public:
	NetTrainer(QObject *parent = 0);
	~NetTrainer();
	/**
	 * Vráti počet epoch učenia.
	 */
	int numEpoch() const;
	/**
	 * Nastavenie počtu epochu učenia na hodnotu \a numEpoch.
	 */
	void setNumEpoch(int numEpoch);
	/**
	 * Vráti veľkosť tréningovej množiny.
	 */
	std::size_t trainingSetSize() const;
	/**
	 * Nastavenie veľkosti tréningovej množiny na \a size.
	 */
	void setTrainingSetSize(std::size_t size);
	TrainingDataReader *trainingDataReader() const;
	void setTrainingDataReader(TrainingDataReader *reader);
	void stop();
	void trainNet(FaceDetect::NeuralNet *net);

signals:
	/**
	 * Signál o dokončení tréningu pre vzorku slúži len pre informatívne účely.
	 * Vyšle sa štandardne raz za niekoľko vzoriek. Argumentom \a sample je číslo
	 * vzorky a \a epoch je číslo aktuálnej epochy.
	 */
	void sampleFinished(std::size_t sample, int epoch);
	/**
	 * Signál sa vyšle po ukončení tréningovej epochy. Argument \a epoch je číslom
	 * epochy, \a msea je chyba na tréningovej množine a \a msee je chyba na
	 * validačnej množine. Binárne varianty chyby rozdeľujú vstup podľa nejakej
	 * hranice threshold na hodnoty 0 a 1.
	 */
	void epochFinished(int epoch, double msea, double msee, double msebina = 0, double msebine = 0, double thresholda = 0, double thresholde = 0);

private:
	using QThread::start;
	void run();
	double calcMse(std::size_t from, std::size_t to, bool binary = false, double *thresholdOut = 0);
	void finishEpoch(int epoch, std::string &bestNet, double &bestMse);

private:
	/// Objekt čítajúci tréningové vzorky.
	TrainingDataReader *m_reader;
	/// Ak je táto premenná \e true tréning sa ukončí hneď po dokončení vzorky.
	bool m_stop;
	/// Ochrana premennej stop.
	QMutex m_stopMutex;
	/// Počet tréningových epoch.
	int m_numEpoch;
	/// Neurónova sieť, ktorá sa má natrénovať.
	NeuralNet *m_net;
	/// Veľkosť tréningovej množiny.
	std::size_t m_trainingSetSize;

	Q_DISABLE_COPY(NetTrainer)
}; /* -----  end of class NetTrainer  ----- */

} /* end of namespace FaceDetect */

