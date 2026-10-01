// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "dartlanguageserver.h"

#include "../common/dartconstants.h"
#include "../common/dartsdk.h"
#include "../common/darttr.h"

#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/editormanager/ieditor.h>
#include <coreplugin/icore.h>
#include <coreplugin/idocument.h>
#include <coreplugin/messagemanager.h>
#include <languageclient/languageclientmanager.h>
#include <projectexplorer/project.h>
#include <projectexplorer/projectmanager.h>
#include <utils/algorithm.h>

using namespace Core;
using namespace LanguageClient;
using namespace ProjectExplorer;
using namespace Utils;

namespace Dart::Internal {

const char autoSetupDoneKey[] = "Dart/LanguageServerAutoSetupDone";

static bool hasDartLanguageServer()
{
    return Utils::anyOf(LanguageClientManager::currentSettings(), [](BaseSettings *settings) {
        return settings->mimeTypes().contains(QString(Constants::DART_MIMETYPE));
    });
}

static void ensureDartLanguageServer()
{
    if (ICore::settings()->value(autoSetupDoneKey).toBool() || hasDartLanguageServer())
        return;

    const FilePath dart = dartExecutable();
    if (dart.isEmpty())
        return;

    auto settings = new StdIOSettings;
    settings->name.setValue(Tr::tr("Dart Language Server"));
    settings->executable.setValue(dart);
    settings->arguments.setValue("language-server --protocol=lsp --client-id=qt-creator");
    settings->mimeTypes.setValue(QStringList{Constants::DART_MIMETYPE});
    settings->startBehavior.setValue(BaseSettings::RequiresProject);
    LanguageClientSettings::addSettings(settings);
    LanguageClientManager::applySettings();

    ICore::settings()->setValue(autoSetupDoneKey, true);
    MessageManager::writeSilently(
        Tr::tr("Set up the Dart language server (%1).").arg(dart.toUserOutput()));
}

void setupDartLanguageServer(QObject *guard)
{
    QObject::connect(EditorManager::instance(), &EditorManager::editorOpened, guard,
                     [](IEditor *editor) {
                         if (editor && editor->document()
                             && editor->document()->mimeType() == Constants::DART_MIMETYPE) {
                             ensureDartLanguageServer();
                         }
                     });
    QObject::connect(ProjectManager::instance(), &ProjectManager::projectAdded, guard,
                     [](Project *project) {
                         if ((project->projectDirectory() / "pubspec.yaml").isFile())
                             ensureDartLanguageServer();
                     });
}

} // namespace Dart::Internal
