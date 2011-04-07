/*
  Trimble - a small GPS library for Arduino providing basic TSIP parsing
  Based on work by Mikal Hart at Arduiniana.
  Copyright (c) 2011 N7MG Brett Howard & N7UL Andrew Stern

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

#ifndef Trimble_h
#define Trimble_h

#include "WProgram.h"

#define BIT0  0x0001
#define BIT1  0x0002
#define BIT2  0x0004
#define BIT3  0x0008
#define BIT4  0x0010
#define BIT5  0x0020
#define BIT6  0x0040
#define BIT7  0x0080
#define BIT8  0x0100
#define BIT9  0x0200
#define BIT10 0x0400
#define BIT11 0x0800
#define BIT12 0x1000
#define BIT13 0x2000
#define BIT14 0x4000
#define BIT15 0x8000

#define _GPS_VERSION 0 // software version of this library
#define _GPS_MPH_PER_KNOT 1.15077945
#define _GPS_MPS_PER_KNOT 0.51444444
#define _GPS_KMPH_PER_KNOT 1.852
#define _GPS_MILES_PER_METER 0.00062137112
#define _GPS_KM_PER_METER 0.001

//match Trimble Documentation Datatypes to Arduino Datatypes
#define UINT8 unsigned char
#define SINT8 char
#define INT16 int
#define SINT16 int
#define UINT16 unsigned int
#define UINT32 unsigned long
#define SINT32 long
#define SINGLE float    //?
#define DOUBLE double         //?

#define MAX_DATA 1024

#define DLE 0x10
#define ETX 0x03
#define TRUE 1
#define FALSE 0
//state values (should probably be an enum)
#define START = 1
#define FRAMING_DLE = 2
#define DATA = 4
#define DATA_DLE = 8


class Trimble
{
    public:
        Trimble();
        bool encode(byte c);
        void parsePacket();
	Trimble &operator << (char c) {encode(c); return *this;}

    private:
        
	byte 	m_read_buffer[MAX_DATA];
	int	m_num_chars;
	bool	m_dle_seen;
	int	m_state;
	
        //8F-AB Packet Elements
        struct _8FAB {
            UINT32  timeOfWeek;
            UINT16  weekNumber;
            SINT16  UTCOffset;
            UINT8    TimingFlag;
            UINT8   Seconds;
            UINT8   Minutes;
            UINT8   Hours;
            UINT8   DayOfMonth;
            UINT8   Month;
            UINT16  Year;
        };
        
        //Timing Flags
        //BIT0 - 0=GPS Time 1=UTC Time ; Set using 8E-A2
        //BIT1 - 0=1PPS output aligned to GPS (0) or UTC (1) ; Set using 8E-A2
        //BIT2 - 0=Time set from GPS, 1=Time not yet set from GPS
        //BIT3 - 0=UTC offset information known, 1=UTC offset not known
        //BIT4 - 0=Time coming from GPS, 1=Test mode generated time
        //8F-AC Packet Elements
        struct _8FAC {
            UINT8   ReceiverMode;
            UINT8   DiscipliningMode;
            UINT8   SelfSurveyProgress;
            UINT32  HoldoverDuration;
            UINT16  CriticalAlarms;
            UINT16  MinorAlarms;
            UINT8   GPSDecodingStatus;
            UINT8   DiscipliningActivity;
            UINT8   SpareStatus1;
            UINT8   SpareStatus2;
            SINGLE  PPSOffset;
            SINGLE  _10MhzOffset;
            UINT32  DACValue;
            SINGLE  DACVoltage;
            SINGLE  Temperature;
            DOUBLE  Latitude;
            DOUBLE  Longitude;
            DOUBLE  Altitude;
            UINT8    _8F_AC_Byte60;
            UINT8    _8F_AC_Byte61;
            UINT8    _8F_AC_Byte62;
            UINT8    _8F_AC_Byte63;
            UINT8    _8F_AC_Byte64;
            UINT8    _8F_AC_Byte65;
            UINT8    _8F_AC_Byte66;
            UINT8    _8F_AC_Byte67;
        };

        /*
        enum ReceiverModeType { "Auto 2D/3D",
                                "Single Sat (Time)",
                                "Horiz (2D)",
                                "Full Pos (3D)",
                                "DGPS Ref",
                                "Clk Hold",
                                "Overdet Clock"
                              };
                          
       enum DiscipliningModeType {  "Norm",
                                    "Pwr Up",
                                    "Auto Hldovr",
                                    "Man. Hldovr",
                                    "Recovery",
                                    "Not Used",
                                    "Discp Off"
                                 };
                                 
        enum GPSDecodingStatusType {    "Doing Fixes" = 0,
                                        "No GPS Time" = 1,
                                        "PDOP Too High" = 3,
                                        "No Sats" = 8,
                                        "Only 1 Sat" = 9,
                                        "Only 2 Sats" = 0x0A,
                                        "Only 3 Sats" = 0x0B,
                                        "Chosen Sat Unavail" = 0x0C,
                                        "TRAIM Rej Fix" = 0x10
                                    };
                                    
        enum DiscipliningActivityType { "Phase Lock",
                                        "OSC Warming",
                                        "Freq Lock",
                                        "Placing PPS",
                                        "Init Loop Fil",
                                        "Comp OCXO",
                                        "Inactive",
                                        "Not Used",
                                        "Recovery"
                                      }; 
                                      
                                        
        
        //Critical Alarms
        //BIT0 - ROM Checksum Error
        //BIT1 - RAM Failure
        //BIT2 - Power Failure
        //BIT3 - FPGA Failure
        //BIT4 - OSC Control Voltage at Rail
        
        //Minor Alarms"
        //BIT0 - OSC Control Voltage Near Rail
        //BIT1 - Antenna Open
        //BIT2 - Antenna Shorted
        //BIT3 - No Sats
        //BIT4 - OSC Not Disciplined
        //BIT5 - Self Survey in Progress
        //BIT6 - No Stored Position
        //BIT7 - Leap Second Pending
        //BIT8 - Test Mode
        //BIT9 - Position Error //Stored pos != cur pos, Solve by clearing stored pos (8E-45) and setting a new position (packet 0x31 or 0x32) or by starting a self survey (packet 0x8E-A6)
        //BIT10 - EEPROM corrupt settings were defaulted //Packet 0x3F-11 will give further details
        //BIT11 - Almanac Not Current or Incomplete*/
};

// Arduino 0012 workaround
#undef int
#undef char
#undef long
#undef byte
#undef float
#undef abs
#undef round 

#endif 
