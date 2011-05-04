/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  03.05.2011 19:00:01
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0
import "../"

GroupBox {
	property int lineHeight: 32
	property string textColor: "black"
	width: parent.width
	background: "img/item_header.sci"
	titleLeft: ExpandButton{ id: closeImage; open: false }
	closed: !closeImage.open
	Item {
		Component {
			id: label
			Text {
				color: "black"
				elide: Text.ElideRight
				width: grid.width / 2; height: lineHeight
				verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignRight
			}
		}
	}
	Grid {
		id: grid
		//property bool more: moreButton.checked
		columns: 2; spacing: 5
		height: childrenRect.height + 5; y: 5
		anchors { left: parent.left; right: parent.right }
		Loader { sourceComponent: label; Component.onCompleted: item.text = qsTr("Lambda") }
		Item {
			width: grid.width / 2 - grid.spacing; height: lineHeight
			SpinBox {
				id: lambdaValue
				anchors { verticalCenter: parent.verticalCenter; left: parent.left }
			}
		}
		Loader { sourceComponent: label; Component.onCompleted: item.text = qsTr("Theta") }
		Item {
			width: grid.width / 2 - grid.spacing; height: lineHeight
			SpinBox {
				id: thetaValue
				anchors { verticalCenter: parent.verticalCenter; left: parent.left }
			}
		}
	}
	Grid {
		id: gridMore
		property bool more: moreButton.checked || lambdaValue.value != sigmaValue.value * 2.0
		columns: 2; spacing: 5
		height: childrenRect.height + 5
		anchors { left: parent.left; right: parent.right; top: grid.bottom }
		Loader { sourceComponent: label; Component.onCompleted: item.text = qsTr("Psi") }
		Item {
			width: grid.width / 2 - grid.spacing; height: lineHeight
			SpinBox {
				id: psiValue
				anchors { verticalCenter: parent.verticalCenter; left: parent.left }
			}
		}
		Loader { sourceComponent: label; Component.onCompleted: item.text = qsTr("Sigma") }
		Item {
			width: grid.width / 2 - grid.spacing; height: lineHeight
			SpinBox {
				id: sigmaValue
				anchors { verticalCenter: parent.verticalCenter; left: parent.left }
			}
		}
		Loader { sourceComponent: label; Component.onCompleted: item.text = qsTr("Gamma") }
		Item {
			width: grid.width / 2 - grid.spacing; height: lineHeight
			SpinBox {
				anchors { verticalCenter: parent.verticalCenter; left: parent.left }
			}
		}
		Loader { sourceComponent: label; Component.onCompleted: item.text = qsTr("Luminance") }
		Item {
			width: grid.width / 2 - grid.spacing; height: lineHeight
			SpinBox {
				id: lumValue
				anchors { verticalCenter: parent.verticalCenter; left: parent.left }
			}
		}
		states: [
			State {
				name: "open"; when: gridMore.more == true
				PropertyChanges { target: gridMore; opacity: 1; height: gridMore.childrenRect.height + 5 }
			},
			State {
				name: "closed"; when: gridMore.more == false
				PropertyChanges { target: gridMore; opacity: 0; height: 0 }
			}
		]
		transitions: [
			Transition {
				SequentialAnimation {
					PropertyAction { target: gridMore; property: "clip"; value: true }
					ParallelAnimation {
						NumberAnimation { properties: "height, opacity"; duration: 200 }
					}
					PropertyAction { target: gridMore; property: "clip"; value: false }
				}
			}
		]
	}
	Item {
		id: moreButtonContainer
		property bool enabled: lambdaValue.value == sigmaValue.value * 2.0
		clip: true
		anchors { left: parent.left; right: parent.right; top: gridMore.bottom }
		height: lineHeight
		PushButton {
			id: moreButton
			enabled: parent.enabled
			anchors { left: parent.left; right: parent.right; top: parent.top }
			height: lineHeight
			shape: "img/pbutton_normal.sci"
			pressedShape: "img/pbutton_checked.sci"
			text: qsTr("More ...")
			onClicked: {
				checked = !checked;
			}
		}
		states: [
			State {
				name: "disabled"; when: moreButtonContainer.enabled == false
				PropertyChanges { target: moreButtonContainer; height: 0 }
			}
		]
		transitions: Transition {
			NumberAnimation { properties: "height"; duration: 200 }
		}
	}
}

