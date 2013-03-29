QT       -= core gui

CONFIG += 32bit

CONFIG(32bit) {
    TARGET = catchcopy32
    QMAKE_CXXFLAGS += -m32
    QMAKE_CFLAGS = -fno-keep-inline-dllexport -mtune=generic -march=i586 -fno-exceptions -Os -Wall -Wextra -fno-rtti -s -m32
	QMAKE_CXXFLAGS = -fno-keep-inline-dllexport -mtune=generic -march=i586 -fno-exceptions -Os -Wall -Wextra -fno-rtti -s -m32
	QMAKE_LFLAGS += -m32
}
CONFIG(64bit) {
    TARGET = catchcopy64
    INCPATH  += C:\MinGW64\x86_64-w64-mingw32\include
    LIBS += -LC:\MinGW64\x86_64-w64-mingw32\lib
	DEFINES += _M_X64
	QMAKE_CFLAGS = -fno-keep-inline-dllexport -mtune=generic -march=nocona -fno-exceptions -Os -Wall -Wextra -fno-rtti -s
	QMAKE_CXXFLAGS = -fno-keep-inline-dllexport -mtune=generic -march=nocona -fno-exceptions -Os -Wall -Wextra -fno-rtti -s
}

DEF_FILE += CatchCopy.def

LIBS += -lole32 -luuid -lws2_32 -ladvapi32 -lshell32 -luser32 -lkernel32 -lgdi32

TEMPLATE = lib

HEADERS += \
    Variable.h \
    resource.h \
    Reg.h \
    ClientCatchcopy.h \
    DDShellExt.h \
    ClassFactory.h

SOURCES += \
    ClientCatchcopy.cpp \
    Reg.cpp \
    DDShellExt.cpp \
    ClassFactory.cpp \
    CatchCopy.cpp
