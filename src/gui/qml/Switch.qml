/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  01.05.2011 15:55:55
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0

Item {
	id: checkSwitch
	property bool on: false
	width: offImage.width; height: offImage.height
	MouseArea {
		anchors.fill: parent
		onClicked: toggleSwitch()
	}
	Item {
		id: offItem
		width: offImage.width - onItem.width
		height: parent.height
		anchors.right: parent.right
		Image {
			id: offImage
			source: "img/switch_off.png"
			anchors.right: parent.right
		}
	}
	Item {
		id: onItem
		width: switchImage.x / (offImage.width - switchImage.width) * offImage.width
		height: parent.height
		anchors.left: parent.left
		clip: true
		Image {
			id: onImage
			source: "img/switch_on.png"
			anchors.left: parent.left
		}
	}
	Image {
		id: switchImage
		source: "img/switch.png"
		Text {
			id: switchText
			font.pixelSize: 17; font.bold: true
			text: on ? "1" : "0"
			style: Text.Raised; color: "#444"; styleColor: "#80ffffff"
			anchors.centerIn: parent
		}
		MouseArea {
			anchors.fill: parent
			drag.target: switchImage; drag.axis: Drag.XAxis; drag.minimumX: 0; drag.maximumX: offImage.width - switchImage.width
			onClicked: toggleSwitch()
			onReleased: releaseSwitch()
		}
	}

	function toggleSwitch() {
		checkSwitch.on = !checkSwitch.on;
	}

	function releaseSwitch() {
		if (switchImage.x < (offImage.width - switchImage.width) / 2) {
			checkSwitch.on = true;
		}
		else {
			checkSwitch.on = false;
		}
		toggleSwitch();
	}

	states: [
		State {
			name: "on"; when: checkSwitch.on == true
			PropertyChanges { target: switchImage; x: offImage.width - switchImage.width }
			PropertyChanges { target: checkSwitch; on: true }
		},
		State {
			name: "off"; when: checkSwitch.on == false
			PropertyChanges { target: switchImage; x: 0 }
			PropertyChanges { target: checkSwitch; on: false }
		}
	]
	transitions: [
		Transition {
			NumberAnimation { properties: "x"; duration: 150; easing.type: Easing.InOutQuad }
		}
	]
}

