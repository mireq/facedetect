/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  06.07.2010 18:47:57
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0

BorderImage {
	id: progressBar
	property double progress: 0.0
	height: 11
	width: 100
	source: "img/progress-outer.sci"

	BorderImage {
		id: progressInner
		anchors.top: parent.top
		anchors.bottom: parent.bottom
		source: "img/progress-inner.sci"
		width: progressBar.progress * (progressBar.width - progressBar.height) + progressBar.height
	}
}
