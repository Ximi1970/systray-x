SYSTEM = 
ifeq (Cygwin,$(shell uname -o))
 SYSTEM = Windows
else
ifeq (GNU/Linux,$(shell uname -o))
 SYSTEM = Linux

#QMAKE = ../rootfs/usr/Qt-opensource-5.14.2/bin/qmake
#QMAKE = ../rootfs/usr/Qt-opensource-5.14.2-static/bin/qmake
#QMAKE = /usr/Qt-opensource-5.14.2-static/bin/qmake

 QMAKE = qmake-qt5
 ifeq (, $(shell which qmake-qt5 2>/dev/null))
  ifeq (, $(shell which qmake 2>/dev/null))
     $(error "No qmake in $(PATH)")
  endif
  QMAKE = qmake
 endif
else
 $(error "Unknown system")
endif
endif

.PHONY:	clean \
	all-static \
	systray-x-xpi \
	systray-x-app \
	systray-x-app-static \
	qt-static \
	qt-static-clean \
	qt-dynamic \
	qt-dynamic-clean

DATE := `date --utc +'%a %b %_d %H:%M:%S UTC %Y'`

BUILD_NUMBER := $(shell git rev-list --count HEAD)
GIT_HASH := $(shell git rev-parse HEAD)
GIT_BRANCH := $(shell git rev-parse --abbrev-ref HEAD)

GIT_VERSION_LONG := $(shell git describe --long)
GIT_VERSION := $(shell git describe --long | sed "s/-.*//")


all: systray-x-xpi systray-x-app

all-static: systray-x-xpi systray-x-app-static 

clean:
	@echo "Deleting all XPI and app objects" ;\
	rm -f *.xpi ;\
	rm -f SysTray-X ;\
	rm -rf app/build ;\
	rm -rf app/build32 ;\
	rm -rf app/build64 ;\
	rm -rf app/build-* ;\
	rm -rf app/dist* ;\

systray-x-xpi:
	@echo "Creating systray-x@Ximi1970.xpi" ;\
	rm -f systray-x@Ximi1970.xpi ;\
	cd webext ;\
	zip ../systray-x@Ximi1970.xpi -qr * ;\
	cd ..

ifeq (Linux,$(SYSTEM))
systray-x-app:
	@echo "Creating systray-x app" ;\
	rm -f SysTray-X ;\
	mkdir -p app/build ;\
	cd app/build ;\
	$(QMAKE) $(EXT_VERSION) ../SysTray-X/SysTray-X.pro ;\
	make ;\
	cd ../.. ;\
	cp app/build/SysTray-X .

systray-x-app-static:
	@echo "Creating a static systray-x app" ;\
	rm -f SysTray-X ;\
	mkdir -p app/build ;\
	cd app/build ;\
	if [ -d "../rootfs/usr/Qt-opensource-5.14.2-static" ] ; then \
		echo "Using locally build static Qt 5.14.2" ;\
		../rootfs/usr/Qt-opensource-5.14.2-static/bin/qmake $(EXT_VERSION) ../SysTray-X/SysTray-X.pro ;\
	else \
		echo "Using system installed static Qt 5.14.2" ;\
		/usr/Qt-opensource-5.14.2-static/bin/qmake $(EXT_VERSION) ../SysTray-X/SysTray-X.pro ;\
	fi ;\
	make ;\
	cd ../.. ;\
	cp app/build/SysTray-X .

qt-static:
	@echo "Building static Qt library" ;\
	cd libs/qt-static ;\
	./build.sh

qt-static-clean:
	@echo "Removing static Qt library" ;\
	rm -rf app/rootfs/usr/Qt-opensource-5.14.2-static ;\
	rm -rf libs/qt-static/qt-everywhere-src-*/

systray-x-app-dynamic:
	@echo "Creating a dynamic systray-x app" ;\
	rm -f SysTray-X ;\
	mkdir -p app/build ;\
	cd app/build ;\
	if [ -d "../rootfs/usr/Qt-opensource-5.14.2" ] ; then \
		echo "Using locally build Qt 5.14.2" ;\
		../rootfs/usr/Qt-opensource-5.14.2/bin/qmake $(EXT_VERSION) ../SysTray-X/SysTray-X.pro ;\
	else \
		echo "Using system installed Qt 5.14.2" ;\
		/usr/Qt-opensource-5.14.2/bin/qmake $(EXT_VERSION) ../SysTray-X/SysTray-X.pro ;\
	fi ;\
	make ;\
	cd ../.. ;\
	cp app/build/SysTray-X .

qt-dynamic:
	@echo "Building dynamic Qt library" ;\
	cd libs/qt-dynamic ;\
	./build.sh

qt-dynamic-clean:
	@echo "Removing dynamic Qt library" ;\
	rm -rf app/rootfs/usr/Qt-opensource-5.14.2 ;\
	rm -rf libs/qt-dynamic/qt-everywhere-src-*/

else
systray-x-app:
	@echo "Creating systray-x app" ;\
	rm -f SysTray-X ;\
	mkdir -p app/build32 ;\
	mkdir -p app/dist/win32 ;\
	cd app/build32 ;\
	../build.bat x86 5.14.2 ;\
	cd ../.. ;\
	mkdir -p app/build64 ;\
	cd app/build64 ;\
	../build.bat x86_64 5.14.2 ;\
	cd ../..
endif
