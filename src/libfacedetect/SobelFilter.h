/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  14.04.2011 18:25:14
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef SOBELFILTER_RX7XHF0G
#define SOBELFILTER_RX7XHF0G

#include "ImageFilterBase.h"

namespace FaceDetect {

/**
 * \brief Sobel operátor pre detekciu hrán.
 */
class SobelFilter: public ImageFilterBase
{
public:
	SobelFilter();
	~SobelFilter();
	void filter(QImage &sourceImage) const;

private:
}; /* -----  end of class SobelFilter  ----- */

} /* end of namespace FaceDetect */

#endif /* end of include guard: SOBELFILTER_RX7XHF0G */

