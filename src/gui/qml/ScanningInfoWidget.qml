/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  08.05.2011 10:53:46
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0

ProgressTitleItem {
	property variant scanner: null
	progress: scanner == null ? 0 : scanner.progress
	statusText: qsTr("Scanning ...")
	statusExtendedText: ""
	anchors.fill: parent
	onProgressChanged: {
		statusExtendedText =
			qsTr("Scanned") + " " +
			qsTr("%n folder(s)", "", parseInt(scanner == null ? 0 : scanner.scannedDirs)) +
			" " + qsTr("and") + " " +
			qsTr("%n file(s)", "", parseInt(scanner == null ? 0 : scanner.scannedFiles));
	}
}
