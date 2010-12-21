/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  21.12.2010 05:42:06
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0

Item {
	id: transformationContainer
	width: 128
	height: 128
	property string source
	property double transformRotate
	property double transformScale;
	property double transformTranslateX;
	property double transformTranslateY;
	property bool open: transformationInfo.state == "open"
	Image {
		asynchronous: true
		anchors.fill: parent
		source: transformationContainer.source == "" ? "" : "image://faceimage/transformed/" + transformationContainer.source
	}
	Image {
		id: transformAnimation
		parent: main
		x: faceDetectorForeground.x + 10
		y: faceDetectorForeground.y + 10
		width: faceDetectorForeground.width - 20
		height: faceDetectorForeground.height - 20
		source: transformationContainer.source == "" ? "" : "image://faceimage/original/" + transformationContainer.source
		fillMode: Image.PreserveAspectFit
		opacity: 0
		transform: [
			Rotation { id: rotationTransform; angle: 0 },
			Scale { id: scaleTransform; xScale: 1; yScale: 1 },
			Translate { id: translateTransform; x: 0; y: 0 }
		]
		states: [
			State {
				name: "open"; when: transformationContainer.open
				ParentChange { target: transformAnimation; parent: transformationContainer; x: 0; y: 0; width: transformAnimation.sourceSize.width; height: transformAnimation.sourceSize.height }
				PropertyChanges { target: rotationTransform; angle: transformRotate }
				PropertyChanges { target: scaleTransform; xScale: transformScale; yScale: transformScale }
				PropertyChanges { target: translateTransform; x: transformTranslateX; y: transformTranslateY }
				PropertyChanges { target: transformAnimation; opacity: 0.5 }
			}
		]
		transitions: [
			Transition {
				SequentialAnimation {
					ParallelAnimation {
						ParentAnimation {
							target: transformAnimation; via: transformationContainer
							NumberAnimation { properties: "x,y, width, height"; duration: 1000 }
						}
						NumberAnimation {
							properties: "x, y, xScale, yScale"; duration: 1000
						}
						RotationAnimation {
							properties: "angle"; duration: 1000; direction: RotationAnimation.Shortest
						}
						NumberAnimation {
							properties: "opacity"; duration: 200
						}
					}
					NumberAnimation {
						properties: "opacity"; target: transformAnimation; to: 0.0; duration: 200
					}
				}
			}
		]
	}
}

