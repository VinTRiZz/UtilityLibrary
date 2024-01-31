<<<<<<< HEAD
QT += core network
=======
QT += core network widgets
>>>>>>> master
CONFIG += c++14

INCLUDEPATH += src
TEMPLATE = lib
TARGET = UtilityLibrary

LIBS += -lcrypto

include(src/src.pri)
include(build.pri)
