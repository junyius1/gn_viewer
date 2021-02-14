#QT -= gui
QT += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib

CONFIG += c++11 static

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cnsComDefinition.cpp \
    cnsComReference.cpp \
    cnsComponent.cpp \
    cnsDefaultComFactory.cpp \
    cnsDefaultPropertyConverter.cpp \
    cnsDefaultPropertyParser.cpp \
    cnsDefinitionReader.cpp \
    cnscreatecomdefcontext.cpp \
    cnsdefinitioncreator.cpp \
    cnsvalue.cpp \
    framework.cpp \
    log/cnslogger.cpp \
    log/qt5debugappender.cpp

HEADERS += \
    cnsComDefinition.h \
    cnsComReference.h \
    cnsComponent.h \
    cnsDefaultComFactory.h \
    cnsDefaultPropertyConverter.h \
    cnsDefaultPropertyParser.h \
    cnsDefinitionReader.h \
    cnsEnum.h \
    cnsGlobal.h \
    cnsMacroGlobal.h \
    cnscreatecomdefcontext.h \
    cnsdefinitioncreator.h \
    cnsvalue.h \
    framework.h \
    log/cnslogger.h \
    log/qt5debugappender.h

INCLUDEPATH += $$PWD/../log4cplus/include

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
