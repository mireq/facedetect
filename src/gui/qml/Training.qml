/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  07.05.2011 17:54:38
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0

CentralWindow {
	PushButton {
		x: 10; y: 10; width: 100; height: 50
		text: "Start"
		onClicked: {
			runtime.startTraining();
		}
	}
}

