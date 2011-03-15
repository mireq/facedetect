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
#include "core/FileScanner.h"

namespace FaceDetect {

class ImageFileScanner: public FileScanner
{
Q_OBJECT
public:
	ImageFileScanner(QObject *parent = 0);
	~ImageFileScanner();
	virtual void scanFile(const QString &fileName);

signals:
	void imageScanned(const LaVectorDouble &input, const LaVectorDouble &output);

private:
	QImage m_workingImage;
	QVector<QRgb> m_colorTable;

	static const std::size_t sm_imageWidth = 20;
	static const std::size_t sm_imageHeight = 20;
	static const std::size_t sm_inputVectorSize = sm_imageWidth * sm_imageHeight;
}; /* -----  end of class ImageFileScanner  ----- */

} /* end of namespace FaceDetect */

#endif /* end of include guard: IMAGEFILESCANNER_W0ZUXPCL */

