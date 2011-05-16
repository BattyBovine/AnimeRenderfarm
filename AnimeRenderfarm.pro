#-------------------------------------------------
#
# Project created by QtCreator 2010-10-29T18:26:52
#
#-------------------------------------------------

QT       += network

TARGET = AnimeRenderfarm
TEMPLATE = app


SOURCES += main.cpp\
        animerenderfarm.cpp \
    subclasses/qprojectslistmodel.cpp \
    dialogues/rendersettings.cpp \
    dialogues/serversettings.cpp \
    dialogues/consoleoutput.cpp \
    dialogues/renderprogress.cpp

HEADERS  += animerenderfarm.h \
    subclasses/qprojectslistmodel.h \
    dialogues/rendersettings.h \
    dialogues/serversettings.h \
    dialogues/consoleoutput.h \
    dialogues/renderprogress.h

FORMS    += animerenderfarm.ui \
    dialogues/rendersettings.ui \
    dialogues/serversettings.ui \
    dialogues/consoleoutput.ui \
    dialogues/renderprogress.ui

win32:LIBS  += User32.lib \
    Ole32.lib
