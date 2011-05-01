/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  01.05.2011 19:51:03
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0
import org.facedetect 1.0

Item {
	width: 200; height: 32
	property alias color: lineEdit.color
	property alias selectedFile: fileChooser.selectedFile
	property alias acceptMode: fileChooser.acceptMode
	property alias fileMode: fileChooser.fileMode
	property alias chooserTitle: fileChooser.windowTitle
	LineEdit {
		id: lineEdit
		text: fileChooser.selectedFile
		onTextChanged: {
			fileChooser.selectedFile = text;
		}
		anchors {
			left: parent.left; top: parent.top;
			bottom: parent.bottom; right: chooserButton.left
		}
	}
	PushButton {
		id: chooserButton
		anchors { top: parent.top; bottom: parent.bottom; right: parent.right }
		width: parent.width / 6
		shape: "img/pbutton_normal.sci"
		pressedShape: "img/pbutton_checked.sci"
		text: "..."
		onClicked: {
			checked = true;
			fileChooser.selectFile();
			checked = false;
		}
	}
	FileChooser {
		id: fileChooser
	}
}

