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

class ImageFilterBase
{
public:
	ImageFilterBase() {};
	virtual ~ImageFilterBase() {};
	virtual void filter(QImage &sourceImage) const = 0;
}; /* -----  end of class ImageFilterBase  ----- */

} /* end of namespace FaceDetect */

#endif   /* ----- #ifndef IMAGEFILTERBASE_H  ----- */


