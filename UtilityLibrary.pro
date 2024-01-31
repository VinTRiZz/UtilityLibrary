QT += core network
CONFIG += c++14

INCLUDEPATH += src
TEMPLATE = lib
TARGET = UtilityLibrary

LIBS += -lcrypto

include(src/src.pri)
include(build.pri)
