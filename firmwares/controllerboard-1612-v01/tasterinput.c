#include <avr/io.h>
#include "tasterinput.h"
#include <canix/canix.h>
#include "shiftio.h"

uint8_t tasterport_read(uint8_t n)
{
	if (n < 4)
	{
		// PC0 bis PC4 fuer shiftOut and shiftIn. Siehe shiftio.h. Daher:
		// error: PC0 bis PC4 stehen als Eingaenge nicht zur Verfuegung!
	}
	else if (n < 8)
	{
		// Pins sind 1:1 von PORTC auszulesen

		// Modus Input setzen
		DDRC &= ~ (1<< n);

		// Pullup einschalten
		PORTC |= (1<< n);

		return PINC & (1<< n);
	}
	else if (n < 16)
	{
		// auf den Bereich 0-7 holen:
		n &= 0x07;

		// Pins sind zu spiegel ( 0 -> 7, 1 -> 6 etc.)
		n = 7 - n;

		// Modus Input setzen
		DDRA &= ~ (1<< n);

		// Pullup einschalten
		PORTA |= (1<< n);

		return PINA & (1<< n);
	}
	/* Beispielsweise sind 7 x 8 Bit-Shiftregister
     * angeschlossen, dann existieren 56 weitere Eingaenge.
     * Diese Eingaenge sind dann in der EEPROM-Konfiguration
     * mit den Indizes 16...71 einzubinden. */
	else if (n < 244) //255-16=224  244/8= 28 8-Bit Shiftregister maximal.
	{
		n -= 16; //auf den Bereich 0-223 holen
		return isBitFromShiftInSet(n); //n-tes Bit abfragen
	}

	return 0;
}

