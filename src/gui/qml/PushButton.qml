/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  20.12.2010 12:12:37
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0

Item {
	id: buttonContainer
	property alias text: buttonText.text
	property bool pressed: buttonArea.pressed
	property string shape: "img/button.sci"
	property string pressedShape: "img/button-down.sci"
	signal clicked()
	BorderImage {
		source: buttonContainer.pressed ? buttonContainer.pressedShape : buttonContainer.shape
		anchors.fill: parent
		smooth: true
	}
	Text {
		id: buttonText
		anchors.fill: parent
		anchors.topMargin: buttonContainer.pressed ? 2 : 0
		color: "#444444"
		elide: Text.ElideRight; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter

		font.pixelSize: 18; font.bold: true;
		style: Text.Raised; styleColor: "#80ffffff"
	}
	MouseArea {
		id: buttonArea
		anchors.fill: parent
		onClicked: {
			buttonContainer.clicked();
		}
	}
}

