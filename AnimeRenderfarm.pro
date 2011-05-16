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
    dialogues/renderprogress.cpp \
    dialogues/consoleoutput.cpp

HEADERS  += animerenderfarm.h \
    subclasses/qprojectslistmodel.h \
    dialogues/rendersettings.h \
    dialogues/serversettings.h \
    dialogues/renderprogress.h \
    dialogues/consoleoutput.h

FORMS    += animerenderfarm.ui \
    dialogues/rendersettings.ui \
    dialogues/serversettings.ui \
    dialogues/renderprogress.ui \
    dialogues/consoleoutput.ui
