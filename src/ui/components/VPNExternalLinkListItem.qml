/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import QtQuick 2.5
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11
import Mozilla.VPN 1.0
import "../themes/themes.js" as Theme

// VPNExternalLinkListItem
VPNClickableRow {
    property alias title: title.text
    property var iconSource:  "../resources/externalLink.svg"

    backgroundColor: Theme.clickableRowBlue

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: Theme.windowMargin / 2
        anchors.rightMargin: Theme.windowMargin / 2

        VPNBoldLabel {
            id: title
        }

        Item {
            Layout.fillWidth: true
        }

        VPNIcon {
            source: iconSource
        }

    }

}
