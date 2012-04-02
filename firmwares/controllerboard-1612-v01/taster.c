#include <taster.h>
#include <canix/syslog.h>
#include <darlingtonoutput.h>
#include <hcan.h>

#include <canix/canix.h>
#include <canix/led.h>
#include <canix/tools.h>
#include <canix/rtc.h>
#include <hcan.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>

#include <tasterinput.h>
#include <darlingtonoutput.h>

// wird alle 10msec aufgerufen, fuer jede Taster-Instanz einmal
void taster_timer_handler(device_data_taster *p)
{
	canix_frame message;
	uint8_t time;

	// Wenn Taste gedrueckt ist, dann ist der Pin 0, ansonsten 1
	uint8_t status = ! tasterport_read(p->config.port);

	// Message schon mal vorbereiten:
	message.src = canix_selfaddr();
	message.dst = HCAN_MULTICAST_CONTROL;
	message.proto = HCAN_PROTO_SFP;
	message.data[0] = HCAN_SRV_HES;
	// message.data[1] wird unten ausgefuellt
	message.data[2] = p->config.gruppe;
	// message.data[3] wird unten ausgefuellt
	message.size = 4;

	if (status) // gedrueckt
	{
		// Entprellschutz:
		if (p->pressed < 255)
			p->pressed++;
	}
	else
	{
		// Wenn Schalter-Down schon gesendet wurde,
		// dann ein Schalter-Up Event senden:
		if ((p->pressed > 3))
		{
			//...und kein Rollladentaster im 2-Tasterbetrieb konfiguriert ist:
			if ( !(p->config.feature & ((1<<FEATURE_TASTER_ROLLADEN_ZU) | (1<<FEATURE_TASTER_ROLLADEN_AUF))) )
			{
			message.data[1] = HCAN_HES_TASTER_UP;
				message.data[3] = p->config.port;
			canix_frame_send_with_prio(&message, HCAN_PRIO_HI);
		}
		}

		// Taste wurde losgelassen, also Zaehler zuruecksetzen
		p->pressed = 0;
	}

	if (p->config.feature & (1<<FEATURE_TASTER_ENTPRELL_1S))
		time = 100;
	else
		time = 3;

	if (p->pressed == time) // mind. 30msec gedrueckt
	{
		if (p->config.feature & (1<<FEATURE_TASTER_ROLLADEN_ZU))
		{
			message.data[1] = HCAN_HES_ROLLADEN_POSITION_SET;
			message.data[3] = 200; //gewuenschte Rollladenposition: Pos=200=0%=zu
		}
		else if (p->config.feature & (1<<FEATURE_TASTER_ROLLADEN_AUF))
		{
			message.data[1] = HCAN_HES_ROLLADEN_POSITION_SET;
			message.data[3] = 201; //gewuenschte Rollladenposition: Pos=201=100%=auf
		}
		else //kein Rollladentaster im 2-Tasterbetrieb
		{
		message.data[1] = HCAN_HES_TASTER_DOWN;
			message.data[3] = p->config.port;
		}

		canix_frame_send_with_prio(&message, HCAN_PRIO_HI);
	}
}


