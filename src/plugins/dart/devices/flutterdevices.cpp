// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "flutterdevices.h"

#include "flutterdeviceselector.h"
#include "../common/darttr.h"

#include <coreplugin/icore.h>
#include <coreplugin/statusbarmanager.h>
#include <utils/macroexpander.h>

#include <QPointer>

using namespace Core;
using namespace Utils;

namespace Dart::Internal {

void setupFlutterDevices()
{
    const QPointer<FlutterDeviceSelector> selector = new FlutterDeviceSelector;
    StatusBarManager::addStatusBarWidget(selector, StatusBarManager::RightCorner);
    globalMacroExpander()->registerVariable(
        "Flutter:DeviceId", Tr::tr("The ID of the Flutter device selected in the status bar."),
        [selector] { return selector ? selector->currentDeviceId() : QString(); });
    QObject::connect(ICore::instance(), &ICore::coreAboutToClose, ICore::instance(), [selector] {
        if (selector)
            StatusBarManager::destroyStatusBarWidget(selector);
    });
}

} // namespace Dart::Internal
