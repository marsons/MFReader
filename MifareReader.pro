#-------------------------------------------------
#
# Project created by QtCreator 2015-09-30T11:56:51
#
#-------------------------------------------------

QT       += core gui
CONFIG += c++11
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MifareReader
TEMPLATE = app


SOURCES += main.cpp\
        fenetre.cpp \
    lecteur.cpp

HEADERS  += fenetre.h \
    ODALID.h \
    lecteur.h \
    Observateur.h

LIBS += -LE:\\Documents\\ESIREM\\4A\\Comm_Sans_Fil\\Thivan\\Test\\MifareReaderLast -lODALID

FORMS    += fenetre.ui
