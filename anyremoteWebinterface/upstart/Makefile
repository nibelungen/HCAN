USER_NAME=$(shell whoami)

install:
	# hcanhidController:
	sudo cp --remove-destination hcanhidController.sh /usr/bin
	cd /usr/bin; sudo chown $(USER_NAME) hcanhidController.sh; sudo chmod +x hcanhidController.sh
	#
	sudo cp --remove-destination hcanhidController.conf /etc/init
	cd /etc/init; sudo chown $(USER_NAME) hcanhidController.conf
	#
	# hcand, hcanaddressd:
	sudo cp --remove-destination hcand.conf hcanaddressd.conf /etc/init
	cd /etc/init; sudo chown $(USER_NAME) hcand.conf hcanaddressd.conf
	#
	# anyremoteAnd2html:
	sudo cp --remove-destination anyremoteAnd2html.sh /usr/bin
	cd /usr/bin; sudo chown $(USER_NAME) anyremoteAnd2html.sh; chmod +x anyremoteAnd2html.sh
	#
	sudo cp --remove-destination anyremoteAnd2html.conf /etc/init
	cd /etc/init; sudo chown $(USER_NAME) anyremoteAnd2html.conf
	# Fuer Firefox-Autostart: 
	# System->Einstellungen->Startprogramme-> Befehl: firefox http://localhost:8079/
	##################################################################################
	#
	cd ./ParalleportIO && make install
	cd ./switchUSB && make install
	#
	# RemoteDroid:
	# System->Einstellungen->Startprogramme-> Befehl: sh /usr/bin/startRemoteDroidServer.sh
	# Beschreibung: Ermoeglicht die Verwendung eines Androidgeraetes als Mauspad und Tastatur
	sudo cp --remove-destination startRemoteDroidServer.sh RemoteDroidServer.jar /usr/bin
	cd /usr/bin; sudo chown $(USER_NAME) startRemoteDroidServer.sh RemoteDroidServer.jar; chmod +x startRemoteDroidServer.sh

clean:
	sudo rm /usr/bin/hcanhidController.sh
	sudo rm /etc/init/hcanhidController.conf
	#
	#
	sudo rm /etc/init/hcand.conf
	sudo rm /etc/init/hcanaddressd.conf	
	#
	#
	sudo rm /usr/bin/anyremoteAnd2html.sh
	sudo rm /etc/init/anyremoteAnd2html.conf
	#
	sudo rm /usr/bin/parallelPortOut
	sudo rm /usr/bin/switchUSB
	#
	sudo rm /usr/bin/startRemoteDroidServer.sh
	sudo rm /usr/bin/RemoteDroidServer.jar

test:
	initctl list | grep hcan
	# initctl start hcand
	# initctl stop hcand
	# initctl restart hcand
	# initctl status hcand

