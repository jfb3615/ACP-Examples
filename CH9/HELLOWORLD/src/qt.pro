TEMPLATE = app
TARGET = helloWorld
DESTDIR = ../local/bin
DEPENDPATH += .
INCLUDEPATH += . .. 

mac {
  CONFIG -= app_bundle
}

QMAKE_CXX=mpicxx
QMAKE_LINK=mpicxx

CONFIG += qt debug c++17

# Input
SOURCES += *.cpp

QMAKE_DEL_FILE=rm -rf
QMAKE_DISTCLEAN += ../local

