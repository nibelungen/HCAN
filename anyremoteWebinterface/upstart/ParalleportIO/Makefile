install:
	sudo install ./parallelPortOut /usr/bin
	# da beim Install Rechte verändert wurden:
	echo "Fuer chmod ug+s ist der Benutzer root gefragt (su ...)"	
	su -c "chmod ug+s /usr/bin/parallelPortOut" root

all:
	cc -o parallelPortOut parallelPortOut.c

clean:
	rm -f parallelPortOut

#wert in Dez: 
#  1 setzt Bit0
#128 setzt Bit7
#129 setzt Bit0 und Bit7
output:
	./parallelPortOut $(wert)
