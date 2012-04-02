/** IO-Extension for HCAN */
#ifndef SHIFTIO_DEBUG_PORT_H_
#define SHIFTIO_DEBUG_PORT_H_

#include <canix/canix.h> //for SYSLOG_PRIO_CRITICAL
#include <canix/syslog.h> //for canix_syslog_P

#define debugPort(portXY) canix_syslog_P(SYSLOG_PRIO_CRITICAL, PSTR("%x%x%x%x%x"),\
                             (portXY & 0x10)!=0, (portXY & 0x08)!=0, (portXY & 0x04)!=0,\
                             (portXY & 0x02)!=0, (portXY & 0x01)!=0); //Zustand von debugPort binaer (5 Bits) ausgeben

#endif /*DEBUG_PORT_H_*/

