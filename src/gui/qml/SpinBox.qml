/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  03.05.2011 08:57:18
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0
import org.facedetect 1.0

Item {
	id: spinBox
	width: 128; height: 32
	property real step: 1.0
	property real value: 0.0
	property real minValue: 0.0
	property real maxValue: 100.0
	property int decimals: 6
	property int dragStepSize: 10
	property bool _downEnabled: value > minValue
	property bool _upEnabled: value < maxValue

	function increment() {
		var newValue = value + step;
		if (newValue > maxValue) {
			newValue = maxValue;
		}
		value = roundValue(newValue, decimals);
	}

	function decrement() {
		var newValue = value - step;
		if (newValue < minValue) {
			newValue = minValue;
		}
		value = roundValue(newValue, decimals);
	}

	function setValue(nVal) {
		var newValue = parseFloat(nVal);
		if (newValue > maxValue) {
			newValue = maxValue;
		}
		if (newValue < minValue) {
			newValue = minValue;
		}
		value = roundValue(newValue, decimals);
	}

	function roundValue(val, dec) {
		return Math.round(val * Math.pow(10, dec)) / Math.pow(10, dec);
	}

	BorderImage {
		id: backgroundImage
		source: "img/input.sci"
		anchors.fill: parent
	}

	// Tlačidlá pre manipuláciu s hodnotou
	Image {
		id: downImage
		source: "img/spin_down.png"
		opacity: _downEnabled ? 1 : 0.5
		anchors {
			left: parent.left
			verticalCenter: parent.verticalCenter
			verticalCenterOffset: downMouseArea.pressed && _downEnabled ? 1 : 0
		}
		MouseArea {
			id: downMouseArea
			anchors.fill: parent
			onClicked: {
				spinBox.decrement();
				spinBox.forceActiveFocus();
			}
		}
	}
	Image {
		id: upImage
		source: "img/spin_up.png"
		opacity: _upEnabled ? 1 : 0.5
		anchors {
			right: parent.right
			verticalCenter: parent.verticalCenter
			verticalCenterOffset: upMouseArea.pressed && _upEnabled ? 1 : 0
		}
		MouseArea {
			id: upMouseArea
			anchors.fill: parent
			onClicked: {
				spinBox.increment();
				spinBox.forceActiveFocus();
			}
		}
	}

	// Textový vstup
	TextInput {
		id: lineEdit
		anchors {
			fill: parent
			leftMargin: backgroundImage.border.left + downImage.width
			rightMargin: backgroundImage.border.right + upImage.width
			topMargin: backgroundImage.border.top
			bottomMargin: backgroundImage.border.bottom
		}
		font.pixelSize: (parent.height - backgroundImage.border.top - backgroundImage.border.top) * 2 / 3
		validator: DoubleValidator { decimals: spinBox.decimals }
		text: spinBox.value
		color: "black"; horizontalAlignment: TextInput.AlignHCenter
		selectByMouse: true
	Item {
		id: dragTarget
	}

		MouseArea {
			property variant oldPos: null
			property bool isDrag: false
			drag.axis: Drag.XAxis
			drag.target: dragTarget
			anchors.fill: parent
			onClicked: {
				if (!isDrag) {
					lineEdit.forceActiveFocus();
					lineEdit.selectAll();
				}
			}
			onPressed: {
				isDrag = false;
				oldPos = Qt.point(mouseX, mouseY);
			}
			onPositionChanged: {
				var xMove = mouseX - oldPos.x;
				if (Math.abs(xMove) >= dragStepSize) {
					var steps = Math.floor(xMove / dragStepSize);
					var up = steps > 0;
					var stepCount = Math.abs(steps);
					for (var step = 0; step < stepCount; ++step) {
						if (up) {
							spinBox.increment();
						}
						else {
							spinBox.decrement();
						}
						isDrag = true;
					}
					oldPos = Qt.point(oldPos.x + steps * dragStepSize, mouseY);
				}
			}
			enabled: !lineEdit.activeFocus
		}

		CursorArea.cursor: activeFocus ? Qt.IBeamCursor : Qt.ArrowCursor
		onTextChanged: spinBox.setValue(text)
		onActiveFocusChanged: {
			if (!activeFocus) {
				lineEdit.select(0, 0);
				spinBox.setValue(text);
			}
		}
	}
}

