/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import components 0.1

VPNLoader {
    objectName: "authenticatingView"

    //% "Waiting for sign in and subscription confirmation…"
    headlineText: qsTrId("vpn.authenticating.waitForSignIn")
}
