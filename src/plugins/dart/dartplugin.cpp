// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "languageserver/dartlanguageserver.h"
#include "project/dartproject.h"

#include <extensionsystem/iplugin.h>

namespace Dart::Internal {

class DartPlugin final : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "Dart.json")

public:
    DartPlugin() = default;
    ~DartPlugin() final = default;

    void initialize() final;
};

void DartPlugin::initialize()
{
    setupDartProject();
    setupDartLanguageServer(this);
}

} // namespace Dart::Internal

#include "dartplugin.moc"
