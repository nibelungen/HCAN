#ifndef WECK_PAGE_H
#define WECK_PAGE_H

#include <inttypes.h>
#include <canix/canix.h>
#include <eds-structs.h>

// Fuer die Powerport-Feature-Konstanten
#include "../controllerboard-1612-v01/powerport.h"
/* POWERPORT_FEATURE_WECK_VERZOEGERT_EIN	4 //Bit4
 * POWERPORT_FEATURE_WECK_AUTO_AUS			5 //Bit5 */

void weck_page_handle_key_down_event(eds_weck_page_block_t *p, uint8_t key);
void weck_page_print_page(eds_weck_page_block_t *p);
void weck_page_can_callback(eds_weck_page_block_t *p, const canix_frame *frame);

#endif
