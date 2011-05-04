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
	property string text
	property bool pressed: buttonArea.pressed && enabled
	property bool checked: false
	property bool enabled: true
	property string shape: "img/pbutton_normal.sci"
	property string pressedShape: "img/pbutton_pressed.sci"
	property int fontPixelSize: 18
	signal clicked()
	width: textSize.paintedWidth + bg.border.left + bg.border.right; height: 32
	BorderImage {
		id: bg
		source: (buttonContainer.pressed || buttonContainer.checked) ? buttonContainer.pressedShape : buttonContainer.shape
		anchors.fill: parent
		smooth: true
		opacity: buttonContainer.enabled ? 1 : 0.5
	}
	Text {
		id: buttonText
		anchors {
			verticalCenterOffset: buttonContainer.pressed ? 2 : 0
			verticalCenter: parent.verticalCenter
			left: parent.left; right: parent.right
			leftMargin: bg.border.left; rightMargin: bg.border.right
		}
		color: "#444444"
		elide: Text.ElideRight; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
		font.pixelSize: fontPixelSize; font.bold: true;
		style: Text.Raised; styleColor: "#80ffffff"
		opacity: buttonContainer.enabled ? 1 : 0.5
		text: buttonContainer.text
	}
	Text {
		id: textSize
		font.pixelSize: fontPixelSize; font.bold: true;
		style: Text.Raised
		text: buttonContainer.text
		visible: false
	}
	MouseArea {
		id: buttonArea
		anchors.fill: parent
		onClicked: {
			if (buttonContainer.enabled) {
				buttonContainer.clicked();
			}
		}
	}
}

