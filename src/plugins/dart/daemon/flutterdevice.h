// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#pragma once

#include <QString>

namespace Dart::Internal {

class FlutterDevice
{
public:
    QString displayName() const
    {
        return category.isEmpty() ? name : QString("%1 (%2)").arg(name, category);
    }

    QString id;
    QString name;
    QString category;
};

} // namespace Dart::Internal
