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

	Row {
		id: gaborFiltersView
		anchors { left: parent.left; right: parent.right }
		y: 5
		height: childrenRect.height + 10
		Repeater {
			model: filtersModel
			delegate: GaborFilterDelegate {
				sourceModel: filtersModel
			}
		}
		ListModel {
			id: filtersModel
			Component.onCompleted: {
				append(getDefaultSettings());
			}
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
		}
	}
}

