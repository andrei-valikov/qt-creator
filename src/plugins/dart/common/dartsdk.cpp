// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "dartsdk.h"

#include <utils/environment.h>
#include <utils/fileutils.h>
#include <utils/hostosinfo.h>

using namespace Utils;

namespace Dart::Internal {

static FilePath executableIn(const FilePath &binDir, const QString &name)
{
    const QStringList suffixes = HostOsInfo::isWindowsHost() ? QStringList{".exe", ".bat"}
                                                             : QStringList{QString()};
    for (const QString &suffix : suffixes) {
        const FilePath candidate = binDir.pathAppended(name + suffix);
        if (candidate.isExecutableFile())
            return candidate;
    }
    return {};
}

static FilePath findInSdkRoots(const QString &name, const QString &envVar, const QStringList &roots)
{
    FilePaths candidates;
    if (const QString root = qtcEnvironmentVariable(envVar); !root.isEmpty())
        candidates << FilePath::fromUserInput(root);
    const FilePath home = FileUtils::homePath();
    for (const QString &root : roots) {
        candidates << (root.startsWith('~') ? home.pathAppended(root.mid(2))
                                            : FilePath::fromUserInput(root));
    }
    for (const FilePath &root : std::as_const(candidates)) {
        if (const FilePath executable = executableIn(root / "bin", name); !executable.isEmpty())
            return executable;
    }
    return {};
}

FilePath flutterExecutable()
{
    const FilePath inPath = Environment::systemEnvironment().searchInPath("flutter");
    if (inPath.isExecutableFile())
        return inPath;
    return findInSdkRoots("flutter", "FLUTTER_ROOT",
                          {"~/flutter", "~/development/flutter", "~/snap/flutter/common/flutter",
                           "~/fvm/default", "/opt/flutter", "/usr/local/flutter",
                           "C:/src/flutter", "C:/flutter", "C:/tools/flutter"});
}

FilePath dartExecutable()
{
    if (const FilePath flutter = flutterExecutable(); !flutter.isEmpty()) {
        const FilePath bundled = executableIn(flutter.canonicalPath().parentDir(), "dart");
        if (!bundled.isEmpty())
            return bundled;
    }
    const FilePath inPath = Environment::systemEnvironment().searchInPath("dart");
    if (inPath.isExecutableFile())
        return inPath;
    return findInSdkRoots("dart", "DART_SDK",
                          {"/usr/lib/dart", "/opt/homebrew/opt/dart/libexec",
                           "/usr/local/opt/dart/libexec", "C:/tools/dart-sdk",
                           "C:/Program Files/Dart/dart-sdk"});
}

/*!
    Returns \a fallbackName if \a executable is what a \c PATH lookup of it finds, so that
    generated project files stay portable, and the full path of \a executable otherwise.
*/
QString toolCommand(const FilePath &executable, const QString &fallbackName)
{
    if (executable.isEmpty())
        return fallbackName;
    const FilePath inPath = Environment::systemEnvironment().searchInPath(fallbackName);
    return inPath == executable ? fallbackName : executable.path();
}

} // namespace Dart::Internal
