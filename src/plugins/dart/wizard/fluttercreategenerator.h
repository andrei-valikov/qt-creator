// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#pragma once

#include <projectexplorer/jsonwizard/jsonwizardgeneratorfactory.h>

#include <utils/commandline.h>

namespace Dart::Internal {

class FlutterCreateGenerator final : public ProjectExplorer::JsonWizardGenerator
{
public:
    Utils::Result<> setup(const QVariant &data);

    Core::GeneratedFiles fileList(Utils::MacroExpander *expander,
                                  const Utils::FilePath &wizardDir,
                                  const Utils::FilePath &projectDir,
                                  QString *errorMessage) final;
    Utils::Result<> writeFile(const ProjectExplorer::JsonWizard *wizard,
                              Core::GeneratedFile *file) final;
    Utils::Result<> allDone(const ProjectExplorer::JsonWizard *wizard,
                            Core::GeneratedFile *file) final;

private:
    QVariantMap m_data;
    Utils::CommandLine m_createCommand;
    Utils::FilePath m_pubspec;
};

void setupFlutterCreateGenerator();

} // namespace Dart::Internal
