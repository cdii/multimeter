import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3
import custom 1.0

ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("multimeter")

    Client {
        id: client
        onErrorOccurred: {
            errorDialog.text = error
            errorDialog.open()
        }
    }

    MessageDialog {
        id: errorDialog
        title: "Error"
        icon: StandardIcon.Critical
    }

    header: ToolBar {
        RowLayout {
            anchors.fill: parent

            Button {
                id: connectButton
                text: client.connected ? qsTr("Disconnect") : qsTr("Connect")
                onClicked: {
                    if (client.connected) {
                        client.disconnectFromServer()
                    } else {
                        client.connectToServer(
                                    "/tmp/leaf/9Lq7BNBnBycd6nxy.socket")
                    }
                }
            }

            Button {
                id: echoButton
                text: qsTr("Send Hello")
                enabled: client.connected
                onClicked: {
                    client.sendMessage("Hello")
                }
            }
        }
    }

    TextArea {
        id: resultsArea
        readOnly: true
        wrapMode: Text.Wrap
    }

    Connections {
        target: client

        function onResponseReceived(response) {
            resultsArea.text = response
        }
    }
}
