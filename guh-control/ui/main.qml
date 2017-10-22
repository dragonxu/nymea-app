import QtQuick 2.5
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import Qt.labs.settings 1.0
import Guh 1.0

ApplicationWindow {
    id: app
    visible: true
    width: 270 * 1.5
    height: 480 * 1.5

//    Material.primary: "#ff57baae"
    Material.primary: "white"
    Material.accent: "#ff57baae"

    property int margins: 10
    property int bigMargins: 20
    property int smallFont: 10
    property int largeFont: 20
    property int delegateHeight: 60

    Settings {
        id: settings
        property string lastConnectedHost: ""
    }

    Component.onCompleted: {
        pageStack.push(Qt.resolvedUrl("ConnectPage.qml"))
    }

    Connections {
        target: Engine.jsonRpcClient
        onConnectedChanged: {
            print("json client connected changed")
            if (Engine.jsonRpcClient.connected) {
                settings.lastConnectedHost = Engine.connection.url
            }
            init();
        }

        onAuthenticationRequiredChanged: {
            print("auth required changed")
            init();
        }
        onInitialSetupRequiredChanged: {
            print("setup required changed")
            init();
        }
    }

    function init() {
        pageStack.clear()
        if (Engine.jsonRpcClient.authenticationRequired || Engine.jsonRpcClient.initialSetupRequired) {
            var page = pageStack.push(Qt.resolvedUrl("LoginPage.qml"));
            page.backPressed.connect(function() {
                settings.lastConnectedHost = "";
                Engine.connection.disconnect()
            })
        } else if (Engine.jsonRpcClient.connected) {
            pageStack.push(Qt.resolvedUrl("MainPage.qml"))
        } else {
            pageStack.push(Qt.resolvedUrl("ConnectPage.qml"))
        }
    }

    StackView {
        id: pageStack
        anchors.fill: parent
        initialItem: Page {}
    }

    UpnpDiscovery {
        id: upnpDiscovery
    }
}
