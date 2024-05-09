QT += core network widgets sql
CONFIG += c++17

INCLUDEPATH += src
INCLUDEPATH += $$PWD/include
TEMPLATE = lib
TARGET = UtilityLibrary

LIBS += -lcrypto

include(src/src.pri)
include(build.pri)
