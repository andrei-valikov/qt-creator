// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#pragma once

#include "flutterdevice.h"

#include <utils/qtcprocess.h>

#include <QList>
#include <QObject>

#include <memory>

namespace Dart::Internal {

class FlutterDaemon final : public QObject
{
    Q_OBJECT

public:
    explicit FlutterDaemon(QObject *parent = nullptr)
        : QObject(parent)
    {}
    ~FlutterDaemon() final;

    void start();
    void restart();
    const QList<FlutterDevice> &devices() const { return m_devices; }

signals:
    void devicesChanged();

private:
    void stop();
    void handleLine(const QString &line);
    void setDevices(const QList<FlutterDevice> &devices);

    std::unique_ptr<Utils::Process> m_process;
    QList<FlutterDevice> m_devices;
};

} // namespace Dart::Internal
