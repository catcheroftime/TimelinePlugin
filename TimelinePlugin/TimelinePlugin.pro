CONFIG      += plugin debug_and_release
TARGET      = Timelineplugin
TEMPLATE    = lib

HEADERS     = timelinepluginplugin.h
SOURCES     = timelinepluginplugin.cpp
RESOURCES   = icons.qrc
LIBS        += -L. 

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += designer
} else {
    CONFIG += designer
}

target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS    += target


Debug:DESTDIR = $$PWD/../bin/debug
Release:DESTDIR = $$PWD/../bin/release

include(timelineplugin.pri)
