TEMPLATE = app
TARGET = doublePendulum
DESTDIR = ../local/bin
DEPENDPATH += . /usr/local/include /usr/include
INCLUDEPATH += . .. 

mac {
  CONFIG -= app_bundle
}

QT += widgets
CONFIG += qt debug c++17

# Input
SOURCES += *.cpp
HEADERS += *.h
FORMS   += *.ui

QMAKE_DEL_FILE=rm -rf
QMAKE_DISTCLEAN += ../local

mac {
  PKG_CONFIG_PATH += $$[QT_INSTALL_LIBS]/pkgconfig
  PKG_CONFIG = PKG_CONFIG_PATH=$$PKG_CONFIG_PATH pkg-config
}

CONFIG    += link_pkgconfig
PKGCONFIG += QatPlotWidgets



