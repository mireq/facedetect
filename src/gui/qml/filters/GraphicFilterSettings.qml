/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  03.05.2011 16:11:41
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0
import ".."

GroupBox {
	id: filterSettings
	property alias enabled: sw.on
	property variant settings: { "on": sw.on }
	property bool hasContents: content.length > 0
	onSettingsChanged: {
		if (settings != undefined && settings.enabled != undefined) {
			sw.on = settings.enabled;
		}
	}
	signal changed()
	titleRight: Switch {
		id: sw
		anchors.verticalCenter: parent.verticalCenter
		onOnChanged: {
			var tmp = settings;
			tmp.enabled = on;
			settings = tmp;
			filterSettings.changed();
		}
	}
	titleLeft: ExpandButton {
		id: closeImage
		opacity: (sw.on && hasContents) ? 1 : 0
		Behavior on opacity { NumberAnimation { duration: 200 } }
	}
	onTitleClicked: closeImage.open = !closeImage.open
	closed: !(sw.on && closeImage.open)
	clip: true
}

