/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  14.04.2011 18:25:02
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <cmath>
#include "SobelFilter.h"

using std::sqrt;

namespace FaceDetect {

SobelFilter::SobelFilter():
	ImageFilterBase()
{
}

SobelFilter::~SobelFilter()
{
}

/**
 * Detekcia hrán obrazu filtrom sobel.
 */
void SobelFilter::filter(QImage &sourceImage) const
{
	if (sourceImage.depth() < 8) {
		return;
	}

	QImage imgCopy = sourceImage;

	int imgWidth = imgCopy.width();
	int imgHeight = imgCopy.height();
	int depth = imgCopy.depth() / 8;
	int bytesPerLine = imgWidth * depth;
	double maxComb = sqrt(20.0);

	uchar *copyLine = 0;
	uchar *sourceLine = 0;
	uchar *prevLine = 0;
	uchar *nextLine = 0;
	int imgFragment[3][3];
	imgFragment[1][1] = 0;

	for (int yPos = 0; yPos < imgHeight; ++yPos) {
		if (yPos == 0) {
			prevLine = 0;
		}
		else {
			prevLine = imgCopy.scanLine(yPos - 1);
		}
		if (yPos == imgHeight - 1) {
			nextLine = 0;
		}
		else {
			nextLine = imgCopy.scanLine(yPos + 1);
		}
		copyLine = imgCopy.scanLine(yPos);
		sourceLine = sourceImage.scanLine(yPos);

		for (int bytePos = 0; bytePos < bytesPerLine; ++bytePos) {
			int xGrad = 0;
			int yGrad = 0;
			if (prevLine == 0) {
				imgFragment[0][0] = copyLine[bytePos];
				imgFragment[0][1] = copyLine[bytePos];
				imgFragment[0][2] = copyLine[bytePos];
			}
			else {
				imgFragment[0][0] = bytePos - depth < 0 ? copyLine[bytePos] : prevLine[bytePos - depth];
				imgFragment[0][1] = prevLine[bytePos];
				imgFragment[0][2] = bytePos + depth >= bytesPerLine ? copyLine[bytePos] : prevLine[bytePos + depth];
			}

			imgFragment[1][0] = bytePos - depth < 0 ? copyLine[bytePos] : copyLine[bytePos - depth];
			imgFragment[1][2] = bytePos + depth >= bytesPerLine ? copyLine[bytePos] : copyLine[bytePos + depth];

			if (nextLine == 0) {
				imgFragment[2][0] = copyLine[bytePos];
				imgFragment[2][1] = copyLine[bytePos];
				imgFragment[2][2] = copyLine[bytePos];
			}
			else {
				imgFragment[2][0] = bytePos - depth < 0 ? copyLine[bytePos] : nextLine[bytePos - depth];
				imgFragment[2][1] = nextLine[bytePos];
				imgFragment[2][2] = bytePos + depth >= bytesPerLine ? copyLine[bytePos] : nextLine[bytePos + depth];
			}
			xGrad = imgFragment[0][0] - imgFragment[0][2] + 2 * imgFragment[1][0] - 2 * imgFragment[1][2] + imgFragment[0][0] - imgFragment[0][2];
			yGrad = imgFragment[0][0] + 2 * imgFragment[0][1] + imgFragment[0][2] - imgFragment[2][0] - 2 * imgFragment[2][1] - imgFragment[2][2];
			sourceLine[bytePos] = sqrt(xGrad * xGrad + yGrad * yGrad) / maxComb;
		}
	}

	//normalizeImage(sourceImage);
}

} /* end of namespace FaceDetect */

