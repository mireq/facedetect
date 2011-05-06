/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  14.04.2011 18:07:08
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <lapackpp/blas3pp.h>
#include <lapackpp/lavli.h>
#include <lapackpp/laslv.h>
#include "IlluminationFilter.h"

namespace FaceDetect {

IlluminationFilter::IlluminationFilter():
	ImageFilterBase(),
	m_illuminationPlaneOnly(false),
	m_illuminationCorrectHistogram(true)
{
}

IlluminationFilter::~IlluminationFilter()
{
}

/**
 * Korekcia osvetlenia pomocou najlepšieho gradientu.
 */
void IlluminationFilter::filter(QImage &sourceImage) const
{
	int depth = sourceImage.depth() / 8;
	int imgWidth = sourceImage.width();
	int imgHeight = sourceImage.height();
	int resolution = imgWidth * imgHeight;
	int byteCount = resolution * depth;

	// Inicializácia matíc
	if (m_illuminationPlane.rows() != 3) {
		m_illuminationPlane = LaGenMatDouble(3, 1);
	}
	if (m_imageCoordinateMatrix.rows() != byteCount) {
		m_imageCoordinateMatrix = LaGenMatDouble(byteCount, 3);
		int row = 0;
		int column = 0;
		int bit = 0;
		for (int pixel = 0; pixel < byteCount; ++pixel) {
			m_imageCoordinateMatrix(pixel, 0) = column;
			m_imageCoordinateMatrix(pixel, 1) = row;
			m_imageCoordinateMatrix(pixel, 2) = 1;
			bit++;
			if (bit == depth) {
				bit = 0;
				column++;
			}
			if (column == imgWidth) {
				column = 0;
				row++;
			}
		}

		m_ilCorrectionMatrix = LaGenMatDouble(3, byteCount);
		LaGenMatDouble pom = LaGenMatDouble(3, 3);
		// Výpočet $\textbf{N}^T \textbf{N}$
		Blas_Mat_Mat_Mult(m_imageCoordinateMatrix, m_imageCoordinateMatrix, pom, true);

		// Inverzia pomocnej premennej ($(\textbf{N}^T \textbf{N})^-1$)
		LaVectorLongInt pivots(3);
		LUFactorizeIP(pom, pivots);
		LaLUInverseIP(pom, pivots);

		// $(\textbf{N}^T \textbf{N})^-1$) \textbf{N}^T$
		Blas_Mat_Mat_Mult(pom, m_imageCoordinateMatrix, m_ilCorrectionMatrix, false, true);
	}

	// Naplnenie vektoru $\textbf{x}$
	LaVectorDouble xVect(resolution * depth);
	uchar *imgData = sourceImage.bits();
	int xPos = 0;
	int yPos = 0;

	for (int pixel = 0; pixel < byteCount; ++pixel) {
		xVect(pixel) = imgData[xPos];
		xPos++;
		if (xPos == imgWidth * depth) {
			xPos = 0;
			yPos++;
			imgData = sourceImage.scanLine(yPos);
		}
	}

	Blas_Mat_Mat_Mult(m_ilCorrectionMatrix, xVect, m_illuminationPlane);

	// IP(x, y) = a * x + b * y + c
	double a = m_illuminationPlane(0, 0);
	double b = m_illuminationPlane(1, 0);
	double c = m_illuminationPlane(2, 0);
	for (int yPos = 0; yPos < imgHeight; ++yPos) {
		imgData = sourceImage.scanLine(yPos);
		int bitPos = 0;
		for (int xPos = 0; xPos < imgWidth; xPos++) {
			for (int bitId = 0; bitId < depth; ++bitId) {
				int ipVal = a * bitPos + b * yPos + c;
				int bit = imgData[bitPos] - ipVal + 127;
				bit = qMin(qMax(bit, 0), 255);
				if (m_illuminationPlaneOnly) {
					imgData[bitPos] = ipVal;
				}
				else {
					imgData[bitPos] = bit;
				}
				bitPos++;
			}
		}
	}

	if (m_illuminationPlaneOnly || !m_illuminationCorrectHistogram) {
		return;
	}

	equalizeHistogram(sourceImage);
}

/**
 * Vráti \e true, ak sa má renderovať len korekčná funkcia.
 */
bool IlluminationFilter::illuminationPlaneOnly() const
{
	return m_illuminationPlaneOnly;
}

/**
 * Ak je \a planeOnly nastavené na \e true namiesto korigovaného obrázku vráti
 * filter len korekčnú plochu.
 */
void IlluminationFilter::setIlluminationPlaneOnly(bool planeOnly)
{
	m_illuminationPlaneOnly = planeOnly;
}

/**
 * Vráti \e true, ak sa bude korigovať histogram.
 */
bool IlluminationFilter::illuminationCorrectHistogram() const
{
	return m_illuminationCorrectHistogram;
}

/**
 * Ak je \a correctHistogram nastavené na \e true bude histogram obrázku
 * s upraveným osvetlením roztiahnutý na celý rozsah.
 */
void IlluminationFilter::setIlluminationCorrectHistogram(bool correctHistogram)
{
	m_illuminationCorrectHistogram = correctHistogram;
}

/**
 * Ekvalizácia histogramu.
 */
void IlluminationFilter::equalizeHistogram(QImage &sourceImage) const
{
	if (sourceImage.depth() < 8) {
		return;
	}

	int imgWidth = sourceImage.width();
	int imgHeight = sourceImage.height();
	int depth = sourceImage.depth() / 8;
	uchar *imgData = 0;

	// Početnosť hodnoty pixelov
	int frequencies[256];
	for (int freq = 0; freq < 256; ++freq) {
		frequencies[freq] = 0;
	}

	for (int yPos = 0; yPos < imgHeight; ++yPos) {
		imgData = sourceImage.scanLine(yPos);
		int dataPos = 0;
		for (int xPos = 0; xPos < imgWidth; ++xPos) {
			for (int bit = 0; bit < depth; ++bit) {
				frequencies[imgData[dataPos]]++;
				dataPos++;
			}
		}
	}

	// Vypočet kumulatívnych početností
	int cumulative = 0;
	for (int freq = 0; freq < 256; ++freq) {
		cumulative += frequencies[freq];
		frequencies[freq] = cumulative;
	}

	// Korekcia histogramu
	double alpha = 255.0 / (imgWidth * imgHeight * depth);
	for (int yPos = 0; yPos < imgHeight; ++yPos) {
		imgData = sourceImage.scanLine(yPos);
		int dataPos = 0;
		for (int xPos = 0; xPos < imgWidth; ++xPos) {
			for (int bit = 0; bit < depth; ++bit) {
				imgData[dataPos] = frequencies[imgData[dataPos]] * alpha;
				dataPos++;
			}
		}
	}
}

} /* end of namespace FaceDetect */

