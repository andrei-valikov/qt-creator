// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "flutterdaemon.h"

#include "../common/dartsdk.h"

#include <utils/algorithm.h>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

using namespace Utils;

namespace Dart::Internal {

FlutterDaemon::~FlutterDaemon()
{
    stop();
}

void FlutterDaemon::start()
{
    if (m_process && m_process->isRunning())
        return;
    const FilePath flutter = flutterExecutable();
    if (flutter.isEmpty())
        return;

    m_process.reset(new Process);
    m_process->setProcessMode(ProcessMode::Writer);
    m_process->setCommand(CommandLine(flutter, {"daemon"}));
    m_process->setUtf8StdOutCodec();
    m_process->setStdOutLineCallback([this](const QString &line) { handleLine(line); });
    connect(m_process.get(), &Process::started, this, [this] {
        m_process->write("[{\"id\":1,\"method\":\"device.enable\"}]\n");
    });
    connect(m_process.get(), &Process::done, this, [this] { setDevices({}); });
    m_process->start();
}

void FlutterDaemon::restart()
{
    stop();
    setDevices({});
    start();
}

void FlutterDaemon::stop()
{
    if (!m_process)
        return;
    m_process->disconnect(this);
    if (m_process->isRunning()) {
        m_process->write("[{\"id\":2,\"method\":\"daemon.shutdown\"}]\n");
        m_process->closeWriteChannel();
    }
    m_process.reset();
}

void FlutterDaemon::handleLine(const QString &line)
{
    const QJsonDocument message = QJsonDocument::fromJson(line.trimmed().toUtf8());
    if (!message.isArray())
        return;

    QList<FlutterDevice> devices = m_devices;
    for (const QJsonValue &value : message.array()) {
        const QJsonObject object = value.toObject();
        const QString event = object.value("event").toString();
        const QJsonObject params = object.value("params").toObject();
        const QString id = params.value("id").toString();
        if (event != "device.added" && event != "device.removed")
            continue;
        devices.removeIf([id](const FlutterDevice &device) { return device.id == id; });
        if (event == "device.added") {
            devices.append({id, params.value("name").toString(),
                            params.value("category").toString()});
        }
    }
    setDevices(devices);
}

void FlutterDaemon::setDevices(const QList<FlutterDevice> &devices)
{
    const auto ids = [](const QList<FlutterDevice> &list) {
        return Utils::transform<QStringList>(list, [](const FlutterDevice &d) { return d.id; });
    };
    if (ids(devices) == ids(m_devices))
        return;
    m_devices = devices;
    emit devicesChanged();
}

} // namespace Dart::Internal
