.PHONY : tc ts uc us clean install uninstall android-install android-uninstall

all: tc ts uc us

tc:
	cd tc; make; cd ..

ts:
	cd ts; make; cd ..

uc:
	cd uc; make; cd ..

us:
	cd us; make; cd ..

clean:
	cd tc; make clean; cd ..
	cd ts; make clean; cd ..
	cd uc; make clean; cd ..
	cd us; make clean; cd ..

install:
	sudo cp bin/tc /usr/sbin
	sudo cp bin/ts /usr/sbin
	sudo cp bin/uc /usr/sbin
	sudo cp bin/us /usr/sbin

uninstall:
	sudo rm /usr/sbin/tc /usr/sbin/ts /usr/sbin/uc /usr/sbin/us

android-install:
	adb push bin/tc bin/ts bin/uc bin/us /data/local/tmp
	adb exec-out "su -c 'mount -o rw,remount /system'"
	adb exec-out "su -c 'cp /data/local/tmp/tc /data/local/tmp/ts /data/local/tmp/uc /data/local/tmp/us /system/xbin'"
	adb exec-out "su -c 'chmod 755 /system/xbin/tc'"
	adb exec-out "su -c 'chmod 755 /system/xbin/ts'"
	adb exec-out "su -c 'chmod 755 /system/xbin/uc'"
	adb exec-out "su -c 'chmod 755 /system/xbin/us'"
	adb exec-out "su -c 'mount -o ro,remount /system'"
	adb exec-out "su -c 'rm /data/local/tmp/tc /data/local/tmp/ts /data/local/tmp/uc /data/local/tmp/us'"

android-uninstall:
	adb exec-out "su -c 'mount -o rw,remount /system'"
	adb exec-out "su -c 'rm /system/xbin/tc /system/xbin/ts /system/xbin/uc /system/xbin/us'"
	adb exec-out "su -c 'mount -o ro,remount /system'"
