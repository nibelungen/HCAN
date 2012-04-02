#include <zentralheizungspumpe.h>
#include <canix/syslog.h>
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

static inline void sendMessage(device_data_zentralheizungspumpe *p, uint8_t active);
static inline void waermebedarfSpeichern(device_data_zentralheizungspumpe *p, uint8_t id, uint8_t waermebedarf);
static inline uint8_t getMittlererWaermebedarf(device_data_zentralheizungspumpe *p);

void zentralheizungspumpe_init(device_data_zentralheizungspumpe *p, eds_block_p it)
{
	uint8_t n;
	for(n=0; n<MAX_ANZ_HEIZUNGEN; n++)
	{
		p->heizung[n].id = 255; // Kennzeichnet ein leeres Feld
		p->heizung[n].waermebedarf = 0;
	}
}

void zentralheizungspumpe_can_callback(device_data_zentralheizungspumpe *p, const canix_frame *frame)
{
	uint8_t mittlererWaermebedarf = 0;

	if(frame->data[1] == HCAN_HES_HEIZUNG_WAERMEBEDARF_INFO)
	{
		waermebedarfSpeichern(p, frame->data[2], frame->data[3]); //id, waermebedarf

		mittlererWaermebedarf = getMittlererWaermebedarf(p);
		if(mittlererWaermebedarf >= EINSCHALT_SCHWELLE)
		{
			sendMessage(p, 1); // Heizkoerperumwaelzpumpe einschalten
		}
		else if(mittlererWaermebedarf <= AUSSCHALT_SCHWELLE)
		{
			sendMessage(p, 0); // Heizkoerperumwaelzpumpe ausschalten
		}
	}
}

static inline void waermebedarfSpeichern(device_data_zentralheizungspumpe *p, uint8_t id, uint8_t waermebedarf)
{
	uint8_t n;

	// 1. Durchgang: id finden und ggf. den Waermebedarf speichern:
	for (n=0; n<MAX_ANZ_HEIZUNGEN; n++)
	{
		if (p->heizung[n].id == id)   // id gefunden
		{
			p->heizung[n].waermebedarf = waermebedarf;
			return; // Eintrag geschrieben
		}
	}

	// 2. Durchgang, falls die id noch nicht angelegt ist:
	for (n=0; n<MAX_ANZ_HEIZUNGEN; n++)
	{
		if (p->heizung[n].id == 255) // erstes noch ungenutztes Feld (id noch leer)
		{
			p->heizung[n].id = id; // fuer Ersteintrag mit id
			p->heizung[n].waermebedarf = waermebedarf;
			return; // Eintrag erzeugt und geschrieben
		}
	}
}

static inline uint8_t getMittlererWaermebedarf(device_data_zentralheizungspumpe *p)
{
	uint16_t gesamtWaermebedarf = 0;
	uint8_t i = 0;
	uint8_t n = 0;
	for(n=0; n<MAX_ANZ_HEIZUNGEN; n++)
	{
		if (p->heizung[n].waermebedarf > 0)
		{
			gesamtWaermebedarf += p->heizung[n].waermebedarf;
			i++;
		}
	}

	if(gesamtWaermebedarf == 0)
	{
		return 0;
	}
	else
	{
		canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("WB=%d, i=%d"), (uint8_t) (gesamtWaermebedarf / i), i);
		return (uint8_t) gesamtWaermebedarf / i;
	}
}

static inline void sendMessage(device_data_zentralheizungspumpe *p, uint8_t active)
{
	canix_frame message;

	message.src = canix_selfaddr();
	message.dst = HCAN_MULTICAST_CONTROL;
	message.proto = HCAN_PROTO_SFP;
	message.data[0] = HCAN_SRV_HES;
	if(active)	message.data[1] = HCAN_HES_POWER_GROUP_ON; // Zentralheizungspumpe schalten
	else		message.data[1] = HCAN_HES_POWER_GROUP_OFF;
	message.data[2] = p->config.gruppe;
	message.size = 3;
	canix_frame_send_with_prio(&message, HCAN_PRIO_HI);
}
