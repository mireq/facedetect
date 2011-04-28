/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  14.04.2011 13:55:07
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef  IMAGEFILTERBASE_H
#define  IMAGEFILTERBASE_H

#include <QImage>

namespace FaceDetect {

/**
 * \brief Filter bitmapových obrázkov.
 */
class ImageFilterBase
{
public:
	ImageFilterBase() {};
	virtual ~ImageFilterBase() {};
	/**
	 * Použitie filtra na obrázok \a sourceImage. Výstupom filtra je vstupný
	 * parameter.
	 */
	virtual void filter(QImage &sourceImage) const = 0;
}; /* -----  end of class ImageFilterBase  ----- */

} /* end of namespace FaceDetect */

#endif   /* ----- #ifndef IMAGEFILTERBASE_H  ----- */


