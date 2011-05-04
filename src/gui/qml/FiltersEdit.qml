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
	property variant filterSettings: runtime.filterSettings
	anchors.margins: 10
	ListView {
		id: listView
		anchors.fill: parent
		model: itemsModel
		spacing: 10
	}

	Component.onCompleted: {
		reloadSettings();
	}
	function reloadSettings() {
		grayscaleSettings.settings = runtime.filterSettings.grayscale;
		illuminationSettings.settings = runtime.filterSettings.illumination;
		sobelSettings.settings = runtime.filterSettings.sobel;
		gaborSettings.settings = runtime.filterSettings.gabor;
	}

	VisualItemModel {
		id: itemsModel
		GraphicFilterSettings {
			id: grayscaleSettings
			width: listView.width
			title: qsTr("Grayscale")
			onChanged: {
				if (filterSettings != undefined && filterSettings.grayscale != undefined) {
					var s = filterSettings;
					s.grayscale = settings;
					runtime.filterSettings = s;
				}
			}
		}
		IlluminationFilterSettings {
			id: illuminationSettings
			width: listView.width
			title: qsTr("Illumination correction")
			onChanged: {
				if (filterSettings != undefined && filterSettings.illumination != undefined) {
					var s = filterSettings;
					s.illumination = settings;
					runtime.filterSettings = s;
				}
			}
		}
		GraphicFilterSettings {
			id: sobelSettings
			width: listView.width
			title: qsTr("Sobel")
			onChanged: {
				if (filterSettings != undefined && filterSettings.sobel != undefined) {
					var s = filterSettings;
					s.sobel = settings;
					runtime.filterSettings = s;
				}
			}
		}
		GaborFilterSettings {
			id: gaborSettings
			width: listView.width
			title: qsTr("Gabor")
			onChanged: {
				if (filterSettings != undefined && filterSettings.gabor != undefined) {
					var s = filterSettings;
					s.gabor = settings;
					runtime.filterSettings = s;
				}
			}
		}
	}
}

