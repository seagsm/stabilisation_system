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

#include "api_nmea.h"

#define _GPRMC_TERM     "$GPRMC,"       /* GPRMC datatype identifier        */
#define _GPGGA_TERM     "$GPGGA,"       /* GPGGA datatype identifier        */
#define _GPVTG_TERM     "$GPVTG,"       /* GPVTG datatype identifier        */

#define _LIB_VERSION    1U               /* software version of this library */


static int32_t  _i32_gprmc_only;
static float    _gprmc_utc;
static char     _gprmc_status;
static float    _gprmc_lat;
static float    _gprmc_long;
static float    _gprmc_speed;
static float    _gprmc_angle;

static float    _gpgga_utc;
static float    _gpgga_cog;
static float    _gpvtg_cog;



static char      f_sentence[100];
static char      f_term[30][15];
static int       f_terms;
static int      _terms;
static char     _sentence[100];
static char     _term[30][15];
static int32_t   i32_n;  
static int      _gprmc_tag;
static int      _gpgga_tag;
static int      _gpvtg_tag;
static int      _state;
static uint16_t _parity;
static int32_t  _i32_nt;
static float    _degs;

/* static char _header[6]; */


BOARD_ERROR nmea_init(int32_t connect)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;  

    _i32_gprmc_only     = connect;
    _gprmc_utc      = 0.0f;
    _gprmc_status   = 'V';
    _gprmc_lat      = 0.0f;
    _gprmc_long     = 0.0f;
    _gprmc_speed    = 0.0f;
    _gprmc_angle    = 0.0f;
    _terms          = 0;
     i32_n          = 0;
    _state          = 0;
    _parity         = 0U;
    _i32_nt         = 0;

    f_sentence[0]   = 0U;
    f_terms         = 0;
	
    /* allocate memory for individual terms of sentence */
    for (int t=0; t<30; t++) 
    {
        f_term[t][0] = 0U;
    }
  
    return(be_result);
}



BOARD_ERROR api_nmea_decode(USART_TypeDef*  USARTx) 
{
      BOARD_ERROR be_result = BOARD_ERR_OK;

      uint8_t  u8_read_byte;
      uint8_t  u8_i = 0U; 
      
      while(be_result == BOARD_ERR_OK )
      {
          /* Read byte from UART1 Rx buffer. Remember, after reading size--, tail++. */
          be_result = be_board_r_buff_USARTx_RX_GET_byte(USARTx, &u8_read_byte);
          if(be_result == BOARD_ERR_OK)
          {  
              /*   u8_tx_UART3_data_packet[0] = u8_read_byte ; */
              _api_nmea_decode((char)u8_read_byte); 
              /*  sv_board_dma_ch2_send_packet(2U); */
          }
          else
          {
              u8_i++ ;
          }
      }
      
      return (be_result);
}

static BOARD_ERROR _api_nmea_decode(char c) 
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    
    /* Characters filter avoid runaway sentences (>99 chars or >29 terms) and terms (>14 chars) */
    if ((i32_n >= 100) || (_terms >= 30) || (_i32_nt >= 15)) 
    {      
        _state = 0; 
    }
  
    /* LF and CR always reset parser */
    if ((c == 0x0AU) || (c == 0x0DU)) 
    { 
        _state = 0; 
    }
    /* '$' always starts a new sentence */
    if (c == '$') 
    {
        _gprmc_tag      = 0;
        _gpgga_tag      = 0;
        _gpvtg_tag      = 0;
        _parity         = 0U;
        _terms          = 0;
        _i32_nt         = 0;
        _sentence[0]    = c;
        i32_n           = 1;
        _state          = 4;
        
        be_result = BOARD_ERR_OK;
    }
    
    /* parse other chars according to parser state */
    switch(_state) 
    {
        case 0:
            /* waiting for '$', do nothing */
            break;
        case 1:
            /* decode chars after '$' and before '*' found */
            if (i32_n < 7) 
            {
                /* see if first seven chars match "$GPRMC," */
                if (c == _GPRMC_TERM[i32_n]) 
                { 
                    _gprmc_tag++; 
                }
                if (c == _GPGGA_TERM[i32_n]) 
                { 
                    _gpgga_tag++; 
                }
                if (c == _GPVTG_TERM[i32_n]) 
                { 
                    _gpvtg_tag++; 
                }

            }
            /* add received char to sentence*/
            _sentence[i32_n++] = c;
            switch (c) 
            {
                case ',':
                    /* ',' delimits the individual terms */
                    (_term[_terms++])[_i32_nt] = 0U;
                    _i32_nt = 0;
                    _parity = _parity ^ c;
                    break;
                case '*':
                    /* '*' delimits term and precedes checksum term */
                    _term[_terms++][_i32_nt] = 0U;
                    _i32_nt = 0;
                    _state++;
                    break;
                default:
                    /* all other chars between '$' and '*' are part of a term */
                    (_term[_terms])[_i32_nt++] = c;
                     _parity = _parity ^ c;
                    break;
            }
            break;
        case 2:
            /* first char following '*' is checksum MSB */
            _sentence[i32_n++] = c;
            (_term[_terms])[_i32_nt++] = c;
            _parity = _parity - (16U * (uint16_t)u8_api_nmea_dehex(c));		/* replace with bitshift? */
            _state++;
            break;
        case 3:
            /* second char after '*' completes the checksum (LSB) */
            _sentence[i32_n++] = c;
            _sentence[i32_n++] = 0U;
            (_term[_terms])[_i32_nt++] = c;
            (_term[_terms++])[_i32_nt] = 0U;
            _state = 0;
            _parity = _parity - (uint16_t)u8_api_nmea_dehex(c);
            /* when parity is zero, checksum was correct! */
            if (_parity == 0U) 
            {
                /* accept all sentences, or only GPRMC datatype? */
                if ((!_i32_gprmc_only) || (_gprmc_tag == 6) || (_gpgga_tag == 6) || (_gpvtg_tag == 6)) 
                {
                    /* copy _sentence[] to f_sentence[] */
                    while ((--i32_n) >= 0) 
                    { 
                        f_sentence[i32_n] = _sentence[i32_n]; 
                    }
                    /* copy all _terms[] to f_terms[] */
                    for (f_terms=0; f_terms<_terms; f_terms++) 
                    {
                        _i32_nt = 0;
                        while ((_term[f_terms])[_i32_nt]) 
                        {
                            (f_term[f_terms])[_i32_nt] = (_term[f_terms])[_i32_nt];
                            _i32_nt++;
                        }
                        f_term[f_terms][_i32_nt] = 0U;
                    }
                    /* when sentence is of datatype GPRMC */
                    if (_gprmc_tag == 6) 
                    {
                        /* store values of relevant GPRMC terms */
                        /*_gprmc_utc = _decimal(_term[1]);*/
                        _gprmc_utc = f_api_nmea_decimal(_term[1]);  
                        _gprmc_status = (_term[2])[0];
                        /* calculate signed degree-decimal value of latitude term */
                        _gprmc_lat = f_api_nmea_decimal(_term[3]) / 100.0f;
                        _degs = (float)floor((double)_gprmc_lat);
                        _gprmc_lat = (100.0f * (_gprmc_lat - _degs)) / 60.0f;
                        _gprmc_lat += _degs;
                        /* southern hemisphere is negative-valued */
                        if ((_term[4])[0] == 'S') 
                        {
                            _gprmc_lat = 0.0f - _gprmc_lat;
                        }
                        /* calculate signed degree-decimal value of longitude term */
                        _gprmc_long = f_api_nmea_decimal(_term[5]) / 100.0f;
                        _degs = (float)floor((double)_gprmc_long);
                        _gprmc_long = (100.0f * (_gprmc_long - _degs)) / 60.0f;
                        _gprmc_long += _degs;
                        /* western hemisphere is negative-valued */
                        if ((_term[6])[0] == 'W') 
                        {
                            _gprmc_long = 0.0f - _gprmc_long;
                        }
                        _gprmc_speed = f_api_nmea_decimal(_term[7]);
                        _gprmc_angle = f_api_nmea_decimal(_term[8]);
                    }
                    else if (_gpgga_tag == 6) 
                    {
                        /* store values of relevant GPGGA terms */
                        /* UTC time*/
                        _gpgga_utc = f_api_nmea_decimal(_term[1]);  
                        /* add more data */
                    }
                    else if (_gpvtg_tag == 6) 
                    {
                        /* store values of relevant GPVTG terms */
                        /* course over ground */  
                        _gpvtg_cog = f_api_nmea_decimal(_term[1]);  
                    }
                    else
                    {
                    
                    }
                    
#if 0               /* TEST TX: This part send to TX received packet. */
                    { 
                        uint16_t  u16_i = 0U;
                        while(f_sentence[u16_i] != 0U)
                        {
                            u8_tx_UART3_data_packet[u16_i] = f_sentence[u16_i];
                            u16_i++;
                        }  
                        u8_tx_UART3_data_packet[u16_i] = 0x0AU;
                        u16_i++;
                        u8_tx_UART3_data_packet[u16_i] = 0x0DU;
                        u16_i++;
                        sv_board_dma_ch2_send_packet(u16_i);
                     
                    }
                    /********/
 #endif                    
                    
                    
                    
                    
                    
                    
                    /* sentence accepted! */
                   be_result =  BOARD_ERR_PACKET_OK;
              }
          }
          break;
      case 4:
          _state = 1;
          break;      
      default:
          _state = 0;
          break;
    }

    be_result = BOARD_ERR_OK;
    return (be_result);
    
}

float f_api_nmea_gprmc_utc(void) 
{
	/* returns decimal value of UTC term of last-known GPRMC sentence */
	return _gprmc_utc;
}

char c_api_nmea_gprmc_status(void) 
{
	/* returns status character of last-known GPRMC sentence ('A' or 'V')*/
	return _gprmc_status;
}

void    v_api_nmea_clear_gprmc_status(void)
{
	/* returns status character of last-known GPRMC sentence ('A' or 'V')*/
	_gprmc_status = (char)0;
}

float f_api_nmea_gprmc_latitude(void)
{
	/* returns signed degree-decimal latitude value of last-known GPRMC position */
	return _gprmc_lat;
}

float f_api_nmea_gprmc_longitude(void) 
{
	/* returns signed degree-decimal longitude value of last-known GPRMC position */
	return _gprmc_long;
}

float f_api_nmea_gprmc_speed(float unit) 
{
	/* returns speed-over-ground from last-known GPRMC sentence */
	return (_gprmc_speed * unit);
}

float f_api_nmea_gprmc_course(void) 
{
	/* returns decimal value of track-angle-made-good term in last-known GPRMC sentence */
	return _gprmc_angle;
}

float f_api_nmea_gprmc_distance_to(float f_latitude, float f_longitude, float f_unit) 
{
	/* returns distance from last-known GPRMC position to given position */
	return f_api_nmea_distance_between( _gprmc_lat, _gprmc_long, f_latitude, f_longitude, f_unit);
}

float f_api_nmea_gprmc_course_to(float f_latitude, float f_longitude) 
{
	/* returns initial course in degrees from last-known GPRMC position to given position */
	return f_api_nmea_initial_course( _gprmc_lat, _gprmc_long, f_latitude, f_longitude);
}






/*
float NMEA::gprmc_rel_course_to(float latitude, float longitude) {
	// returns course in degrees to given position, relative to last-known GPRMC track angle
	float rc = initial_course( _gprmc_lat, _gprmc_long, latitude, longitude) - _gprmc_angle;
	if (rc < 0.0) {
		rc += 360.0;
	}
	return rc;
}
*/



char* pc_api_nmea_sentence(void) 
{
    /*returns last received full sentence as zero terminated string */
    return f_sentence;
}

uint32_t u32_api_nmea_terms(void) 
{
    /* returns number of terms (including data type and checksum) in last received full sentence */
    return (uint32_t)f_terms;
}



char* pc_api_nmea_term(int t) 
{
    /* returns term t of last received full sentence as zero terminated string */
    return f_term[t];
}

float f_api_nmea_term_decimal(int t) 
{
    /* returns value of decimally coded term t */
    return f_api_nmea_decimal(f_term[t]);
}

uint32_t u32_api_nmea_libversion(void) 
{
    /* returns software version of this library */
    return _LIB_VERSION;
}



/* private methods */

float f_api_nmea_distance_between (float lat1, float long1, float lat2, float long2, float units_per_meter) 
{
    /*  
        returns distance in meters between two positions, both specified
        as signed decimal-degrees latitude and longitude. Uses great-circle
        distance computation for hypothised sphere of radius 6372795 meters.
        Because Earth is no exact sphere, rounding errors may be upto 0.5%.
    */
    float delta = deg2rad(long1-long2);
    float sdlong = f_sin(delta);
    float cdlong = f_cos(delta);
    lat1 = deg2rad(lat1);
    lat2 = deg2rad(lat2);
    float slat1 = f_sin(lat1);
    float clat1 = f_cos(lat1);
    float slat2 = f_sin(lat2);
    float clat2 = f_cos(lat2);
    delta = (clat1 * slat2) - (slat1 * clat2 * cdlong);
    delta = (float)sqrt((double)delta);
    double d_tmp = ((double)clat2) * ((double)sdlong);
    delta += (float)sqrt(d_tmp);  /*  delta += (float)sqrt((double)(clat2 * sdlong)); */ 
    delta = (float)sqrt((double)delta);
    float denom = (slat1 * slat2) + (clat1 * clat2 * cdlong);
    delta = (float)atan2((double)delta, (double)denom);
    return delta * 6372795.0f * units_per_meter;
}


float f_api_nmea_initial_course (float lat1, float long1, float lat2, float long2) 
{
    /*
        returns initial course in degrees (North=0, West=270) from
        position 1 to position 2, both specified as signed decimal-degrees
        latitude and longitude.
    */
  
    float dlon = deg2rad(long2-long1);
    lat1 = deg2rad(lat1);
    lat2 = deg2rad(lat2);
    float a1 = f_sin(dlon) * f_cos(lat2);
    float a2 = f_sin(lat1) * f_cos(lat2) * f_cos(dlon);
    a2 = f_cos(lat1) * f_sin(lat2) - a2;
    a2 = (float)atan2((double)a1, (double)a2);
    if (a2 < 0.0f) 
    {
        a2 +=(float)(_M_PI + _M_PI ); /* TWO_PI; */			/* modulo operator doesn't seem to work on floats */
    }
    
    return rad2deg(a2);
}



static uint8_t u8_api_nmea_dehex(char a) 
{
    /* 
        returns base-16 value of chars '0'-'9' and 'A'-'F';
	does not trap invalid chars!
    */
    uint8_t u8_tmp = 0U;
    u8_tmp = (uint8_t)a;
  
    if (u8_tmp >= 65U) 
    {
        u8_tmp = u8_tmp - 55U;
    }
    else 
    {
        u8_tmp = u8_tmp - 48U;
    }
    
    return (u8_tmp);
}





static float f_api_nmea_decimal(char s[]) 
{
    /*
        returns base-10 value of zero-termindated string
	that contains only chars '+', '-', '0'-'9', '.';
	does not trap invalid strings!
    */
    char c_tmp;
    long  rl = 0;
    float rr = 0.0f;
    float rb = 0.1f;
    _boolean dec = _false;
    int i = 0;
    
    if ((s[i] == '-') || (s[i] == '+')) 
    { 
        i++; 
    }
    while (s[i] != 0U) 
    {
        if (s[i] == '.') 
        {
            dec = _true;
        }
        else
        {
            if (!dec)
            {
                rl = ((long)10 * rl);
                c_tmp = s[i] - 48U;
                rl = rl + (long)c_tmp;
            }
            else 
            {
                c_tmp = s[i] - 48U;
                rr += rb * (float)c_tmp;
                rb /= 10.0f;
            }
        }
        i++;
    }
    rr += (float)rl;
    if (s[0] == '-') 
    {
        rr = 0.0f - rr;
    }
    return rr;
}





float f_sin(float f_value)
{
  return ((float)asin((double)f_value));
}

float f_cos(float f_value)
{
  return ((float)acos((double)f_value));
}
