#ifndef HELLIGKEITSSENSOR_H
#define HELLIGKEITSSENSOR_H

#include <canix/eds.h>
#include <inttypes.h>
#include <eds-structs.h>
#include "floatingAverage.h"

typedef struct
{
	uint8_t type;
	eds_helligkeitssensor_block_t config;
	uint16_t secsUntilNextSend; // zyklisch Helligkeitsnachrichten versenden
	tFloatAvgFilter filterBrightness; // Speicher des gleitenden Mittelwerts zur Glättung
} device_data_helligkeitssensor;

void helligkeitssensor_init(device_data_helligkeitssensor *p, eds_block_p it);
void helligkeitssensor_timer_handler(device_data_helligkeitssensor *p);

#endif
