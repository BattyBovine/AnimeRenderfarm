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
    objects/serverthread.cpp \
    dialogues/servermanager.cpp \
    objects/clientthread.cpp \
    objects/tcpcommunicator.cpp

HEADERS  += animerenderfarm.h \
    subclasses/qprojectslistmodel.h \
    dialogues/preferences.h \
    dialogues/consoleoutput.h \
    dialogues/rendermanager.h \
    objects/renderthread.h \
    objects/serverthread.h \
    dialogues/servermanager.h \
    objects/clientthread.h \
    objects/tcpcommunicator.h

FORMS    += animerenderfarm.ui \
    dialogues/preferences.ui \
    dialogues/consoleoutput.ui \
    dialogues/rendermanager.ui \
    dialogues/servermanager.ui



win32 {
    LIBS  += User32.lib Ole32.lib
    RC_FILE += resources/AnimeRenderfarm.rc
}



macx {
    CONFIG += x86 ppc
    ICON = resources/AnimeRenderfarm.icns
}
