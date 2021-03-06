/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  19.12.2010 16:15:40
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0

Item {
	id: faceDelegate
	property int size: 192
	property variant facesModel: model
	width: size
	height: size
	//property alias state: faceImageWrapper.state
	//property string state: "grid"
	state: "grid"

	BorderImage {
		id: imageBorder
		property double xSpace: Math.max(1.0 - (faceImage.sourceSize.width / faceImage.sourceSize.height), 0.0)
		property double ySpace: Math.max(1.0 - (faceImage.sourceSize.height / faceImage.sourceSize.width), 0.0)
		anchors.fill: parent
		anchors.leftMargin: isNaN(xSpace) ? 0 : xSpace * (size / 2 - 10)
		anchors.topMargin: isNaN(ySpace) ? 0 : ySpace * (size / 2 - 10)
		anchors.rightMargin: anchors.leftMargin
		anchors.bottomMargin: anchors.topMargin
		source: "img/imgbackground.sci"
	}

	function hide() {
		state = "grid";
	}

	Item {
		id: faceImageWrapper
		x: 10
		y: 10
		width: faceDelegate.size - 20
		height: faceDelegate.size - 20
		Item {
			anchors.fill: parent
			anchors.margins: 10
			Image {
				id: faceImage
				source: "image://faceimage/original/" + model.image
				asynchronous: true
				fillMode: Image.PreserveAspectFit
				anchors.fill: parent
			}
		}
		Item {
			id: controlPointsWrapper
			anchors.fill: parent
			anchors.margins: 10
			property alias sourceSize: faceImage.sourceSize
		}
		LoadingIndicator {
			anchors.centerIn: parent
			running: faceImage.status != Image.Ready
		}
		MouseArea {
			anchors.fill: parent
			onClicked: {
				if (faceDelegate.state == "zoom") {
					hide();
				}
				else if (faceImage.status == Image.Ready) {
					faceDelegate.state = "zoom";
					controlPointsDisplay.leftEyeX = model.faceData[0].leftEyeX;
					controlPointsDisplay.leftEyeY = model.faceData[0].leftEyeY;
					controlPointsDisplay.rightEyeX = model.faceData[0].rightEyeX;
					controlPointsDisplay.rightEyeY = model.faceData[0].rightEyeY;
					controlPointsDisplay.noseX = model.faceData[0].noseX;
					controlPointsDisplay.noseY = model.faceData[0].noseY;
					controlPointsDisplay.mouthX = model.faceData[0].mouthX;
					controlPointsDisplay.mouthY = model.faceData[0].mouthY;
				}
				faceDelegate.GridView.view.currentIndex = index;
			}
		}
	}
	states: [
		State {
			name: "grid"; when: state == "grid"
			ParentChange { target: faceImageWrapper; parent: faceDelegate; x: 10; y: 10; width: faceDelegate.size - 20; height: faceDelegate.size - 20 }
			ParentChange { target: controlPointsDisplay; parent: faceDetectorForeground }
		},
		State {
			name: "zoom"; when: state == "zoom"
			ParentChange { target: faceImageWrapper; parent: faceDetectorForeground; x: 0; y: 0; width: faceDetectorForeground.width; height: faceDetectorForeground.height }
			ParentChange { target: controlPointsDisplay; parent: controlPointsWrapper }
		}
	]
	transitions: [
		Transition {
			ParallelAnimation {
				ParentAnimation {
					target: controlPointsDisplay; via: controlPointsWrapper
					NumberAnimation { properties: "x,y, width, height"; duration: 250 }
				}
				ParentAnimation {
					target: faceImageWrapper; via: faceDetectorForeground
					NumberAnimation { properties: "x,y, width, height"; duration: 250 }
				}
			}
		}
	]
}

