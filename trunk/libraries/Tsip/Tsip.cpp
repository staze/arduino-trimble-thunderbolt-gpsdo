/*
  tsip.cpp - class implementation for the simplified Trimble Standard
             Interface Protocol (TSIP) library for the Arduino platform.
             
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

#include "Tsip.h"


Tsip::Tsip()
{
	m_report_length = 0;
	m_state = START;
	m_updated.value = 0;
	m_ecef_position.valid = false;
	m_ecef_velocity.valid = false;
	m_sw_version.valid = false;
	m_single_position.valid = false;
	m_io_options.valid = false;
	m_enu_velocity.valid = false;
	m_primary_time.valid = false;
	m_secondary_time.valid = false;
	m_unknown.valid = false;
}


/** encode byte stream into TSIP packets
*
*   The TSIP packet encode state machine that collects a stream
*   of bytes into TSIP packets.Encodes byte steam from a serial
*   byte stream from a Trimble Thunderbolt GPSDO into TSIP
*   packets. When a packet has been completed the corresponding
*   report is updated.
*/
int Tsip::encode(byte c)
{
	switch (m_state) {
 
	case START:
		// search for start
		if (c == DLE)
			m_state = FRAME;
		break;

	case FRAME:
		// check if mis-framed
		if (c == DLE || c == ETX)
			m_state = START;
		else {
			m_state = DATA;
			m_report_length = 0;
			m_report.raw.data[m_report_length++] = c;
		}
		break;

	case DATA:
		// found data DLE
		if (c == DLE) {
			m_state = DATA_DLE;
		}
		// add byte to report packet
		else if (m_report_length < MAX_DATA) {
			m_report.raw.data[m_report_length++] = c;
		}
		break;

	case DATA_DLE:
		// escaped data 
		if (c == DLE) {
			m_state = DATA;
			if (m_report_length < MAX_DATA) {
				m_report.raw.data[m_report_length++] = c;
			}
		}
		// end of frame
		else if (c == ETX) {
			m_state = START;
			return update_report();    //Whoohoo the moment we've been waitin for
		}
		// mis-framed
		else
			m_state = START;
		break;

	default:
		m_state = START;
		break;
	}

	return 0;
}


/** update received report
*
*   Update received report's property buffer.
*
*   Returns: m_updated to indicate which report was received.
*/
int Tsip::update_report()
{
	void *src;
	void *dst;
	int  rlen = 0;

	// save report
	switch (m_report.report.code) {

	case REPORT_ECEF_POSITION:
		m_updated.report.ecef_position = 1;
		m_ecef_position.valid = true;
		src = m_report.report.data;
		dst = &m_ecef_position.report;
		rlen = sizeof(m_ecef_position.report);
		break;

	case REPORT_ECEF_VELOCITY:
		m_updated.report.ecef_velocity = 1;
		m_ecef_velocity.valid = true;
		src = m_report.report.data;
		dst = &m_ecef_position.report;
		rlen = sizeof(m_ecef_velocity.report);
		break;

	case REPORT_SW_VERSION:
		m_updated.report.sw_version = 1;
		m_sw_version.valid = true;
		src = m_report.report.data;
		dst = &m_sw_version.report;
		rlen = sizeof(m_sw_version.report);
		break;

	case REPORT_SINGLE_POSITION:
		m_updated.report.single_position = 1;
		m_single_position.valid = true;
		src = m_report.report.data;
		dst = &m_single_position.report;
		rlen = sizeof(m_single_position.report);
		break;

	case REPORT_IO_OPTIONS:
		m_updated.report.io_options = 1;
		m_io_options.valid = true;
		src = m_report.report.data;
		dst = &m_io_options.report;
		rlen = sizeof(m_io_options.report);
		break;

	case REPORT_ENU_VELOCITY:
		m_updated.report.enu_velocity = 1;
		m_enu_velocity.valid = true;
		src = m_report.report.data;
		dst = &m_enu_velocity.report;
		rlen = sizeof(m_enu_velocity.report);
		break;

	case REPORT_SUPER:
		switch (m_report.extended.subcode) {

		case REPORT_SUPER_PRIMARY_TIME:
			m_updated.report.primary_time = 1;
			m_primary_time.valid = true;
			src = m_report.extended.data;
			dst = &m_primary_time.report;
			rlen = sizeof(m_primary_time.report);
			break;

		case REPORT_SUPER_SECONDARY_TIME:
			m_updated.report.secondary_time = 1;
			m_secondary_time.valid = true;
			src = m_report.extended.data;
			dst = &m_secondary_time.report;
			rlen = sizeof(m_secondary_time.report);
			break;

		default:
			m_updated.report.unknown = 1;
			m_unknown.valid = true;
		    src = m_report.raw.data;
			dst = m_unknown.report.raw.data;
			rlen = sizeof(m_unknown.report.raw.data);
			break;
		}

	default:
		m_updated.report.unknown = 1;
		m_unknown.valid = true;
	    src = m_report.raw.data;
		dst = m_unknown.report.raw.data;
		rlen = sizeof(m_unknown.report.raw.data);
        break;
	}

	// update the report
	if (rlen > 0) {
		memcpy(dst, src, min(m_report_length, rlen));
		return m_updated.value;
	}

	return 0;
}


