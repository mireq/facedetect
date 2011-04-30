/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  20.12.2010 17:05:26
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0

Item {
	id: metadataContainer
	property variant metadata: null
	property int fontSize: 12
	property string fontColor: "#eeeeee"
	property string leftEyeStr
	property string rightEyeStr
	property string noseStr
	property string mouthStr
	width: parent.width
	height: childrenRect.height
	onMetadataChanged: {
		if (metadata != null) {
			leftEyeStr = metadata[0]["leftEyeX"] + ", " + metadata[0]["leftEyeY"]
			rightEyeStr = metadata[0]["rightEyeX"] + ", " + metadata[0]["rightEyeY"]
			noseStr = metadata[0]["noseX"] + ", " + metadata[0]["noseY"]
			mouthStr = metadata[0]["mouthX"] + ", " + metadata[0]["mouthY"]
		}
	}
	Row {
		spacing: 10
		Column {
			width: metadataContainer.width / 3 - 10
			Text {
				width: parent.width
				text: qsTr("Left eye")
				horizontalAlignment: Text.AlignRight
				font.pixelSize: fontSize; font.bold: true; color: fontColor
			}
			Text {
				width: parent.width
				text: qsTr("Right eye")
				horizontalAlignment: Text.AlignRight
				font.pixelSize: fontSize; font.bold: true; color: fontColor
			}
			Text {
				width: parent.width
				text: qsTr("Nose")
				horizontalAlignment: Text.AlignRight
				font.pixelSize: fontSize; font.bold: true; color: fontColor
			}
			Text {
				width: parent.width
				text: qsTr("Mouth")
				horizontalAlignment: Text.AlignRight
				font.pixelSize: fontSize; font.bold: true; color: fontColor
			}
		}
		Column {
			width: (metadataContainer.width / 3) * 2
			Text {
				width: parent.width
				font.pixelSize: fontSize; color: fontColor
				text: leftEyeStr
			}
			Text {
				width: parent.width
				font.pixelSize: fontSize; color: fontColor
				text: rightEyeStr
			}
			Text {
				width: parent.width
				font.pixelSize: fontSize; color: fontColor
				text: noseStr
			}
			Text {
				width: parent.width
				font.pixelSize: fontSize; color: fontColor
				text: mouthStr
			}
		}
	}
}

