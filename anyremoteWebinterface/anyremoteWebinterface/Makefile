USER_NAME=$(shell whoami)

getUsername:
	@echo $(USER_NAME)

start_anyremote:
	@#echo $(USER_NAME)
	make kill_anyremote &
	sleep 1
	make install # SpecMitCheckboxen = anyremote2html
	/usr/bin/anyremote -f /usr/share/anyremote/cfg-data/Server-mode/hcan.cfg -s socket:5001 -http &
	python /usr/bin/anyremote2html -w 8079 -a 5001 -r 15 -d &
	# Original:
	#/usr/bin/anyremote -f /usr/share/anyremote/cfg-data/Server-mode/hcan.cfg -s socket:5002 -http &
	#/usr/bin/anyremote2htmlSpec -w 8080 -a 5002 -r 15 -d &

kill_anyremote:
	# kill -s $(pidof anyremote) 
	sudo killall -s 9 anyremote; sudo killall -s 9 anyremote2html; sudo killall -s 9 anyremote2htmlSpec

install:
	cd ../anyremote2html; make install
	sudo cp --remove-destination ./hcan.cfg /usr/share/anyremote/cfg-data/Server-mode
	cd /usr/share/anyremote/cfg-data/Server-mode; sudo chown $(USER_NAME) hcan.cfg
	#
	sudo cp --remove-destination ./telicanStrKonvLib.sh /usr/bin
	cd /usr/bin; sudo chown $(USER_NAME) telicanStrKonvLib.sh; sudo chmod +x telicanStrKonvLib.sh
	# wenn anyremote2html nicht aus einem Paket installiert wurde:
	test -d /usr/share/pixmaps/anyremote2html || sudo mkdir /usr/share/pixmaps/anyremote2html
	test -d /usr/share/pixmaps/anyremote2html/16 || sudo mkdir /usr/share/pixmaps/anyremote2html/16
	test -d /usr/share/pixmaps/anyremote2html/32 || sudo mkdir /usr/share/pixmaps/anyremote2html/32
	test -d /usr/share/pixmaps/anyremote2html/48 || sudo mkdir /usr/share/pixmaps/anyremote2html/48
	test -d /usr/share/pixmaps/anyremote2html/64 || sudo mkdir /usr/share/pixmaps/anyremote2html/64
	test -d /usr/share/pixmaps/anyremote2html/128 || sudo mkdir /usr/share/pixmaps/anyremote2html/128
	# vorhandenes erweitern:
	sudo cp --remove-destination ./Icons/hcan/16/*.png /usr/share/pixmaps/anyremote2html/16
	sudo cp --remove-destination ./Icons/hcan/32/*.png /usr/share/pixmaps/anyremote2html/32
	sudo cp --remove-destination ./Icons/hcan/48/*.png /usr/share/pixmaps/anyremote2html/48
	sudo cp --remove-destination ./Icons/hcan/64/*.png /usr/share/pixmaps/anyremote2html/64
	sudo cp --remove-destination ./Icons/hcan/128/*.png /usr/share/pixmaps/anyremote2html/128
	make password
	#
	test -d /etc/hcan || sudo mkdir /etc/hcan
	sudo cp --remove-destination /home/$(USER_NAME)/Dropbox/HCAN/HI_SERVER/conf/telican_Control_modus/installation.xml /etc/hcan

password:
	test -d $(HOME)/.anyRemote || mkdir $(HOME)/.anyRemote
	echo "1111" > $(HOME)/.anyRemote/password
