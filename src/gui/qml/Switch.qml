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
	state: "off"
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
		MouseArea {
			anchors.fill: parent
			drag.target: switchImage; drag.axis: Drag.XAxis; drag.minimumX: 0; drag.maximumX: offImage.width - switchImage.width
			onClicked: toggleSwitch()
			onReleased: releaseSwitch()
		}
	}

	function toggleSwitch() {
		if (checkSwitch.state == "off") {
			checkSwitch.state = "on";
		}
		else {
			checkSwitch.state = "off";
		}
	}

	function releaseSwitch() {
		if (switchImage.x < (offImage.width - switchImage.width) / 2) {
			checkSwitch.state = "on";
		}
		else {
			checkSwitch.state = "off";
		}
		toggleSwitch();
	}

	states: [
		State {
			name: "on"
			PropertyChanges { target: switchImage; x: offImage.width - switchImage.width }
			PropertyChanges { target: checkSwitch; on: true }
		},
		State {
			name: "off"
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

