/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  11.05.2011 23:08:30
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0
import org.facedetect 1.0

CentralWindow {
	id: detection
	property string detectFile
	Flickable {
		anchors.fill: parent
		contentWidth: imageWrapper.width; contentHeight: imageWrapper.height
		Item {
			id: imageWrapper
			width: Math.max(detection.width, imageItem.width)
			height: Math.max(detection.height, imageItem.height)
			BorderImage {
				id: imageItem
				anchors.centerIn: parent
				width: (detectionImage.status == Image.Ready ? detectionImage.width : loadingImage.width) + 7
				height: (detectionImage.status == Image.Ready ? detectionImage.height : loadingImage.height) + 7
				source: "img/photo_frame.png"
				border { left: 3; top: 3; right: 4; bottom: 4 }
				Image {
					id: detectionImage
					width: 500; height: 500
					asynchronous: true
				}
				Image {
					id: loadingImage
					source: "img/photo.png"
					width: 255; height: 255
					x: 3; y: 3
					visible: detectionImage.status != Image.Ready
				}
				Column {
					anchors.centerIn: parent
					height: childrenRect.height
					opacity: (imageButtonsArea.containsMouse || detectFile == "") ? 1 : 0
					Behavior on opacity {
						NumberAnimation { duration: 250 }
					}
					PushButton {
						text: qsTr("Select image")
						width: 200; height: 32
						onClicked: {
							if (detectFileChooser.selectFile()) {
								detectFile = detectFileChooser.selectedFile;
							}
						}
					}
					PushButton {
						text: qsTr("Clear image")
						width: 200
						height: visible ? 32 : 0
						visible: detectFile != ""
						onClicked: detectFile = ""
					}
				}
				FileChooser {
					id: detectFileChooser
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
}

