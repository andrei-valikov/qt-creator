// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "flutterdeviceselector.h"

#include "../common/darttr.h"
#include "../project/dartproject.h"

#include <coreplugin/icore.h>
#include <projectexplorer/project.h>
#include <projectexplorer/projectmanager.h>
#include <utils/algorithm.h>
#include <utils/hostosinfo.h>

#include <QMenu>

using namespace Core;
using namespace ProjectExplorer;
using namespace Utils;

namespace Dart::Internal {

const char selectedDeviceKey[] = "Dart/FlutterDevice";

static QString hostDeviceId()
{
    if (HostOsInfo::isWindowsHost())
        return QString("windows");
    return HostOsInfo::isMacHost() ? QString("macos") : QString("linux");
}

FlutterDeviceSelector::FlutterDeviceSelector()
{
    setAutoRaise(true);
    setPopupMode(QToolButton::InstantPopup);
    setToolTip(Tr::tr("The device that \"flutter run\" runs the application on."));
    setMenu(new QMenu(this));
    // Queued, as rebuilding the menu deletes the action that may have triggered the change.
    connect(&m_daemon, &FlutterDaemon::devicesChanged,
            this, &FlutterDeviceSelector::updateFromDevices, Qt::QueuedConnection);
    connect(ProjectManager::instance(), &ProjectManager::startupProjectChanged,
            this, &FlutterDeviceSelector::updateForStartupProject);
    updateFromDevices();
    updateForStartupProject(ProjectManager::startupProject());
}

QString FlutterDeviceSelector::currentDeviceId() const
{
    const QString selected = ICore::settings()->value(selectedDeviceKey).toString();
    const QList<FlutterDevice> &devices = m_daemon.devices();
    if (devices.isEmpty())
        return selected.isEmpty() ? hostDeviceId() : selected;
    if (Utils::anyOf(devices, [&selected](const FlutterDevice &d) { return d.id == selected; }))
        return selected;
    return devices.first().id;
}

void FlutterDeviceSelector::updateFromDevices()
{
    const QString current = currentDeviceId();
    menu()->clear();
    setText(Tr::tr("No Flutter Devices"));
    for (const FlutterDevice &device : m_daemon.devices()) {
        QAction *action = menu()->addAction(device.displayName());
        action->setCheckable(true);
        action->setChecked(device.id == current);
        connect(action, &QAction::triggered, this, [this, id = device.id] { selectDevice(id); });
        if (device.id == current)
            setText(device.displayName());
    }
    menu()->addSeparator();
    menu()->addAction(Tr::tr("Restart Flutter Daemon"), this, [this] { m_daemon.restart(); });
}

void FlutterDeviceSelector::updateForStartupProject(Project *project)
{
    const bool isFlutter = project
                           && isFlutterPackage(project->projectDirectory() / "pubspec.yaml");
    setVisible(isFlutter);
    if (isFlutter)
        m_daemon.start();
}

void FlutterDeviceSelector::selectDevice(const QString &id)
{
    ICore::settings()->setValue(selectedDeviceKey, id);
    QMetaObject::invokeMethod(this, &FlutterDeviceSelector::updateFromDevices,
                              Qt::QueuedConnection);
}

} // namespace Dart::Internal
