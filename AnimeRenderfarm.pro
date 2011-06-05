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
    dialogues/preferences.cpp \
    dialogues/consoleoutput.cpp \
    dialogues/rendermanager.cpp \
    objects/renderthread.cpp \
    objects/serverthread.cpp

HEADERS  += animerenderfarm.h \
    subclasses/qprojectslistmodel.h \
    dialogues/preferences.h \
    dialogues/consoleoutput.h \
    dialogues/rendermanager.h \
    objects/renderthread.h \
    objects/serverthread.h

FORMS    += animerenderfarm.ui \
    dialogues/preferences.ui \
    dialogues/consoleoutput.ui \
    dialogues/rendermanager.ui



win32:LIBS  += User32.lib \
    Ole32.lib
win32:RC_FILE += resources/AnimeRenderfarm.rc



macx:CONFIG += x86 ppc
macx:ICON = resources/AnimeRenderfarm.icns
