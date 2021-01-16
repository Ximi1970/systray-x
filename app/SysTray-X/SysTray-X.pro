TEMPLATE = subdirs
CONFIG += ordered

#
#	Get the defaults
#
include( SysTray-X.pri )

#
#	The projects
#
unix: {
SUBDIRS +=  SysTray-X-lib-x11
}
#SUBDIRS +=  SysTray-X-lib-win
SUBDIRS +=  SysTray-X-app
