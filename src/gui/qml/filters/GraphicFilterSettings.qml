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
	titleLeft: ExpandButton{ id: closeImage }
	onTitleClicked: closeImage.open = !closeImage.open
	closed: !(sw.on && closeImage.open)
	clip: true
}

