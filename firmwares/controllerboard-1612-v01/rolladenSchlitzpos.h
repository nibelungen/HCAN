#ifndef ROLLADENSCHLITZPOS_H
#define ROLLADENSCHLITZPOS_H

#include <canix/eds.h>
#include <inttypes.h>
#include <eds-structs.h>

#define SCHLITZ_POS					17 //17% (ist die Postion, in der die Rolllaeden auf Schlitz stehen)
#define SCHLITZ_JOB_ABGEMELDET		255 //wenn ungleich 255 wird der job nach x * 100ms ausgefuehrt
#define SCHLITZ_JOB_TIMER_LOAD		20 //20 * 100ms = 2 s bis der job ausgefuehrt wird


typedef struct
{
	uint8_t type;
	eds_rolladenSchlitzpos_block_t config;
	uint8_t rolladenSchlitzJob;
	uint8_t rolladenIstposition;
	uint8_t state1stReedkontakt;
} device_data_rolladenSchlitzpos;

void rolladenSchlitzpos_init(device_data_rolladenSchlitzpos *p, eds_block_p it);
void rolladenSchlitzpos_timer_handler(device_data_rolladenSchlitzpos *p);
void rolladenSchlitzpos_can_callback(device_data_rolladenSchlitzpos *p,
		const canix_frame *frame);

#endif

