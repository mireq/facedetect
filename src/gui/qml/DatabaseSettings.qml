/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  30.04.2011 17:59:15
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0
import org.facedetect 1.0

CentralWindow {
	id: settingsWindow
	anchors.margins: 10
	ListView {
		id: listView
		anchors.fill: parent
		model: itemsModel
	}

	VisualItemModel {
		id: itemsModel
		GroupBox {
			width: listView.width
			title: qsTr("Path settings")
			closed: false
			clip: true
			Item {
				id: pathSettings
				anchors { left: parent.left; right: parent.right; margins: 5 }
				height: childrenRect.height + 10
				Grid {
					id : pathSettingsGrid
					property int lineHeight: 32
					property string textColor: "black"
					y: 5
					columns: 2
					spacing: 5
					Text {
						color: pathSettingsGrid.textColor
						text: qsTr("Face database path")
						elide: Text.ElideRight
						width: pathSettings.width / 3 - pathSettingsGrid.spacing
						height: pathSettingsGrid.lineHeight
						verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignRight
					}
					FileSelector {
						id: facesPathSelector
						width: pathSettings.width / 3 * 2
						height: pathSettingsGrid.lineHeight
						color: pathSettingsGrid.textColor
						fileMode: FileChooser.Directory;
						chooserTitle: qsTr("Select face database path")
						Component.onCompleted: selectedFile = runtime.facesPath;
						onSelectedFileChanged: {
							runtime.facesPath = selectedFile;
						}
					}
					Text {
						color: pathSettingsGrid.textColor
						text: qsTr("Non face path")
						elide: Text.ElideRight
						width: pathSettings.width / 3 - pathSettingsGrid.spacing
						height: pathSettingsGrid.lineHeight
						verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignRight
					}
					FileSelector {
						id: nonFacesPathSelector
						width: pathSettings.width / 3 * 2
						height: pathSettingsGrid.lineHeight
						color: pathSettingsGrid.textColor
						fileMode: FileChooser.Directory;
						chooserTitle: qsTr("Select non face path")
						Component.onCompleted: selectedFile = runtime.nonFacesPath;
						onSelectedFileChanged: {
							runtime.nonFacesPath = selectedFile;
						}
					}
				}
			}
		}
	}
}

