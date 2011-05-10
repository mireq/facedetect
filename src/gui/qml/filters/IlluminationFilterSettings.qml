/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  03.05.2011 18:02:55
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0
import "../"

GraphicFilterSettings {
	id: ilSettings
	property int lineHeight: 32
	onSettingsChanged: {
		if (settings != undefined) {
			if (settings.illuminationPlaneOnly != undefined) {
				illuminationPlaneSwitch.on = settings.illuminationPlaneOnly;
			}
			if (settings.illuminationCorrectHistogram != undefined) {
				correctHistogramSwitch.on = settings.illuminationCorrectHistogram;
			}
		}
	}
	Grid {
		id: grid
		property string textColor: "black"
		y: 5; columns: 2; spacing: 5
		anchors { left: parent.left; right: parent.right }
		height: childrenRect.height + 10
		Text {
			color: grid.textColor
			text: qsTr("Illumination plane only")
			elide: Text.ElideRight
			width: Math.round(grid.width / 2); height: lineHeight
			verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignRight
		}
		Item {
			width: Math.round(grid.width / 2) - grid.spacing; height: lineHeight
			Switch {
				id: illuminationPlaneSwitch
				anchors { verticalCenter: parent.verticalCenter; left: parent.left }
				onOnChanged: {
					var tmp = settings;
					tmp.illuminationPlaneOnly = on;
					settings = tmp;
					changed();
				}
			}
		}
		Text {
			color: grid.textColor
			text: qsTr("Correct histogram")
			elide: Text.ElideRight
			width: Math.round(grid.width / 2); height: lineHeight
			verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignRight
		}
		Item {
			width: Math.round(grid.width / 2) - grid.spacing; height: lineHeight
			Switch {
				id: correctHistogramSwitch
				anchors { verticalCenter: parent.verticalCenter; left: parent.left }
				onOnChanged: {
					var tmp = settings;
					tmp.illuminationCorrectHistogram = on;
					settings = tmp;
					changed();
				}
			}
		}
	}
}

