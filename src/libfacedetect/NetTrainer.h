/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  09.04.2011 20:09:29
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <QMetaType>
#include <QMutex>
#include <QThread>
#include <QVector>

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
/**
 * Handicap pri nesprávnych pozitívnych výsledkoch.
 */
Q_PROPERTY(int fapsePositiveHandicap READ falsePositiveHandicap WRITE setFalsePositiveHandicap);
/**
 * Handicap pri nesprávnych negatívnych výsledkoch.
 */
Q_PROPERTY(int falseNegativeHandicap READ falseNegativeHandicap WRITE setFalseNegativeHandicap);
public:
	struct MatchStat {
		MatchStat(): threshold(0), falsePositive(0), falseNegative(0) {};
		double threshold;
		long falsePositive;
		long falseNegative;
	};
	struct EpochStats {
		EpochStats(): epoch(0), mseA(0), mseE(0), mseBinA(0), mseBinE(0), thresholdA(0), thresholdE(0), sizeA(0), sizeE(0), facesA(0), facesE(0), falsePositiveA(0), falsePositiveE(0), falseNegativeA(0), falseNegativeE(0) {};
		int epoch;
		double mseA;
		double mseE;
		double mseBinA;
		double mseBinE;
		double thresholdA;
		double thresholdE;
		long sizeA;
		long sizeE;
		long facesA;
		long facesE;
		long falsePositiveA;
		long falsePositiveE;
		long falseNegativeA;
		long falseNegativeE;
		QVector<MatchStat> matchStatA;
		QVector<MatchStat> matchStatE;
	};

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
	/**
	 * Vráti hodontu handicapu pre nesprávne pozitívne výsledky.
	 */
	int falsePositiveHandicap() const;
	/**
	 * Nastavenie handicapu pre nesprávne pozitívne výsledky.
	 */
	void setFalsePositiveHandicap(int handicap);
	/**
	 * Vráti hodontu handicapu pre nesprávne negatívne výsledky.
	 */
	int falseNegativeHandicap() const;
	/**
	 * Nastavenie handicapu pre nesprávne negatívne výsledky.
	 */
	void setFalseNegativeHandicap(int handicap);
	TrainingDataReader *trainingDataReader() const;
	void setTrainingDataReader(TrainingDataReader *reader);
	void stop();
	void trainNet(FaceDetect::NeuralNet *net);
	NetTrainer::EpochStats bestEpochStats() const;

signals:
	/**
	 * Signál o dokončení tréningu pre vzorku slúži len pre informatívne účely.
	 * Vyšle sa štandardne raz za niekoľko vzoriek. Argumentom \a sample je číslo
	 * vzorky a \a epoch je číslo aktuálnej epochy.
	 */
	void sampleFinished(std::size_t sample, int epoch);
	/**
	 * Signál sa vyšle po ukončení tréningovej epochy. Argument \a stats
	 * obsahuje štatistiky behu epochy.
	 */
	void epochFinished(const FaceDetect::NetTrainer::EpochStats &stats);
	/**
	 * Signál sa vyšle po výpočte chýb pre určitý počet vzoriek.
	 */
	void errorCalculated(std::size_t sample, std::size_t sampleCount, double errorSum);

private:
	using QThread::start;
	void run();
	double calcMse(std::size_t from, std::size_t to, bool binary = false, double *thresholdOut = 0, QVector<MatchStat> *matchStat = 0, long *fNeg = 0, long *fPos = 0, double *mse = 0, long *faces = 0);
	void calcMseForEpoch(int epoch, std::string &bestNet, double &bestMse, EpochStats &bestEpoch);
	void saveNet(std::string &net);
	void restoreNet(const std::string &net);
	void exportMseStats(std::size_t count, double errorSum);

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
	/// Štatistiky voľby rozsahu pre MSEA.
	QVector<MatchStat> m_matchStatA;
	/// Štatistiky voľby rozsahu pre MSEE.
	QVector<MatchStat> m_matchStatE;
	/// Najlepšia štatistika voľby rozsahu pre MSEA.
	QVector<MatchStat> m_bestMatchStatA;
	/// Najlepšia štatistika voľby rozsahu pre MSEE.
	QVector<MatchStat> m_bestMatchStatE;
	/// Štatistiky najlepšej tréningovej epochy.
	EpochStats m_bestEpochStats;
	/// Handicap pre nesprávne pozitívne výsledky.
	int m_falsePositiveHandicap;
	/// Handicap pre nesprávne negatívne výsledky.
	int m_falseNegativeHandicap;
	/// Počet vzoriek, pre ktoré bolo vypočítané MSE.
	std::size_t m_mseSampleCount;

	Q_DISABLE_COPY(NetTrainer)
}; /* -----  end of class NetTrainer  ----- */

} /* end of namespace FaceDetect */

Q_DECLARE_METATYPE(FaceDetect::NetTrainer::EpochStats);

