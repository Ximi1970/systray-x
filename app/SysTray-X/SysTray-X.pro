#-------------------------------------------------
#
# Project created by QtCreator 2020-01-12T19:19:44
#
#-------------------------------------------------

#DEFINES += NO_KDE_INTEGRATION


!contains(DEFINES,NO_KDE_INTEGRATION) {
    DEFINES += KDE_INTEGRATION
}

#
# Set the Qt modules
#
QT += core gui
unix:!macx: {
    contains(DEFINES,KDE_INTEGRATION) {
        QT += dbus KNotifications
    }
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#
# Define the target
#
TARGET = SysTray-X
TEMPLATE = app

#
#  Translations
#
TRANSLATIONS = \
    languages/$${TARGET}.en_US.ts \
    languages/$${TARGET}.bg.ts \
    languages/$${TARGET}.de.ts \
    languages/$${TARGET}.es_AR.ts \
    languages/$${TARGET}.fr.ts \
    languages/$${TARGET}.hr.ts \
    languages/$${TARGET}.it.ts \
    languages/$${TARGET}.nl.ts \
    languages/$${TARGET}.pl.ts \
    languages/$${TARGET}.ru_RU.ts \
    languages/$${TARGET}.sk.ts \
    languages/$${TARGET}.uk.ts \
    languages/$${TARGET}.zh_CN.ts \
    languages/$${TARGET}.zh_TW.ts

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11
#CONFIG += staticlib

#
#   System dependency
#
unix:!macx: {
    QMAKE_CFLAGS += $(RPM_OPT_FLAGS)
    QMAKE_CXXFLAGS += $(RPM_OPT_FLAGS)
    QMAKE_LFLAGS += $(RPM_OPT_FLAGS)
#    QMAKE_LFLAGS += -static-libgcc -static-libstdc++

    LIBS += -lX11
    
    #
    #   To solve _glapi_tls_Current undefined ref
    #
    exists( /usr/lib/libGLdispatch* ) {
        LIBS += -lGLdispatch
    }
    exists( /usr/lib64/libGLdispatch* ) {
        LIBS += -lGLdispatch
    }

}
win32: {
#    QMAKE_LFLAGS += -static -lwinpthread -static-libgcc -static-libstdc++ $$(QMAKE_LFLAGS_WINDOWS)

    #
    #	Windows host (not used in cross compiling with mingw on Linux)
    #
    !mingw:contains(QMAKE_HOST.os, Windows): {
        LIBS += User32.lib
        LIBS += Comctl32.lib
    }
}
unix:macx: {
    QMAKE_LFLAGS += -framework IOKit -framework Foundation
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7

    LIBS += -dead_strip
}

#
#	Git version getters
#
win32: {
    contains(QMAKE_HOST.os, Windows): {
        #
        #	Host is Windows
        #
        GIT = ""C:\\Progra~1\\Git\\bin\\git.exe""
    }
    else {
        #
        #	Host is Linux, cross compiling with mingw
        #
        GIT = git
    }
}
unix:GIT = git

#
#	Define the app version strings
#
BUILD_NUMBER = $$system($$GIT rev-list --count HEAD)
GIT_HASH = $$system($$GIT rev-parse HEAD)
GIT_BRANCH = $$system($$GIT rev-parse --abbrev-ref HEAD)

GIT_VERSION_LONG = $$system($$GIT describe --long)
GIT_VERSION = $$section(GIT_VERSION_LONG, -, 0, 0)

VERSION_MAJOR = $$section(GIT_VERSION, ., 0, 0)
VERSION_MINOR = $$section(GIT_VERSION, ., 1, 1)
VERSION_PATCH = $$section(GIT_VERSION, ., 2, 2)

!contains(DEFINES,EXT_VERSION) {
    DEFINES += APP_VERSION_MAJOR=\\\"$$VERSION_MAJOR\\\"
    DEFINES += APP_VERSION_MINOR=\\\"$$VERSION_MINOR\\\"
    DEFINES += APP_VERSION_PATCH=\\\"$$VERSION_PATCH\\\"
    DEFINES += APP_BUILD=\\\"$$BUILD_NUMBER\\\"
    DEFINES += APP_GITHASH=\\\"$$GIT_HASH\\\"
    DEFINES += APP_GITBRANCH=\\\"$$GIT_BRANCH\\\"

    message("Buildnumber: "$$BUILD_NUMBER)
    message("Git hash: "$$GIT_HASH)
    message("Git branch: "$$GIT_BRANCH)
    message("Version: "$$VERSION_MAJOR"."$$VERSION_MINOR"."$$VERSION_PATCH)
    #message($$QMAKESPEC)
}

unix: {
    #
    #   Generate JSON
    #
    QMAKE_POST_LINK = cp -f "$${_PRO_FILE_PWD_}/../config/linux/SysTray_X.json.template" "$${_PRO_FILE_PWD_}/../config/linux/SysTray_X.json" ;
    QMAKE_POST_LINK += sed -i -e "s?SYSTRAY_X_PATH?$${OUT_PWD}/$${TARGET}?" "$${_PRO_FILE_PWD_}/../config/linux/SysTray_X.json" ;
#    QMAKE_POST_LINK += cp -f "$${_PRO_FILE_PWD_}/../config/linux/SysTray_X.json" ~/.mozilla/native-messaging-hosts/SysTray_X.json ;
}

win32: {
    CONFIG(debug, debug|release) {
        JSON_EXE_PATH = $$system(powershell -Command "('$$shell_path($${OUT_PWD}/debug/$${TARGET}.exe)').replace('\\','\\\\')")

        QMAKE_POST_LINK = $$[QT_INSTALL_BINS]\windeployqt.exe \"$$shell_path($${OUT_PWD}/debug/$${TARGET}.exe)\" &
        QMAKE_POST_LINK += powershell -Command \"(Get-Content \'$$shell_path($${_PRO_FILE_PWD_}/../config/win32/SysTray_X.json.template)\' ).replace(\'SYSTRAY_X_PATH\',\'$$JSON_EXE_PATH\') | Set-Content \'$$shell_path($${_PRO_FILE_PWD_}/../config/win32/SysTray_X.json)\'\" &
    } else {
        JSON_EXE_PATH = $$system(powershell -Command "('$$shell_path($${OUT_PWD}/release/$${TARGET}.exe)').replace('\\','\\\\')")

        QMAKE_POST_LINK = $$[QT_INSTALL_BINS]\windeployqt.exe \"$$shell_path($${OUT_PWD}/release/$${TARGET}.exe)\" &
        QMAKE_POST_LINK += powershell -Command \"(Get-Content \'$$shell_path($${_PRO_FILE_PWD_}/../config/win32/SysTray_X.json.template)\' ).replace(\'SYSTRAY_X_PATH\',\'$$JSON_EXE_PATH\') | Set-Content \'$$shell_path($${_PRO_FILE_PWD_}/../config/win32/SysTray_X.json)\'\" &
    }

    JSON_JSON_PATH = $$system(powershell -Command "('$$shell_path($${_PRO_FILE_PWD_}/../config/win32/SysTray_X.json)').replace('\\','\\\\')")

    QMAKE_POST_LINK += powershell -Command \"(Get-Content \'$$shell_path($${_PRO_FILE_PWD_}/../config/win32/SysTray-X-User.reg.template)\' ).replace(\'SYSTRAY_X_JSON_PATH\',\'$$JSON_JSON_PATH\') | Set-Content \'$$shell_path($${_PRO_FILE_PWD_}/../config/win32/SysTray-X-User.reg)\'\" &
}

#
#	DO NOT COMPRESS THE RESOURCES. QFile.map() cannot handle it...
#
#QMAKE_RESOURCE_FLAGS += -no-compress


SOURCES += \
        main.cpp \
        systrayxlink.cpp \
        systrayxicon.cpp \
        systrayx.cpp \
        debugwidget.cpp \
        preferencesdialog.cpp \
        preferences.cpp \
        windowctrl.cpp
unix: {
SOURCES += \
        windowctrl-unix.cpp
    contains(DEFINES,KDE_INTEGRATION) {
        SOURCES += \
            systrayxstatusnotifier.cpp
    }
}
win32: {
SOURCES += \
        windowctrl-win.cpp
}

HEADERS += \
        systrayxlink.h \
        systrayxicon.h \
        systrayx.h \
        debugwidget.h \
        preferencesdialog.h \
        preferences.h \
        windowctrl.h
unix: {
HEADERS += \
        windowctrl-unix.h

    contains(DEFINES,KDE_INTEGRATION) {
        HEADERS += \
            systrayxstatusnotifier.h
    }
}
win32: {
HEADERS += \
        windowctrl-win.h
}

FORMS += \
        debugwidget.ui \
        preferences.ui \
        aboutdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


RESOURCES += \
        SysTray-X.qrc

win32:RC_FILE += \
        SysTray-X.rc

#
#   Needs to be at the end of the .pro file (bug in qmake?)
#
unix:macx: {
    #
    #   Use custom plist
    #
    QMAKE_INFO_PLIST = files/mac/SysTrayXInfo.plist

    #
    #   Set company name
    #
    QMAKE_TARGET_BUNDLE_PREFIX = Ximi1970.

    #
    #   Set icon
    #
    ICON = files/mac/SysTray-X.icns

    #
    #   Set version
    #
    VERSION = "$$VERSION_MAJOR"."$$VERSION_MINOR"."$$VERSION_PATCH"
    VERSION_LONG = "$$VERSION_MAJOR"."$$VERSION_MINOR"."$$VERSION_PATCH" "$$LITERAL_HASH$$BUILD_NUMBER" - "$$GIT_HASH" - "$$GIT_BRANCH"

    QMAKE_POST_LINK = /usr/libexec/PlistBuddy -c \"Set :CFBundleShortVersionString $${VERSION}\" $${OUT_PWD}/$${TARGET}.app/Contents/Info.plist ;
    QMAKE_POST_LINK += /usr/libexec/PlistBuddy -c \"Set :CFBundleVersion $${VERSION_LONG}\" $${OUT_PWD}/$${TARGET}.app/Contents/Info.plist ;

    #
    #   Add license file
    #
    APP_LICENSE_FILES.files = files/mac/LICENSE
    APP_LICENSE_FILES.path = Contents/SharedSupport
    QMAKE_BUNDLE_DATA += APP_LICENSE_FILES

    #
    #   Create and use internal frameworks
    #
    QMAKE_LFLAGS += '-Wl,-rpath,\'@executable_path/../Frameworks\''

    QMAKE_POST_LINK += rm -rf $${OUT_PWD}/$${TARGET}.app/Contents/Frameworks ;
    QMAKE_POST_LINK += mkdir -p $${OUT_PWD}/$${TARGET}.app/Contents/Frameworks ;

    QMAKE_POST_LINK += cp -R $$[QT_INSTALL_LIBS]/QtCore.framework $${OUT_PWD}/$${TARGET}.app/Contents/Frameworks ;
    QMAKE_POST_LINK += cp -R $$[QT_INSTALL_LIBS]/QtGui.framework $${OUT_PWD}/$${TARGET}.app/Contents/Frameworks ;
}
