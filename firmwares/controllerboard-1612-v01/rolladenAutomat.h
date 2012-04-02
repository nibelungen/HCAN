#ifndef ROLLADENAUTOMAT_H
#define ROLLADENAUTOMAT_H

#include <canix/eds.h>
#include <inttypes.h>
#include <eds-structs.h>

#define N_ZEITZONEN 2

typedef struct
{
	uint8_t type;
	eds_rolladenAutomat_block_t config;

	uint16_t brightness; // hier ist der aktuelle Helligkeitswert gespeichert
	uint8_t msgIstVersendet; // ROLLADEN_POSITION_SET ist einmalig versendet wurde
	uint8_t automatikEin; // Automatik aktiviert oder nicht
} device_data_rolladenAutomat;

void rolladenAutomat_init(device_data_rolladenAutomat *p, eds_block_p it);
void rolladenAutomat_timer_handler(device_data_rolladenAutomat *p);
void rolladenAutomat_can_callback(device_data_rolladenAutomat *p, const canix_frame *frame);

#endif
