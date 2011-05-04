/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  01.05.2011 11:01:32
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0

Item {
	id: groupContainer
	default property alias content: contentsItem.children
	property string title
	property string background: "img/groupbox.sci"
	property alias titleLeft: titleLeftItem.children
	property alias titleRight: titleRightItem.children
	property bool closed: true
	height: contentsItem.height + bgImage.border.top + bgImage.border.bottom
	BorderImage {
		id: bgImage
		source: background
		anchors.fill: parent
	}
	Item {
		width: parent.width; height: bgImage.border.top
		Item {
			id: titleContainer
			anchors { leftMargin: 6; rightMargin: 6; topMargin: 2; fill: parent }
			Item {
				id: titleLeftItem
				width: childrenRect.width; height: parent.height
				anchors.left: parent.left
			}
			Text {
				id: titleText
				font.pixelSize: 17; font.bold: true
				text: groupContainer.title
				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter
				height: parent.height
				style: Text.Raised; color: "#444"; styleColor: "#80ffffff"
				elide: Text.ElideRight
				anchors { left: titleLeftItem.right; right: titleRightItem.left }
			}
			Item {
				id: titleRightItem
				width: childrenRect.width; height: parent.height
				anchors.right: parent.right
			}
		}
	}
	Item {
		id: contentsItem
		x: bgImage.border.left; y: bgImage.border.top
		width: parent.width - bgImage.border.left - bgImage.border.right
		height: 0; opacity: 0
	}
	states: [
		State {
			name: "closed"; when: groupContainer.closed == true
			PropertyChanges { target: contentsItem; height: 0; opacity: 0 }
		},
		State {
			name: "open"; when: groupContainer.closed == false
			PropertyChanges { target: contentsItem; height: contentsItem.childrenRect.height; opacity: 1 }
		}
	]
	transitions: [
		Transition {
			NumberAnimation { properties: "x,y,width,height,opacity"; duration: 250; easing.type: Easing.InOutQuad }
		}
	]
}

