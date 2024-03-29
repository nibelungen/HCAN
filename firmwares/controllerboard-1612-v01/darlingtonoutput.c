#include <avr/io.h>
#include <canix/syslog.h>
#include "darlingtonoutput.h"
#include "shiftio.h"

void darlingtonoutput_init(void)
{
	// Darlington Ports auf Output setzen
	DDRD |= 0xff;
	DDRB |= 0x0f;

	// Darlington Ports ausschalten
	PORTD = 0;
	PORTB &= ~ (0x0f);

	initShiftInOut(); // -> Die Tasterports 0...4 sind hiermit belegt!
}

uint8_t darlingtonoutput_getpin(uint8_t n)
{
	if (n < 8)
	{
		return PORTD & (1<< n);
	}
	else if (n < 12)
	{
		n = n - 8;
		return PORTB & (1<< n);
	}
	else
	{
		n -= 12; // n auf 0...95 bringen
		return getShiftOutPinState(n);
	}
}

void darlingtonoutput_setpin(uint8_t n, uint8_t state)
{
	if (n < 8)
	{
		// Output Modus setzen
		DDRD |= (1<< n);

		if (state)
			PORTD |= (1<< n);
		else
			PORTD &= ~ (1<< n);
	}
	else if (n < 12)
	{
		n = n - 8;

		// Output Modus setzen
		DDRB |= (1<< n);

		if (state)
			PORTB |= (1<< n);
		else
			PORTB &= ~ (1<< n);
	}
	else
	{
		n -= 12; // n auf 0...95 bringen
		//canix_syslog_P(SYSLOG_PRIO_ERROR, PSTR("chgShOutPinStat,n=%d,stat=%d"), n, state);
		changeShiftOutPinState(n, state);
	}
}
