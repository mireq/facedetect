/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  17.02.2011 10:24:40
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0
import org.facedetect 1.0

CentralWindow {
	id: faceDatabaseView
	property bool scanning: runtime.faceFileScanner.scanning
	property double scanningProgress: runtime.faceFileScanner.progress
	property string stateOverlay: "grid"
	property string state: (facesView.currentItem != null && stateOverlay == "grid") ? facesView.currentItem.state : stateOverlay

	property int _infoButtonTranslate: -tabWidget.topBar.height
	property int _backButtonTranslate: -tabWidget.topBar.height
	property int _cancelButtonTranslate: -tabWidget.topBar.height

	property Component scanningInfoComponent: Qt.createComponent("ScanningInfoWidget.qml")

	// Nastavenie kurzoru počas skenovania
	CursorArea.cursor: scanning ? Qt.BusyCursor : Qt.ArrowCursor

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
				shape: "img/button.sci"
				pressedShape: "img/button-down.sci"
				onClicked: {
					runtime.faceFileScanner.stop();
				}
				transform: Translate { y: faceDatabaseView._cancelButtonTranslate }
			}
			PushButton {
				id: backButton
				text: qsTr("Back")
				anchors { fill: parent; margins: 5 }
				shape: "img/button.sci"
				pressedShape: "img/button-down.sci"
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
				shape: "img/button.sci"
				pressedShape: "img/button-down.sci"
				onClicked: {
					if (facesView.currentItem != null) {
						facesView.currentItem.hide();
					}
					faceDatabaseView.stateOverlay = "info";
				}
				transform: Translate { y: faceDatabaseView._infoButtonTranslate }
			}
		}
	}

	function back() {
		if (facesView.currentItem != null) {
			facesView.currentItem.hide();
		}
		if (faceDatabaseView.stateOverlay == "info") {
			faceDatabaseView.stateOverlay = "grid";
		}
	}

	Component.onCompleted: {
		leftTitleWidget = leftTitleButtons.createObject(tabWidget.topBar);
		rightTitleWidget = rightTitleButtons.createObject(tabWidget.topBar);
	}

	onScanningChanged: {
		if (scanning && active) {
			centralTitleWidget = scanningInfoComponent.createObject(tabWidget.topBar);
			centralTitleWidget.scanner = runtime.faceFileScanner;
		}
		else {
			if (centralTitleWidget != null) {
				centralTitleWidget.destroy();
			}
			centralTitleWidget = null;
		}
	}

	onActiveChanged: {
		if (active) {
			if (scanning && centralTitleWidget == null) {
				centralTitleWidget = scanningInfoComponent.createObject(tabWidget.topBar);
				centralTitleWidget.scanner = runtime.faceFileScanner;
			}
			else if (!scanning) {
				runtime.faceFileScanner.start();
			}
		}
		if (!active) {
			back();
		}
	}

	GridView {
		id: facesView
		anchors.fill: parent
		model: runtime.faceBrowserModel
		delegate: FaceDelegate{}
		cellWidth: 192
		cellHeight: 192
		onCurrentItemChanged: {
			faceDetailsInfo.state = "closed";
			definitionFileInfo.state = "closed";
			transformationInfo.state = "closed";
			if (currentItem == null) {
				faceMetadata.metadata = null;
				definitionFileText.text = "";
				transformationDelegate.transformRotate = 0;
				transformationDelegate.transformScale = 0;
				transformationDelegate.transformTranslateX = 0;
				transformationDelegate.transformTranslateY = 0;
			}
		}
		onCountChanged: {
			if (count == 1) {
				currentIndex = -1;
				currentIndex = 0;
			}
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
						id: faceMetadata
					}
				}
				InfoListItem {
					id: definitionFileInfo
					title: qsTr("Definition file")
					width: imageDetailsView.width
					Text {
						id: definitionFileText
						color: "white"
						textFormat: Text.PlainText
					}
				}
				InfoListItem {
					id: transformationInfo
					title: qsTr("Transformation")
					width: imageDetailsView.width
					TransformationDelegate {
						id: transformationDelegate
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
		if (state == "grid") {
			faceDetailsInfo.state = "closed";
			definitionFileInfo.state = "closed";
			transformationInfo.state = "closed";
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
			PropertyChanges { target: faceMetadata; metadata: null }
			PropertyChanges { target: definitionFileText; text: "" }
			PropertyChanges {
				target: transformationDelegate
				transformRotate: 0
				transformScale: 0
				transformTranslateX: 0
				transformTranslateY: 0
				source: ""
			}
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
			PropertyChanges { target: faceMetadata; metadata: facesView.currentItem == null ? null : facesView.currentItem.facesModel.faceData }
			PropertyChanges { target: definitionFileText; text: facesView.currentItem == null ? "" : facesView.currentItem.facesModel.definitionFile }
			PropertyChanges {
				target: transformationDelegate
				source: facesView.currentItem == null ? "" : facesView.currentItem.facesModel.image
				transformRotate: facesView.currentItem == null ? 0 : facesView.currentItem.facesModel.transformRotate
				transformScale: facesView.currentItem == null ? 0 : facesView.currentItem.facesModel.transformScale
				transformTranslateX: facesView.currentItem == null ? 0 : facesView.currentItem.facesModel.transformTranslateX
				transformTranslateY: facesView.currentItem == null ? 0 : facesView.currentItem.facesModel.transformTranslateY
			}
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

