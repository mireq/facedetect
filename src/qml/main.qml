import QtQuick 1.0

Item {
	id: main
	width: 1024
	height: 768
	state: "start"
	property double scanningProgress: runtime.faceFileScanner.progress

	Rectangle {
		anchors.fill: parent
		color: "#aaaaaa"
	}
	GridView {
		id: facesView
		property Item selectedItem
		anchors { left: parent.left; top: parent.top; bottom: parent.bottom; right: parent.right }
		anchors.topMargin: topBar.height
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
		anchors { left: parent.left; top: topBar.bottom; bottom: parent.bottom; right: parent.right }
		anchors.rightMargin: parent.width / 2
		ControlPointsDisplay {
			id: controlPointsDisplay
			anchors.fill: parent
			opacity: 0
		}
	}
	Rectangle {
		id: imageDetails
		anchors { left: parent.left; top: topBar.bottom; bottom: parent.bottom; right: parent.right }
		anchors.leftMargin: parent.width / 2
		color: "#444444"
		ListView {
			id: imageDetailsView
			anchors.fill: parent
			anchors.leftMargin: 10
			anchors.rightMargin: 10
			model: VisualItemModel {
				InfoListItem {
					id: faceDetailsInfo
					title: "Podrobnosti"
					width: imageDetailsView.width
					FaceMetadata {
						metadata: facesView.selectedItem == null ? null : facesView.selectedItem.facesModel.faceData
					}
				}
				InfoListItem {
					id: definitionFileInfo
					title: "Definičný súbor"
					width: imageDetailsView.width
					Text {
						text: facesView.selectedItem == null ? "" : facesView.selectedItem.facesModel.definitionFile
						color: "white"
						textFormat: Text.PlainText
					}
				}
				InfoListItem {
					id: transformationInfo
					title: "Transformácia"
					width: imageDetailsView.width
					TransformationDelegate {
						source: facesView.selectedItem == null ? "" : facesView.selectedItem.facesModel.image
						transformRotate: facesView.selectedItem == null ? 0.0 : facesView.selectedItem.facesModel.transformRotate
						transformScale: facesView.selectedItem == null ? 0.0 : facesView.selectedItem.facesModel.transformScale
						transformTranslateX: facesView.selectedItem == null ? 0.0 : facesView.selectedItem.facesModel.transformTranslateX
						transformTranslateY: facesView.selectedItem == null ? 0.0 : facesView.selectedItem.facesModel.transformTranslateY
					}
				}
			}
		}
		BorderImage {
			anchors.fill: parent
			source: "img/innerbg.sci"
		}
		transform: Translate { id: imageDetailsTranslate; x: imageDetails.width }
	}
	Item {
		id: databaseDetails
		anchors { left: parent.left; top: topBar.bottom; bottom: parent.bottom; right: parent.right }
		transform: Translate { id: databaseDetailsTranslate; x: databaseDetails.width }
		Item {
			anchors.top: parent.top
			anchors.bottom: parent.bottom
			anchors.left: parent.left
			width: parent.width / 2 - 10
			Text {
				anchors.horizontalCenter: statisticsImage.horizontalCenter
				anchors.bottom: statisticsImage.top
				text: "Rozdelenie kontrolných bodov"
				color: "#444444"
				elide: Text.ElideRight
				font.pixelSize: 20; font.bold: true;
				style: Text.Raised; styleColor: "#80ffffff"
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
			anchors.top: parent.top
			anchors.bottom: parent.bottom
			anchors.right: parent.right
			width: parent.width / 2
			model: VisualItemModel {
				InfoListItem {
					title: "Štatistiky skenovania súborov"
					width: databaseDetails.width
					color: "#444444"
					styleColor: "#80ffffff"
					Column {
						Text { text: "Preskenovaných adresárov: " + runtime.faceFileScanner.scannedDirs; font.pixelSize: 14; color: "#333333" }
						Text { text: "Preskenovaných súborov: " + runtime.faceFileScanner.scannedFiles; font.pixelSize: 14; color: "#333333" }
						Text { text: "Celkovo adresárov: " + runtime.faceFileScanner.totalDirs; font.pixelSize: 14; color: "#333333" }
						Text { text: "Celkovo súborov: " + runtime.faceFileScanner.totalFiles; font.pixelSize: 14; color: "#333333" }
					}
				}
			}
		}
	}
	Rectangle {
		id: topBar
		color: "#eeeeee"
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		height: 72

		property bool scanning: runtime.faceFileScanner.scanning
		property double scanningProgress: runtime.faceFileScanner.progress
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
			scanningExtendedText = "Prehľadaných " + runtime.faceFileScanner.scannedDirs + " adresárov a " + runtime.faceFileScanner.scannedFiles + " súborov";
		}

		BorderImage {
			anchors.fill: parent
			anchors.leftMargin: -15
			anchors.topMargin: -15
			anchors.rightMargin: -15
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
			PushButton {
				id: backButton
				text: "Späť"
				anchors.fill: parent
				anchors.margins: 5
				opacity: 0
				onClicked: {
					if (facesView.selectedItem != null) {
						facesView.selectedItem.hide();
					}
					if (main.state == "info") {
						main.state = "grid";
					}
				}
				transform: Translate { id: backButtonTranslate; y: -backBtnRect.height }
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
					if (facesView.selectedItem != null) {
						facesView.selectedItem.hide();
					}
					main.state = "info";
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
					progress: runtime.faceFileScanner.progress
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
				PropertyChanges { target: scanningProgress; opacity: 0 }
			}
		]
		transitions: [
			Transition {
				NumberAnimation { properties: "x,y,width,height,opacity"; duration: 250; easing.type: Easing.InOutQuad }
			}
		]
	}
	StartPanel {
		id: startPanel
		anchors.fill: parent
		opacity: 0
	}
	states: [
		State {
			name: "start"; when: main.state == "start"
			PropertyChanges { target: foregroundDim; color: "#ee808080" }
			PropertyChanges { target: topBar; anchors.topMargin: -72; anchors.bottomMargin: -72 }
			PropertyChanges { target: startPanel; opacity: 1 }
		},
		State {
			name: "grid"; when: main.state == "grid"
			PropertyChanges { target: foregroundDim; color: "transparent" }
			PropertyChanges { target: foregroundDimArea; enabled: false }
			PropertyChanges { target: controlPointsDisplay; opacity: 0 }
			PropertyChanges { target: infoButtonTranslate; y: 0 }
			PropertyChanges { target: infoButton; opacity: 1 }
			PropertyChanges { target: facesView; selectedItem: null }
		},
		State {
			name: "zoom"; when: main.state == "zoom"
			PropertyChanges { target: foregroundDim; color: "#ee000000" }
			PropertyChanges { target: foregroundDimArea; enabled: true }
			PropertyChanges { target: controlPointsDisplay; opacity: 1 }
			PropertyChanges { target: imageDetailsTranslate; x: 0 }
			PropertyChanges { target: statusRect; state: "zoom" }
			PropertyChanges { target: backButtonTranslate; y: 0 }
			PropertyChanges { target: backButton; opacity: 1 }
		},
		State {
			name: "info"; when: main.state == "info"
			PropertyChanges { target: backButtonTranslate; y: 0 }
			PropertyChanges { target: backButton; opacity: 1 }
			PropertyChanges { target: facesViewTranslate; x: -facesView.width }
			PropertyChanges { target: databaseDetailsTranslate; x: 0 }
		}
	]
	transitions: [
		Transition {
			NumberAnimation { properties: "x,y,width,height,opacity"; duration: 250; easing.type: Easing.InOutQuad }
			ColorAnimation { duration: 250 }
		}
	]
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
}
