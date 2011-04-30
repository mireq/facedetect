/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  17.02.2011 10:24:40
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0

CentralWindow {
	id: faceDatabaseView
	property bool _started: false
	property bool scanning: runtime.faceFileScanner.scanning
	property double scanningProgress: runtime.faceFileScanner.progress
	property string stateOverlay: ""
	property string state: (facesView.currentItem != null && stateOverlay == "") ? facesView.currentItem.state : stateOverlay

	property int _infoButtonTranslate: -tabWidget.topBar.height
	property int _backButtonTranslate: -tabWidget.topBar.height
	property int _cancelButtonTranslate: -tabWidget.topBar.height

	Component {
		id: scanningInfo
		Item {
			property double scanningProgress: runtime.faceFileScanner.progress
			property string scanningText: qsTr("Scanning ...")
			property string scanningExtendedText: ""
			onScanningProgressChanged: {
				scanningExtendedText = qsTr("Scanned") + " " + qsTr("%n folder(s)", "", parseInt(runtime.faceFileScanner.scannedDirs)) + " " + qsTr("and") + " " + qsTr("%n file(s)", "", parseInt(runtime.faceFileScanner.scannedFiles));
			}
			anchors.fill: parent
			Text {
				id: statusText
				anchors { top: parent.top; left: parent.left; right: parent.right }
				color: "#555555"
				elide: Text.ElideRight; horizontalAlignment: Text.AlignHCenter
				font { pixelSize: 22; bold: true }
				style: Text.Raised; styleColor: "#ffffff"
				text: scanningText
			}
			Text {
				id: extendedStatus
				anchors { top: statusText.bottom; left: parent.left; right: parent.right }
				color: "#777777"
				elide: Text.ElideRight; horizontalAlignment: Text.AlignHCenter
				font.pixelSize: 12
				style: Text.Raised; styleColor: "#ffffff"
				text: scanningExtendedText
			}
			Item {
				anchors { top: extendedStatus.bottom; left: parent.left; right: parent.right; bottom: parent.bottom }
				ProgressBar {
					id: scanningProgress
					anchors.left: parent.left
					anchors.right: parent.right
					anchors.verticalCenter: parent.verticalCenter
					progress: runtime.faceFileScanner.progress
				}
			}
		}
	}
	Component {
		id: leftTitleButtons
		Item {
			width: 192
			anchors { top: parent.top; bottom: parent.bottom }
			visible: false
			PushButton {
				id: cancelButton
				text: qsTr("Cancel")
				anchors { fill: parent; margins: 5 }
				onClicked: {
					runtime.faceFileScanner.stop();
				}
				transform: Translate { y: faceDatabaseView._cancelButtonTranslate }
			}
			PushButton {
				id: backButton
				text: qsTr("Back")
				anchors { fill: parent; margins: 5 }
				transform: Translate { y: faceDatabaseView._backButtonTranslate }
				onClicked: {
					back();
				}
			}
		}
	}
	Component {
		id: rightTitleButtons
		Item {
			width: 192
			anchors { top: parent.top; bottom: parent.bottom }
			visible: false
			PushButton {
				id: infoButton
				text: qsTr("Info")
				anchors { fill: parent; margins: 5 }
				onClicked: {
					if (facesView.selectedItem != null) {
						facesView.selectedItem.hide();
					}
					faceDatabaseView.stateOverlay = "info";
				}
				transform: Translate { y: faceDatabaseView._infoButtonTranslate }
			}
		}
	}

	function back() {
		if (facesView.selectedItem != null) {
			facesView.selectedItem.hide();
		}
		if (faceDatabaseView.stateOverlay == "info") {
			faceDatabaseView.stateOverlay = "";
		}
	}

	Component.onCompleted: {
		leftTitleWidget = leftTitleButtons.createObject(tabWidget.topBar);
		rightTitleWidget = rightTitleButtons.createObject(tabWidget.topBar);
	}

	onScanningChanged: {
		if (scanning) {
			centralTitleWidget = scanningInfo.createObject(tabWidget.topBar);
		}
		else {
			centralTitleWidget = null;
		}
	}

	onActiveChanged: {
		if (active && !_started) {
			runtime.faceFileScanner.start();
			_started = true;
		}
		if (!active) {
			back();
		}
	}

	GridView {
		id: facesView
		property Item selectedItem
		anchors.fill: parent
		model: runtime.faceBrowserModel
		delegate: FaceDelegate{}
		cellWidth: 192
		cellHeight: 192
		onCurrentItemChanged: {
			faceDetailsInfo.state = "closed";
			definitionFileInfo.state = "closed";
			transformationInfo.state = "closed";
		}
		transform: Translate { id: facesViewTranslate }
	}
	Rectangle {
		id: foregroundDim
		anchors.fill: parent
		color: "#ee808080"
		MouseArea {
			id: foregroundDimArea
			anchors.fill: parent
			enabled: false
		}
	}
	Item {
		id: faceDetectorForeground
		anchors.fill: parent
		anchors.rightMargin: Math.round(parent.width / 2)
		ControlPointsDisplay {
			id: controlPointsDisplay
			anchors.fill: parent
			opacity: 0
		}
	}
	Rectangle {
		id: imageDetails
		anchors.fill: parent
		anchors.leftMargin: Math.round(parent.width / 2)
		color: "#444444"
		ListView {
			id: imageDetailsView
			anchors.fill: parent
			anchors.leftMargin: 10
			anchors.rightMargin: 10
			model: VisualItemModel {
				InfoListItem {
					id: faceDetailsInfo
					title: qsTr("Details")
					width: imageDetailsView.width
					FaceMetadata {
						metadata: facesView.selectedItem == null ? null : facesView.selectedItem.facesModel.faceData
					}
				}
				InfoListItem {
					id: definitionFileInfo
					title: qsTr("Definition file")
					width: imageDetailsView.width
					Text {
						text: facesView.selectedItem == null ? "" : facesView.selectedItem.facesModel.definitionFile
						color: "white"
						textFormat: Text.PlainText
					}
				}
				InfoListItem {
					id: transformationInfo
					title: qsTr("Transformation")
					width: imageDetailsView.width
					TransformationDelegate {
						source: facesView.selectedItem == null ? "" : facesView.selectedItem.facesModel.image
						transformRotate: facesView.selectedItem == null ? 0.0 : facesView.selectedItem.facesModel.transformRotate
						transformScale: facesView.selectedItem == null ? 0.0 : facesView.selectedItem.facesModel.transformScale
						transformTranslateX: facesView.selectedItem == null ? 0.0 : facesView.selectedItem.facesModel.transformTranslateX
						transformTranslateY: facesView.selectedItem == null ? 0.0 : facesView.selectedItem.facesModel.transformTranslateY
						parentWindow: faceDatabaseView
					}
				}
			}
		}
		transform: Translate { id: imageDetailsTranslate; x: imageDetails.width }
	}
	Item {
		id: databaseDetails
		anchors.fill: parent
		transform: Translate { id: databaseDetailsTranslate; x: databaseDetails.width }
		Item {
			anchors { top: parent.top; bottom: parent.bottom; left: parent.left }
			width: Math.round(parent.width / 2) - 10
			Text {
				anchors { bottom: statisticsImage.top; left: parent.left; right: parent.right }
				text: qsTr("Control points")
				color: "#444444"
				elide: Text.ElideRight
				font.pixelSize: 16; font.bold: true;
				style: Text.Raised; styleColor: "#80ffffff"
				horizontalAlignment: Text.AlignHCenter
			}
			Image {
				id: statisticsImage
				property int index: 0
				anchors.centerIn: parent
				width: Math.min(parent.width, parent.height) - 50
				height: width
				asynchronous: true
				fillMode: Image.PreserveAspectFit
				source: "image://faceimage/statimage/" + index
			}
		}
		ListView {
			id: databaseDetailsView
			anchors { top: parent.top; bottom: parent.bottom; right: parent.right }
			width: Math.round(parent.width / 2)
			model: VisualItemModel {
				InfoListItem {
					title: qsTr("Scanning statistics")
					width: databaseDetailsView.width
					color: "#444444"
					styleColor: "#80ffffff"
					Column {
						Text { text: qsTr("Scanned directories") + ": " + runtime.faceFileScanner.scannedDirs; font.pixelSize: 14; color: "#333333" }
						Text { text: qsTr("Scanned files") + ": " + runtime.faceFileScanner.scannedFiles; font.pixelSize: 14; color: "#333333" }
						Text { text: qsTr("All directories") + ": " + runtime.faceFileScanner.totalDirs; font.pixelSize: 14; color: "#333333" }
						Text { text: qsTr("All files") + ": " + runtime.faceFileScanner.totalFiles; font.pixelSize: 14; color: "#333333" }
					}
				}
			}
		}
	}
	onStateChanged: {
		if (state == "info") {
			statisticsImage.index++;
		}
	}
	onScanningProgressChanged: {
		if (state == "info") {
			statisticsImage.index++;
		}
	}
	states: [
		State {
			name: "grid"; when: (faceDatabaseView.state == "grid" && !faceDatabaseView.scanning)
			PropertyChanges { target: foregroundDim; color: "transparent" }
			PropertyChanges { target: foregroundDimArea; enabled: false }
			PropertyChanges { target: controlPointsDisplay; opacity: 0 }
			PropertyChanges { target: faceDatabaseView; _infoButtonTranslate: 0 }
			PropertyChanges { target: facesView; selectedItem: null }
		},
		State {
			name: "scanning"; extend: "grid"; when: (faceDatabaseView.state == "grid" && faceDatabaseView.scanning)
			PropertyChanges { target: faceDatabaseView; _cancelButtonTranslate: 0 }
		},
		State {
			name: "zoom"; when: faceDatabaseView.state == "zoom"
			PropertyChanges { target: foregroundDim; color: "#ee000000" }
			PropertyChanges { target: foregroundDimArea; enabled: true }
			PropertyChanges { target: controlPointsDisplay; opacity: 1 }
			PropertyChanges { target: imageDetailsTranslate; x: 0 }
			PropertyChanges { target: faceDatabaseView; _backButtonTranslate: 0 }
		},
		State {
			name: "info"; when: faceDatabaseView.state == "info"
			PropertyChanges { target: facesViewTranslate; x: -facesView.width }
			PropertyChanges { target: databaseDetailsTranslate; x: 0 }
			PropertyChanges { target: faceDatabaseView; _backButtonTranslate: 0 }
		}
	]
	transitions: [
		Transition {
			NumberAnimation { properties: "x,y,width,height,opacity,_cancelButtonTranslate,_backButtonTranslate,_infoButtonTranslate"; duration: 250; easing.type: Easing.InOutQuad }
			ColorAnimation { duration: 250 }
		}
	]
}

