// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#pragma once

#include <utils/commandline.h>

#include <QStringList>

namespace Dart::Internal {

class FlutterCreateOptions
{
public:
    QString projectName;
    QString organization;
    QString description;
    QString projectTemplate;
    QStringList platforms;
};

Utils::CommandLine flutterCreateCommand(const Utils::FilePath &flutter,
                                        const Utils::FilePath &projectDir,
                                        const FlutterCreateOptions &options);
Utils::FilePath flutterMainFile(const Utils::FilePath &projectDir,
                                const FlutterCreateOptions &options);

} // namespace Dart::Internal
