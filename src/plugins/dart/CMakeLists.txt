add_qtc_plugin(Dart
  PLUGIN_DEPENDS Core ProjectExplorer LanguageClient
  SOURCES
    common/dartconstants.h
    common/dartsdk.cpp
    common/dartsdk.h
    common/darttr.h
    dartplugin.cpp
    languageserver/dartlanguageserver.cpp
    languageserver/dartlanguageserver.h
    project/dartproject.cpp
    project/dartproject.h
    project/dartprojectdefaults.cpp
    project/dartprojectdefaults.h
)
