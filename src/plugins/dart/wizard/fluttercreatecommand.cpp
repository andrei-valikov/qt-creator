// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "fluttercreatecommand.h"

using namespace Utils;

namespace Dart::Internal {

static bool isApplicationTemplate(const QString &projectTemplate)
{
    return projectTemplate == "app" || projectTemplate == "empty"
           || projectTemplate == "skeleton" || projectTemplate == "module";
}

static bool supportsPlatforms(const QString &projectTemplate)
{
    return projectTemplate == "app" || projectTemplate == "empty"
           || projectTemplate == "skeleton" || projectTemplate == "plugin";
}

/*!
    Returns the command that creates a Flutter project in \a projectDir. Packages are not
    fetched, so that the command finishes quickly also without network access.
*/
CommandLine flutterCreateCommand(const FilePath &flutter,
                                 const FilePath &projectDir,
                                 const FlutterCreateOptions &options)
{
    CommandLine command(flutter, {"create", "--no-pub", "--project-name", options.projectName});
    if (!options.organization.isEmpty())
        command.addArgs({"--org", options.organization});
    if (!options.description.isEmpty())
        command.addArgs({"--description", options.description});
    if (options.projectTemplate == "empty")
        command.addArgs({"--template", "app", "--empty"});
    else
        command.addArgs({"--template", options.projectTemplate});
    if (supportsPlatforms(options.projectTemplate) && !options.platforms.isEmpty())
        command.addArgs({"--platforms", options.platforms.join(',')});
    command.addArg(projectDir.nativePath());
    return command;
}

FilePath flutterMainFile(const FilePath &projectDir, const FlutterCreateOptions &options)
{
    const QString fileName = isApplicationTemplate(options.projectTemplate)
                                 ? QString("main.dart")
                                 : options.projectName + ".dart";
    return projectDir / "lib" / fileName;
}

} // namespace Dart::Internal
