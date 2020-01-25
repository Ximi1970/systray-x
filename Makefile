QMAKE=qmake-qt5

.PHONY:	clean \
	systray-x-xpi

all: systray-x-xpi systray-x-app

clean:
	@echo "Deleting all XPI and app objects" ;\
	rm -f *.xpi ;\
	rm -f SysTray-X ;\
	rm -rf app/build ;\

systray-x-xpi:
	@echo "Creating systray-x.xpi" ;\
	rm -f systray-x.xpi ;\
	cd webext ;\
	zip ../systray-x.xpi -qr * ;\
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
