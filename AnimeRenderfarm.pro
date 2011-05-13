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
    consoleoutput.cpp \
    subclasses/qprojectslistmodel.cpp \
    aboutapplication.cpp \
    rendersettings.cpp

HEADERS  += animerenderfarm.h \
    consoleoutput.h \
    subclasses/qprojectslistmodel.h \
    aboutapplication.h \
    rendersettings.h

FORMS    += animerenderfarm.ui \
    consoleoutput.ui \
    aboutapplication.ui \
    rendersettings.ui
