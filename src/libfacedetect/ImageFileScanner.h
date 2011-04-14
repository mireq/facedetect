/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  22.02.2011 20:09:06
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef IMAGEFILESCANNER_W0ZUXPCL
#define IMAGEFILESCANNER_W0ZUXPCL

#include <lapackpp/lavd.h>
#include <QImage>
#include <QVector>
#include "FileScanner.h"

namespace FaceDetect {

/**
 * \brief Skener pre obrázky neobsahujúce tvár.
 */
class ImageFileScanner: public FileScanner
{
Q_OBJECT
public:
	ImageFileScanner(QObject *parent = 0);
	~ImageFileScanner();

protected:
	virtual void scanFile(const QString &fileName);

signals:
/**
 * Signál sa vyšle pre každý skenovaný segment obrázku. Všetky obrázky väčšie
 * ako štandardné výška a šírka sú automaticky segmentované. Výstupom je vždy
 * hodnota 0 (tento skener nekontroluje prítomnosť tváre).
 */
	void imageScanned(const LaVectorDouble &input, const LaVectorDouble &output);

private:
	/// Štandardná šírka obrázka.
	static const int ImageWidth = 20;
	/// Štandardná výška obrázka.
	static const int ImageHeight = 20;
	/// Veľkosť vstupného vektoru (každý pixel má samostatnú položku).
	static const int InputVectorSize = ImageWidth * ImageHeight;
}; /* -----  end of class ImageFileScanner  ----- */

} /* end of namespace FaceDetect */

#endif /* end of include guard: IMAGEFILESCANNER_W0ZUXPCL */

