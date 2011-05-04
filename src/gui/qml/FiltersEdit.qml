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
	property string filterStr

	ListView {
		id: listView
		anchors { left: parent.left; top: parent.top; bottom: parent.bottom; right: previewPanel.left }
		model: itemsModel
		spacing: 10
		anchors.margins: 10
	}
	Rectangle {
		id: previewPanel
		width: 256
		anchors { top: parent.top; bottom: parent.bottom; right: parent.right }
		color: "#dedede"
		Text {
			id: previewTitle
			anchors { left: parent.left; top: parent.top; right: parent.right }
			elide: Text.ElideRight; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
			font.pixelSize: 18; styleColor: "#80ffffff"; style: Text.Sunken; color: "#444"
			text: qsTr("Preview")
		}
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
	function updatePreview() {
		filterStr = runtime.encodeFilterString();
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
					updatePreview();
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
					updatePreview();
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
					updatePreview();
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
					updatePreview();
				}
			}
		}
		GroupBox {
			id: previewGroup
			titleRight: Switch {
				id: sw
				anchors.verticalCenter: parent.verticalCenter
			}
			closed: !sw.on
			width: listView.width
			title: qsTr("Preview")
			Image {
				id: previewImage
				width: 200; height: 200
				source: "image://filter/" + filterStr + "|" + ".jpg"
				asynchronous: true
			}
		}
	}
}

