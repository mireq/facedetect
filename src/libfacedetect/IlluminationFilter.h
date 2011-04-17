/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  14.04.2011 18:07:11
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef ILLUMINATIONFILTER_9VZQ0NP9
#define ILLUMINATIONFILTER_9VZQ0NP9

#include <lapackpp/lavd.h>
#include "ImageFilterBase.h"

namespace FaceDetect {

class IlluminationFilter: public ImageFilterBase
{
public:
	IlluminationFilter();
	~IlluminationFilter();
	virtual void filter(QImage &sourceImage) const;
	// Len pre štatistické účely
	void setIlluminationPlaneOnly(bool planeOnly);
	void setIlluminationCorrectHistogram(bool correctHistogram);

private:
	void equalizeHistogram(QImage &sourceImage) const;

	/// Zobrazenie korekčnej iluminačnej plochy.
	bool m_illuminationPlaneOnly;
	/// Roztiahnutie svetlosti pri korekcii osvetlenia.
	bool m_illuminationCorrectHistogram;
	/// Parametre osvetľovacieho gradientu
	mutable LaGenMatDouble m_illuminationPlane;
	/// Matica koordinátov
	mutable LaGenMatDouble m_imageCoordinateMatrix;
	/// Spracovaná matica pre výpočet svetelnej korekcie
	mutable LaGenMatDouble m_ilCorrectionMatrix;
}; /* -----  end of class IlluminationFilter  ----- */

} /* end of namespace FaceDetect */

#endif /* end of include guard: ILLUMINATIONFILTER_9VZQ0NP9 */

