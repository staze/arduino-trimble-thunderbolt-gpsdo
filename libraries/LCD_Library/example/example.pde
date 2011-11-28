//************************************************************************
//					Nokia Shield
//************************************************************************
//* Derived from code by James P. Lynch and Mark Sproul.	
//*
//*Edit History
//*		<MLS>	= Mark Sproul, msproul -at- jove.rutgers.edu
//*             <PD>   = Peter Davenport, electrifiedpete -at- gmail.com
//************************************************************************
//*	Apr  2,	2010	<MLS> I received my Color LCD Shield sku: LCD-09363 from sparkfun.
//*	Apr  2,	2010	<MLS> The code was written for WinAVR, I modified it to compile under Arduino.
//*     Aug  7, 2010    <PD> Organized code and removed unneccesary elements.
//*     Aug 23, 2010    <PD> Added LCDSetLine, LCDSetRect, and LCDPutStr.
//*     Oct 31, 2010    <PD> Added circle code from Carl Seutter and added contrast code.
//************************************************************************
//    External Component Libs
#include "LCD_driver.h"
//#include "nokia_tester.h"
//    Included files
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "WProgram.h"
#include "HardwareSerial.h"

//************************************************************************
//					Main Code
//************************************************************************
void	setup()
{
  	ioinit();           //Initialize I/O
	LCDInit();	    //Initialize the LCD
        LCDContrast(40);
        LCDClear(BLACK);    // Clear LCD to a solid color

        // LARGE
        LCDPutStr("XXXXXXXXXXXXXXXX", 2, 1, LARGE, GREEN, BLACK); // Write instructions on display
        for (int x=18; x < 118; x += 16)
        {
            LCDPutChar(0x58, x, 1, LARGE, GREEN, BLACK);
            LCDPutChar(0x58, x, 121, LARGE, GREEN, BLACK);
        }
        LCDPutStr("XXXXXXXXXXXXXXXX", 116, 1, LARGE, GREEN, BLACK); // Write instructions on display

        // MEDIUM
        LCDPutStr("XXXXXXXXXXXXXX", 14, 9, MEDIUM, RED, BLUE); // Write instructions on display
        LCDPutStr("XXXXXXXXXXXXXX", 22, 9, MEDIUM, RED, BLUE); // Write instructions on display
        for (int x=30; x < 99; x += 8)
        {
            LCDPutStr("XX", x, 9, MEDIUM, RED, BLUE);
            LCDPutStr("XX", x, 105, MEDIUM, RED, BLUE);
        }
        LCDPutStr("XXXXXXXXXXXXXX", 102, 9, MEDIUM, RED, BLUE); // Write instructions on display
        LCDPutStr("XXXXXXXXXXXXXX", 110, 9, MEDIUM, RED, BLUE); // Write instructions on display

        // SMALL
        LCDPutStr("This library", 32, 26, SMALL, WHITE, BLACK);
        LCDPutStr("is for world", 40, 26, SMALL, BLUE, BLACK);
        LCDPutStr("domination!", 48, 26, SMALL, RED, BLACK);
        LCDPutStr("and other", 56, 26, SMALL, GREEN, BLACK);
        LCDPutStr("fun stuff.", 64, 26, SMALL, YELLOW, BLACK);
        LCDPutStr("The quick dog", 72, 26, SMALL, WHITE, BLACK);
        LCDPutStr("jumps over a", 80, 26, SMALL, WHITE, BLACK);
        LCDPutStr("brown lazy", 88, 26, SMALL, WHITE, BLACK);
        LCDPutStr("fox.", 96, 26, SMALL, WHITE, BLACK);
}

int num = 1;

//************************************************************************
//					Loop
//************************************************************************
void	loop()
{
int	s1, s2, s3;
	s1	=	!digitalRead(kSwitch1_PIN);
	s2	=	!digitalRead(kSwitch2_PIN);
	s3	=	!digitalRead(kSwitch3_PIN);

	if (s1){
    LCDClear(WHITE);    // Clear LCD to WHITE
    LCDPutStr("Lines!", 0, 4, LARGE, ORANGE, WHITE); // Write information on display
    LCDSetLine(45, 5, 21, 130, BLACK); // Write a bunch of lines
    LCDSetLine(46, 5, 22, 130, RED);
    LCDSetLine(47, 5, 23, 130, GREEN);
    LCDSetLine(48, 5, 24, 130, BLUE);
    LCDSetLine(49, 5, 25, 130, CYAN);
    LCDSetLine(50, 5, 26, 130, MAGENTA);
    LCDSetLine(51, 5, 27, 130, YELLOW);
    LCDSetLine(52, 5, 28, 130, BROWN);
    LCDSetLine(53, 5, 29, 130, ORANGE);
    LCDSetLine(54, 5, 30, 130, PINK);
    
    LCDPutStr("LCDSetLine(x0,", 60, 4, LARGE, BLUE, WHITE); // Write code information on display
    LCDPutStr("y0, x1, y1,", 76, 4, LARGE, BLUE, WHITE);
    LCDPutStr("color);", 92, 4, LARGE, BLUE, WHITE);
    
    LCDSetLine(50, 50, 130, 130, PINK);
    LCDSetLine(80, 50, 50, 130, BROWN);
    LCDSetLine(110, 12, 30, 80, BROWN);
	}

	else if (s2){
    LCDClear(WHITE);    // Clear LCD to WHITE
    LCDPutStr("Rectangles!", 0, 4, LARGE, ORANGE, WHITE); // Write information on display
    LCDSetRect(20, 20, 50, 60, 1, YELLOW); // filled rectangle
    LCDSetRect(20, 20, 50, 60, 0, MAGENTA);// Line around filled rectangle
    LCDSetRect(60, 40, 80, 60, 0, GREEN);// Unfilled rectangle
    LCDSetRect(60, 70, 80, 120, 0, BLUE);// Unfilled rectangle number 2
    LCDSetRect(20, 70, 50, 120, 0, BLUE);// Unfilled rectangle number 3
    LCDPutStr("LCDSetRect(x0,", 78, 4, LARGE, BLUE, WHITE); // Write information on display
    LCDPutStr("y0, x1, y1, ", 94, 4, LARGE, BLUE, WHITE); // Write information on display
    LCDPutStr("fill, color);", 110, 4, LARGE, BLUE, WHITE); // Write information on display
  //  LCDSetRect(x0, y0, x1, y1, fill, color);
	}

	else if (s3){
    LCDClear(WHITE);    // Clear LCD to WHITE
    LCDPutStr("Setting pixels!", 0, 4, LARGE, ORANGE, WHITE); // Write information on display
    LCDSetPixel(BLACK, 30, 10);
     LCDSetPixel(BLACK, 30, 11);
    LCDSetPixel(RED, 30, 20);
      LCDSetPixel(RED, 30, 21);
    LCDSetPixel(GREEN, 30, 30);
     LCDSetPixel(GREEN, 30, 31);
    LCDSetPixel(BLUE, 30, 40);
      LCDSetPixel(BLUE, 30, 41);
    LCDSetPixel(CYAN, 30, 50);
     LCDSetPixel(CYAN, 30, 51);
    LCDSetPixel(MAGENTA, 30, 60);
     LCDSetPixel(MAGENTA, 30, 61);
    LCDSetPixel(YELLOW, 30, 70);
       LCDSetPixel(YELLOW, 30, 71);
    LCDSetPixel(BROWN, 30, 80);
      LCDSetPixel(BROWN, 30, 81);
    LCDSetPixel(ORANGE, 30, 90);
     LCDSetPixel(ORANGE, 30, 91);
    LCDSetPixel(PINK, 30, 100);
      LCDSetPixel(PINK, 30, 101);
      LCDPutStr("LCDSetPixel", 40, 4, LARGE, BLUE, WHITE);
      LCDPutStr("(color, x, y);", 56, 4, LARGE, BLUE, WHITE);
      LCDPutStr("& circles.", 88, 4, LARGE, BLUE, WHITE);
      LCDDrawCircle (55, 55, 30, RED, FULLCIRCLE); // draw a circle
      LCDDrawCircle (70, 70, 20, BLUE, FULLCIRCLE); // draw a circle
      LCDDrawCircle (85, 85, 10, GREEN, FULLCIRCLE); // draw a circle
	}
  s1 = 0;
  s2 = 0;
  s3 = 0;
  delay(200);

}
