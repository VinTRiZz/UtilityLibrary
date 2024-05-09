QT += core network widgets sql
CONFIG += c++17

INCLUDEPATH += src
TEMPLATE = lib
TARGET = UtilityLibrary

LIBS += -lcrypto
LIBS += -lsqlite3

include(src/src.pri)
include(build.pri)
