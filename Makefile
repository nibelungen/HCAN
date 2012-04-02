.PHONY: touch-depend
DIRS = xml hcand hcanhid hcanaddressd libhcan++ libhcandata telican hcanswd hcandq check_hcan anyremoteWebinterface 

all: touch-depend xml
	find . -name .depend | while read i; do > $$i; done
	for d in $(DIRS); do $(MAKE) -C $$d || exit $$?; done

clean:
	for d in $(DIRS); do $(MAKE) -C $$d clean || exit $$?; done
	
install:
	find . -name .depend | while read i; do > $$i; done
	for d in $(DIRS); do $(MAKE) -C $$d install || exit $$?; done

website-package:
	find . -name .depend | while read i; do > $$i; done
	(cd firmwares/controllerboard-1612-v01/ && make clean)
	(cd firmwares/userpanel-v01/ && make clean)
	(cd firmwares/usv-modul/ && make clean)
	(cd firmwares/hostinterface-v02/ && make clean)
	(cd hcanbl32 && make clean)
	(cd hcanbl8 && make clean)
	(cd hcand && make clean)
	(cd hcanhid && make clean)
	(cd hcanaddressd && make clean)
	(cd libhcan++ && make clean)
	(cd libhcandata && make clean)
	(cd telican && make clean)
	(cd hcanswd && make clean)
	(cd hcandq && make clean)
	(cd check_hcan && make clean)
	(cd .. && tar cvzf hcan.tar.gz hcan/canix hcan/doc hcan/firmwares/controllerboard-1612-v01 hcan/firmwares/userpanel-v01 hcan/firmwares/usv-modul hcan/firmwares/hostinterface-v02 hcan/hcanbl32 hcan/hcanbl8 hcan/hcand hcan/hcanhid hcan/hcanaddressd hcan/include hcan/libhcandata hcan/libhcan++ hcan/telican hcan/xml hcan/hcanswd hcan/hcandq hcan/check_hcan hcan/tools)
	(cd anyremoteWebinterface && make install)

deb: all
	dpkg-buildpackage -rfakeroot -b -uc

touch-depend: 
	for d in $(DIRS); do test -e $$d/.depend || touch $$d/.depend; done

V=2.0

hexfiles:
	(cd hcanbl32 && make clean all)
	(cd firmwares/controllerboard-1612-v01 && make clean all)
	(cd firmwares/userpanel-v01 && make clean all)
	(cd firmwares/hostinterface-v02 && make clean all)
	rm -rf /tmp/hcan-hexfiles-$V
	mkdir /tmp/hcan-hexfiles-$V
	cp hcanbl32/main.hex /tmp/hcan-hexfiles-$V/bootloader32-$V.hex
	cp firmwares/controllerboard-1612-v01/main.hex /tmp/hcan-hexfiles-$V/controller-1612-$V.hex
	cp firmwares/userpanel-v01/main.hex /tmp/hcan-hexfiles-$V/userpanel-$V.hex
	cp firmwares/hostinterface-v02/main.hex /tmp/hcan-hexfiles-$V/hostinterface-v02-$V.hex
	(cd /tmp && tar cvfz hcan-hexfiles-$V.tar.gz hcan-hexfiles-$V)
	mv /tmp/hcan-hexfiles-$V.tar.gz ../

