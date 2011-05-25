#-------------------------------------------------
#
# Project created by QtCreator 2010-10-29T18:26:52
#
#-------------------------------------------------

QT       += network

TARGET = "Anime Renderfarm"
TEMPLATE = app


SOURCES += main.cpp\
        animerenderfarm.cpp \
    subclasses/qprojectslistmodel.cpp \
    dialogues/rendersettings.cpp \
    dialogues/serversettings.cpp \
    dialogues/consoleoutput.cpp \
    dialogues/rendermanager.cpp \
    objects/renderthread.cpp

HEADERS  += animerenderfarm.h \
    subclasses/qprojectslistmodel.h \
    dialogues/rendersettings.h \
    dialogues/serversettings.h \
    dialogues/consoleoutput.h \
    dialogues/rendermanager.h \
    objects/renderthread.h

FORMS    += animerenderfarm.ui \
    dialogues/rendersettings.ui \
    dialogues/serversettings.ui \
    dialogues/consoleoutput.ui \
    dialogues/rendermanager.ui

win32:LIBS  += User32.lib \
    Ole32.lib

win32:RC_FILE += resources/AnimeRenderfarm.rc

macx:ICON = resources/AnimeRenderfarm.icns
