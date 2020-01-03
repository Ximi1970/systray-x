.PHONY:	clean \
	systray-x-xpi

all: systray-x-xpi

clean:
	echo "Deleting all XPI"
	@rm -f *.xpi

systray-x-xpi:
	@echo "Creating systray-x.xpi" ;\
	rm -f systray-x.xpi ;\
	cd webext ;\
	zip ../systray-x.xpi -qr * ;\
	cd ..
