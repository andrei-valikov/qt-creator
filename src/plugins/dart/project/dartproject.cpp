// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "dartproject.h"

#include "dartprojectdefaults.h"
#include "../common/dartconstants.h"

#include <projectexplorer/projectmanager.h>
#include <projectexplorer/workspaceproject.h>

#include <QRegularExpression>

using namespace ProjectExplorer;
using namespace Utils;

namespace Dart::Internal {

bool isFlutterPackage(const FilePath &pubspec)
{
    static const QRegularExpression flutterSdk(R"(^\s+sdk:\s*flutter\b)",
                                               QRegularExpression::MultilineOption);
    const Result<QByteArray> contents = pubspec.fileContents();
    return contents && flutterSdk.match(QString::fromUtf8(*contents)).hasMatch();
}

static QJsonObject defaultProjectJson(const FilePath &pubspec)
{
    return isFlutterPackage(pubspec) ? flutterProjectJson(pubspec.parentDir()) : dartProjectJson();
}

class DartProject final : public WorkspaceProject
{
public:
    explicit DartProject(const FilePath &pubspec)
        : WorkspaceProject(pubspec.parentDir(), defaultProjectJson(pubspec))
    {}
};

void setupDartProject()
{
    ProjectManager::registerProjectType<DartProject>(Constants::PUBSPEC_MIMETYPE);
}

} // namespace Dart::Internal
