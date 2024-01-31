QT += core network
CONFIG += c++14 console

INCLUDEPATH += src
TEMPLATE = lib
TARGET = UtilityLibrary

include(src/src.pri)
include(build.pri)
