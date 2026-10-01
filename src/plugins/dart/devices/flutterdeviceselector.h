// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#pragma once

#include "../daemon/flutterdaemon.h"

#include <QToolButton>

namespace ProjectExplorer { class Project; }

namespace Dart::Internal {

class FlutterDeviceSelector final : public QToolButton
{
public:
    FlutterDeviceSelector();

    QString currentDeviceId() const;

private:
    void updateFromDevices();
    void updateForStartupProject(ProjectExplorer::Project *project);
    void selectDevice(const QString &id);

    FlutterDaemon m_daemon;
};

} // namespace Dart::Internal
