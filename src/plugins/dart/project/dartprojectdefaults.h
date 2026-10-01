// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#pragma once

#include <QJsonObject>

namespace Utils { class FilePath; }

namespace Dart::Internal {

QJsonObject flutterProjectJson(const Utils::FilePath &projectDir);
QJsonObject dartProjectJson();

} // namespace Dart::Internal
