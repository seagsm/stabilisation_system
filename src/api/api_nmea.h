#ifndef API_NMEA_H
#define API_NMEA_H 1

#include "stm32f10x.h"
#include "board_system_type.h"
#include "math.h"
#include "api_common.h"



/*
  nmea.cpp - NMEA 0183 sentence decoding library for Wiring & Arduino
  Copyright (c) 2008 Maarten Lamers, The Netherlands.
  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#define	ALL             0               /* connect to all datatypes            */
#define	GPRMC           1               /* connect only to GPRMC datatype      */
#define	MTR             1.0             /* meters per meter                    */
#define	KM              0.001           /* kilometers per meter                */
#define	MI              0.00062137112   /* miles per meter                     */
#define	NM              0.00053995680   /* nautical miles per meter            */
#define	PARSEC          0.000000000000  /* parsecs per meter (approximation)   */
#define	MPS             0.51444444      /* meters-per-second in one knot       */
#define	KMPH            1.852           /* kilometers-per-hour in one knot     */
#define	MPH             1.1507794       /* miles-per-hour in one knot          */
#define	KTS             1.0             /* knots in one knot                   */
#define	LIGHTSPEED      0.000000001716  /* lightspeeds in one knot             */



#define _boolean    uint32_t
#define _true    1U
#define _false   0U

BOARD_ERROR nmea_init(int32_t connect);   /* constructor for NMEA parser object; parse sentences of GPRMC or all datatypes. */

BOARD_ERROR api_nmea_decode(char c);                  /* parse one character received from GPS; returns 1 when full sentence found w/ checksum OK, 0 otherwise */

float   f_api_nmea_gprmc_utc(void);                    /* returns decimal value of UTC term in last full GPRMC sentence*/
char    c_api_nmea_gprmc_status(void);                 /* returns status character in last full GPRMC sentence ('A' or 'V')*/
float   f_api_nmea_gprmc_latitude(void);               /* signed degree-decimal value of latitude terms in last full GPRMC sentence*/
float   f_api_nmea_gprmc_longitude(void);              /* signed degree-decimal value of longitude terms in last full GPRMC sentence*/
float   f_api_nmea_gprmc_speed(float unit);        /* speed-on-ground term in last full GPRMC sentence*/
float   f_api_nmea_gprmc_course(void);                 /* track-angle-made-good term in last full GPRMC sentence*/
float   f_api_nmea_gprmc_distance_to(float f_latitude, float f_longitude, float f_unit); /* returns distance from last-known GPRMC position to given position*/
float   f_api_nmea_gprmc_course_to(float f_latitude, float f_longitude);               /* returns initial course in degrees from last-known GPRMC position to given position*/


char*       pc_api_nmea_sentence(void);                     /* returns last received full sentence as zero terminated string*/
uint32_t    u32_api_nmea_terms(void);                        /* returns number of terms (including data type and checksum) in last received full sentence*/
char*       pc_api_nmea_term(int t);                    /* returns term t of last received full sentence as zero terminated string*/
float       f_api_nmea_term_term_decimal(int t);            /* returns the base-10 converted value of term[t] in last full sentence received*/
uint32_t    u32_api_nmea_libversion(void);                   /* returns software version number of NMEA library*/
float f_api_nmea_term_decimal(int t);

float f_api_nmea_gprmc_distance_to(float f_latitude, float f_longitude, float f_unit); 


float   f_api_nmea_distance_between (float lat1, float long1, float lat2, float long2, float units_per_meter);
float   f_api_nmea_initial_course(float lat1, float long1, float lat2, float long2);
uint8_t u8_api_nmea_dehex(char a); 
/* int     _dehex(char a);*/

float f_api_nmea_decimal(char s[]); 
/* float	_decimal(char* s); */



float f_sin(float f_value);
float f_cos(float f_value);




#endif

