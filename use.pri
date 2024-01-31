INCLUDEPATH += $$PWD/include

LIBS += -L$$PWD/BIN -lUtilityLibrary

QMAKE_LFLAGS    += -Wl,-rpath,"'\$$ORIGIN'"/lib
