/*
 *  This file is part of the HCAN tools suite.
 *
 *  HCAN is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free
 *  Software Foundation; either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  HCAN is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with HCAN; if not, write to the Free Software Foundation, Inc., 51
 *  Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 *
 *  (c) 2006 by Martin Haller, mah (at) iuse (dot) org
 */

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/boot.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/crc16.h>

#include "chipdef.h"
#include "mcp2515.h"
#include "mcp2515_defs.h"

#define CANIX_NO_LED_SETUP
//#define CANIX_LED_SETUP2 //IL: for Test
#include "../canix/led.h"

#include "../include/hcan_multicast.h"
#include "../include/hcan_eeprom.h"

#define true  1
#define false 0

#define bool char

void (*jump_to_app)(void) = 0x0000;

#define SPI_PORT_DDR DDRB
#define SPI_PORT     PORTB
#define SPI_MISO    6
#define SPI_MOSI    5
#define SPI_SCK     7
#define SPI_CS		4

#define MCP2515_OK         (0)
#define MCP2515_FAIL       (1)

void delay(uint16_t ms)
{
	uint16_t zaehler;

	while (ms)
	{
		zaehler = F_CPU / 5000;
		while (zaehler)
		{
			asm volatile ("nop");
			zaehler--;
		}
		ms--;
	}
}

void spi_init(void)
{
	// Aktivieren der SCK, MISO und MOSI Ports
	// Die CS Ports (ChipSelect) werden von den can_open_interface
	// Aufrufen initialisiert
	
	SPI_PORT_DDR |= (1<<SPI_SCK)|(1<<SPI_MOSI);
	SPI_PORT &= ~(1<<SPI_SCK)|(1<<SPI_MOSI)|(1<<SPI_MISO);

	// SPI Masterinterface aktivieren
	// f(spi) = f(cpu) / 4
	
	SPCR = (1<<SPE)|(1<<MSTR);
}

uint8_t spi_putc( uint8_t data )
{
    /* Sendet ein Byte */
    SPDR = data;

    /* Wartet bis Byte gesendet wurde */
    while( !( SPSR & (1<<SPIF) ) )
        ;

    return SPDR;
}

uint8_t spi_getc(void)
{
    return spi_putc(0);
}

typedef struct
{
	uint8_t reg_oport;
	uint8_t reg_iport;
	uint8_t reg_ddr;
	uint8_t pin;
} pin_desc;

typedef struct
{
	pin_desc cs;  // Beschreibung des CS Pins
} can_interface;

void can_enable_mcp2515(void)
{
	SPI_PORT &= ~(1<< SPI_CS);
}

void can_disable_mcp2515(void)
{
	SPI_PORT |= (1<< SPI_CS);
}

void mcp2515_bit_modify(uint8_t adress, uint8_t mask, uint8_t data)
{
	can_enable_mcp2515();
    spi_putc(SPI_BIT_MODIFY);
    spi_putc(adress);
    spi_putc(mask);
    spi_putc(data);
	can_disable_mcp2515();
}

uint8_t mcp2515_read_register(const uint8_t address)
{
    uint8_t ret;

	can_enable_mcp2515();
    spi_putc(SPI_READ);
    spi_putc(address);
    ret = spi_getc();
	can_disable_mcp2515();

    return ret;
}

void mcp2515_write_register(uint8_t adress, uint8_t data )
{
	can_enable_mcp2515();
    spi_putc(SPI_WRITE);
    spi_putc(adress);
    spi_putc(data);
	can_disable_mcp2515();
}

uint8_t mcp2515_set_mode(uint8_t mode)
{
	uint8_t i;

	mcp2515_bit_modify( MCP_CANCTRL, MODE_MASK, mode);

	// verify as advised in datasheet
	i = mcp2515_read_register( MCP_CANCTRL);
	i &= MODE_MASK;

	if ( i == mode )
		return MCP2515_OK;
	else
	{
		return MCP2515_FAIL;
	}
}

// -------------------------------------------------------------------------
/*      
 *  Beschreibt mehrere Register auf einmal                                       */
void mcp2515_write_register_p(uint8_t adress, uint8_t *data, uint8_t length )
{       
    uint8_t i;
    
	can_enable_mcp2515();

    spi_putc(SPI_WRITE);
    
    spi_putc(adress);

    for (i=0; i<length ;i++ )
        spi_putc(*data++);
    
	can_disable_mcp2515();
}       

// -------------------------------------------------------------------------
/*  
 *  Beschreibt mehrer Register auf einmal
 */ 
void mcp2515_read_register_p(uint8_t adress, uint8_t *data, uint8_t length )
{   
    uint8_t i;

	can_enable_mcp2515();
 
    spi_putc(SPI_READ);

    spi_putc(adress);

    for (i=0; i<length ;i++ )
        *data++ = spi_putc(0xff);

	can_disable_mcp2515();
}


uint8_t can_open_interface(void)
{
	uint8_t res;

	// 1. Aktivieren des CS Pins
	SPI_PORT_DDR |= (1<< SPI_CS);
	SPI_PORT |= (1<< SPI_CS);

	// 2. MCP2515 Software Reset
	can_enable_mcp2515();
	spi_putc(SPI_RESET);
	can_disable_mcp2515();

	delay(255);	

	// Device in den Konfigurations Modus versetzten
	res = mcp2515_set_mode( MODE_CONFIG);
	if (res)
		return res;

	// Speed einstellen: ACHTUNG: gilt fuer MCP Quarz mit 16MHz !!
	// BRP = 7
	//mcp2515_write_register( CNF1, (1<<BRP0)|(1<<BRP1)|(1<<BRP2) );
	mcp2515_write_register( CNF1, (F_CAN / (CAN_SPEED*2*8)) - 1);

	// Prop Seg und Phase Seg1 einstellen
	mcp2515_write_register( CNF2, (1<<BTLMODE)|(1<<PHSEG11) );

	// Wake-up Filter deaktivieren, Phase Seg2 einstelen
	mcp2515_write_register( CNF3, (1<<PHSEG21) );

	// Aktivieren der Rx Buffer Interrupts
	mcp2515_write_register( CANINTE, /*(1<<RX1IE)|(1<<RX0IE)*/ 0);

	/* Buffer 0 : Empfangen aller Nachrichten */
	mcp2515_write_register( RXB0CTRL, (1<<RXM1)|(1<<RXM0) );

	// Deaktivieren der Pins RXnBF Pins ( High Impedance State )
	mcp2515_write_register( BFPCTRL, 0 );

	// TXnRTS Bits als Inputs schalten
	mcp2515_write_register( TXRTSCTRL, 0 );

	// Zurueck in den Normal-Mode
	res = mcp2515_set_mode( MODE_NORMAL);
    if (res)
        return res;
		
	return 0;
}

void can_send_message(uint32_t id, uint8_t *data, uint8_t length)
{
	uint8_t temp[4];

	// Identifier einstellen
	temp[0] = (uint8_t) (id>>21);
	temp[1] = (((uint8_t) (id>>16)) & 0x03) |
		(((uint8_t) (id>>13)) & 0xE0) | (1<<EXIDE);
	temp[2] = (uint8_t) (id>>8);
	temp[3] = (uint8_t) id;
	mcp2515_write_register_p( TXB0SIDH, temp, 4 );


	// Datenlaenge einstellen
	mcp2515_write_register( TXB0DLC, length );

	// Datenbytes schreiben
	mcp2515_write_register_p( TXB0D0, data, length );

	// Nachricht senden
	can_enable_mcp2515();
	spi_putc(SPI_RTS | 0x01);
	can_disable_mcp2515();

}

bool can_message_available(void)
{
	char temp;
	temp = mcp2515_read_register(CANINTF);
	return (temp & ((1 << RX0IF)));
}

// WICHTIG: data muss ein data[8] array sein, um einen kompletten
// Frame aufnehmen zu koennen
uint8_t can_read_message(uint32_t *extended, char *data)
{
	uint8_t size;
	uint8_t id[4];
	uint8_t i;

	mcp2515_read_register_p( RXB0SIDH, id, 4 );
	*extended = id[3] | 
		(((uint32_t) id[2]) << 8) |
		(((uint32_t) id[1] & 0x03) << 16) | 
		(((uint32_t) id[1] & 0xe0) >> 3 << 16) | 
		(((uint32_t) id[0]) << 21);

	// Frame Payload size
	size = mcp2515_read_register( RXB0DLC ) & 0x0F;

	// Daten kopieren
	for (i = 0; i < size; i++)
		data[i] = mcp2515_read_register( RXB0D0+ i);

	// Auf jeden Fall Flag loeschen (Std Frame wird ignoriert!)
	mcp2515_bit_modify(CANINTF, 0x01, 0x00 );

	return size;
}

uint16_t calculate_crc16_over_flash(uint16_t size)
{
	uint16_t i;
	uint16_t crc;
	uint16_t p;

	crc = 0xffff;
	p   = 0;

	// ">> 1" wegen word access!
	for (i = 0; i < (size >> 1); i++)
	{
		uint16_t temp;

		temp = pgm_read_word((const uint16_t*)p);
		crc = _crc16_update(crc, (uint8_t)(temp >> 8));
		crc = _crc16_update(crc, (uint8_t)temp);

		p += 2;
	}

	return crc;
}

uint8_t check_firmware(void)
{
	uint16_t firmware_size;
	uint16_t firmware_crc16;
	uint16_t flash_crc16;

	// 1. Firmware Groesse auslesen
	firmware_size = eeprom_read_word((uint16_t *)EEPR_FIRMWARE_SIZE);

	// 2. Firmware CRC16 auslesen
	firmware_crc16 = eeprom_read_word((uint16_t *)EEPR_FIRMWARE_CRC16);

	// 3. CRC16 ueber Firmware aus Flash errechnen
	flash_crc16 = calculate_crc16_over_flash(firmware_size);

	// 4. Pruefen, ob die CRC16 Summe stimmt
	return (flash_crc16 == firmware_crc16);
}

//#define EEPR_SELF_ADDR	2

#define ADDR_MASK       0x000003ff

#define DEV_STATE_BOOTING           0
#define DEV_STATE_BOOTLOADER        1

int main(void)
{
	uint8_t bootCounter  = 1;
	uint16_t lineCounter = 0;
	uint16_t self_addr;
	uint8_t size;
	uint8_t data[8];
	uint32_t id1;
	uint32_t id2;
	uint16_t addr;
#if defined (__AVR_ATmega8__) || (__AVR_ATmega32__)
	uint8_t i;
	#define SIZE_OF_FLASHBUFFER SPM_PAGESIZE
	#if SIZE_OF_FLASHBUFFER > 255
	#error
	#endif
#elif defined (__AVR_ATmega644__)
	uint16_t i;
	#define SIZE_OF_FLASHBUFFER SPM_PAGESIZE
	#if SIZE_OF_FLASHBUFFER > 511
#error
#endif
#endif
	uint8_t flash_buffer[SIZE_OF_FLASHBUFFER];

	// WD enablen und auf 2 Sekunden setzen:
	wdt_enable(WDTO_2S);
	wdt_reset();

	canix_led_init_ports();

	self_addr = (eeprom_read_byte((uint8_t *)EEPR_SELF_ADDR) | 
			eeprom_read_byte((uint8_t *)(EEPR_SELF_ADDR + 1)) << 8) & ADDR_MASK;

	spi_init();

	// Adressen auf dem Atmega8, siehe /usr/avr/include/avr/iom8.h 
	can_open_interface();

	// Bootflags Status versenden:
	id2 = (((uint32_t)(40) & ADDR_MASK) <<10) | 
		(self_addr) | ((uint32_t)(1) <<20);
	data[0] = 4; // Syslog Service
	data[1] = 1; // Boot Resetflag Log
#if defined (__AVR_ATmega32__) || (__AVR_ATmega8__)
	data[2] = MCUCSR;
	MCUCSR = 0; // Reset Register Flags resetten
#elif defined (__AVR_ATmega644__)
	data[2] = MCUSR;
	MCUSR = 0; // Reset Register Flags resetten
#endif
	size = 3;
	can_send_message(id2, data, size);


	while (1)
	{
		wdt_reset();

		// Pruefen ob Nachricht empfangen wurde:
		if (can_message_available())
		{
			size = can_read_message(&id1, (char *) data);
			if (((
							(((id1 >> 10) & ADDR_MASK) == self_addr) ||
							((id1 >> 10) & ADDR_MASK) == HCAN_MULTICAST_PING)) 
					&& (size >= 2) && (data[0] == 1)) // data[0] == HMS
			{
				switch (data[1])
				{
					case 1 : // HCAN Ping Request (HCAN_HMS_PING_REQUEST / nicht HCAN_EDS_GET_NEXT_BLOCK)
						size = 2;
						data[1] = 2;
						goto _send;
					case 3 : // Device State Req (HCAN_HMS_DEVICE_STATE_REQUEST)
						size = 3;
						data[1] = 4; // Replay (HCAN_HMS_DEVICE_STATE_REPLAY)
						if (bootCounter)
							data[2] = DEV_STATE_BOOTING;
						else
							data[2] = DEV_STATE_BOOTLOADER;
						goto _send;
					case 5 : // Device Reset (HCAN_HMS_DEVICE_RESET)
						wdt_enable(WDTO_15MS);
						while (1);
					case 6 : // Device Boot Stop (HCAN_HMS_DEVICE_BOOT_STOP)
						bootCounter = 0; // Boot beenden
						size = 2;
						data[1] = 7; // ACK senden (HCAN_HMS_DEVICE_BOOT_STOP_ACK)
						goto _send;
					case 8 : // Device Load App (HCAN_HMS_DEVICE_LOAD_APP)
						jump_to_app();
					case 9 : // HCAN_HMS_DEVICE_TYPE_REQUEST
						data[1] = 10; //HCAN_HMS_DEVICE_TYPE_REPLAY
#if defined (__AVR_ATmega8__)
						data[2] = 0;
#elif defined (__AVR_ATmega32__)
						data[2] = 1;
#elif defined (__AVR_ATmega644__)
						data[2] = 2;
#endif
						data[3] = eeprom_read_byte((uint8_t *)4); //4=EEPR_BOARD_TYPE
						size = 4;
						goto _send;
					case 16 : // Flash Buffer Fill (HCAN_HMS_FLASH_BUFFER_FILL)
						if (!(data[2] & 0x0f))
							lineCounter = 0;

						for (i = 0; i < 4; i++)
							flash_buffer[data[2]+i] = data[3+i];

						lineCounter += 4; // 4 Bytes gekommen
						if (lineCounter >= 16) // ACK faellig
						{
							size = 2;
							data[1] = 17; //HCAN_HMS_FLASH_BUFFER_FILL_ACK (eine Zeile wurde gefuellt...)
							goto _send;
						}
						else 
							break;
					case 18 : // Flash Write (HCAN_HMS_FLASH_BUFFER_WRITE)
						addr = data[2];
						addr |= data[3] << 8;

						eeprom_busy_wait();

						boot_page_erase(addr);
						boot_spm_busy_wait();

						uint8_t * buf = flash_buffer;

						for (i = 0; i < SIZE_OF_FLASHBUFFER; i += 2)
						//for (i = 0; i < SPM_PAGESIZE; i += 2)
						{
							// Set up little-endian word.

							uint16_t w = *buf++;
							w += (*buf++) << 8;

							boot_page_fill (addr + i, w);
						}

						boot_page_write(addr);
						boot_spm_busy_wait();
						boot_rww_enable();

						size = 2;
						data[1] = 19; // ACK (HCAN_HMS_FLASH_BUFFER_WRITE_ACK)
						goto _send;
					case 20 : // EEPROM Write (HCAN_HMS_INTERNAL_EEPROM_WRITE)
						eeprom_write_byte((uint8_t *)(data[2] | 
									(data[3] << 8)), data[4]);
						size = 2;
						data[1] = 21;
						goto _send;
					case 22 : // EEPROM Read (HCAN_HMS_INTERNAL_EEPROM_READ_REQUEST)
						size = 3;
						data[1] = 23;
						data[2] = eeprom_read_byte((uint8_t *)(data[2] | 
									(data[3] << 8)));
						goto _send;
					case 37 : // FLASH_CRC16_QUERY (HCAN_HMS_FLASH_CRC16_QUERY)
						{
							uint16_t sum;

							sum = calculate_crc16_over_flash(
									((uint16_t)data[2] << 8) | data[3]);
							size = 4;
							data[1] = 38; //HCAN_HMS_FLASH_CRC16_REPLAY
							data[2] = sum >> 8;
							data[3] = sum & 0xff;
							goto _send;
						}

					default:
						break;
_send:
						id2 = (((uint32_t)(id1) & ADDR_MASK) <<10) |
							(self_addr) | ((uint32_t)(1) <<20);
						can_send_message(id2, data, size);
				}
			}
		}
		if (bootCounter > 0)
		{
			bootCounter++;
			canix_led_enable(CANIX_LED_RED); //bei IL: LED off
			delay(50);
			canix_led_disable(CANIX_LED_RED);
			delay(50);
			if (bootCounter > 50)
			{
				if (!check_firmware())
				{
					uint32_t id;
					uint8_t data[8];
					id = (((uint32_t)(40) & ADDR_MASK) <<10) | 
						(self_addr) | ((uint32_t)(1) <<20);
					data[0] = 4; // Syslog Service
					data[1] = 2; // Firmware CRC16 Error
					size = 2;
					can_send_message(id, data, size);

					// Reset nach einer Sekunde
					wdt_enable(WDTO_1S);
					while(1);
				}
				else
					jump_to_app();
			}
		}
	}

	return 0;
}

