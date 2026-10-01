// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "dartprojectdefaults.h"

#include "../common/dartsdk.h"

#include <utils/filepath.h>
#include <utils/hostosinfo.h>

#include <QJsonArray>

using namespace Utils;

namespace Dart::Internal {

const char projectDirectory[] = "%{ActiveProject:ProjectDirectory}";

static QJsonObject step(const QString &tool, const QStringList &arguments)
{
    return {{"executable", tool},
            {"arguments", QJsonArray::fromStringList(arguments)},
            {"workingDirectory", projectDirectory}};
}

static QJsonObject buildConfiguration(const QString &name, const QJsonArray &steps)
{
    return {{"name", name}, {"steps", steps}};
}

static QJsonObject runTarget(const QString &name, const QJsonObject &command, bool useTerminal)
{
    QJsonObject target = command;
    target.insert("name", name);
    target.insert("useTerminal", useTerminal);
    return target;
}

static QString releasePlatform(const FilePath &projectDir)
{
    const QString host = HostOsInfo::isWindowsHost() ? QString("windows")
                         : HostOsInfo::isMacHost()   ? QString("macos")
                                                     : QString("linux");
    const QList<std::pair<QString, QString>> platforms
        = {{host, host}, {"android", "apk"}, {"web", "web"}};
    for (const auto &[directory, platform] : platforms) {
        if ((projectDir / directory).isDir())
            return platform;
    }
    return {};
}

static QJsonObject projectJson(const QJsonArray &builds, const QJsonArray &targets,
                               const QStringList &excludes)
{
    return {{"build.configuration", builds},
            {"targets", targets},
            {"files.exclude", QJsonArray::fromStringList(excludes)}};
}

QJsonObject flutterProjectJson(const FilePath &projectDir)
{
    const QString flutter = toolCommand(flutterExecutable(), "flutter");
    const QJsonObject pubGet = step(flutter, {"pub", "get"});
    QJsonArray builds = {buildConfiguration("Debug", {pubGet})};
    if (const QString platform = releasePlatform(projectDir); !platform.isEmpty()) {
        builds.append(buildConfiguration(QString("Release (%1)").arg(platform),
                                         {pubGet, step(flutter, {"build", platform})}));
    }
    const QJsonArray targets
        = {runTarget("flutter run", step(flutter, {"run"}), true),
           runTarget("flutter run --release", step(flutter, {"run", "--release"}), true),
           runTarget("flutter test", step(flutter, {"test"}), false)};
    return projectJson(builds, targets,
                       {"build", ".dart_tool", ".idea", "*.iml", ".flutter-plugins",
                        ".flutter-plugins-dependencies", "pubspec.lock"});
}

QJsonObject dartProjectJson()
{
    const QString dart = toolCommand(dartExecutable(), "dart");
    const QJsonObject pubGet = step(dart, {"pub", "get"});
    const QJsonArray builds = {buildConfiguration("Debug", {pubGet})};
    const QJsonArray targets = {runTarget("dart run", step(dart, {"run"}), true),
                                runTarget("dart test", step(dart, {"test"}), false)};
    return projectJson(builds, targets, {"build", ".dart_tool", "pubspec.lock"});
}

} // namespace Dart::Internal
