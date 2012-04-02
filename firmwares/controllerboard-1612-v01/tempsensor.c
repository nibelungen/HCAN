#include <tempsensor.h>
#include <canix/syslog.h>
#include <darlingtonoutput.h>
#include <hcan.h>

#include <canix/canix.h>
#include <canix/led.h>
#include <canix/tools.h>
#include <canix/rtc.h>
#include <canix/syslog.h>
#include <hcan.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>

#include "tasterinput.h"
#include "darlingtonoutput.h"
#include "onewire.h"
#include <hcan_multicast.h>

#include <avr/io.h>

void tempsensor_init(device_data_tempsensor *p, eds_block_p it)
{
	// Festlegung des Initialwertes des Intervals
	// Sinn und Zweck ist, dass kein Sensor zur gleichen
	// Sekunde den Bus verwendet, um so die Last schoen
	// zufaellig und gleichmaessig zu verteilen
	// Ausserdem werden so Sensoren, die auf dem gleichen pys.
	// 1wire Bus sitzen, nicht zeitgleich ausgelesen, sondern
	// mit mind. 4 Sekunden Abstand (vorausgesetzt, ihre Gruppen-IDs
	// sind fortlaufend konfiguriert).

	p->convert_sent = 0;
	p->timer_counter =  (p->config.gruppe << 2) & 0x3f;
	p->temperature = 0;
	ow_reset();
}

uint8_t tempsensor_str2id(char *str, char *id)
{
	// id ist 8 Bytes lang, der String sollte 16 Bytes lang
	// und mit einem '\0' abgeschlossen sein
	//

	uint8_t i,v = 0;	

	for (i = 0; i < 8; i++)
	{
		// Falls wir am Ende des Strings sind, dann Error
		// (d.h. falls der String zu kurz ist...)
		if ((str[i*2] == 0) || (str[(i*2)+1] == 0))
			return 1;

		if (( str[i*2] >= 'a') && ( str[i*2] <= 'f'))
			v = ((str[i*2] - 'a') + 10) << 4;
		if (( str[i*2] >= '0') && ( str[i*2] <= '9'))
			v = ((str[i*2] - '0')) << 4;

		if (( str[i*2+1] >= 'a') && ( str[i*2+1] <= 'f'))
			v |= ((str[i*2+1] - 'a') + 10);
		if (( str[i*2+1] >= '0') && ( str[i*2+1] <= '9'))
			v |= ((str[i*2+1] - '0'));

		id[i] = v;
	}

	return 0;
}

void tempsensor_timer_handler(device_data_tempsensor *p)
{
	uint8_t old = ow_getpin();
	ow_setpin(p->config.io_pin);


	// 2 Sekunden vor dem Ablaufen des Zeit-Zaehlers:
	// Wenn der Sensor aktiv ist, dann eine Messung anstossen:
	if ((p->timer_counter == 2) && (p->config.modus))
	{
		char id[8];

		if (tempsensor_str2id(p->config.ow_id, id) != 0)
		{
			canix_syslog_P(SYSLOG_PRIO_ERROR, 
					PSTR("tempsensor: invalid sensor id"));
			return;
		}

		// Messung starten:
		ow_command(CONVERT_T, (uint8_t *) id);
		p->convert_sent = 1;
	}

	if ((p->timer_counter == 0) && (p->convert_sent))
	{
		canix_frame message;
		char id[8];

		p->convert_sent = 0;

		if (tempsensor_str2id(p->config.ow_id, id) != 0)
		{
			canix_syslog_P(SYSLOG_PRIO_ERROR, 
					PSTR("tempsensor: invalid sensor id"));
			return;
		}

		ow_command(READ, (uint8_t *) id);

		// Achtung: Lesereihenfolge beachten
		p->temperature = ow_read_byte() | (ow_read_byte() << 8);

		message.src = canix_selfaddr();
		message.dst = HCAN_MULTICAST_INFO;
		message.proto = HCAN_PROTO_SFP;
		message.data[0] = HCAN_SRV_HES;
		message.data[1] = HCAN_HES_1WIRE_TEMPERATURE;
		message.data[2] = p->config.gruppe;
		message.data[3] = p->temperature >> 8;
		message.data[4] = p->temperature;
		message.size    = 5;
		canix_frame_send(&message);

		// Timer laden
		p->timer_counter = p->config.interval;
	}

	p->timer_counter--;
	ow_setpin(old);

}

void tempsensor_can_callback(device_data_tempsensor *p, 
		const canix_frame *frame)
{
	canix_frame answer;

	answer.src = canix_selfaddr();
	answer.dst = frame->src;
	answer.proto = HCAN_PROTO_SFP;
	answer.data[0] = HCAN_SRV_HES;


	switch (frame->data[1])
	{
		case HCAN_HES_1WIRE_TEMPERATURE_QUERY :
			{
				if (frame->data[2] == p->config.gruppe)
				{
					canix_frame message;
					message.src = canix_selfaddr();
					message.dst = frame->src;
					message.proto = HCAN_PROTO_SFP;
					message.data[0] = HCAN_SRV_HES;
					message.data[1] = HCAN_HES_1WIRE_TEMPERATURE_REPLAY;
					message.data[2] = p->config.gruppe;
					message.data[3] = p->temperature >> 8;
					message.data[4] = p->temperature;
					message.size    = 5;
					canix_frame_send(&message);
				}
			}
			break;
	}
}

