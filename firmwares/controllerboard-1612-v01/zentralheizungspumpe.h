#ifndef ZENTRALHEIZUNGSPUMPE_H
#define ZENTRALHEIZUNGSPUMPE_H

#include <canix/eds.h>
#include <inttypes.h>
#include <eds-structs.h>

#define EINSCHALT_SCHWELLE 10 // Wertebereich 0..100
#define AUSSCHALT_SCHWELLE EINSCHALT_SCHWELLE - 5 //Hysterese von 5

#define MAX_ANZ_HEIZUNGEN 15

typedef struct
{
	uint8_t type;
	eds_zentralheizungspumpe_block_t config;
	struct
	{
		uint8_t id; //ID der Heizung
		uint8_t waermebedarf;
	} heizung[MAX_ANZ_HEIZUNGEN];
	uint8_t gesamtWaermebedarf;
} device_data_zentralheizungspumpe;

void zentralheizungspumpe_init(device_data_zentralheizungspumpe *p, eds_block_p it);
void zentralheizungspumpe_can_callback(device_data_zentralheizungspumpe *p, const canix_frame *frame);

#endif
