/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  20.12.2010 13:55:39
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0

Item {
	property double sourceW: parent.sourceSize != undefined ? parent.sourceSize.width : 0
	property double sourceH: parent.sourceSize != undefined ? parent.sourceSize.height : 0
	property double leftEyeX
	property double leftEyeY
	property double rightEyeX
	property double rightEyeY
	property double noseX
	property double noseY
	property double mouthX
	property double mouthY
	function transformXCoord(x) {
		var width = parent.width;
		var freeSpace = 0;
		var sourceAspect = sourceW / sourceH;
		var parentAspect = parent.width / parent.height;
		if (sourceAspect < parentAspect) {
			freeSpace = parent.width - (sourceAspect / parentAspect) * parent.width
		}
		return (width - freeSpace) / sourceW * x + freeSpace / 2;
	}
	function transformYCoord(y) {
		var height = parent.height;
		var freeSpace = 0;
		var sourceAspect = sourceW / sourceH;
		var parentAspect = parent.width / parent.height;
		if (sourceAspect > parentAspect) {
			freeSpace = parent.height - (parentAspect / sourceAspect) * parent.height
		}
		return (height - freeSpace) / sourceH * y + freeSpace / 2;
	}
	Item {
		width: 0
		height: 0
		x: transformXCoord(leftEyeX)
		y: transformYCoord(leftEyeY)
		Image {
			source: "img/pointer.png"
			anchors.centerIn: parent
		}
	}
	Item {
		width: 0
		height: 0
		x: transformXCoord(rightEyeX)
		y: transformYCoord(rightEyeY)
		Image {
			source: "img/pointer.png"
			anchors.centerIn: parent
		}
	}
	Item {
		width: 0
		height: 0
		x: transformXCoord(noseX)
		y: transformYCoord(noseY)
		Image {
			source: "img/pointer.png"
			anchors.centerIn: parent
		}
	}
	Item {
		width: 0
		height: 0
		x: transformXCoord(mouthX)
		y: transformYCoord(mouthY)
		Image {
			source: "img/pointer.png"
			anchors.centerIn: parent
		}
	}
}

