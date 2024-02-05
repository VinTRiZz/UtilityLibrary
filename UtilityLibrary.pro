QT += core network widgets
CONFIG += c++14

INCLUDEPATH += src
TEMPLATE = lib
TARGET = UtilityLibrary

LIBS += -lcrypto
LIBS += -lsqlite3

include(src/src.pri)
include(build.pri)
