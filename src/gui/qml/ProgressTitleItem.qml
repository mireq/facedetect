/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  07.05.2011 17:42:42
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0

Item {
	id: progressItem
	property double progress: 0
	property string statusText: ""
	property string statusExtendedText: ""
	width: 200; height: 50
	Text {
		id: statusText
		anchors { top: parent.top; left: parent.left; right: parent.right }
		color: "#555555"
		elide: Text.ElideRight; horizontalAlignment: Text.AlignHCenter
		font { pixelSize: 22; bold: true }
		style: Text.Raised; styleColor: "#ffffff"
		text: progressItem.statusText
	}
	Text {
		id: extendedStatus
		anchors { top: statusText.bottom; left: parent.left; right: parent.right }
		color: "#777777"
		elide: Text.ElideRight; horizontalAlignment: Text.AlignHCenter
		font.pixelSize: 12
		style: Text.Raised; styleColor: "#ffffff"
		text: progressItem.statusExtendedText
	}
	Item {
		anchors { top: extendedStatus.bottom; left: parent.left; right: parent.right; bottom: parent.bottom }
		ProgressBar {
			id: progress
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.verticalCenter: parent.verticalCenter
			progress: progressItem.progress
		}
	}
}

