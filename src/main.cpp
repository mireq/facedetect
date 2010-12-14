/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  07.12.2010 00:31:52
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <QApplication>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QDeclarativeView>
#include "core/FaceBrowserModel.h"
#include "core/FaceImageProvider.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QDeclarativeView view;
	FaceBrowserModel *model = new FaceBrowserModel;
	model->setSearchPrefix("/home/mirec/Documents/Skola/diplomka/resources/colorferet/dvd1/");

	// Naplnenie modelu
	QStringList files;
	files << "/home/mirec/Documents/Skola/diplomka/resources/colorferet/dvd1/data/ground_truths/xml/00001/00001_930831_fa_a.xml";
	files << "/home/mirec/Documents/Skola/diplomka/resources/colorferet/dvd1/data/ground_truths/xml/00001/00001_930831_fb_a.xml";
	//files << "/home/mirec/Documents/Skola/diplomka/resources/colorferet/dvd1/data/ground_truths/xml/00001/00001_930831_hl_a.xml";
	//files << "/home/mirec/Documents/Skola/diplomka/resources/colorferet/dvd1/data/ground_truths/xml/00001/00001_930831_hr_a.xml";
	//files << "/home/mirec/Documents/Skola/diplomka/resources/colorferet/dvd1/data/ground_truths/xml/00001/00001_930831_pr_a.xml";
	files << "/home/mirec/Documents/Skola/diplomka/resources/colorferet/dvd1/data/ground_truths/xml/00002/00002_930831_fa.xml";
	files << "/home/mirec/Documents/Skola/diplomka/resources/colorferet/dvd1/data/ground_truths/xml/00002/00002_930831_fb.xml";
	//files << "/home/mirec/Documents/Skola/diplomka/resources/colorferet/dvd1/data/ground_truths/xml/00002/00002_931230_fa.xml";
	//files << "/home/mirec/Documents/Skola/diplomka/resources/colorferet/dvd1/data/ground_truths/xml/00002/00002_931230_fb.xml";
	files << "/home/mirec/Documents/Skola/diplomka/resources/colorferet/dvd1/data/ground_truths/xml/00002/00002_940128_fa.xml";
	files << "/home/mirec/Documents/Skola/diplomka/resources/colorferet/dvd1/data/ground_truths/xml/00002/00002_940128_fb.xml";
	files << "/home/mirec/Documents/Skola/diplomka/resources/colorferet/dvd1/data/ground_truths/xml/00002/00002_940422_fa.xml";
	files << "/home/mirec/Documents/Skola/diplomka/resources/colorferet/dvd1/data/ground_truths/xml/00002/00002_940422_fb.xml";
	files << "/home/mirec/Documents/Skola/diplomka/resources/colorferet/dvd1/data/ground_truths/xml/00002/00002_940928_fa.xml";
	files << "/home/mirec/Documents/Skola/diplomka/resources/colorferet/dvd1/data/ground_truths/xml/00002/00002_940928_fb.xml";
	foreach (const QString &file, files) {
		model->addDefinitionFile(file);
	}

	QDeclarativeContext *rootContext = view.rootContext();
	QDeclarativeEngine *engine = view.engine();
	engine->addImageProvider(QLatin1String("faceimage"), new FaceImageProvider);
	rootContext->setContextProperty("facesModel", model);
	view.setSource(QUrl("qrc:/qml/main.qml"));
	view.show();

	return app.exec();
}

