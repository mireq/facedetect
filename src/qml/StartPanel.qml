/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  21.12.2010 12:00:51
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0

Item {
	Item {
		width: childrenRect.width
		height: childrenRect.height
		anchors.centerIn: parent
		Column {
			width: parent.parent.width * 0.8
			spacing: 25
			Item {
				width: parent.width
				height: 25
				Text {
					id: databaseLabel
					text: "Databáza"
					color: "#333333"
					font.pixelSize: 14; font.bold: true;
					style: Text.Raised; styleColor: "#80ffffff"
				}
				Rectangle {
					color: "#a0f8f8f8"
					radius: 3
					border.color: "#80000000"
					anchors { fill: databaseEdit; margins: -5 }
				}
				TextInput {
					id: databaseEdit
					color: "#333333"
					height: databaseLabel.height
					anchors { left: databaseLabel.right; right: parent.right; leftMargin: 10 }
					font.pixelSize: 14; font.bold: true;
					text: runtime.scanPath
				}
			}
			PushButton {
				width: 200; height: 60
				text: "Štart"
				onClicked: {
					runtime.scanPath = databaseEdit.text
					runtime.faceFileScanner.start();
					main.state = "grid";
				}
				anchors.horizontalCenter: parent.horizontalCenter
			}
		}
	}
}

