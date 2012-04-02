#ifndef GESAMT_HEIZ_PAGE_H
#define GESAMT_HEIZ_PAGE_H

#include <inttypes.h>
#include <canix/canix.h>
#include <eds-structs.h>

#define MAX_HEIZ_IDS 12

// aus heizung.h:   HEIZUNG_MODE_OFF             0
// nicht verwendet: HEIZUNG_MODE_MANUAL          1
#define HEIZUNG_MODE_THERMOSTAT_FROSTSCHUTZ      1
#define HEIZUNG_MODE_THERMOSTAT_FROSTSCHUTZ_KURZ 2
#define HEIZUNG_MODE_THERMOSTAT_X                3 // _X, da _KURZ eingefuegt ist
#define HEIZUNG_MODE_AUTOMATIK_X                 4


void gesamt_heiz_page_handle_key_down_event(eds_gesamt_heiz_page_block_t *p,
		uint8_t key);
void gesamt_heiz_page_print_page(eds_gesamt_heiz_page_block_t *p);

#endif
