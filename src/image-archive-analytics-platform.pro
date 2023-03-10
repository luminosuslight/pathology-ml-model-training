include(core/luminosus-core.pri)

include(universal-blocks/universal-blocks.pri)
include(microscopy/microscopy.pri)

include(qsyncable/qsyncable.pri)

CONFIG += c++17

QT += quickcontrols2

#CONFIG(debug, debug|release):!emscripten:!macx {
#    LIBS += -lasan
#    QMAKE_CXXFLAGS += -fsanitize=address
#}

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp

RESOURCES += qml.qrc \
    fonts.qrc \
    images.qrc

#INCLUDEPATH += /usr/local/include /usr/lib/gcc/x86_64-linux-gnu/9/include/

#QMAKE_LFLAGS += -lomp
LIBS += -L/usr/lib/llvm-10/lib/ -L/usr/lib/llvm-14/lib/ -lomp
QMAKE_CXXFLAGS += -fopenmp
