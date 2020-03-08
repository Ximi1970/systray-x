QMAKE = qmake-qt5
ifeq (, $(shell which qmake-qt5 2>/dev/null))
 ifeq (, $(shell which qmake 2>/dev/null))
     $(error "No qmake in $(PATH)")
 endif
 QMAKE = qmake
endif

.PHONY:	clean \
	systray-x-xpi \
	systray-x-app

DATE := `date --utc +'%a %b %_d %H:%M:%S UTC %Y'`

BUILD_NUMBER := $(shell git rev-list --count HEAD)
GIT_HASH := $(shell git rev-parse HEAD)
GIT_BRANCH := $(shell git rev-parse --abbrev-ref HEAD)

GIT_VERSION_LONG := $(shell git describe --long)
GIT_VERSION := $(shell git describe --long | sed "s/-.*//")


all: systray-x-xpi systray-x-app

clean:
	@echo "Deleting all XPI and app objects" ;\
	rm -f *.xpi ;\
	rm -f SysTray-X ;\
	rm -rf app/build ;\

systray-x-xpi:
	@echo "Creating systray-x@Ximi1970.xpi" ;\
	rm -f systray-x@Ximi1970.xpi ;\
	cd webext ;\
	zip ../systray-x@Ximi1970.xpi -qr * ;\
	cd ..

systray-x-app:
	@echo "Creating systray-x app" ;\
	rm -f SysTray-X ;\
	mkdir -p app/build ;\
	cd app/build ;\
	$(QMAKE) ../SysTray-X/SysTray-X.pro ;\
	make ;\
	cd ../.. ;\
	cp app/build/SysTray-X .
