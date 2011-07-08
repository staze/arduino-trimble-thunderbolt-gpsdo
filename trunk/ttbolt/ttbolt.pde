/*
* Arduino based Thunderbolt GPSDO Status Display
*
* This application provides a display of basic
* status information for a Trimble ThunderBolt GPS
* Disciplined Oscillator (GPSDO).  Limited control
* of the GPSDO is also provided with the ability to
* initiate a cold or warm reset as well as self
* servey.
*
* Copyright (c) 2011 Brett Howard (N7MG)
* Copyright (c) 2011 Andrew Stern (N7UL)
*
* GOAL:
* I started my frequency reference project when I was a
* field tester for the Elecraft K3EXTREF accessory.  I
* liked the idea of having good frequency stability and
* having a reference that would also automatically and
* regularly calibrate my benchtop function generator.
* 
* This sketch is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "WProgram.h"

#include <Tsip.h>
#include <LCD_driver.h>
//#include <SoftwareSerial.h>

#define LINE_HEIGHT 16
#define gpsTX 6
#define gpsRX 7

// use bit-banged serial port for GPS for debug on hardware port
//SoftwareSerial gpsio(gpsRX,gpsTX);

// initialize TSIP state machine
Tsip tsip;


void setup()
{
	//char buff[17];

	//gpsio.begin(9600);
	ioinit();

	LCDInit();
	LCDContrast(0);  //can be set from 0 to 63
	LCDClear(WHITE);

	LCDPutStr("< N7MG GPSDO >",LINE_HEIGHT*1,(128-(8*14))/2,1,BLUE,WHITE);
	LCDPutStr("05:44:05 - 8Sats",LINE_HEIGHT*2,3,1,BLUE,WHITE);
	LCDPutStr("156 ppt | 116 nS",LINE_HEIGHT*3,(128-(16*8))/2,0,BLUE,WHITE);
	LCDPutStr("Phase Locked",LINE_HEIGHT*4,(128-(12*8))/2,0,BLUE,WHITE);
	LCDPutStr("GPSD Mode: Norm",LINE_HEIGHT*5,8,3,BLUE,WHITE);
	LCDPutStr("OverDet Clk",LINE_HEIGHT*6,8,3,BLUE,WHITE);
	LCDPutStr("No Errors",LINE_HEIGHT*7,(128-(9*8))/2,0,BLUE,WHITE);
	LCDPutStr("Hi Andrew",LINE_HEIGHT*8,(128-(9*8))/2,0,BLUE,WHITE);

}


void loop()
{
	int status;

	status = 0;
	//if(gpsio.available())
	//	status = tsip.encode(gpsio.read());
	//else
	//	status = 0;

}

