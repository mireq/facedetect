/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  20.12.2010 17:11:49
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0

Item {
	id: listItemContainer
	default property alias content: contentsItem.children
	property string title
	state: "closed"
	height: contentsItem.height + 50
	Text {
		anchors.left: listItemContainer.left
		anchors.right: listItemContainer.right
		anchors.bottom: contentsItem.top
		text: (listItemContainer.state == "open" ? "▼ " : "▶ ") + listItemContainer.title
		color: "#eeeeee"
		elide: Text.ElideRight
		font.pixelSize: 15; font.bold: true;
		style: Text.Raised; styleColor: "#80000000"
	}
	MouseArea {
		anchors.top: listItemContainer.top
		anchors.left: listItemContainer.left
		anchors.right: listItemContainer.right
		anchors.bottom: contentsItem.top
		onClicked: {
			if (listItemContainer.state == "open") {
				listItemContainer.state = "closed";
			}
			else {
				listItemContainer.state = "open";
			}
		}
	}
	Item {
		id: contentsItem
		y: 50
		width: parent.width
		opacity: 0
		height: childrenRect.height
	}
	states: [
		State {
			name: "closed"; when: listItemContainer.state == "closed"
			PropertyChanges { target: contentsItem; height: 0; opacity: 0 }
		},
		State {
			name: "open"; when: listItemContainer.state == "open"
			PropertyChanges { target: contentsItem; height: contentsItem.childrenRect.height; opacity: 1 }
		}
	]
	transitions: [
		Transition {
			NumberAnimation { properties: "x,y,width,height,opacity"; duration: 250; easing.type: Easing.InOutQuad }
			ColorAnimation { duration: 250 }
		}
	]
}

