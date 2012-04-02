#ifndef SCHALTER_H
#define SCHALTER_H

#include <canix/eds.h>
#include <inttypes.h>
#include <eds-structs.h>

#define FALLING	0 //Eingangs-Port 1->0
#define RISING 	1 //Eingangs-Port 0->1

#define FEATURE_SCHALTER_INVERTIEREN  0 // Bit0 (the LSB)

typedef struct
{
	uint8_t type;
	eds_schalter_block_t config;
	uint8_t newState; // fuer Entprellung
	uint8_t lastEdge; // letzte erkannte Flanke (steigend oder fallend)
} device_data_schalter;

void schalter_init(device_data_schalter *p, eds_block_p it);
void schalter_timer_handler(device_data_schalter *p);

#endif

