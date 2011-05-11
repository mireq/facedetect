/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  07.05.2011 17:54:38
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0
import org.facedetect 1.0

CentralWindow {
	id: trainingView
	property int _startButtonTranslate: -tabWidget.topBar.height
	property int _cancelButtonTranslate: -tabWidget.topBar.height
	property bool faceScanning: runtime.faceFileScanner.scanning
	property bool nonFaceScanning: runtime.nonFaceFileScanner.scanning
	property bool training: runtime.netTrainer.running
	property bool running: faceScanning || nonFaceScanning || training
	property Component scanningInfoComponent: Qt.createComponent("ScanningInfoWidget.qml")
	property variant neuralNet: runtime.neuralNet
	property int lineHeight: 32
	property string epochText

	CursorArea.cursor: training ? Qt.WaitCursor : (running ? Qt.BusyCursor : Qt.ArrowCursor)

	Component {
		id: trainingInfoComponent
		ProgressTitleItem {
			anchors.fill: parent
		}
	}
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
		contentWidth: width; contentHeight: settingsGroup.height + graphs.height
		GroupBox {
			id: settingsGroup
			clip: true
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
		Column {
			id: graphs
			anchors { left: parent.left; right: parent.right; top: settingsGroup.bottom; margins: 5 }
			height: childrenRect.height
			spacing: 5
			Item {
				anchors { left: parent.left; right: parent.right }
				height: trainingView.height / 2
				PlotWidget {
					id: msePlot
					title: qsTr("MSE")
					anchors.fill: parent
					curves: [
						PlotCurve {
							id: mseaCurve
						}
					]
				}
			}
		}
	}

	Connections {
		id: netTrainerConnections
		target: runtime.netTrainer
		onEpochProgressChanged: netTrainerConnections.updateProgress();
		onEpochChanged: {
			netTrainerConnections.updateProgress();
			epochText = qsTr("Epoch") + " " + epoch;
			if (centralTitleWidget != null) {
				centralTitleWidget.statusExtendedText = epochText;
			}
		}
		function updateProgress() {
			if (centralTitleWidget != null) {
				centralTitleWidget.extendedProgress = runtime.netTrainer.epochProgress;
				if (runtime.netTrainer.epoch >= 1) {
					var epochStep = 1 / runtime.netTrainer.numEpoch
					centralTitleWidget.progress = runtime.netTrainer.epochProgress * epochStep + (runtime.netTrainer.epoch - 1) * epochStep;
				}
			}
		}
	}
	Connections {
		id: runtimeConnections
		target: runtime
		onEpochFinished: {
			mseaCurve.addSample(epoch, mseA);
		}
	}

	function openProgressWidget() {
		if (!active) {
			return;
		}
		if (centralTitleWidget != null) {
			if (training) {
				if (centralTitleWidget.scanner != undefined) {
					centralTitleWidget.destroy();
					centralTitleWidget = null;
				}
			}
			else {
				if (centralTitleWidget.scanner == undefined) {
					centralTitleWidget.destroy();
					centralTitleWidget = null;
				}
			}
		}

		if (centralTitleWidget == null) {
			if (training) {
				centralTitleWidget = trainingInfoComponent.createObject(tabWidget.topBar);
			}
			else {
				centralTitleWidget = scanningInfoComponent.createObject(tabWidget.topBar);
			}
		}
		if (training) {
			centralTitleWidget.statusText = qsTr("Training");
			centralTitleWidget.progressText = qsTr("Overall progress");
			if (epochText == "") {
				epochText = "           ";
			}
			centralTitleWidget.statusExtendedText = epochText;
			centralTitleWidget.extendedProgress = 0;
		}
		else if (faceScanning) {
			centralTitleWidget.scanner = runtime.faceFileScanner;
			centralTitleWidget.statusText = qsTr("Scanning faces");
		}
		else if (nonFaceScanning) {
			centralTitleWidget.scanner = runtime.nonFaceFileScanner;
			centralTitleWidget.statusText = qsTr("Scanning non faces");
		}
		if (!training) {
			epochText = "";
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
			openProgressWidget();
		}
	}
	onNonFaceScanningChanged: {
		if (nonFaceScanning) {
			openProgressWidget();
		}
	}
	onTrainingChanged: {
		if (training) {
			mseaCurve.clearSamples();
			msePlot.setAxisScale(PlotWidget.XBottom, 0, runtime.netTrainer.numEpoch);
			openProgressWidget();
		}
	}
	Component.onCompleted: {
		rightTitleWidget = startButtonComponent.createObject(tabWidget.topBar);
		leftTitleWidget = cancelButtonComponent.createObject(tabWidget.topBar);
		trainingSetPercent.value = runtime.trainingSetPercent;
		numEpoch.value = runtime.netTrainer.numEpoch;
		learningSpeed.value = runtime.learningSpeed;
	}
	onRunningChanged: {
		if (!running) {
			closeScanningWidget();
		}
	}
	onActiveChanged: {
		if (active) {
			if (running) {
				openProgressWidget();
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
			name: "default"; when: !running
			PropertyChanges { target: trainingView; _startButtonTranslate: 0 }
			PropertyChanges { target: trainingView; _cancelButtonTranslate: -tabWidget.topBar.height }
			PropertyChanges { target: settingsGroup; height: settingsGroup.baseHeight; opacity: 1 }
		},
		State {
			name: "running"; when: running && !training
			PropertyChanges { target: trainingView; _startButtonTranslate: -tabWidget.topBar.height }
			PropertyChanges { target: trainingView; _cancelButtonTranslate: 0 }
		},
		State {
			name: "training"; when: running && training; extend: "running"
			PropertyChanges { target: settingsGroup; height: 0; opacity: 0 }
		}
	]
	transitions: [
		Transition {
			NumberAnimation { properties: "_startButtonTranslate, _cancelButtonTranslate,height,opacity"; duration: 250; easing.type: Easing.InOutQuad }
		}
	]
}

