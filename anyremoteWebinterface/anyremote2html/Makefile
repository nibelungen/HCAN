USER_NAME=$(shell whoami)

#ORIG_VERSION=anyremote2html_orig_1_4
ORIG_VERSION=anyremote2html_SpecMitCheckboxen
SPEZ_VERSION=anyremote2htmlSpec

install:
	sudo cp  --remove-destination ./$(ORIG_VERSION) /usr/bin
	cd /usr/bin; sudo mv $(ORIG_VERSION) anyremote2html
	cd /usr/bin; sudo chown $(USER_NAME) anyremote2html; sudo chmod +x anyremote2html
	#
	sudo cp  --remove-destination ./$(SPEZ_VERSION) /usr/bin
	cd /usr/bin; sudo chown $(USER_NAME) $(SPEZ_VERSION); sudo chmod +x $(SPEZ_VERSION)
	#
	# Zuerst muss anyremote installiert werden
	sudo apt-get update
	sudo apt-get -yfm install anyremote
	cd /usr/bin; sudo chown $(USER_NAME) anyremote; sudo chmod +x anyremote
	#
	sudo apt-get -yfm install libboost-all-dev
	sudo apt-get -yfm install libxml++2.6-2
