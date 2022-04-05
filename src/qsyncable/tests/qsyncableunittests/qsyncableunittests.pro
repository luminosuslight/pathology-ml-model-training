QT       += qml quick testlib
QT       -= gui

TARGET = qsyncableunittests
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += \
    main.cpp \
    qsyncabletests.cpp \
    benchmarktests.cpp \
    integrationtests.cpp \
    fastdifftests.cpp \
    immutabletype1.cpp \
    immutabletype2.cpp \
    immutabletype3.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

DEFINES += QUICK_TEST_SOURCE_DIR=\\\"$$PWD\\\"


HEADERS += \
    qsyncabletests.h \
    benchmarktests.h \
    integrationtests.h \
    fastdifftests.h \
    immutabletype1.h \
    immutabletype2.h \
    immutabletype3.h

include(vendor/vendor.pri)
include(../../qsyncable.pri)

DISTFILES += \
    tst_jsonModel.qml \
    tst_uuid.qml \
    ../../README.md \
    SampleData1.qml \
    SampleData1.json \
    tst_QSyncable.qml
