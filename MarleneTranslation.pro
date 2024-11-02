QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
RC_ICONS += ressources/MarleneTranslation.ico

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/chatgptclient.cpp \
    src/interfacemanager.cpp \
    src/lupdate/cppfileparser.cpp \
    src/lupdate/translationextractor.cpp \
    src/lupdate/uifileparser.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/tstranslationreader.cpp \
    src/3rdParty/TinyXml2/tinyxml2.cpp

HEADERS += \
    src/FileAggregator.h \
    src/InterfaceDictionary.h \
    src/chatgptclient.h \
    src/interfacemanager.h \
    src/lupdate/cppfileparser.h \
    src/lupdate/translationextractor.h \
    src/lupdate/uifileparser.h \
    src/mainwindow.h \
    src/tstranslationreader.h \
    src/3rdParty/TinyXml2/tinyxml2.h
FORMS += \
    src/interfacemanager.ui \
    src/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ressources.qrc

INCLUDEPATH += $$PWD/src/3rdParty/TinyXml2
