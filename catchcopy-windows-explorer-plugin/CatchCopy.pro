QT       -= core gui

DEFINES += UNICODE _UNICODE

CONFIG += 32bit

CONFIG(32bit) {
    TARGET = catchcopy32
    QMAKE_CFLAGS = -fno-keep-inline-dllexport -mtune=generic -march=i586 -fno-exceptions -Os -Wall -Wextra -fno-rtti -s -m32
	QMAKE_CXXFLAGS = -fno-keep-inline-dllexport -mtune=generic -march=i586 -fno-exceptions -Os -Wall -Wno-write-strings -Wextra -fno-rtti -s -m32
	QMAKE_LFLAGS += -m32
	CONFIG(CATCHCOPY_EXPLORER_PLUGIN_DEBUG) {
	TARGET = catchcopy32d
	}
}
CONFIG(64bit) {
    TARGET = catchcopy64
    LIBS += -LC:\Qt\Qt5.0.1\Tools\MinGW\i686-w64-mingw32\lib64
	DEFINES += _M_X64
	QMAKE_CFLAGS = -fno-keep-inline-dllexport -mtune=generic -march=nocona -fno-exceptions -Os -Wall -Wextra -fno-rtti -s
	QMAKE_CXXFLAGS = -fno-keep-inline-dllexport -mtune=generic -march=nocona -fno-exceptions -Os -Wall -Wno-write-strings -Wextra -fno-rtti -s
	CONFIG(CATCHCOPY_EXPLORER_PLUGIN_DEBUG) {
	TARGET = catchcopy64d
	}
}

DEF_FILE += CatchCopy.def

LIBS+= -lws2_32 -lole32 -luuid

TEMPLATE = lib

HEADERS += \
    Variable.h \
    Deque.h \
    resource.h \
    Reg.h \
    ClientCatchcopy.h \
    DDShellExt.h \
    ClassFactory.h

SOURCES += \
    Deque.cpp \
    ClientCatchcopy.cpp \
    Reg.cpp \
    DDShellExt.cpp \
    ClassFactory.cpp \
    CatchCopy.cpp
