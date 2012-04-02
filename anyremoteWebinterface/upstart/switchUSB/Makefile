install:
	#make clean; make all
	#echo "Fuer chmod root ist der Benutzer root gefragt (su ...)"	
	#su -c "chown root ./switchUSB" root
	#su -c "chmod ug+s ./switchUSB" root 
	sudo install ./switchUSB /usr/bin
	# da beim Install Rechte verändert wurden:
	echo "Fuer chmod ug+s ist der Benutzer root gefragt (su ...)"	
	su -c "chmod ug+s /usr/bin/switchUSB" root

all:
	cc -o switchUSB switchUSB.c

clean:
	rm -f switchUSB

#wert in Dez: 
# 1 setzt bind   usb
# 0 setzt unbind usb
output:
	./switchUSB	$(wert)

on:
	make output wert=1

off:
	make output wert=0

