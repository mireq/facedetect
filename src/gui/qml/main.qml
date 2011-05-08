/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  06.05.2011 14:19:27
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0
import org.facedetect 1.0

Item {
	id: main
	width: 1024
	height: 768

	TabWidget {
		id: tabWidget
		property alias topBar: topBarItem
		anchors { left: parent.left; top: topBar.bottom; right: parent.right; bottom: parent.bottom }
		DatabaseSettings {
			title: qsTr("Database settings")
			anchors.fill: parent
		}
		FaceDatabaseView {
			title: qsTr("Face database")
			anchors.fill: parent
		}
		FiltersEdit {
			title: qsTr("Filters")
			anchors.fill: parent
		}
		Training {
			title: qsTr("Training")
			anchors.fill: parent
		}
	}
	Rectangle {
		id: topBarItem
		anchors { left: parent.left; top: parent.top; right: parent.right }
		height: 72
		color: "#dedede"
		Image {
			source: "img/title_bg.png"
			anchors.fill: parent
		}
		TitleWidget {
			id: titleWidget
			anchors.fill: parent
			text: (tabWidget.currentItem != null && tabWidget.currentItem.title != undefined) ? tabWidget.currentItem.title : ""
			leftWidget: (tabWidget.currentItem != null && tabWidget.currentItem.leftTitleWidget != undefined) ? tabWidget.currentItem.leftTitleWidget : null
			rightWidget: (tabWidget.currentItem != null && tabWidget.currentItem.rightTitleWidget != undefined) ? tabWidget.currentItem.rightTitleWidget : null
			centralWidget: (tabWidget.currentItem != null && tabWidget.currentItem.centralTitleWidget != undefined) ? tabWidget.currentItem.centralTitleWidget : null
		}
	}
}
