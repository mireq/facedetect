/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  16.02.2011 13:19:17
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0

Item {
	id: tabWidget
	default property alias content: tabView.children
	property alias currentIndex: tabsView.currentIndex
	property Item currentItem: null
	width: 400; height: 300

	Component.onCompleted: updateCurrent()
	onCurrentIndexChanged: updateCurrent()
	function updateCurrent() {
		tabWidget.currentItem = null;
		for (var item = 0; item < tabView.children.length; ++item) {
			tabView.children[item].opacity = (item == currentIndex ? 1 : 0);
			if (tabView.children[item].active != undefined) {
				tabView.children[item].active = (item == currentIndex ? 1 : 0);
			}
			if (item == currentIndex) {
				tabWidget.currentItem = tabView.children[item];
			}
		}
	}

	Component {
		id: tabDelegate
		Item {
			id: tabDelegateItem
			property bool active: ListView.view.currentIndex == index
			width: ListView.view.width; height: 38
			BorderImage {
				anchors.fill: parent
				source: "img/tab-left-item.sci"
			}
			Item {
				id: activeOverlay
				clip: true
				anchors { top: parent.top; right: parent.right; bottom: parent.bottom }
				width: 0
				BorderImage {
					width: tabDelegateItem.width
					anchors { top: parent.top; right: parent.right; bottom: parent.bottom }
					source: "img/tab-left-aitem.sci"
				}
			}
			Text {
				anchors { fill: parent; rightMargin: 16; leftMargin: 5 }
				verticalAlignment: Text.AlignVCenter
				text: tabView.children[index].title == undefined ? ("Tab " + index) : tabView.children[index].title
				color: parent.active ? "#000000" : "#333333"
				styleColor: parent.active ? "#aaaaaa" : "#eeeeee"
				font.pixelSize: 17
				style: parent.active ? Text.Raised : Text.Sunken
				elide: Text.ElideRight
			}
			MouseArea {
				anchors.fill: parent
				onClicked: {
					ListView.view.currentIndex = index;
				}
			}
			states:
				State {
					name: "active"; when: tabDelegateItem.active
					PropertyChanges { target: activeOverlay; width: tabDelegateItem.width }
				}
			transitions:
				Transition {
					NumberAnimation { properties: "width"; duration: 150; easing.type: Easing.InQuad }
				}
		}
	}

	Item {
		id: tabView
		anchors { left: tabsView.right; top: parent.top; right: parent.right; bottom: parent.bottom }
	}
	BorderImage {
		anchors.fill: tabsView
		source: "img/tab-left-bg.sci"
	}
	ListView {
		id: tabsView
		width: Math.round(parent.width / 6 + 80)
		anchors { left: parent.left; top: parent.top; bottom: parent.bottom }
		model: tabView.children.length
		delegate: tabDelegate
		header: Item{ height: 20 }
		footer: Item{ height: 20 }
	}
}

