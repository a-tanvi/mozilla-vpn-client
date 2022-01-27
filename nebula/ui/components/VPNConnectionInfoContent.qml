import QtQuick 2.5
import QtQuick.Layouts 1.14
import QtQuick.Controls 2.14

import Mozilla.VPN 1.0

Flickable {
    id: root

    height: parent.height
    contentHeight: Math.max(content.height, height)
    width: parent.width

    ColumnLayout {
        id: content

        spacing: VPNTheme.theme.vSpacingSmall
        width: parent.width

        // IP Adresses
        RowLayout {
            width: parent.width

            ColumnLayout {
                Text {
                    color: VPNTheme.colors.grey20
                    text: "IP: 103.231.88.10"
                }
                Text {
                    color: VPNTheme.colors.grey20
                    text: "IPv6: 2001:ac8:40:b9::a09e"
                }
            }
        }

        Rectangle {
            color: "black"
            height: 1
            Layout.fillWidth: true
        }

        // Lottie animation
        Rectangle {
            color: "black"

            Layout.alignment: Qt.AlignHCenter
            Layout.preferredHeight: 100
            Layout.preferredWidth: 100
            Layout.minimumHeight: 50
            Layout.minimumWidth: 50
        }

        Rectangle {
            color: VPNTheme.colors.grey20
            height: 1
            Layout.fillWidth: true
        }

        // Bullet list
        RowLayout {

            ColumnLayout {
                Text {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    color: VPNTheme.colors.white
                    text: "At your current speed, here's what your device is optimized for:"
                }
                Text {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    color: VPNTheme.colors.grey20
                    text: "Streaming in 4K"
                }
                Text {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    color: VPNTheme.colors.grey20
                    text: "High-speed downloads"
                }
                Text {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    color: VPNTheme.colors.grey20
                    text: "Online gaming"
                }
            }

        }

        Rectangle {
            color: VPNTheme.colors.grey20
            height: 1
            Layout.fillWidth: true
        }

        // Detailed info section
        GridLayout {
            width: parent.width
            columns: 2

            VPNConnectionInfoItem {
                Layout.fillWidth: true

                title: VPNServerCountryModel.getLocalizedCountryName(VPNCurrentServer.exitCountryCode)
                subtitle: VPNCurrentServer.localizedCityName
                iconPath: "qrc:/nebula/resources/flags/" + VPNCurrentServer.exitCountryCode.toUpperCase() + ".png"
            }
            VPNConnectionInfoItem {
                Layout.fillWidth: true

                title: "Ping"
                subtitle: "15 ms"
                iconPath: "qrc:/nebula/resources/connection.svg"
            }
            VPNConnectionInfoItem {
                Layout.fillWidth: true

                title: "Download"
                subtitle: "300.06 Mbps"
                iconPath: "qrc:/nebula/resources/download.svg"
            }
            VPNConnectionInfoItem {
                Layout.fillWidth: true

                title: "Upload"
                subtitle: "21.60 Mbps"
                iconPath: "qrc:/nebula/resources/upload.svg"
            }
        }

        // Rectangle {
        //     color: "gray"

        //     anchors.fill: parent
        //     border.color: "red"
        //     border.width: 2
        //     z: -1
        // }

    }

}