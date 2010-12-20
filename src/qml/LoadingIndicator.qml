/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  20.12.2010 09:11:53
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0

Image {
	id: loadingImage
	property bool running: false
	source: "img/loading.png"
	visible: running
	NumberAnimation on rotation {
		from: 360; to: 0; loops: Animation.Infinite; duration: 800
		running: loadingImage.running
	}
}

