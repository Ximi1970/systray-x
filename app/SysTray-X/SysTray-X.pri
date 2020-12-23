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
