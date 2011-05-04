/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  03.05.2011 19:39:50
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0

Image {
	id: closeImage
	property bool open: true
	source: "img/arrow_down.png"
	anchors.verticalCenter: parent.verticalCenter
	transform: Rotation {
		id: rotateTransform
		origin.x: closeImage.width / 2; origin.y: closeImage.height / 2
	}
	MouseArea {
		anchors.fill: parent
		onClicked: {
			open = !open;
		}
	}
	states: [
		State {
			name: "closed"; when: closeImage.open == false
			PropertyChanges { target: closeImage; source: "img/arrow_right.png" }
		},
		State {
			name: "open"; when: closeImage.open == true
			PropertyChanges { target: closeImage; source: "img/arrow_down.png" }
		}
	]
	transitions: [
		Transition {
			from: "open"; to: "closed"
			SequentialAnimation {
				NumberAnimation {
					from: 0; to: -45
					target: rotateTransform; duration: 50; property: "angle"
				}
				PropertyAction {
					target: closeImage; property: "source"
				}
				PropertyAction {
					target: rotateTransform; property: "angle"; value: 45
				}
				NumberAnimation {
					from: 45; to: 0
					target: rotateTransform; duration: 50; property: "angle"
				}
			}
		},
		Transition {
			from: "closed"; to: "open"
			SequentialAnimation {
				NumberAnimation {
					from: 0; to: 45
					target: rotateTransform; duration: 50; property: "angle"
				}
				PropertyAction {
					target: closeImage; property: "source"
				}
				PropertyAction {
					target: rotateTransform; property: "angle"; value: -45
				}
				NumberAnimation {
					from: -45; to: 0
					target: rotateTransform; duration: 50; property: "angle"
				}
			}
		}
	]
}

