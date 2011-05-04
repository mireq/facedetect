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
	property alias enabled: sw.on
	titleRight: Switch { id: sw; anchors.verticalCenter: parent.verticalCenter }
	titleLeft: ExpandButton{ id: closeImage }
	onTitleClicked: closeImage.open = !closeImage.open
	closed: !(sw.on && closeImage.open)
	clip: true
}

