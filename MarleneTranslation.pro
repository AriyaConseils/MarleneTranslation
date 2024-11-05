QT       += core gui network svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
RC_ICONS += ressources/MarleneTranslation.ico

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/lrelease/lreleasepagecontroler.cpp \
    src/AcceuilPanel/Dialog/dialogabout.cpp \
    src/AcceuilPanel/Dialog/newprojectdialog.cpp \
    src/AcceuilPanel/vues/projectlist.cpp \
    src/AcceuilPanel/welcomepagecontroller.cpp \
    src/interfacemanager.cpp \
    src/linguist/linguistpagecontroller.cpp \
    src/linguist/chatgptclient.cpp \
    src/linguist/translationtablemodel.cpp \
    src/lupdate/cppfileparser.cpp \
    src/lupdate/lupdatepagecontroller.cpp \
    src/lupdate/projectvue.cpp \
    src/lupdate/translationextractor.cpp \
    src/lupdate/uifileparser.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/3rdParty/TinyXml2/tinyxml2.cpp

HEADERS += \
    src/lrelease/lreleasepagecontroler.h \
    src/AcceuilPanel/Dialog/dialogabout.h \
    src/AcceuilPanel/Dialog/newprojectdialog.h \
    src/AcceuilPanel/vues/ProjectFileWidget.h \
    src/AcceuilPanel/vues/projectlist.h \
    src/AcceuilPanel/welcomepagecontroller.h \
    src/linguist/widgets/FlagsPickerBox.h \
    src/linguist/translationtablemodel.h \
    src/tools/FileAggregator.h \
    src/interfacemanager.h \
    src/linguist/linguistpagecontroller.h \
    src/linguist/chatgptclient.h \
    src/linguist/InterfaceDictionary.h \
    src/lupdate/cppfileparser.h \
    src/lupdate/lupdatepagecontroller.h \
    src/lupdate/projectvue.h \
    src/lupdate/translationextractor.h \
    src/lupdate/uifileparser.h \
    src/mainwindow.h \
    src/3rdParty/TinyXml2/tinyxml2.h

FORMS += \
    src/lrelease/lreleasepagecontroler.ui \
    src/AcceuilPanel/Dialog/dialogabout.ui \
    src/AcceuilPanel/Dialog/newprojectdialog.ui \
    src/AcceuilPanel/vues/projectlist.ui \
    src/AcceuilPanel/welcomepagecontroller.ui \
    src/interfacemanager.ui \
    src/linguist/linguistpagecontroller.ui \
    src/lupdate/lupdatepagecontroller.ui \
    src/lupdate/projectvue.ui \
    src/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ressources.qrc \
    src/ressources.qrc \
    src/ressources.qrc

INCLUDEPATH +=  $$PWD/src/3rdParty/TinyXml2 \
                $$PWD/src/tools
