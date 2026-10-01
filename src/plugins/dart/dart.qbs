import qbs 1.0

QtcPlugin {
    name: "Dart"

    Depends { name: "Core" }
    Depends { name: "ProjectExplorer" }
    Depends { name: "LanguageClient" }
    Depends { name: "Qt"; submodules: ["widgets"] }

    files: [
        "common/dartconstants.h",
        "common/dartsdk.cpp",
        "common/dartsdk.h",
        "common/darttr.h",
        "dartplugin.cpp",
        "languageserver/dartlanguageserver.cpp",
        "languageserver/dartlanguageserver.h",
        "project/dartproject.cpp",
        "project/dartproject.h",
        "project/dartprojectdefaults.cpp",
        "project/dartprojectdefaults.h",
    ]
}
