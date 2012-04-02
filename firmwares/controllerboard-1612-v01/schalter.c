#include <schalter.h>
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

static inline void sendMessage(device_data_schalter *p, uint8_t active);

void schalter_init(device_data_schalter *p, eds_block_p it)
{
	// Nach einem Reboot den Zustand einmalig senden:
	if( ! tasterport_read(p->config.port) ) // active?
		sendMessage(p, 1);
	else
		sendMessage(p, 0);
}

// wird alle 100 msec aufgerufen, fuer jede Schalter-Instanz einmal
void schalter_timer_handler(device_data_schalter *p)
{
	// µC-interner-Pullup am Eingangport aktiv:
	// Wenn Schalter high liefert, dann ist der Pin 0, ansonsten 1
	uint8_t active = ! tasterport_read(p->config.port);

	if ((active && FALLING == p->lastEdge) || 	// Zustandswechsel 0->1?
		(!active && RISING == p->lastEdge))		// Zustandswechsel 1->0?
	{
		// Schalterstellung geaendert:
		if (p->newState < 255)
			p->newState++; // Entprellschutz
	}


	if (p->newState == 3) // mind. 30msec veraenderte Schalterstellung
	{
		sendMessage(p, active);
		p->newState = 0;
	}
}

static inline void sendMessage(device_data_schalter *p, uint8_t active)
{
	canix_frame message;

	if(active)
		p->lastEdge = RISING;
	else
		p->lastEdge = FALLING;


	if(p->config.feature & (1<<FEATURE_SCHALTER_INVERTIEREN))
		active = !active; // invertieren

	message.src = canix_selfaddr();
	message.dst = HCAN_MULTICAST_CONTROL;
	message.proto = HCAN_PROTO_SFP;
	message.data[0] = HCAN_SRV_HES;
	if(active)	message.data[1] = HCAN_HES_POWER_GROUP_ON;
	else		message.data[1] = HCAN_HES_POWER_GROUP_OFF;
	message.data[2] = p->config.gruppe;
	message.size = 3;
	canix_frame_send_with_prio(&message, HCAN_PRIO_HI);
}
