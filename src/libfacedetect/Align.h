/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  14.12.2010 11:43:15
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef ALIGN_L4LZ6THZ
#define ALIGN_L4LZ6THZ

#include <QString>
#include <lapackpp/gmd.h>
#include <lapackpp/lacvd.h>
#include <cstdlib>
#include "FaceFileReader.h"

namespace FaceDetect {

class Align
{
public:
	Align();
	~Align();
	void scanImage(const QString &definitionFile);

private:
	bool checkControlPoints(const FaceFileReader::FaceData &data);
	LaColVectorDouble getControlPointsVector(const FaceFileReader::FaceData &data);
	void calcAvg();

private:
	LaColVectorDouble m_faceFeaturesSum;
	LaColVectorDouble m_avgFaceFeatures;
	std::size_t m_imgCount;
	static const int sm_faceFeaturesCount = 4;
	bool m_avgDirty;
}; /* -----  end of class Align  ----- */

} /* end of namespace FaceDetect */

#endif /* end of include guard: ALIGN_L4LZ6THZ */

