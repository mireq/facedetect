/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  06.02.2011 10:02:36
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef TRAININGDATAREADER_1URWRPKT
#define TRAININGDATAREADER_1URWRPKT

#include <lapackpp/lavd.h>
#include <QObject>

namespace FaceDetect {

/**
 * \brief Načítanvanie trénigových dát
 *
 * Trieda má za úlohu čítať tréningové dáta a poskytnúť pre každú položku
 * vstupný vektor a očakávaný výstupný vektor.
 */
class TrainingDataReader: public QObject
{
Q_OBJECT
public:
/**
 * Vytvorenie novej inštancie.
 */
	explicit TrainingDataReader(QObject *parent = 0);
	virtual ~TrainingDataReader();
/**
 * Zamiešanie vstupných vzoriek.
 */
	virtual void shuffle() = 0;
/**
 * Vráti veľkosť vstupného vektoru
 */
	virtual int inputVectorSize() const = 0;
/**
 * Vráti veľkosť výstupného vektoru.
 */
	virtual int outputVectorSize() const = 0;
/**
 * Vráti počet trénovacích vzorov.
 */
	virtual std::size_t trainingSetSize() const = 0;
/**
 * Vráti vstupný vektor pre vzorku číslo \a sample.
 */
	virtual LaVectorDouble inputVector(std::size_t sample) const = 0;
/**
 * Vráti očakávaný výstupný vektor pre vzorku číslo \a sample.
 */
	virtual LaVectorDouble outputVector(std::size_t sample) const = 0;

private:
}; /* -----  end of class TrainingDataReader  ----- */

} /* end of namespace FaceDetect */

#endif /* end of include guard: TRAININGDATAREADER_1URWRPKT */

