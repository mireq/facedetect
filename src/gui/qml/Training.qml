/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  07.05.2011 17:54:38
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0

CentralWindow {
	id: trainingView
	property int _startButtonTranslate: -tabWidget.topBar.height
	property int _cancelButtonTranslate: -tabWidget.topBar.height
	property bool faceScanning: runtime.faceFileScanner.scanning
	property bool nonFaceScanning: runtime.nonFaceFileScanner.scanning
	property bool scanning: faceScanning || nonFaceScanning
	property Component scanningInfoComponent: Qt.createComponent("ScanningInfoWidget.qml")
	property variant neuralNet: runtime.neuralNet
	property int lineHeight: 32

	Component {
		id: startButtonComponent
		Item {
			width: 192; anchors { top: parent.top; bottom: parent.bottom }
			visible: false
			PushButton {
				id: infoButton
				text: qsTr("Start")
				anchors { fill: parent; margins: 5 }
				shape: "img/button.sci"; pressedShape: "img/button-down.sci"
				onClicked: {
					runtime.startTraining()
				}
				transform: Translate { y: trainingView._startButtonTranslate }
			}
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
					runtime.stop()
				}
				transform: Translate { y: trainingView._cancelButtonTranslate }
			}
		}
	}
	Item {
		Component {
			id: label
			Text {
				color: "black"
				elide: Text.ElideRight
				width: Math.round(settingsColumn.width / 2); height: lineHeight
				verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignRight
			}
		}
	}
	Flickable {
		anchors { fill: parent; margins: 10 }
		contentWidth: width; contentHeight: childrenRect.height
		GroupBox {
			title: qsTr("Settings")
			closed: false
			anchors { left: parent.left; right: parent.right }
			Column {
				id: settingsColumn
				height: childrenRect.height + 5; y: 5
				spacing: 5
				anchors { left: parent.left; right: parent.right }
				Row {
					spacing: 5; height: childrenRect.height
					Loader { sourceComponent: label; Component.onCompleted: item.text = qsTr("Epoch count") }
					Item {
						width: Math.round(settingsColumn.width / 2 - s1NeuronRow.spacing); height: lineHeight
						SpinBox {
							id: numEpoch
							minValue: 1; maxValue: 100000; step: 1; decimals: 0
							onValueChanged: {
								runtime.netTrainer.numEpoch = value;
							}
						}
					}
				}
				Row {
					spacing: 5; height: childrenRect.height
					Loader { sourceComponent: label; Component.onCompleted: item.text = qsTr("Training set percent") }
					Item {
						width: Math.round(settingsColumn.width / 2 - s1NeuronRow.spacing); height: lineHeight
						SpinBox {
							id: trainingSetPercent
							minValue: 1; maxValue: 100; step: 1; decimals: 0
							onValueChanged: {
								runtime.trainingSetPercent = value;
							}
						}
					}
				}
				Row {
					spacing: 5; height: childrenRect.height
					Loader { sourceComponent: label; Component.onCompleted: item.text = qsTr("Learning speed") }
					Item {
						width: Math.round(settingsColumn.width / 2 - s1NeuronRow.spacing); height: lineHeight
						SpinBox {
							id: learningSpeed
							minValue: 0; maxValue: 100; step: 0.01; decimals: 10
							onValueChanged: {
								runtime.learningSpeed = value;
							}
						}
					}
				}
				Row {
					id: netTypeRow
					spacing: 5
					Loader { sourceComponent: label; Component.onCompleted: item.text = qsTr("Net type"); height: lineHeight * 2 }
					Item {
						width: Math.round(settingsColumn.width / 2 - stredNeuronRow.spacing); height: lineHeight
						Column {
							width: parent.width
							PushButton {
								id: bpButton
								width: parent.width; height: lineHeight
								shape: "img/pbutton_normal.sci"
								pressedShape: "img/pbutton_checked.sci"
								text: qsTr("One layer")
								checked: runtime.netType == "bp"
								onClicked: {
									if (runtime.netType != "bp") {
										runtime.netType = "bp";
									}
								}
							}
							PushButton {
								id: fsButton
								width: parent.width; height: lineHeight
								shape: "img/pbutton_normal.sci"
								pressedShape: "img/pbutton_checked.sci"
								text: qsTr("Two layer")
								checked: runtime.netType == "fs"
								onClicked: {
									if (runtime.netType != "fs") {
										runtime.netType = "fs";
									}
								}
							}
						}
					}
				}
				Row {
					id: stredNeuronRow
					spacing: 5; height: childrenRect.height
					Loader { sourceComponent: label; Component.onCompleted: item.text = qsTr("Hidden layer neurons") }
					Item {
						width: Math.round(settingsColumn.width / 2 - stredNeuronRow.spacing); height: lineHeight
						SpinBox {
							id: stredNeuronov
							minValue: 1; maxValue: 1000; step: 1; decimals: 0
							onValueChanged: {
								if (neuralNet.stredNeuronov != undefined) {
									neuralNet.stredNeuronov = value;
								}
							}
						}
					}
				}
				Row {
					id: s1NeuronRow
					spacing: 5; height: childrenRect.height
					Loader { sourceComponent: label; Component.onCompleted: item.text = qsTr("Layer 1 neurons") }
					Item {
						width: Math.round(settingsColumn.width / 2 - s1NeuronRow.spacing); height: lineHeight
						SpinBox {
							id: s1Neuronov
							minValue: 1; maxValue: 1000; step: 1; decimals: 0
							onValueChanged: {
								if (neuralNet.s1Neuronov != undefined) {
									neuralNet.s1Neuronov = value;
								}
							}
						}
					}
				}
			}
		}
	}

	function openScanningWidget() {
		if (!active) {
			return;
		}
		if (centralTitleWidget == null) {
			centralTitleWidget = scanningInfoComponent.createObject(tabWidget.topBar);
		}
		if (faceScanning) {
			centralTitleWidget.scanner = runtime.faceFileScanner;
			centralTitleWidget.statusText = qsTr("Scanning faces");
		}
		else if (nonFaceScanning) {
			centralTitleWidget.scanner = runtime.nonFaceFileScanner;
			centralTitleWidget.statusText = qsTr("Scanning non faces");
		}
	}
	function closeScanningWidget() {
		if (centralTitleWidget != null) {
			centralTitleWidget.destroy();
			centralTitleWidget = null;
		}
	}
	onFaceScanningChanged: {
		if (faceScanning) {
			openScanningWidget();
		}
	}
	onNonFaceScanningChanged: {
		if (nonFaceScanning) {
			openScanningWidget();
		}
	}
	Component.onCompleted: {
		rightTitleWidget = startButtonComponent.createObject(tabWidget.topBar);
		leftTitleWidget = cancelButtonComponent.createObject(tabWidget.topBar);
		trainingSetPercent.value = runtime.trainingSetPercent;
		numEpoch.value = runtime.netTrainer.numEpoch;
		learningSpeed.value = runtime.learningSpeed;
	}
	onScanningChanged: {
		if (!scanning) {
			closeScanningWidget();
		}
	}
	onActiveChanged: {
		if (active) {
			if (scanning) {
				openScanningWidget();
			}
		}
		else {
			closeScanningWidget();
		}
	}
	onNeuralNetChanged: {
		if (neuralNet.stredNeuronov == undefined) {
			stredNeuronRow.visible = false;
		}
		else {
			stredNeuronRow.visible = true;
			stredNeuronov.value = neuralNet.stredNeuronov;
		}
		if (neuralNet.s1Neuronov == undefined) {
			s1NeuronRow.visible = false;
		}
		else {
			s1NeuronRow.visible = true;
			s1Neuronov.value = neuralNet.s1Neuronov;
		}
	}

	states: [
		State {
			name: "default"; when: !scanning
			PropertyChanges { target: trainingView; _startButtonTranslate: 0 }
			PropertyChanges { target: trainingView; _cancelButtonTranslate: -tabWidget.topBar.height }
		},
		State {
			name: "scanning"; when: scanning
			PropertyChanges { target: trainingView; _startButtonTranslate: -tabWidget.topBar.height }
			PropertyChanges { target: trainingView; _cancelButtonTranslate: 0 }
		}
	]
	transitions: [
		Transition {
			NumberAnimation { properties: "_startButtonTranslate, _cancelButtonTranslate"; duration: 250; easing.type: Easing.InOutQuad }
		}
	]
}

