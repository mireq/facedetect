/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  03.05.2011 18:51:57
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0
import "../"

GraphicFilterSettings {
	id: gaborSettings
	property bool initialization: true

	Component.onCompleted: {
		for (var filter = 0; filter < settings.filters.length; ++filter) {
			filtersModel.append(settings.filters[filter]);
		}
		initialization = false;
	}

	Column {
		id: gaborFiltersView
		anchors { left: parent.left; right: parent.right }
		y: 5
		height: childrenRect.height + 10
		Repeater {
			model: filtersModel
			delegate: GaborFilterDelegate {
				sourceModel: filtersModel
				listView: gaborFiltersView
				onChanged: {
					if (!initialization) {
						filtersModel.updateSettings();
						gaborSettings.changed();
					}
				}
			}
			onCountChanged: {
				if (!initialization) {
					filtersModel.updateSettings();
					gaborSettings.changed();
				}
			}
		}
		ListModel {
			id: filtersModel
			function getDefaultSettings() {
				return {
					"lambda": 4.0,
					"theta": 0.0,
					"psi": 0.0,
					"sigma": 2.0,
					"gamma": 1.0,
					"lum": 0.0
				}
			}
			function updateSettings() {
				var filterList = [];
				for (var filter = 0; filter < count; ++filter) {
					var obj = get(filter);
					filterList[filter] = {};
					filterList[filter]["lambda"] = obj.lambda;
					filterList[filter]["theta"] = obj.theta;
					filterList[filter]["psi"] = obj.psi;
					filterList[filter]["sigma"] = obj.sigma;
					filterList[filter]["gamma"] = obj.gamma;
					filterList[filter]["lum"] = obj.lum;
				}
				var tmp = settings;
				tmp.filters = filterList;
				settings = tmp;
			}
		}
		/*move: Transition {
			NumberAnimation { properties: "y"; duration: 100 }
		}*/
	}
	PushButton {
		id: addButton
		anchors { left: parent.left; right: parent.right; top: gaborFiltersView.bottom }
		height: 32
		text: qsTr("Add filter")
		shape: "img/pbutton_normal.sci"
		pressedShape: "img/pbutton_pressed.sci"
		onClicked: {
			filtersModel.append(filtersModel.getDefaultSettings());
		}
	}
	Item {
		height: 5; anchors{ top: addButton.bottom; left: parent.left; right: parent.right }
	}
}

