#ifndef POTI_H
#define POTI_H

#include <canix/eds.h>
#include <inttypes.h>
#include <eds-structs.h>

#define POTI_VALUE_TOLERANCE 30 //min. Toleranz, damit ein der Potiwert als veraendert gilt

/* Anzahl Perioden fuer die der Potiwert konstant sein muss, 
 * damit eine neue Potinachricht versendet wird. Mindestprojektierung = 1 */
#define POTI_TELEGRAM_COUNTDOWN_INIT 4

typedef struct
{
	uint8_t type;
	eds_poti_block_t config;
	int16_t lastPotiValue; //kann durch MIN_POTI_VALUE_DIFF auch negativ werden
	uint8_t potiTelegramCountdown;
} device_data_poti;

void poti_init(device_data_poti *p, eds_block_p it);
void poti_timer_handler(device_data_poti *p);
int16_t getADCvalue(void);

#endif
