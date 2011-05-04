/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  03.05.2011 08:54:30
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0
import "filters"

CentralWindow {
	id: filtersEdit
	anchors.margins: 10
	ListView {
		id: listView
		anchors.fill: parent
		model: itemsModel
		spacing: 10
	}

	VisualItemModel {
		id: itemsModel
		GraphicFilterSettings {
			width: listView.width
			title: qsTr("Grayscale")
		}
		IlluminationFilterSettings {
			width: listView.width
			title: qsTr("Illumination correction")
		}
		GraphicFilterSettings {
			width: listView.width
			title: qsTr("Sobel")
		}
		GaborFilterSettings {
			width: listView.width
			title: qsTr("Gabor")
		}
	}
}
