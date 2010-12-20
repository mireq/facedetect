/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  19.12.2010 14:38:23
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef FACEFILESCANNER_YXFS5ITD
#define FACEFILESCANNER_YXFS5ITD

#include "FileScanner.h"

class FaceFileScanner: public FileScanner
{
Q_OBJECT
public:
	FaceFileScanner(QObject *parent = 0);
	~FaceFileScanner();
	virtual void scanFile(const QString &fileName);

signals:
	void fileScanned(const QString &fileName);
}; /* -----  end of class FaceFileScanner  ----- */

#endif /* end of include guard: FACEFILESCANNER_YXFS5ITD */

