import QtQuick 1.0

Item {
	id: main
	width: 1024
	height: 768
	state: "grid"

	Rectangle {
		anchors.fill: parent
		color: "#aaaaaa"
	}
	GridView {
		id: facesView
		anchors.fill: parent
		anchors.topMargin: 72
		model: facesModel
		delegate: FaceDelegate{}
		cellWidth: 192
		cellHeight: 192
	}
	Rectangle {
		id: foregroundDim
		anchors.fill: parent
		color: "transparent"
		/*
		states: [
			State {
				name: "hidden"; when: foregroundDim.state == "hidden"
				PropertyChanges { target: foregroundDim; color: "transparent" }
			},
			State {
				name: "visible"; when: foregroundDim.state == "visible"
				PropertyChanges { target: foregroundDim; color: "#ee000000" }
			}
		]
		transitions: [
			Transition {
				ColorAnimation { duration: 250 }
			}
		]
		*/
		MouseArea {
			id: foregroundDimArea
			anchors.fill: parent
			enabled: false
		}
	}
	Item {
		id: faceDetectorForeground
		anchors.fill: parent
		anchors.rightMargin: parent.width / 2
		anchors.topMargin: 72
	}
	Rectangle {
		id: topBar
		color: "#eeeeee"
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		height: 72

		property bool scanning: faceScanner.scanning
		property double scanningProgress: faceScanner.progress
		property string scanningText: "Skenovanie ..."
		property string scanningExtendedText: ""
		state: "idle"
	
		onScanningChanged: {
			if (scanning) {
				topBar.state = "scanning";
			}
			else {
				topBar.state = "idle";
			}
		}
		onScanningProgressChanged: {
			scanningExtendedText = "Prehľadaných " + faceScanner.scannedDirs + " adresárov a " + faceScanner.scannedFiles + " súborov";
		}

		BorderImage {
			anchors.fill: parent
			source: "img/innerbg.sci"
		}
		Item {
			id: backBtnRect
			width: 192
			anchors.top: parent.top
			anchors.bottom: parent.bottom
			anchors.left: parent.left
			PushButton {
				id: cancelButton
				text: "Zrušiť"
				anchors.fill: parent
				anchors.margins: 5
				opacity: 0
				onClicked: {
					faceScanner.stop();
				}
				transform: Translate { id: cancelButtonTranslate; y: -backBtnRect.height }
			}
		}
		Item {
			id: infoBtnRect
			width: 192
			anchors.top: parent.top
			anchors.bottom: parent.bottom
			anchors.right: parent.right
			PushButton {
				id: infoButton
				text: "Info"
				anchors.fill: parent
				anchors.margins: 5
				opacity: 0
				onClicked: {
				}
				transform: Translate { id: infoButtonTranslate; y: -infoBtnRect.height }
			}
		}
		Item {
			id: statusRect
			anchors.left: backBtnRect.right
			anchors.right: infoBtnRect.left
			anchors.top: parent.top
			anchors.bottom: parent.bottom

			Text {
				id: statusText
				anchors.top: parent.top
				anchors.left: parent.left
				anchors.right: parent.right
				color: "#555555"
				elide: Text.ElideRight; horizontalAlignment: Text.AlignHCenter
				font.pixelSize: 22; font.bold: true;
				style: Text.Raised; styleColor: "#ffffff"
				text: topBar.state == "scanning" ? topBar.scanningText : ""
			}
			Text {
				id: extendedStatus
				anchors.top: statusText.bottom
				anchors.left: parent.left
				anchors.right: parent.right
				color: "#777777"
				elide: Text.ElideRight; horizontalAlignment: Text.AlignHCenter
				font.pixelSize: 12
				style: Text.Raised; styleColor: "#ffffff"
				text: topBar.state == "scanning" ? topBar.scanningExtendedText : ""
			}
			Item {
				anchors.top: extendedStatus.bottom
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.bottom: parent.bottom
				ProgressBar {
					id: scanningProgress
					anchors.left: parent.left
					anchors.right: parent.right
					anchors.verticalCenter: parent.verticalCenter
					progress: faceScanner.progress
					opacity: 0
				}
			}
		}
		states: [
			State {
				name: "scanning"; when: topBar.state == "scanning"
				PropertyChanges { target: cancelButtonTranslate; y: 0 }
				PropertyChanges { target: cancelButton; opacity: 1 }
				PropertyChanges { target: scanningProgress; opacity: 1 }
			},
			State {
				name: "idle"; when: topBar.state == "idle"
				PropertyChanges { target: infoButtonTranslate; y: 0 }
				PropertyChanges { target: infoButton; opacity: 1 }
				PropertyChanges { target: scanningProgress; opacity: 0 }
			}
		]
		transitions: [
			Transition {
				NumberAnimation { properties: "x,y,width,height,opacity"; duration: 250; easing.type: Easing.InOutQuad }
			}
		]
	}
	states: [
		State {
			name: "grid"; when: main.state == "grid"
			PropertyChanges { target: foregroundDim; color: "transparent" }
			PropertyChanges { target: foregroundDimArea; enabled: false }
		},
		State {
			name: "zoom"; when: main.state == "zoom"
			PropertyChanges { target: foregroundDim; color: "#ee000000" }
			PropertyChanges { target: foregroundDimArea; enabled: true }
		}
	]
	transitions: [
		Transition {
			NumberAnimation { properties: "x,y,width,height,opacity"; duration: 250; easing.type: Easing.InOutQuad }
			ColorAnimation { duration: 250 }
		}
	]
}
