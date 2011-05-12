/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  11.05.2011 23:08:30
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0
import org.facedetect 1.0

CentralWindow {
	id: detection
	property string detectFile
	property bool detectionRunning: runtime.faceDetector == null ? false : runtime.faceDetector.running
	property double detectionProgress: runtime.faceDetector == null ? 0 : runtime.faceDetector.progress
	property bool netInitialized: runtime.neuralNet.initialized
	property int _cancelButtonTranslate: -tabWidget.topBar.height
	property int _infoButtonTranslate: -tabWidget.topBar.height
	Component {
		id: progressWidgetComponent
		ProgressTitleItem {
			statusText: qsTr("Detection")
			anchors.fill: parent
			progress: detection.detectionProgress
		}
	}
	Component {
		id: cancelButtonComponent
		Item {
			width: 192; anchors { top: parent.top; bottom: parent.bottom }
			visible: false
			PushButton {
				id: infoButton
				text: qsTr("Cancel")
				anchors { fill: parent; margins: 5 }
				shape: "img/button.sci"; pressedShape: "img/button-down.sci"
				onClicked: {
					runtime.stopFaceDetector();
				}
				transform: Translate { y: detection._cancelButtonTranslate }
			}
		}
	}
	Component {
		id: infoButtonComponent
		Item {
			width: 192; anchors { top: parent.top; bottom: parent.bottom }
			visible: false
			PushButton {
				id: infoButton
				text: qsTr("Info")
				anchors { fill: parent; margins: 5 }
				shape: "img/button.sci"; pressedShape: "img/button-down.sci"
				transform: Translate { y: detection._infoButtonTranslate }
			}
		}
	}
	CursorArea.cursor: detectionRunning ? Qt.WaitCursor : Qt.ArrowCursor
	Flickable {
		anchors.fill: parent
		contentWidth: imageWrapper.width; contentHeight: imageWrapper.height
		Item {
			id: imageWrapper
			width: Math.max(detection.width, imageItem.width)
			height: Math.max(detection.height, imageItem.height)
			BorderImage {
				id: imageItem
				anchors.centerIn: parent
				width: (detectionImage.status == Image.Ready ? detectionImage.width : loadingImage.width) + 7
				height: (detectionImage.status == Image.Ready ? detectionImage.height : loadingImage.height) + 7
				source: "img/photo_frame.png"
				border { left: 3; top: 3; right: 4; bottom: 4 }
				Image {
					id: detectionImage
					asynchronous: true
					x: 3; y: 3
					source: "image://detector/" + detectFile
					Repeater {
						model: resultsModel
						delegate: Rectangle {
							x: model.rect.x
							y: model.rect.y
							width: model.rect.width
							height: model.rect.height
							color: "transparent"
							border.color: Qt.rgba(model.value < 0.5 ? model.value * 2 : 1.0, model.value < 0.5 ? 1.0 : (1.0 - model.value) * 2.0, 0, model.value)
							border.width: 1
						}
					}
					Rectangle {
						color: "#40ffffff"
						width: parent.width
						height: parent.height - runtime.detectCenter.y
						y: runtime.detectCenter.y
					}
					Item {
						width: 0; height: 0
						x: runtime.detectCenter.x
						y: runtime.detectCenter.y
						Rectangle {
							width: 20; height: 20
							border.color: "#40000000"; border.width: 1
							color: "transparent"
							visible: detectionRunning
							anchors.centerIn: parent
						}
					}
				}
				Image {
					id: loadingImage
					source: "img/photo.png"
					width: 255; height: 255
					x: 3; y: 3
					visible: detectionImage.status != Image.Ready
				}
				Column {
					anchors.centerIn: parent
					height: childrenRect.height
					opacity: (imageButtonsArea.containsMouse || detectFile == "") ? 1 : 0
					Behavior on opacity {
						NumberAnimation { duration: 250 }
					}
					PushButton {
						text: qsTr("Select image")
						width: 200; height: 32
						onClicked: {
							if (detectFileChooser.selectFile()) {
								detectFile = detectFileChooser.selectedFile;
							}
						}
						enabled: runtime.neuralNet.initialized
					}
					Text {
						text: qsTr("Net not trained")
						width: 200
						height: visible ? 32 : 0
						visible: !runtime.neuralNet.initialized
						color: "#441111"
						elide: Text.ElideRight; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
						font.pixelSize: 18; font.bold: true;
						style: Text.Raised; styleColor: "#80ffffff"
					}
					PushButton {
						text: qsTr("Clear image")
						width: 200
						height: visible ? 32 : 0
						visible: detectFile != ""
						onClicked: detectFile = ""
					}
					PushButton {
						text: qsTr("Reload")
						width: 200
						height: visible ? 32 : 0
						onClicked: {
							runtime.detect(detectFile);
						}
						visible: runtime.neuralNet.initialized && detectFile != "" && !detectionRunning
					}
				}
				FileChooser {
					id: detectFileChooser
					windowTitle: qsTr("Select preview image")
					acceptMode: FileChooser.AcceptOpen
					fileMode: FileChooser.ExistingFile
				}
				MouseArea {
					id: imageButtonsArea
					anchors.fill: parent
					hoverEnabled: true
					acceptedButtons: Qt.NoButton
				}
			}
		}
	}
	ListModel {
		id: resultsModel
	}
	Connections {
		target: runtime
		onScanResultReturned: {
			resultsModel.append(result);
		}
	}
	Component.onCompleted: {
		leftTitleWidget = cancelButtonComponent.createObject(tabWidget.topBar);
		rightTitleWidget = infoButtonComponent.createObject(tabWidget.topBar);
	}
	onNetInitializedChanged: {
		if (!netInitialized) {
			detectFile = "";
		}
	}
	onActiveChanged: {
		if (active) {
			if (detectionRunning) {
				openProgressWidget();
			}
		}
		else {
			closeProgressWidget();
		}
	}
	function openProgressWidget() {
		if (centralTitleWidget == null) {
			centralTitleWidget = progressWidgetComponent.createObject(tabWidget.topBar);
		}
	}
	function closeProgressWidget() {
		if (centralTitleWidget != null) {
			centralTitleWidget.destroy();
			centralTitleWidget = null;
		}
	}
	onDetectFileChanged: {
		if (detectFile != "") {
			runtime.detect(detectFile);
		}
		else {
			runtime.stopFaceDetector();
			resultsModel.clear();
		}
	}

	onDetectionRunningChanged: {
		if (detectionRunning) {
			resultsModel.clear();
		}
		if (detectionRunning && active) {
			openProgressWidget();
		}
		else {
			closeProgressWidget();
		}
	}
	states: [
		State {
			name: "default"; when: !detectionRunning
			//PropertyChanges { target: detection; _infoButtonTranslate: -tabWidget.topBar.height }
			PropertyChanges { target: detection; _cancelButtonTranslate: -tabWidget.topBar.height }
		},
		State {
			name: "running"; when: detectionRunning
			//PropertyChanges { target: detection; _infoButtonTranslate: -tabWidget.topBar.height }
			PropertyChanges { target: detection; _cancelButtonTranslate: 0 }
		}
	]
	transitions: [
		Transition {
			NumberAnimation { properties: "_infoButtonTranslate, _cancelButtonTranslate,height,opacity"; duration: 250; easing.type: Easing.InOutQuad }
		}
	]
}

