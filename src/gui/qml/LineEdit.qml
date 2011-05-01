/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  01.05.2011 14:23:17
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0

Item {
	id: textInput
	property alias color: inputElement.color
	property alias text: inputElement.text
	width: 200; height: 32
	BorderImage {
		id: backgroundImage
		source: "img/input.sci"
		anchors.fill: parent
	}
	TextInput {
		id: inputElement
		anchors {
			fill: parent
			leftMargin: backgroundImage.border.left
			rightMargin: backgroundImage.border.right
			topMargin: backgroundImage.border.top
			bottomMargin: backgroundImage.border.bottom
		}
		font.pixelSize: (parent.height - backgroundImage.border.top - backgroundImage.border.top) * 2 / 3
		color: "black"
	}
}

