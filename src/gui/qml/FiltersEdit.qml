/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  03.05.2011 08:54:30
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0
import org.facedetect 1.0
import "filters"

CentralWindow {
	id: filtersEdit
	property variant filterSettings: runtime.filterSettings
	property string filterStr
	property string previewFile
	property int gaborCount: 1
	property int previewHeight: (gaborCount == 0 || !gaborSettings.enabled ? 1 : gaborCount) * 255

	ListView {
		id: listView
		anchors { left: parent.left; top: parent.top; bottom: parent.bottom; right: previewPanel.left }
		model: itemsModel
		spacing: 10
		anchors.margins: 10
		cacheBuffer: 1024
	}
	Rectangle {
		id: previewPanel
		width: 272
		anchors { top: parent.top; bottom: parent.bottom; right: parent.right }
		color: "#dedede"
		BorderImage {
			anchors { fill: parent }
			source: "img/innerbg.sci"
		}
		Text {
			id: previewTitle
			anchors { left: parent.left; top: parent.top; right: parent.right; topMargin: 10 }
			elide: Text.ElideRight; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
			font.pixelSize: 18; styleColor: "#80ffffff"; style: Text.Sunken; color: "#444"
			text: qsTr("Preview")
		}
		Flickable {
			clip: true
			contentWidth: previewBackground.width; contentHeight: previewHeight + 7
			anchors { horizontalCenter: parent.horizontalCenter; top: previewTitle.bottom; bottom: parent.bottom}
			width: 262
			BorderImage {
				id: previewBackground
				//width: 262; height: previewImage.height + 7
				width: 262; height: previewHeight + 7
				source: "img/photo_frame.png"
				border { left: 3; top: 3; right: 4; bottom: 4 }
				Image {
					id: previewImage
					width: 255; height: previewHeight
					x: 3; y: 3
					sourceSize.width: 255; sourceSize.height: height
					source: "image://filter/" + filterStr + "|" + previewFile
					asynchronous: true
				}
				Image {
					id: loadingImage
					source: "img/photo.png"
					width: 255; height: previewHeight
					x: 3; y: 3
					visible: previewImage.status != Image.Ready
				}
				Column {
					anchors.centerIn: parent
					height: childrenRect.height
					spacing: 10
					opacity: (imageButtonsArea.containsMouse || previewFile == "") ? 1 : 0
					Behavior on opacity {
						NumberAnimation { duration: 250 }
					}
					PushButton {
						text: qsTr("Select image")
						width: Math.round(previewBackground.width / 3 * 2)
						onClicked: {
							if (previewFileChooser.selectFile()) {
								previewFile = previewFileChooser.selectedFile;
							}
						}
						height: 32
					}
					PushButton {
						text: qsTr("Clear image")
						width: Math.round(previewBackground.width / 3 * 2)
						visible: previewFile != ""
						height: visible ? 32 : 0
						onClicked: previewFile = ""
					}
				}
				FileChooser {
					id: previewFileChooser
					windowTitle: qsTr("Select preview image")
					acceptMode: FileChooser.AcceptOpen
					fileMode: FileChooser.ExistingFile
				}
				MouseArea {
					id: imageButtonsArea
					anchors.fill: parent
					hoverEnabled: true
					acceptedButtons: Qt.NoButton
				}
			}
		}
	}

	Component.onCompleted: {
		reloadSettings();
	}
	onFilterSettingsChanged: reloadSettings();
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
					gaborCount = settings.filters.length;
					updatePreview();
				}
			}
		}
	}
}

