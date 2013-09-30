QT       -= core gui

DEFINES += UNICODE _UNICODE

CONFIG += MSVC64bit

CONFIG(32bit) {
    TARGET = catchcopy32
    QMAKE_CFLAGS = -fno-keep-inline-dllexport -mtune=generic -march=i586 -fno-exceptions -Os -Wall -Wextra -fno-rtti -s -m32 -static-libgcc -static-libstdc++ -static
	QMAKE_CXXFLAGS = -fno-keep-inline-dllexport -mtune=generic -march=i586 -fno-exceptions -Os -Wall -Wno-write-strings -Wextra -fno-rtti -s -m32 -static-libgcc -static-libstdc++ -static
	QMAKE_LFLAGS += -m32 -static-libgcc -static-libstdc++ -fno-exceptions -static
	QMAKE_CXXFLAGS -= -fexceptions -O2
	CONFIG(CATCHCOPY_EXPLORER_PLUGIN_DEBUG) {
	TARGET = catchcopy32d
	}
}
CONFIG(64bit) {
    TARGET = catchcopy64
	DEFINES += _M_X64
	QMAKE_CFLAGS = -fno-keep-inline-dllexport -mtune=generic -march=nocona -fno-exceptions -Os -Wall -Wextra -fno-rtti -s -m64 -static-libgcc -static-libstdc++ -static
	QMAKE_CXXFLAGS = -fno-keep-inline-dllexport -mtune=generic -march=nocona -fno-exceptions -Os -Wall -fno-rtti -s -m64 -static-libgcc -static-libstdc++ -static
	QMAKE_LFLAGS += -m64 -static-libgcc -static-libstdc++ -fno-exceptions -static
	QMAKE_CXXFLAGS -= -fexceptions -O2
	CONFIG(CATCHCOPY_EXPLORER_PLUGIN_DEBUG) {
	TARGET = catchcopy64d
	}
}

CONFIG(MSVC32bit) {
    TARGET = catchcopy32
    QMAKE_CFLAGS = -Wall
	QMAKE_CXXFLAGS = -Wall
	CONFIG(CATCHCOPY_EXPLORER_PLUGIN_DEBUG) {
	TARGET = catchcopy32d
	}
}
CONFIG(MSVC64bit) {
    TARGET = catchcopy64
	QMAKE_CFLAGS = -Wall
	QMAKE_CXXFLAGS = -Wall
	CONFIG(CATCHCOPY_EXPLORER_PLUGIN_DEBUG) {
	TARGET = catchcopy64d
	}
}

DEF_FILE += CatchCopy.def

LIBS+= -lws2_32 -lole32 -luuid -ladvapi32 -lshell32 -luser32

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
