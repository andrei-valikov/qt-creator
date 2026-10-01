// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "fluttercreategenerator.h"

#include "fluttercreatecommand.h"
#include "../common/dartsdk.h"
#include "../common/darttr.h"

#include <coreplugin/progressmanager/processprogress.h>
#include <utils/globaltasktree.h>
#include <utils/macroexpander.h>
#include <utils/qtcprocess.h>

using namespace Core;
using namespace ProjectExplorer;
using namespace QtTaskTree;
using namespace Utils;

namespace Dart::Internal {

Result<> FlutterCreateGenerator::setup(const QVariant &data)
{
    if (data.typeId() != QMetaType::QVariantMap)
        return ResultError(Tr::tr("Key is not an object."));
    m_data = data.toMap();
    if (!m_data.contains("projectName"))
        return ResultError(Tr::tr("No \"projectName\" given."));
    return ResultOk;
}

GeneratedFiles FlutterCreateGenerator::fileList(MacroExpander *expander,
                                                const FilePath &,
                                                const FilePath &projectDir,
                                                QString *errorMessage)
{
    const FilePath flutter = flutterExecutable();
    if (flutter.isEmpty()) {
        *errorMessage = Tr::tr("The Flutter SDK was not found. Install Flutter and add its "
                               "\"bin\" directory to PATH, or set FLUTTER_ROOT.");
        return {};
    }

    const auto value = [this, expander](const QString &key) {
        return expander->expand(m_data.value(key).toString()).trimmed();
    };
    const FlutterCreateOptions options{value("projectName"),
                                       value("organization"),
                                       value("description"),
                                       value("template"),
                                       value("platforms").split(',', Qt::SkipEmptyParts)};
    m_createCommand = flutterCreateCommand(flutter, projectDir, options);
    m_pubspec = projectDir / "pubspec.yaml";

    GeneratedFile pubspec(m_pubspec);
    pubspec.setAttributes(pubspec.attributes() | GeneratedFile::OpenProjectAttribute
                          | GeneratedFile::CustomGeneratorAttribute);
    GeneratedFile mainFile(flutterMainFile(projectDir, options));
    mainFile.setAttributes(mainFile.attributes() | GeneratedFile::OpenEditorAttribute
                           | GeneratedFile::SkipFormat | GeneratedFile::CustomGeneratorAttribute);
    return {pubspec, mainFile};
}

Result<> FlutterCreateGenerator::writeFile(const JsonWizard *, GeneratedFile *file)
{
    if (file->filePath() != m_pubspec)
        return ResultOk;

    Process process;
    process.setCommand(m_createCommand);
    process.runBlocking(std::chrono::seconds(180));
    if (process.result() != ProcessResult::FinishedWithSuccess)
        return ResultError(process.verboseExitMessage());
    return ResultOk;
}

Result<> FlutterCreateGenerator::allDone(const JsonWizard *, GeneratedFile *file)
{
    if (file->filePath() != m_pubspec)
        return ResultOk;

    const auto onSetup = [flutter = m_createCommand.executable(),
                          projectDir = m_pubspec.parentDir()](Process &process) {
        process.setCommand(CommandLine(flutter, {"pub", "get"}));
        process.setWorkingDirectory(projectDir);
        auto progress = new ProcessProgress(&process);
        progress->setDisplayName(Tr::tr("Fetching Flutter Packages"));
    };
    GlobalTaskTree::start(Group{ProcessTask(onSetup)});
    return ResultOk;
}

void setupFlutterCreateGenerator()
{
    static JsonWizardGeneratorTypedFactory<FlutterCreateGenerator> factory("FlutterCreate");
}

} // namespace Dart::Internal
