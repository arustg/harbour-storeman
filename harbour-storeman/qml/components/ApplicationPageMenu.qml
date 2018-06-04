import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0

PullDownMenu {
    readonly property bool _enableMenu: networkManager.online &&
                                        !itemInProgress(app.repoAlias) &&
                                        !itemInProgress(app.packageName)

    id: pullMenu
    visible: OrnPm.initialised

    MenuItem {
        id: repoMenuItem
        visible: text
        enabled: _enableMenu
        text: {
            if (!app.repoAlias) {
                return ""
            }
            switch (app.repoStatus) {
            case OrnPm.RepoNotInstalled:
                //% "Add repository"
                return qsTrId("orn-repo-add")
            case OrnPm.RepoDisabled:
                //% "Enable repository"
                return qsTrId("orn-repo-enable")
            default:
                return qsTrId("orn-refresh-cache")
            }
        }

        onClicked: {
            switch (app.repoStatus) {
            case OrnPm.RepoNotInstalled:
                //% "Adding"
                Remorse.popupAction(page, qsTrId("orn-adding-repo"), function() {
                    OrnPm.addRepo(app.userName)
                })
                break
            case OrnPm.RepoDisabled:
                OrnPm.modifyRepo(app.repoAlias, OrnPm.EnableRepo)
                break
            default:
                OrnPm.refreshRepo(app.repoAlias, true)
                break
            }
        }
    }

    MenuItem {
        id: installMenuItem
        visible: text
        enabled: _enableMenu
        text: {
            switch (_packageStatus) {
            case OrnPm.PackageAvailable:
                //% "Install"
                return qsTrId("orn-install")
            case OrnPm.PackageInstalled:
            case OrnPm.PackageUpdateAvailable:
                //% "Remove"
                return qsTrId("orn-remove")
            default:
                return ""
            }
        }

        onClicked: {
            switch (_packageStatus) {
            case OrnPm.PackageAvailable:
                OrnPm.installPackage(app.availableId)
                break
            case OrnPm.PackageInstalled:
                Remorse.popupAction(page, qsTrId("orn-removing"), function() {
                    OrnPm.removePackage(app.installedId)
                })
                break
            }
        }
    }

    MenuItem {
        id: updateMenuItem
        visible: _packageStatus == OrnPm.PackageUpdateAvailable
        enabled: _enableMenu
        //% "Update"
        text: qsTrId("orn-update")
        onClicked: OrnPm.updatePackage(app.packageName)
    }

    MenuItem {
        id: launchMenuItem
        visible: app.desktopFile
        //% "Launch"
        text: qsTrId("orn-launch")
        onClicked: Qt.openUrlExternally(app.desktopFile)
    }

    MenuStatusLabel { }
}
