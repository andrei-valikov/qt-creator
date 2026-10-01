// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#pragma once

#include <utils/filepath.h>

namespace Dart::Internal {

Utils::FilePath flutterExecutable();
Utils::FilePath dartExecutable();

QString toolCommand(const Utils::FilePath &executable, const QString &fallbackName);

} // namespace Dart::Internal
