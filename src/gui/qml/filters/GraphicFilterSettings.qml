/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  03.05.2011 16:11:41
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0
import ".."

GroupBox {
	property alias enabled: sw.on
	titleRight: Switch { id: sw; anchors.verticalCenter: parent.verticalCenter }
	titleLeft: Image {
		id: closeImage
		source: "../img/arrow_down.png"
		state: "open"
		anchors.verticalCenter: parent.verticalCenter
		transform: Rotation {
			id: rotateTransform
			origin.x: closeImage.width / 2; origin.y: closeImage.height / 2
		}
		MouseArea {
			anchors.fill: parent
			onClicked: {
				if (closeImage.state == "open") {
					closeImage.state = "closed";
				}
				else {
					closeImage.state = "open";
				}
			}
		}
		states: [
			State {
				name: "closed"; when: closeImage.state == "closed"
				PropertyChanges { target: closeImage; source: "../img/arrow_right.png" }
			},
			State {
				name: "open"; when: closeImage.state == "open"
				PropertyChanges { target: closeImage; source: "../img/arrow_down.png" }
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
	closed: !(sw.on && closeImage.state == "open"); clip: true
}

