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
	BorderImage {
		id: bg
		anchors.fill: parent
		source: "img/title_border.sci"
	}
	Text {
		id: statusText
		anchors {
			top: parent.top; left: parent.left; right: parent.right
			leftMargin: bg.border.left; topMargin: bg.border.top; rightMargin: bg.border.right
		}
		color: "#fff"
		elide: Text.ElideRight; horizontalAlignment: Text.AlignHCenter
		font { pixelSize: 16; bold: true }
		style: Text.Raised; styleColor: "#111830"
		text: progressItem.statusText
	}
	Text {
		id: extendedStatus
		anchors {
			top: statusText.bottom; left: parent.left; right: parent.right
			leftMargin: bg.border.left; rightMargin: bg.border.right
		}
		color: "#eee"
		elide: Text.ElideRight; horizontalAlignment: Text.AlignHCenter
		font.pixelSize: 11
		style: Text.Raised; styleColor: "#80000000"
		text: progressItem.statusExtendedText
	}
	Item {
		anchors {
			top: extendedStatus.bottom; left: parent.left; right: parent.right; bottom: parent.bottom
			leftMargin: bg.border.left; rightMargin: bg.border.right
		}
		ProgressBar {
			id: progress
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.verticalCenter: parent.verticalCenter
			progress: progressItem.progress
		}
	}
}

