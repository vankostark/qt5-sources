QT += widgets network testlib
CONFIG += testcase

TEMPLATE = app
!embedded:QT += uitools
TARGET = ../tst_uiloader
DEFINES += SRCDIR=\\\"$$PWD\\\"

win32 {
  CONFIG(debug, debug|release) {
    TARGET = ../../debug/tst_uiloader
} else {
    TARGET = ../../release/tst_uiloader
  }
}

HEADERS += uiloader.h
SOURCES += tst_uiloader.cpp uiloader.cpp
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
