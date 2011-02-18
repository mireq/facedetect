/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  16.02.2011 19:12:13
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0

Item {
	id: titleWidget

	// Text titulku
	property alias text: titleText.text
	// Ľavá časť titulku
	property Item leftWidget: null
	// Pravá časť titulku
	property Item rightWidget: null
	// Stredná časť titulku (ak je null použije sa text titulku)
	property Item centralWidget: null

	property Item _oldLeftWidget: null
	property Item _oldRightWidget: null
	property Item _oldCentralWidget: null
	onLeftWidgetChanged: {
		if (_oldLeftWidget != null) {
			_oldLeftWidget.visible = false;
		}
		if (leftWidget != null) {
			leftWidget.visible = true;
		}
		_oldLeftWidget = leftWidget;
	}
	onRightWidgetChanged: {
		if (_oldRightWidget != null) {
			_oldRightWidget.visible = false;
		}
		if (rightWidget != null) {
			rightWidget.visible = true;
		}
		_oldRightWidget = rightWidget;
	}
	onCentralWidgetChanged: {
		if (_oldCentralWidget != null) {
			_oldCentralWidget.visible = false;
		}
		if (centralWidget != null) {
			centralWidget.parent = centralWidgetItem;
			centralWidget.visible = true;
		}
		_oldCentralWidget = centralWidget;
	}

	width: 200; height: 80

	Item {
		id: leftWidgetItem
		width: leftWidget == null ? 0 : leftWidget.width
		anchors { left: parent.left; top: parent.top; bottom: parent.bottom }
		children: leftWidget == null ? [] : leftWidget
	}
	Item {
		id: rightWidgetItem
		width: rightWidget == null ? 0 : rightWidget.width
		anchors { right: parent.right; top: parent.top; bottom: parent.bottom }
		children: rightWidget == null ? [] : rightWidget
	}
	Item {
		id: centralWidgetItem
		anchors { left: leftWidgetItem.right; right: rightWidgetItem.left; top: parent.top; bottom: parent.bottom }
		children: centralWidget == null ? _titleText : centralWidget
		// Text titulku
		property Item _titleText: Text {
			id: titleText
			width: centralWidgetItem.width; height: centralWidgetItem.height
			visible: titleWidget.centralWidget == null
			verticalAlignment: Text.AlignVCenter
			horizontalAlignment:  Text.AlignHCenter
			elide: Text.ElideRight
			font.pixelSize: 17; font.bold: true
			color: "#555555"
		}
	}
}

