/*
  tsip.h - A simplified Trimble Standard Interface Protocol (TSIP) library
           for the Arduino platform.

           The library configured to work with the Trimble Thunderbolt GPS
           Disciplined Oscillator (GPSDO) and is based on the
           "ThunderBolt GPS Disciplined Clock User Guide" Version 5.0
           Part Number: 35326-30  November 2003  

  Copyright (c) 2011 N7MG Brett Howard
  Copyright (c) 2011 Andrew Stern (N7UL)

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

  $Id: $

*/

#ifndef _tsip_h
#define _tsip_h

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

#define _GPS_VERSION 0x00 // software version of this library
#define _GPS_MPH_PER_KNOT 1.15077945
#define _GPS_MPS_PER_KNOT 0.51444444
#define _GPS_KMPH_PER_KNOT 1.852
#define _GPS_MILES_PER_METER 0.00062137112
#define _GPS_KM_PER_METER 0.001

#define MAX_DATA     1024			// report buffer size
#define MAX_COMMAND  64				// command buffer size

#define DLE		0x10
#define ETX		0x03

#define TRUE	1
#define FALSE	0

//match Trimble Documentation Datatypes to Arduino Datatypes
typedef unsigned char UINT8;
typedef char SINT8;
typedef int INT16;
typedef int SINT16;
typedef unsigned int UINT16;
typedef unsigned long UINT32;
typedef long INT32;
typedef long SINT32;
typedef float SINGLE;
//typedef double DOUBLE;	// not supported in Arduino library

// supported reports and super-reports
const byte REPORT_ECEF_POSITION				= 0x42;
const byte REPORT_ECEF_VELOCITY				= 0x43;
const byte REPORT_SW_VERSION				= 0x45;
const byte REPORT_SINGLE_POSITION			= 0x4a;
const byte REPORT_IO_OPTIONS				= 0x55;
const byte REPORT_ENU_VELOCITY				= 0x56;
const byte REPORT_SUPER						= 0x8f;
const byte REPORT_SUPER_PRIMARY_TIME		= 0xab;
const byte REPORT_SUPER_SECONDARY_TIME		= 0xac;

// supported commands and super-commands
const byte COMMAND_COLD_FACTORY_RESET		= 0x1e;
const byte COMMAND_REQUEST_SW_VERSION		= 0x1f;
const byte COMMAND_WARM_RESET_SELF_TEST		= 0x25; 
const byte COMMAND_SET_IO_OPTIONS			= 0x35;


/****************************/
/* Report packet structures */
/****************************/

// generic report TSIP packet
union _report_packet {
	struct _raw {
		byte data[MAX_DATA];
	} raw;
	struct _report {
		UINT8  code;
		UINT8  data[MAX_DATA-1];
	} report;
	struct _extended {
		UINT8  code;
		UINT8  subcode;
		UINT8  data[MAX_DATA-2];
	} extended;
};

// generic command TSIP packet
union _command_packet {
	struct _raw {
		byte data[MAX_COMMAND];
	} raw;
	struct _report {
		UINT8  code;
		UINT8  data[MAX_COMMAND-1];
	} report;
	struct _extended {
		UINT8  code;
		UINT8  subcode;
		UINT8  data[MAX_COMMAND-2];
	} extended;
};

// XYZ Earth Centered Earth Fixed (ECEF) Position Packet
struct _ecef_position {
	bool  valid;
	struct _0x42 {
		SINGLE x;				// X meters
		SINGLE y; 				// Y meters
		SINGLE z;				// Z meters
		SINGLE time_of_fix;		// time of fix in GPS or UTC seconds 
	} report;
};

// XYZ Earth Centered Earth Fixed (ECEF) Velocity Packet
struct _ecef_velocity {
	bool  valid;
	struct _0x43 {
		SINGLE x;				// X meters/second
		SINGLE y;				// Y meters/second
		SINGLE z;				// Z meters/second
		SINGLE bias_rate;		// meters/second
		SINGLE time_of_fix;		// time of fix in GPS or UTC seconds
	} report;
};

// Software Version Packet
struct _sw_version {
	bool  valid;
	struct _0x45 {
		UINT8 app_major;
		UINT8 app_minor;
		UINT8 app_month;
		UINT8 app_day;
		UINT8 app_year;			// year - 1900
		UINT8 gps_major;
		UINT8 gps_minor;
		UINT8 gps_month;
		UINT8 gps_day;
		UINT8 gps_year;			// year - 1900
	} report;
};

// Single precision LLA position fix
struct _single_position {
	bool  valid;
	struct _0x4A {
		SINGLE latitude;		// radians + north, - south
		SINGLE longitude;		// radians + east, - west
		SINGLE altitude;		// meters
		SINGLE clock_bias;		// meters relative to GPS
		SINGLE time_of_fix;		// seconds (GPS/UTC)
	} report;
};

// I/O options
struct _io_options {
	bool  valid;
	struct _report {
		union _position {
			UINT8 value;						// position options
			struct _bits {
				int ecef				: 1;
				int lla					: 1;
				int msl					: 1;
				int reserved_0			: 1;
				int double_precision	: 1;
				int reserved_1			: 3;
			} bits;
		} position;
		union _velocity {						// velocity options
			UINT8 value;   
			struct _bits {
				int ecef				: 1;
				int enu					: 1;
				int reserved			: 6;
			} bits;
		} velocity;
		union _timing {							// timing options
			UINT8 value;
			struct _bits {
				int utc					: 1;
				int reserved			: 7;
			} bits;
		} timing;
		union _auxiliary {						// auxiliary options
			UINT8 value;
			struct _bits {
				int packet_5A			: 1;
				int filtered_PRs		: 1;
				int reserved_0			: 1;
				int dbhz				: 1;
				int reserved_1			: 4;
			} bits;
		} auxiliary;
	} report;
};

// Single precision East-North-Up (ENU) velocity fix
struct _enu_velocity {
	bool  valid;
	struct _0x56 {
		SINGLE east;			// m/s + east, - west
		SINGLE north;			// m/s + north, - south
		SINGLE up;				// m/s + up, - down
		SINGLE clock_bias;		// meters/second
		SINGLE time_of_fix;		// seconds (GPS/UTC) 
	} report;
};

//8F-AB Primary Timing Packet
struct _primary_time {
	bool  valid;
	struct _0x8FAB {
		UINT32  seconds_of_week; // GPS seconds since GPS Sunday 00:00:00 
		UINT16  week_number;	// GPS week number
		SINT16  utc_offset;		// GPS-UTC seconds difference
		union _flags {
			UINT8 value;
			struct _bits {
				int  utc_time		: 1;	// UTC/GPS time
				int  utc_pps		: 1;	// UTC/GPS PPS
				int  time_not_set	: 1;	// time set/not set
				int  no_utc_info	: 1;	// have/do not have UTC info 
				int  test_mode_time	: 1;	// time from GPS/test mode time
				int  unused			: 4;	// unused bits 
			} bits;
		} flags;				// timing flags
		UINT8   seconds;		// 0-59  (UTC/GPS flags bit0 = 1/0)
		UINT8   minutes;		// 0-59
		UINT8   hours;			// 0-23
		UINT8   day;			// 1-31
		UINT8   month;			// 1-12
		UINT16  year;			// four digit year
	} report;
};

//8F-AC Secondary Timing Packet
struct _secondary_time {
	bool  valid;
	struct _0x8FAC {
		UINT8  receiver_mode;
			#define RECEIVE_MODE_AUTO_2D_3D					0
			#define RECEIVE_MODE_SINGLE_STATELLITE			1
			#define RECEIVE_MODE_HORIZONTAL_2D				3
			#define RECEIVE_MODE_FULL_POSITION_2D			4
			#define RECEIVE_MODE_DGPS_REFERENCE				5
			#define RECEIVE_MODE_CLOCK_HOLD					6
			#define RECEIVE_MODE_OVERDETERMINDE_CLOCK		7
		UINT8  disciplining_mode;
			#define DISCIPLINING_MODE_NORMAL				0
			#define DISCIPLINING_MODE_POWER_UP				1
			#define DISCIPLINING_MODE_AUTO_HOLDOVER			2
			#define DISCIPLINING_MODE_MANUAL_HOLDOVER		3
			#define DISCIPLINING_MODE_RECOVERY				4
			#define DISCIPLINING_MODE_NOT_USED				5
			#define DISCIPLINING_MODE_DISCIPLINING_DISABLED	6
		UINT8   self_survey_progress;	// 0-100%
		UINT32  holdover_duration;		// seconds
		union _critical_alarms {
			UINT16 value;
			struct _bits {
				int rom_checksum_error			: 1;
				int ram_check_failed			: 1;
				int power_supply_failure		: 1;
				int fpga_check_failed			: 1;
				int control_voltage_at_rail		: 1;
				int unused						: 11;
			} bits;
		} critical_alarms;
		union _minor_alarms {
			UINT16 value;
			struct _bits {
				int control_voltage_near_rail	: 1;
				int antenna_open				: 1;
				int antenna_shorted				: 1;
				int not_tracking_satellites		: 1;
				int oscillator_not_disciplined	: 1;
				int self_survey_in_progress		: 1;
				int no_accurate_stored_position	: 1;
				int leap_second_pending			: 1;
				int in_test_mode				: 1;
				int inaccurate_position			: 1;
				int eeprom_segment_corrupt		: 1;
				int almanac_not_current			: 1;
				int unused						: 4;
			} bits;
		} minor_alarms;
		UINT8   gps_decoding_status;
			#define GPS_DECODING_STATUS_DOING_FIXES			0
			#define GPS_DECODING_STATUS_NO_GPS_TIME			1
			#define GPS_DECODING_STATUS_PDOP_TOO_HIGH		3
			#define GPS_DECODING_STATUS_NO_SATELLITES		8
			#define GPS_DECODING_STATUS_ONE_SATELLITE		9
			#define GPS_DECODING_STATUS_TWO_SATELLITES		0x0A
			#define GPS_DECODING_STATUS_THREE_SATELLITES	0x0B
			#define GPS_DECODING_STATUS_CHOSEN_SAT_UNUSABLE	0x0C
			#define GPS_DECODING_STATUS_TRAIM_REJECTED_FIX	0x10
		UINT8   disciplining_activity;
			#define DISCIPLINING_ACTIVITY_PHASE_LOCKING		0
			#define DISCIPLINING_ACTIVITY_OSC_WARMING_UP	1
			#define DISCIPLINING_ACTIVITY_FREQUENCY_LOCKING	2
			#define DISCIPLINING_ACTIVITY_PLACING_PPS		3
			#define DISCIPLINING_ACTIVITY_INIT_LOOP_FILTER	4
			#define DISCIPLINING_ACTIVITY_COMPENSATING_OCXO	5
			#define DISCIPLINING_ACTIVITY_INACTIVE			6
			#define DISCIPLINING_ACTIVITY_NOT_USED			7
			#define DISCIPLINING_ACTIVITY_RECOVERY_MODE		8
		UINT8   spare_status1;
		UINT8   spare_status2;
		SINGLE  pps_offset;				// estimate of UTC/GPS offset (ns) 
		SINGLE  tenMHz_offset;			// estimate of UTC/GPS offset (ppb)
		UINT32  dac_value;
		SINGLE  dac_voltage;			// voltage
		SINGLE  temperature;			// degress C
		UINT8   latitude[8];			// DOUBLE radians
		UINT8   longitude[8];			// DOUBLE radians
		UINT8   altitude[8];			// DOUBLE meters
		UINT8   spare[8];
	} report;
};

// unknown report packet
struct _unknown {
	bool  valid;
	union _report_packet report;
};


// Trimble Standard Interface Protocol (TSIP) class
class Tsip
{
	public:
		Tsip(void);
		//~Tsip();
		int encode(byte c);			// encode byte stream into packets

		// received reports
		struct _ecef_position		m_ecef_position;
		struct _ecef_velocity		m_ecef_velocity;
		struct _sw_version			m_sw_version;
		struct _single_position		m_single_position;
		struct _io_options			m_io_options;
		struct _enu_velocity		m_enu_velocity;
		struct _primary_time		m_primary_time;
		struct _secondary_time		m_secondary_time;
		struct _unknown				m_unknown;

		// report updated flags
		// there is an update bit per implemented report
		union _update_report {
			int value;
			struct _bits {
				int ecef_position   : 1;
				int ecef_velocity   : 1;
				int sw_version      : 1;
				int single_position : 1;
				int io_options      : 1;
				int enu_velocity    : 1;
				int primary_time    : 1;
				int secondary_time  : 1;
				int unused          : 7;	// future expansion
				int unknown			: 1;	// unknown report
			} report;
		} m_updated;

		// TSIP command packet buffer
		union _command_packet m_command;

		// TSIP report packet buffer
		union _report_packet  m_report;
		int   m_report_length;

	private:
		int update_report(void);		// update report with packet data

		// packet decoder states
		enum t_state {
			START=1,
			FRAME,
			DATA,
			DATA_DLE
		} m_state;
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
