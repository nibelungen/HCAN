#include <powerportAutomat.h>
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

#include <darlingtonoutput.h>
#include <devices.h>

static inline void sendMessage(device_data_powerportAutomat *p, uint8_t active);

void powerportAutomat_init(device_data_powerportAutomat *p, eds_block_p it)
{
	// 65535 bedeutet das noch kein Helligkeitstelegramm empfangen wurde
	// Wertebereich des Helligkeitssensor: 0 ... 1023
	p->brightness = 65535;
	
	p->msgIstVersendet = false;

	if( p->config.automatikEin_schalter_gruppe == 255 )
		p->automatikEin = true; // ohne Autowahlschalter immer Automatik
}

void powerportAutomat_can_callback(device_data_powerportAutomat *p, const canix_frame *frame)
{
	switch (frame->data[1])
	{
		case HCAN_HES_POWER_GROUP_ON :
			if (frame->data[2] == p->config.automatikEin_schalter_gruppe)
			{
				p->automatikEin = true;
			}
			else if (frame->data[2] == p->config.steller_gruppe)
			{
				sendMessage(p, HCAN_HES_POWER_GROUP_ON);
			}
			break;

		case HCAN_HES_POWER_GROUP_OFF :
			if (frame->data[2] == p->config.automatikEin_schalter_gruppe)
			{
				p->automatikEin = false;
			}
			else if (frame->data[2] == p->config.steller_gruppe)
			{
				sendMessage(p, HCAN_HES_POWER_GROUP_OFF);
			}
			break;

		case HCAN_HES_HELLIGKEITS_INFO :
			if (p->config.helligkeits_gruppe != 255 // Helligkeitssensor vorhanden
				&& frame->data[2] == p->config.helligkeits_gruppe) // Helligkeits-Gruppe
			{
				// Helligkeitswert speichern:
				p->brightness = (frame->data[3] << 8) + frame->data[4]; // [3] = brightness_hi
			}
			break;
	}
}

static inline void sendMessage(device_data_powerportAutomat *p, uint8_t active)
{
	canix_frame message;

	// Helligkeitssensor:
	if(	p->automatikEin // Automatikbetrieb
		&&	HCAN_HES_POWER_GROUP_ON == active // HCAN_HES_POWER_GROUP_OFF soll immer moeglich sein
		&& (p->brightness != 65535) // Helligkeitssensor verwenden?
		&& (p->brightness > p->config.helligkeitsschwelle) ) // zu hell?
	{
		return; // zu hell -> kein HCAN_HES_POWER_GROUP_ON senden
	}

	message.src = canix_selfaddr();
	message.dst = HCAN_MULTICAST_CONTROL;
	message.proto = HCAN_PROTO_SFP;
	message.data[0] = HCAN_SRV_HES;
	message.data[1] = active; // HCAN_HES_POWER_GROUP_ON oder HCAN_HES_POWER_GROUP_OFF
	message.data[2] = p->config.powerport_gruppe;
	message.size = 3;
	canix_frame_send_with_prio(&message, HCAN_PRIO_HI);
}
