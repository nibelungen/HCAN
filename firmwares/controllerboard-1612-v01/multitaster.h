#ifndef MUTLITASTER_H
#define MUTLITASTER_H

#include <canix/eds.h>
#include <inttypes.h>
#include <eds-structs.h>

#define MAX_MULTITASTER_GROUPS 10

#define FEATURE_MULTITASTER_ENTPRELL_1S  0 //Bit0 (the LSB)
#define FEATURE_MULTITASTER_ROLLADEN_AUF 1 //Bit1
#define FEATURE_MULTITASTER_ROLLADEN_ZU  2 //Bit2


typedef struct
{
	uint8_t type;
	eds_multitaster_block_t config;
	uint8_t pressed; // fuer die Taster Entprellung	
	uint8_t lastPotiPos; // Ist der Potiabschnitt, in dem das Poti zuletzt stand (letzter Gruppenindex)
	uint16_t oneScale; // Groesse eines Potiabschnitts (durch die Anzahl konfigurierter Gruppen festgelegt)
} device_data_multitaster;

void multitaster_init(device_data_multitaster *p, eds_block_p it);
void multitaster_timer_handler(device_data_multitaster *p);
void multitaster_can_callback(device_data_multitaster *p, const canix_frame *frame);

#endif
