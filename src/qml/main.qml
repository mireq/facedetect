import Qt 4.7

Item {
	id: main
	width: 512
	height: 768
	Rectangle {
		anchors.fill: parent
		color: "black"
	}
	ListView {
		id: facesView
		snapMode: ListView.SnapOneItem
		orientation: ListView.Horizontal
		model: facesModel
		delegate: faceDelegate
		anchors.fill: parent
	}
	Component {
		id: faceDelegate
		Item {
			width: main.width
			height: main.height
			Image {
				anchors.fill: parent
				source: model.display
				asynchronous: true
				fillMode: Image.PreserveAspectFit
			}
		}
	}
}
