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
	property double extendedProgress: -1
	property string statusText: ""
	property string statusExtendedText: ""
	property string progressText: ""
	property int progressMarginSize: 0
	property bool progressMarginEnabled: statusExtendedText != "" && progressText != ""
	property int progressMargin: progressMarginEnabled ? progressMarginSize : 0
	onProgressMarginEnabledChanged: {
		if (progressMarginEnabled) {
			progressMarginSize = Math.max(extendedStatus.paintedWidth, progressText.paintedWidth);
		}
	}
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
	Item {
		id: statusRow
		anchors {
			top: statusText.bottom; left: parent.left; right: parent.right
			leftMargin: bg.border.left; rightMargin: bg.border.right
		}
		height: extendedStatus.height
		Text {
			id: extendedStatus
			anchors {
				left: extendedProgress.visible ? undefined : parent.left
				right: extendedProgress.visible ? extendedProgress.left : parent.right
				rightMargin: 5
			}
			color: "#eee"
			elide: Text.ElideRight
			horizontalAlignment: extendedProgress.visible ? Text.AlignRight : Text.AlignHCenter
			font.pixelSize: 11
			style: Text.Raised; styleColor: "#80000000"
			text: progressItem.statusExtendedText
			visible: text != ""
		}
		ProgressBar {
			id: extendedProgress
			anchors { left: parent.left; right: parent.right; verticalCenter: parent.verticalCenter }
			anchors.leftMargin: progressMargin > 0 ? progressMargin + 5 : (extendedStatus.visible ? extendedStatus.paintedWidth + 5 : 0)
			progress: progressItem.extendedProgress < 0 ? 0 : progressItem.extendedProgress
			visible: progressItem.extendedProgress >= 0
		}
	}
	Item {
		anchors {
			top: statusRow.bottom; left: parent.left; right: parent.right; bottom: parent.bottom
			leftMargin: bg.border.left; rightMargin: bg.border.right
		}
		Text {
			id: progressText
			anchors {
				left: progress.visible ? undefined : parent.left
				right: progress.visible ? progress.left : undefined
				rightMargin: 5
				verticalCenter: parent.verticalCenter
			}
			color: "#eee"
			elide: Text.ElideRight
			font.pixelSize: 11
			horizontalAlignment: progress.visible ? Text.AlignRight : Text.AlignHCenter
			style: Text.Raised; styleColor: "#80000000"
			text: progressItem.progressText
			visible: text != ""
		}
		ProgressBar {
			id: progress
			anchors { left: parent.left; right: parent.right; verticalCenter: parent.verticalCenter }
			anchors.leftMargin: progressMargin > 0 ? progressMargin + 5 : (progressText.visible ? progressText.paintedWidth + 5 : 0)
			progress: progressItem.progress
		}
	}
}

