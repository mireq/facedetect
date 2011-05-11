/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  03.05.2011 19:00:01
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

import QtQuick 1.0
import "../"

GroupBox {
	id: gaborDelegate
	property int lineHeight: 32
	property string lambdaStr: "λ=" + model.lambda
	property string thetaStr: "θ=" + model.theta
	property string psiStr: "Ψ=" + model.psi
	property string sigmaStr: "σ=" + model.sigma
	property string gammaStr: "γ=" + model.gamma
	property string lumStr: "l=" + model.lum
	property variant sourceModel: null
	property Item listView
	signal changed()
	property double _moreOpacity: 0
	property int _moreHeight: 0

	Component.onCompleted: {
		lambdaValue.value = model.lambda;
		thetaValue.value = model.theta;
		psiValue.value = model.psi;
		sigmaValue.value = model.sigma;
		gammaValue.value = model.gamma;
		lumValue.value = model.lum;
	}

	width: listView.width
	background: "img/item_header.sci"
	titleLeft: ExpandButton {
		id: closeImage
		open: false
		anchors.verticalCenter: parent.verticalCenter
	}
	titleRight: PushButton {
		id: deleteButton
		anchors.verticalCenter: parent.verticalCenter
		text: qsTr("Delete")
		height: 28; fontPixelSize: 17
		onClicked: sourceModel.remove(index)
	}
	onTitleClicked: closeImage.open = !closeImage.open
	title: lambdaStr + ", " + thetaStr + ", " + psiStr + ", " + sigmaStr + ", " + gammaStr + ", " + lumStr;
	closed: !closeImage.open
	Item {
		Component {
			id: label
			Text {
				color: "black"
				elide: Text.ElideRight
				width: Math.round(grid.width / 2); height: lineHeight
				verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignRight
			}
		}
	}
	Grid {
		id: grid
		columns: 2; spacing: 5
		height: childrenRect.height + 5; y: 5
		anchors { left: parent.left; right: parent.right }
		Loader { sourceComponent: label; Component.onCompleted: item.text = qsTr("Lambda") }
		Item {
			width: Math.round(grid.width / 2) - grid.spacing; height: lineHeight
			SpinBox {
				id: lambdaValue
				anchors { verticalCenter: parent.verticalCenter; left: parent.left }
				minValue: 0.1; maxValue: 40; step: 0.1
				onValueChanged: {
					sourceModel.setProperty(index, "lambda", value);
					if (moreButtonContainer.enabled && !moreButton.checked) {
						sigmaValue.value = value / 2.0;
					}
					gaborDelegate.changed();
				}
			}
		}
		Loader { sourceComponent: label; Component.onCompleted: item.text = qsTr("Theta") }
		Item {
			width: Math.round(grid.width / 2) - grid.spacing; height: lineHeight
			SpinBox {
				id: thetaValue
				anchors { verticalCenter: parent.verticalCenter; left: parent.left }
				minValue: 0; maxValue: 360; step: 15; decimals: 5
				onValueChanged: {
					sourceModel.setProperty(index, "theta", value);
					gaborDelegate.changed();
				}
			}
		}
	}
	Grid {
		id: gridMore
		property bool more: moreButton.checked || lambdaValue.value != sigmaValue.value * 2.0
		columns: 2; spacing: 5
		height: _moreHeight; opacity: _moreOpacity
		anchors { left: parent.left; right: parent.right; top: grid.bottom }
		Loader { sourceComponent: label; Component.onCompleted: item.text = qsTr("Psi") }
		Item {
			width: Math.round(grid.width / 2 - grid.spacing); height: lineHeight
			SpinBox {
				id: psiValue
				anchors { verticalCenter: parent.verticalCenter; left: parent.left }
				step: 0.1
				onValueChanged: {
					sourceModel.setProperty(index, "psi", value);
					gaborDelegate.changed();
				}
			}
		}
		Loader { sourceComponent: label; Component.onCompleted: item.text = qsTr("Sigma") }
		Item {
			width: Math.round(grid.width / 2 - grid.spacing); height: lineHeight
			SpinBox {
				id: sigmaValue
				anchors { verticalCenter: parent.verticalCenter; left: parent.left }
				minValue: 0.05; maxValue: 20; step: 0.1
				onValueChanged: {
					sourceModel.setProperty(index, "sigma", value);
					gaborDelegate.changed();
				}
			}
		}
		Loader { sourceComponent: label; Component.onCompleted: item.text = qsTr("Gamma") }
		Item {
			width: Math.round(grid.width / 2 - grid.spacing); height: lineHeight
			SpinBox {
				id: gammaValue
				anchors { verticalCenter: parent.verticalCenter; left: parent.left }
				step: 0.1
				onValueChanged: {
					sourceModel.setProperty(index, "gamma", value);
					gaborDelegate.changed();
				}
			}
		}
		Loader { sourceComponent: label; Component.onCompleted: item.text = qsTr("Luminance") }
		Item {
			width: Math.round(grid.width / 2 - grid.spacing); height: lineHeight
			SpinBox {
				id: lumValue
				anchors { verticalCenter: parent.verticalCenter; left: parent.left }
				step: 0.01
				onValueChanged: {
					sourceModel.setProperty(index, "lum", value);
					gaborDelegate.changed();
				}
			}
		}
		states: [
			State {
				name: "open"; when: gridMore.more == true
				PropertyChanges { target: gaborDelegate; _moreOpacity: 1; _moreHeight: gridMore.childrenRect.height + 5 }
			},
			State {
				name: "closed"; when: gridMore.more == false
				PropertyChanges { target: gaborDelegate; _moreOpacity: 0; _moreHeight: 0 }
			}
		]
		transitions: [
			Transition {
				SequentialAnimation {
					PropertyAction { target: gridMore; property: "clip"; value: true }
					ParallelAnimation {
						NumberAnimation { properties: "_moreHeight, _moreOpacity"; duration: 200 }
					}
					PropertyAction { target: gridMore; property: "clip"; value: false }
				}
			}
		]
	}
	Item {
		id: moreButtonContainer
		property bool enabled: lambdaValue.value == sigmaValue.value * 2.0
		clip: true
		anchors { left: parent.left; right: parent.right; top: gridMore.bottom }
		height: lineHeight
		PushButton {
			id: moreButton
			enabled: parent.enabled
			anchors { top: parent.top }
			x: Math.round(grid.width / 2 + grid.spacing); height: lineHeight
			shape: "img/pbutton_normal.sci"
			pressedShape: "img/pbutton_checked.sci"
			text: qsTr("More ...")
			onClicked: {
				checked = !checked;
			}
		}
		states: [
			State {
				name: "disabled"; when: moreButtonContainer.enabled == false
				PropertyChanges { target: moreButtonContainer; height: 0 }
			}
		]
		transitions: Transition {
			NumberAnimation { properties: "height"; duration: 200 }
		}
	}
}

