
//************************************************************************
//*
//*				LCD_driver.c: Interface for Nokia LCD
//*
//************************************************************************
//*	Edit History
//*		<MLS>	= Mark Sproul msproul -at- jove.rutgers.edu
//************************************************************************
//*	Apr  2,	2010	<MLS> I received my Color LCD Shield sku: LCD-09363 from sparkfun
//*	Apr  2,	2010	<MLS> The code was written for WinAVR, I modified it to compile under Arduino
//*	Apr  3,	2010	<MLS> Changed LCDSetPixel to make it "RIGHT" side up
//*	Apr  3,	2010	<MLS> Made LCDSetPixel public
//*	Apr  3,	2010	<MLS> Working on MEGA, pin defs in nokia_tester.h
//*	Apr  4,	2010	<MLS> Removed delays from LCDCommand & LCDCommand, increased speed by 75%
//************************************************************************
#include "LCD_driver.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>


#ifdef _USE_ARDUINO_FOR_NOKIEA_
    #include	"WProgram.h"
#endif

//************************************************************************
//Usage: LCDClear(black);
//Inputs: char color: 8-bit color to be sent to the screen.
//Outputs: None
//Description: This function will clear the screen with "color" by writing the
//				color to each location in the RAM of the LCD.
//************************************************************************
void LCDClear(int color)
{
    unsigned int i;

#ifdef EPSON
    LCDCommand(PASET);
    LCDData(0);
    LCDData(131);

    LCDCommand(CASET);
    LCDData(0);
    LCDData(131);

    LCDCommand(RAMWR);
#endif
#ifdef	PHILLIPS
    LCDCommand(PASETP);
    LCDData(0);
    LCDData(131);

    LCDCommand(CASETP);
    LCDData(0);
    LCDData(131);

    LCDCommand(RAMWRP);
#endif

    for (i=0; i < (131*131)/2; i++)
    {
        LCDData((color >> 4) & 0x00FF);
        LCDData(((color & 0x0F) << 4) | (color >> 8));
        LCDData(color & 0x0FF);     // nop(EPSON)
    }

//	x_offset = 0;
//	y_offset = 0;
}


//************************************************************************
void    delay_us(int microSecs)
{
    delayMicroseconds(microSecs);
}

//************************************************************************
void    delay_ms(int millisecs)
{
//	delay(millisecs);
//	delayMicroseconds(millisecs * 1000);
}


//************************************************************************
//Usage: LCDCommand(RAMWR);
//Inputs: char data - character command to be sent to the LCD
//Outputs: None
//************************************************************************
void LCDCommand(unsigned char data)
{
    char jj;
    cbi(LCD_PORT_CS, CS);       // enable chip, p0.20 goes low
    //delay_us(1);
    cbi(LCD_PORT_DIO, DIO);     // output low on data out (9th bit low = command), p0.19
    //delay_us(1);

    cbi(LCD_PORT_SCK, SCK);     // send clock pulse
    delay_us(1);
    sbi(LCD_PORT_SCK, SCK);
    //delay_us(1);

    for (jj = 0; jj < 8; jj++)
    {
        if ((data & 0x80) == 0x80)
        {
            sbi(LCD_PORT_DIO, DIO);
        }
        else
        {
            cbi(LCD_PORT_DIO, DIO);
        }
        //delay_us(1);

        cbi(LCD_PORT_SCK, SCK);     // send clock pulse
        //+	delay_us(1);
        sbi(LCD_PORT_SCK, SCK);

        data <<= 1;
    }

    sbi(LCD_PORT_CS, CS);           // disable
}

//************************************************************************
//Usage: LCDData(RAMWR);
//Inputs: char data - character data to be sent to the LCD
//Outputs: None
//************************************************************************
void LCDData(unsigned char data)
{
    char j;

    cbi(LCD_PORT_CS, CS);           // enable chip, p0.20 goes low
    //delay_us(1);
    sbi(LCD_PORT_DIO, DIO);         // output high on data out (9th bit high = data), p0.19
    //delay_us(1);

    cbi(LCD_PORT_SCK, SCK);         // send clock pulse
//+	delay_us(1);
    sbi(LCD_PORT_SCK, SCK);         // send clock pulse
    //delay_us(1);

    for (j = 0; j < 8; j++)
    {
        if ((data & 0x80) == 0x80)
        {
            sbi(LCD_PORT_DIO, DIO);
        }
        else
        {
            cbi(LCD_PORT_DIO, DIO);
        }
        //delay_us(1);

        cbi(LCD_PORT_SCK, SCK);     // send clock pulse
//+		delay_us(1);
        sbi(LCD_PORT_SCK, SCK);

        data <<= 1;
    }

    LCD_PORT_CS |=  (1<<CS);        // disable
}

//************************************************************************
//Usage: LCDInit();
//Inputs: None
//Outputs: None
//Description:  Initializes the LCD regardless of if the controlller is an EPSON or PHILLIPS.
//************************************************************************
void LCDInit(void)
{
    delay_ms(200);

    cbi(LCD_PORT_SCK, SCK);         //output_low (SPI_CLK);//output_low (SPI_DO);
    cbi(LCD_PORT_DIO, DIO);
    delay_us(10);
    sbi(LCD_PORT_CS, CS);           //output_high (LCD_CS);
    delay_us(10);
    cbi(LCD_PORT_RES, LCD_RES);     //output_low (LCD_RESET);
    delay_ms(200);
    sbi(LCD_PORT_RES, LCD_RES);     //output_high (LCD_RESET);
    delay_ms(200);
    sbi(LCD_PORT_SCK, SCK);
    sbi(LCD_PORT_DIO, DIO);
    delay_us(10);
#if 0
    LCDCommand(DISCTL);     // display control(EPSON)
    LCDData(0x0C);          // 12 = 1100 - CL dividing ratio [don't divide] switching period 8H (default)
    LCDData(0x20);  
    //LCDData(0x02);
    LCDData(0x00);

    LCDData(0x01);

    LCDCommand(COMSCN);     // common scanning direction(EPSON)
    LCDData(0x01);

    LCDCommand(OSCON);      // internal oscialltor ON(EPSON)
#endif
    LCDCommand(SLPOUT);     // sleep out(EPSON)
    LCDCommand(P_SLEEPOUT); //sleep out(PHILLIPS)

    LCDCommand(PWRCTR);     // power ctrl(EPSON)
    LCDData(0x0F);          //everything on, no external reference resistors
    LCDCommand(P_BSTRON);   //Booset On(PHILLIPS)

    LCDCommand(DISINV);     // invert display mode(EPSON)
    LCDCommand(P_INVON);    // invert display mode(PHILLIPS)

//    LCDCommand(DATCTL);     // data control(EPSON)
//    LCDData(0x03);          // correct for normal sin7
//    LCDData(0x00);          // normal RGB arrangement
    //LCDData(0x01);		// 8-bit Grayscale
//    LCDData(0x02);          // 16-bit Grayscale Type A

    LCDCommand(P_MADCTL);   //Memory Access Control(PHILLIPS)
    LCDData(0xC8);

    LCDCommand(P_COLMOD);   // Set Color Mode(PHILLIPS)
    LCDData(0x02);  

//    LCDCommand(VOLCTR);     // electronic volume, this is the contrast/brightness(EPSON)
    //LCDData(0x18);		// volume (contrast) setting - fine tuning, original
//    LCDData(0x24);          // volume (contrast) setting - fine tuning, original
//    LCDData(0x03);          // internal resistor ratio - coarse adjustment
    LCDCommand(P_SETCON);   // Set Contrast(PHILLIPS)
    LCDData(0x30);  

//    LCDCommand(NOP);        // nop(EPSON)
    LCDCommand(P_NOP);      // nop(PHILLIPS)

    delay_ms(200);

    LCDCommand(DISON);      // display on(EPSON)
    LCDCommand(P_DISPON);   // display on(PHILLIPS)
}


//************************************************************************
//Usage: LCDSetPixel(white, 0, 0);
//Inputs: unsigned char color - desired color of the pixel
//			unsigned char x - Page address of pixel to be colored
//			unsigned char y - column address of pixel to be colored
//Outputs: None
//Description: Sets the starting page(row) and column (x & y) coordinates in ram,
//				then writes the colour to display memory.	The ending x & y are left
//				maxed out so one can continue sending colour data bytes to the 'open'
//				RAMWR command to fill further memory.	issuing any red command
//				finishes RAMWR.
//**NOTE** Because this function is static, it is essentially a "private" function
//		and can only be used within this file!
//*	Apr  3,	2010	<MLS> Made LCDSetPixel public
//************************************************************************
void LCDSetPixel(int color, unsigned char x, unsigned char y)
{
    // swap x and swap y directions
//    y = (COL_HEIGHT - 1) - y;
//    x = (ROW_LENGTH - 1) - x;

#ifdef EPSON
    LCDCommand(PASET);  // page start/end ram
    LCDData(x);
    LCDData(ENDPAGE);

    LCDCommand(CASET);  // column start/end ram
    LCDData(y);
    LCDData(ENDCOL);

    LCDCommand(RAMWR);  // write
    LCDData((color>>4)&0x00FF);
    LCDData(((color&0x0F)<<4)|(color>>8));
    LCDData(color&0x0FF);       // nop(EPSON)		
    //LCDData(color);
    //LCDData(NOP);
    //LCDData(NOP);
#endif
#ifdef	PHILLIPS
    LCDCommand(PASETP); // page start/end ram
    LCDData(x);
    LCDData(ENDPAGE);

    LCDCommand(CASETP); // column start/end ram
    LCDData(y);
    LCDData(ENDCOL);

    LCDCommand(RAMWRP); // write

    LCDData((unsigned char)((color >> 4) & 0x00FF));
    LCDData((unsigned char)(((color & 0x0F) << 4) | 0x00));
#endif

}

//************************************************************************
void ioinit(void)
{
    //*	setup the switches for input
    pinMode(kSwitch1_PIN, INPUT);
    pinMode(kSwitch2_PIN, INPUT);
    pinMode(kSwitch3_PIN, INPUT);

    //*	set the pull up resisters
    digitalWrite(kSwitch1_PIN, HIGH);
    digitalWrite(kSwitch2_PIN, HIGH);
    digitalWrite(kSwitch3_PIN, HIGH);


#ifdef _USE_ARDUINO_FOR_NOKIEA_

    //*	do the LCD control lines
    pinMode(CS_PIN,         OUTPUT);
    pinMode(DIO_PIN,        OUTPUT);
    pinMode(SCK_PIN,        OUTPUT);
    pinMode(LCD_RES_PIN,    OUTPUT);

#else
    DDRB    =   ((1<<CS)|(1<<DIO)|(1<<SCK)|(1<<LCD_RES));   //Set the control pins as outputs

    // USART Baud rate: 115200 (With 16 MHz Clock)
    UBRR0H  =   (MYUBRR >> 8) & 0x7F;   //Make sure highest bit(URSEL) is 0 indicating we are writing to UBRRH
    UBRR0L  =   MYUBRR;
    UCSR0A  =   (1<<U2X0);                  //Double the UART Speed
    UCSR0B  =   (1<<RXEN0)|(1<<TXEN0);      //Enable Rx and Tx in UART
    UCSR0C  =   (1<<UCSZ00)|(1<<UCSZ01);        //8-Bit Characters
//	stdout	=	&mystdout; //Required for printf init
    cli();

    // Arduino D3-5 are button pins
    // This is PD3-5
    DDRD    =   0x00;
    PORTD   =   0xFF;

    // Init timer 2
    //Set Prescaler to 8. (Timer Frequency set to 16Mhz)
    //Used for delay routines
    TCCR2B  =   (1<<CS20);  //Divde clock by 1 for 16 Mhz Timer 2 Frequency	
#endif
}

//************************************************************************
//***                        Set line                                  ***
//************************************************************************
void LCDSetLine(int x0, int y0, int x1, int y1, int color)
{
    int dy = y1 - y0;// Difference between y0 and y1
    int dx = x1 - x0;// Difference between x0 and x1
    int stepx, stepy;

    if (dy < 0)
    {
        dy = -dy; stepy = -1;
    }
    else
    {
        stepy = 1;
    }

    if (dx < 0)
    {
        dx = -dx; stepx = -1;
    }
    else
    {
        stepx = 1;
    }

    dy <<= 1;// dy is now 2*dy
    dx <<= 1;// dx is now 2*dx
    LCDSetPixel(color, x0, y0);

    if (dx > dy)
    {
        int fraction = dy - (dx >> 1);
        while (x0 != x1)
        {
            if (fraction >= 0)
            {
                y0 += stepy;
                fraction -= dx;
            }
            x0 += stepx;
            fraction += dy;
            LCDSetPixel(color, x0, y0);
        }
    }
    else
    {
        int fraction = dx - (dy >> 1);
        while (y0 != y1)
        {
            if (fraction >= 0)
            {
                x0 += stepx;
                fraction -= dy;
            }
            y0 += stepy;
            fraction += dx;
            LCDSetPixel(color, x0, y0);
        }
    }
}

//************************************************************************
//***                   Set rectangle                                  ***
//************************************************************************
void LCDSetRect(int x0, int y0, int x1, int y1, unsigned char fill, int color)
{
    // check if the rectangle is to be filled
    if (fill == 1)
    {
        int xDiff;

        if (x0 > x1)
        {
            //Find the difference between the x vars
            xDiff = x0 - x1;
        }
        else
        {
            xDiff = x1 - x0;
        }

        while (xDiff > 0)
        {
            LCDSetLine(x0, y0, x0, y1, color);
            if (x0 > x1)
            {
                x0--;
            }
            else
            {
                x0++;
            }
            xDiff--;
        }
    }
    else
    {
        // best way to draw an unfilled rectangle is to draw four lines
        LCDSetLine(x0, y0, x1, y0, color);
        LCDSetLine(x0, y1, x1, y1, color);
        LCDSetLine(x0, y0, x0, y1, color);
        LCDSetLine(x1, y0, x1, y1, color);
    }
}


// SMALL Font Table in program memory
const prog_uchar FONT6x8[97][8] PROGMEM = {
    0x06,0x08,0x08,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x20,0x20,0x20,0x20,0x20,0x00,0x20,0x00,
    0x50,0x50,0x50,0x00,0x00,0x00,0x00,0x00,
    0x50,0x50,0xF8,0x50,0xF8,0x50,0x50,0x00,
    0x20,0x78,0xA0,0x70,0x28,0xF0,0x20,0x00,
    0xC0,0xC8,0x10,0x20,0x40,0x98,0x18,0x00,
    0x40,0xA0,0xA0,0x40,0xA8,0x90,0x68,0x00,
    0x30,0x30,0x20,0x40,0x00,0x00,0x00,0x00,
    0x10,0x20,0x40,0x40,0x40,0x20,0x10,0x00,
    0x40,0x20,0x10,0x10,0x10,0x20,0x40,0x00,
    0x00,0x20,0xA8,0x70,0x70,0xA8,0x20,0x00,
    0x00,0x20,0x20,0xF8,0x20,0x20,0x00,0x00,
    0x00,0x00,0x00,0x00,0x30,0x30,0x20,0x40,
    0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,
    0x00,0x08,0x10,0x20,0x40,0x80,0x00,0x00,
    0x70,0x88,0x88,0xA8,0x88,0x88,0x70,0x00,
    0x20,0x60,0x20,0x20,0x20,0x20,0x70,0x00,
    0x70,0x88,0x08,0x70,0x80,0x80,0xF8,0x00,
    0xF8,0x08,0x10,0x30,0x08,0x88,0x70,0x00,
    0x10,0x30,0x50,0x90,0xF8,0x10,0x10,0x00,
    0xF8,0x80,0xF0,0x08,0x08,0x88,0x70,0x00,
    0x38,0x40,0x80,0xF0,0x88,0x88,0x70,0x00,
    0xF8,0x08,0x08,0x10,0x20,0x40,0x80,0x00,
    0x70,0x88,0x88,0x70,0x88,0x88,0x70,0x00,
    0x70,0x88,0x88,0x78,0x08,0x10,0xE0,0x00,
    0x00,0x00,0x20,0x00,0x20,0x00,0x00,0x00,
    0x00,0x00,0x20,0x00,0x20,0x20,0x40,0x00,
    0x08,0x10,0x20,0x40,0x20,0x10,0x08,0x00,
    0x00,0x00,0xF8,0x00,0xF8,0x00,0x00,0x00,
    0x40,0x20,0x10,0x08,0x10,0x20,0x40,0x00,
    0x70,0x88,0x08,0x30,0x20,0x00,0x20,0x00,
    0x70,0x88,0xA8,0xB8,0xB0,0x80,0x78,0x00,
    0x20,0x50,0x88,0x88,0xF8,0x88,0x88,0x00,
    0xF0,0x88,0x88,0xF0,0x88,0x88,0xF0,0x00,
    0x70,0x88,0x80,0x80,0x80,0x88,0x70,0x00,
    0xF0,0x88,0x88,0x88,0x88,0x88,0xF0,0x00,
    0xF8,0x80,0x80,0xF0,0x80,0x80,0xF8,0x00,
    0xF8,0x80,0x80,0xF0,0x80,0x80,0x80,0x00,
    0x78,0x88,0x80,0x80,0x98,0x88,0x78,0x00,
    0x88,0x88,0x88,0xF8,0x88,0x88,0x88,0x00,
    0x70,0x20,0x20,0x20,0x20,0x20,0x70,0x00,
    0x38,0x10,0x10,0x10,0x10,0x90,0x60,0x00,
    0x88,0x90,0xA0,0xC0,0xA0,0x90,0x88,0x00,
    0x80,0x80,0x80,0x80,0x80,0x80,0xF8,0x00,
    0x88,0xD8,0xA8,0xA8,0xA8,0x88,0x88,0x00,
    0x88,0x88,0xC8,0xA8,0x98,0x88,0x88,0x00,
    0x70,0x88,0x88,0x88,0x88,0x88,0x70,0x00,
    0xF0,0x88,0x88,0xF0,0x80,0x80,0x80,0x00,
    0x70,0x88,0x88,0x88,0xA8,0x90,0x68,0x00,
    0xF0,0x88,0x88,0xF0,0xA0,0x90,0x88,0x00,
    0x70,0x88,0x80,0x70,0x08,0x88,0x70,0x00,
    0xF8,0xA8,0x20,0x20,0x20,0x20,0x20,0x00,
    0x88,0x88,0x88,0x88,0x88,0x88,0x70,0x00,
    0x88,0x88,0x88,0x88,0x88,0x50,0x20,0x00,
    0x88,0x88,0x88,0xA8,0xA8,0xA8,0x50,0x00,
    0x88,0x88,0x50,0x20,0x50,0x88,0x88,0x00,
    0x88,0x88,0x50,0x20,0x20,0x20,0x20,0x00,
    0xF8,0x08,0x10,0x70,0x40,0x80,0xF8,0x00,
    0x78,0x40,0x40,0x40,0x40,0x40,0x78,0x00,
    0x00,0x80,0x40,0x20,0x10,0x08,0x00,0x00,
    0x78,0x08,0x08,0x08,0x08,0x08,0x78,0x00,
    0x20,0x50,0x88,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0xF8,0x00,
    0x60,0x60,0x20,0x10,0x00,0x00,0x00,0x00,
    0x00,0x00,0x60,0x10,0x70,0x90,0x78,0x00,
    0x80,0x80,0xB0,0xC8,0x88,0xC8,0xB0,0x00,
    0x00,0x00,0x70,0x88,0x80,0x88,0x70,0x00,
    0x08,0x08,0x68,0x98,0x88,0x98,0x68,0x00,
    0x00,0x00,0x70,0x88,0xF8,0x80,0x70,0x00,
    0x10,0x28,0x20,0x70,0x20,0x20,0x20,0x00,
    0x00,0x00,0x70,0x98,0x98,0x68,0x08,0x70,
    0x80,0x80,0xB0,0xC8,0x88,0x88,0x88,0x00,
    0x20,0x00,0x60,0x20,0x20,0x20,0x70,0x00,
    0x10,0x00,0x10,0x10,0x10,0x90,0x60,0x00,
    0x80,0x80,0x90,0xA0,0xC0,0xA0,0x90,0x00,
    0x60,0x20,0x20,0x20,0x20,0x20,0x70,0x00,
    0x00,0x00,0xD0,0xA8,0xA8,0xA8,0xA8,0x00,
    0x00,0x00,0xB0,0xC8,0x88,0x88,0x88,0x00,
    0x00,0x00,0x70,0x88,0x88,0x88,0x70,0x00,
    0x00,0x00,0xB0,0xC8,0xC8,0xB0,0x80,0x80,
    0x00,0x00,0x68,0x98,0x98,0x68,0x08,0x08,
    0x00,0x00,0xB0,0xC8,0x80,0x80,0x80,0x00,
    0x00,0x00,0x78,0x80,0x70,0x08,0xF0,0x00,
    0x20,0x20,0xF8,0x20,0x20,0x28,0x10,0x00,
    0x00,0x00,0x88,0x88,0x88,0x98,0x68,0x00,
    0x00,0x00,0x88,0x88,0x88,0x50,0x20,0x00,
    0x00,0x00,0x88,0x88,0xA8,0xA8,0x50,0x00,
    0x00,0x00,0x88,0x50,0x20,0x50,0x88,0x00,
    0x00,0x00,0x88,0x88,0x78,0x08,0x88,0x70,
    0x00,0x00,0xF8,0x10,0x20,0x40,0xF8,0x00,
    0x10,0x20,0x20,0x40,0x20,0x20,0x10,0x00,
    0x20,0x20,0x20,0x00,0x20,0x20,0x20,0x00,
    0x40,0x20,0x20,0x10,0x20,0x20,0x40,0x00,
    0x40,0xA8,0x10,0x00,0x00,0x00,0x00,0x00,
    0x70,0xD8,0xD8,0x70,0x00,0x00,0x00,0x00};


// MEDIUM Font Table in program memory
const prog_uchar FONT8x8[97][8] PROGMEM = {
    0x08,0x08,0x08,0x00,0x00,0x00,0x00,0x00, // columns, rows, num_bytes_per_char
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // space 0x20
    0x30,0x78,0x78,0x30,0x30,0x00,0x30,0x00, // !
    0x6C,0x6C,0x6C,0x00,0x00,0x00,0x00,0x00, // "
    0x6C,0x6C,0xFE,0x6C,0xFE,0x6C,0x6C,0x00, // #
    0x18,0x3E,0x60,0x3C,0x06,0x7C,0x18,0x00, // $
    0x00,0x63,0x66,0x0C,0x18,0x33,0x63,0x00, // %
    0x1C,0x36,0x1C,0x3B,0x6E,0x66,0x3B,0x00, // &
    0x30,0x30,0x60,0x00,0x00,0x00,0x00,0x00, // '
    0x0C,0x18,0x30,0x30,0x30,0x18,0x0C,0x00, // (
    0x30,0x18,0x0C,0x0C,0x0C,0x18,0x30,0x00, // )
    0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00, // *
    0x00,0x30,0x30,0xFC,0x30,0x30,0x00,0x00, // +
    0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x30, // ,
    0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00, // -
    0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00, // .
    0x03,0x06,0x0C,0x18,0x30,0x60,0x40,0x00, // / (forward slash)
    0x3E,0x63,0x63,0x6B,0x63,0x63,0x3E,0x00, // 0 0x30
    0x18,0x38,0x58,0x18,0x18,0x18,0x7E,0x00, // 1
    0x3C,0x66,0x06,0x1C,0x30,0x66,0x7E,0x00, // 2
    0x3C,0x66,0x06,0x1C,0x06,0x66,0x3C,0x00, // 3
    0x0E,0x1E,0x36,0x66,0x7F,0x06,0x0F,0x00, // 4
    0x7E,0x60,0x7C,0x06,0x06,0x66,0x3C,0x00, // 5
    0x1C,0x30,0x60,0x7C,0x66,0x66,0x3C,0x00, // 6
    0x7E,0x66,0x06,0x0C,0x18,0x18,0x18,0x00, // 7
    0x3C,0x66,0x66,0x3C,0x66,0x66,0x3C,0x00, // 8
    0x3C,0x66,0x66,0x3E,0x06,0x0C,0x38,0x00, // 9
    0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x00, // :
    0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x30, // ;
    0x0C,0x18,0x30,0x60,0x30,0x18,0x0C,0x00, // <
    0x00,0x00,0x7E,0x00,0x00,0x7E,0x00,0x00, // =
    0x30,0x18,0x0C,0x06,0x0C,0x18,0x30,0x00, // >
    0x3C,0x66,0x06,0x0C,0x18,0x00,0x18,0x00, // ?
    0x3E,0x63,0x6F,0x69,0x6F,0x60,0x3E,0x00, // @ 0x40
    0x18,0x3C,0x66,0x66,0x7E,0x66,0x66,0x00, // A
    0x7E,0x33,0x33,0x3E,0x33,0x33,0x7E,0x00, // B
    0x1E,0x33,0x60,0x60,0x60,0x33,0x1E,0x00, // C
    0x7C,0x36,0x33,0x33,0x33,0x36,0x7C,0x00, // D
    0x7F,0x31,0x34,0x3C,0x34,0x31,0x7F,0x00, // E
    0x7F,0x31,0x34,0x3C,0x34,0x30,0x78,0x00, // F
    0x1E,0x33,0x60,0x60,0x67,0x33,0x1F,0x00, // G
    0x66,0x66,0x66,0x7E,0x66,0x66,0x66,0x00, // H
    0x3C,0x18,0x18,0x18,0x18,0x18,0x3C,0x00, // I
    0x0F,0x06,0x06,0x06,0x66,0x66,0x3C,0x00, // J
    0x73,0x33,0x36,0x3C,0x36,0x33,0x73,0x00, // K
    0x78,0x30,0x30,0x30,0x31,0x33,0x7F,0x00, // L
    0x63,0x77,0x7F,0x7F,0x6B,0x63,0x63,0x00, // M
    0x63,0x73,0x7B,0x6F,0x67,0x63,0x63,0x00, // N
    0x3E,0x63,0x63,0x63,0x63,0x63,0x3E,0x00, // O
    0x7E,0x33,0x33,0x3E,0x30,0x30,0x78,0x00, // P 0x50
    0x3C,0x66,0x66,0x66,0x6E,0x3C,0x0E,0x00, // Q
    0x7E,0x33,0x33,0x3E,0x36,0x33,0x73,0x00, // R
    0x3C,0x66,0x30,0x18,0x0C,0x66,0x3C,0x00, // S
    0x7E,0x5A,0x18,0x18,0x18,0x18,0x3C,0x00, // T
    0x66,0x66,0x66,0x66,0x66,0x66,0x7E,0x00, // U
    0x66,0x66,0x66,0x66,0x66,0x3C,0x18,0x00, // V
    0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0x00, // W
    0x63,0x63,0x36,0x1C,0x1C,0x36,0x63,0x00, // X
    0x66,0x66,0x66,0x3C,0x18,0x18,0x3C,0x00, // Y
    0x7F,0x63,0x46,0x0C,0x19,0x33,0x7F,0x00, // Z
    0x3C,0x30,0x30,0x30,0x30,0x30,0x3C,0x00, // [
    0x60,0x30,0x18,0x0C,0x06,0x03,0x01,0x00, // \ (back slash)
    0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3C,0x00, // ]
    0x08,0x1C,0x36,0x63,0x00,0x00,0x00,0x00, // ^
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF, // _
    0x18,0x18,0x0C,0x00,0x00,0x00,0x00,0x00, // ` 0x60
    0x00,0x00,0x3C,0x06,0x3E,0x66,0x3B,0x00, // a
    0x70,0x30,0x3E,0x33,0x33,0x33,0x6E,0x00, // b
    0x00,0x00,0x3C,0x66,0x60,0x66,0x3C,0x00, // c
    0x0E,0x06,0x3E,0x66,0x66,0x66,0x3B,0x00, // d
    0x00,0x00,0x3C,0x66,0x7E,0x60,0x3C,0x00, // e
    0x1C,0x36,0x30,0x78,0x30,0x30,0x78,0x00, // f
    0x00,0x00,0x3B,0x66,0x66,0x3E,0x06,0x7C, // g
    0x70,0x30,0x36,0x3B,0x33,0x33,0x73,0x00, // h
    0x18,0x00,0x38,0x18,0x18,0x18,0x3C,0x00, // i
    0x06,0x00,0x06,0x06,0x06,0x66,0x66,0x3C, // j
    0x70,0x30,0x33,0x36,0x3C,0x36,0x73,0x00, // k
    0x38,0x18,0x18,0x18,0x18,0x18,0x3C,0x00, // l
    0x00,0x00,0x66,0x7F,0x7F,0x6B,0x63,0x00, // m
    0x00,0x00,0x7C,0x66,0x66,0x66,0x66,0x00, // n
    0x00,0x00,0x3C,0x66,0x66,0x66,0x3C,0x00, // o
    0x00,0x00,0x6E,0x33,0x33,0x3E,0x30,0x78, // p 0x70
    0x00,0x00,0x3B,0x66,0x66,0x3E,0x06,0x0F, // q
    0x00,0x00,0x6E,0x3B,0x33,0x30,0x78,0x00, // r
    0x00,0x00,0x3E,0x60,0x3C,0x06,0x7C,0x00, // s
    0x08,0x18,0x3E,0x18,0x18,0x1A,0x0C,0x00, // t
    0x00,0x00,0x66,0x66,0x66,0x66,0x3B,0x00, // u
    0x00,0x00,0x66,0x66,0x66,0x3C,0x18,0x00, // v
    0x00,0x00,0x63,0x6B,0x7F,0x7F,0x36,0x00, // w
    0x00,0x00,0x63,0x36,0x1C,0x36,0x63,0x00, // x
    0x00,0x00,0x66,0x66,0x66,0x3E,0x06,0x7C, // y
    0x00,0x00,0x7E,0x4C,0x18,0x32,0x7E,0x00, // z
    0x0E,0x18,0x18,0x70,0x18,0x18,0x0E,0x00, // {
    0x0C,0x0C,0x0C,0x00,0x0C,0x0C,0x0C,0x00, // |
    0x70,0x18,0x18,0x0E,0x18,0x18,0x70,0x00, // }
    0x3B,0x6E,0x00,0x00,0x00,0x00,0x00,0x00, // ~
    0x1C,0x36,0x36,0x1C,0x00,0x00,0x00,0x00}; // DEL


// LARGE Font Table in program memory
const prog_uchar FONT8x16[97][16] PROGMEM = {
    0x08,0x10,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x18,0x3C,0x3C,0x3C,0x18,0x18,0x18,0x00,0x18,0x18,0x00,0x00,0x00,0x00,
    0x00,0x63,0x63,0x63,0x22,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x36,0x36,0x7F,0x36,0x36,0x36,0x7F,0x36,0x36,0x00,0x00,0x00,0x00,
    0x0C,0x0C,0x3E,0x63,0x61,0x60,0x3E,0x03,0x03,0x43,0x63,0x3E,0x0C,0x0C,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x61,0x63,0x06,0x0C,0x18,0x33,0x63,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x1C,0x36,0x36,0x1C,0x3B,0x6E,0x66,0x66,0x3B,0x00,0x00,0x00,0x00,
    0x00,0x30,0x30,0x30,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x0C,0x18,0x18,0x30,0x30,0x30,0x30,0x18,0x18,0x0C,0x00,0x00,0x00,0x00,
    0x00,0x00,0x18,0x0C,0x0C,0x06,0x06,0x06,0x06,0x0C,0x0C,0x18,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x42,0x66,0x3C,0xFF,0x3C,0x66,0x42,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x18,0x18,0x18,0xFF,0x18,0x18,0x18,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x18,0x30,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x00,
    0x00,0x00,0x01,0x03,0x07,0x0E,0x1C,0x38,0x70,0xE0,0xC0,0x80,0x00,0x00,0x00,0x00,
    0x00,0x00,0x3E,0x63,0x63,0x63,0x6B,0x6B,0x63,0x63,0x63,0x3E,0x00,0x00,0x00,0x00,
    0x00,0x00,0x0C,0x1C,0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3F,0x00,0x00,0x00,0x00,
    0x00,0x00,0x3E,0x63,0x03,0x06,0x0C,0x18,0x30,0x61,0x63,0x7F,0x00,0x00,0x00,0x00,
    0x00,0x00,0x3E,0x63,0x03,0x03,0x1E,0x03,0x03,0x03,0x63,0x3E,0x00,0x00,0x00,0x00,
    0x00,0x00,0x06,0x0E,0x1E,0x36,0x66,0x66,0x7F,0x06,0x06,0x0F,0x00,0x00,0x00,0x00,
    0x00,0x00,0x7F,0x60,0x60,0x60,0x7E,0x03,0x03,0x63,0x73,0x3E,0x00,0x00,0x00,0x00,
    0x00,0x00,0x1C,0x30,0x60,0x60,0x7E,0x63,0x63,0x63,0x63,0x3E,0x00,0x00,0x00,0x00,
    0x00,0x00,0x7F,0x63,0x03,0x06,0x06,0x0C,0x0C,0x18,0x18,0x18,0x00,0x00,0x00,0x00,
    0x00,0x00,0x3E,0x63,0x63,0x63,0x3E,0x63,0x63,0x63,0x63,0x3E,0x00,0x00,0x00,0x00,
    0x00,0x00,0x3E,0x63,0x63,0x63,0x63,0x3F,0x03,0x03,0x06,0x3C,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x18,0x18,0x18,0x30,0x00,0x00,
    0x00,0x00,0x00,0x06,0x0C,0x18,0x30,0x60,0x30,0x18,0x0C,0x06,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x7E,0x00,0x00,0x7E,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x60,0x30,0x18,0x0C,0x06,0x0C,0x18,0x30,0x60,0x00,0x00,0x00,0x00,
    0x00,0x00,0x3E,0x63,0x63,0x06,0x0C,0x0C,0x0C,0x00,0x0C,0x0C,0x00,0x00,0x00,0x00,
    0x00,0x00,0x3E,0x63,0x63,0x6F,0x6B,0x6B,0x6E,0x60,0x60,0x3E,0x00,0x00,0x00,0x00,
    0x00,0x00,0x08,0x1C,0x36,0x63,0x63,0x63,0x7F,0x63,0x63,0x63,0x00,0x00,0x00,0x00,
    0x00,0x00,0x7E,0x33,0x33,0x33,0x3E,0x33,0x33,0x33,0x33,0x7E,0x00,0x00,0x00,0x00,
    0x00,0x00,0x1E,0x33,0x61,0x60,0x60,0x60,0x60,0x61,0x33,0x1E,0x00,0x00,0x00,0x00,
    0x00,0x00,0x7C,0x36,0x33,0x33,0x33,0x33,0x33,0x33,0x36,0x7C,0x00,0x00,0x00,0x00,
    0x00,0x00,0x7F,0x33,0x31,0x34,0x3C,0x34,0x30,0x31,0x33,0x7F,0x00,0x00,0x00,0x00,
    0x00,0x00,0x7F,0x33,0x31,0x34,0x3C,0x34,0x30,0x30,0x30,0x78,0x00,0x00,0x00,0x00,
    0x00,0x00,0x1E,0x33,0x61,0x60,0x60,0x6F,0x63,0x63,0x37,0x1D,0x00,0x00,0x00,0x00,
    0x00,0x00,0x63,0x63,0x63,0x63,0x7F,0x63,0x63,0x63,0x63,0x63,0x00,0x00,0x00,0x00,
    0x00,0x00,0x3C,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00,
    0x00,0x00,0x0F,0x06,0x06,0x06,0x06,0x06,0x06,0x66,0x66,0x3C,0x00,0x00,0x00,0x00,
    0x00,0x00,0x73,0x33,0x36,0x36,0x3C,0x36,0x36,0x33,0x33,0x73,0x00,0x00,0x00,0x00,
    0x00,0x00,0x78,0x30,0x30,0x30,0x30,0x30,0x30,0x31,0x33,0x7F,0x00,0x00,0x00,0x00,
    0x00,0x00,0x63,0x77,0x7F,0x6B,0x63,0x63,0x63,0x63,0x63,0x63,0x00,0x00,0x00,0x00,
    0x00,0x00,0x63,0x63,0x73,0x7B,0x7F,0x6F,0x67,0x63,0x63,0x63,0x00,0x00,0x00,0x00,
    0x00,0x00,0x1C,0x36,0x63,0x63,0x63,0x63,0x63,0x63,0x36,0x1C,0x00,0x00,0x00,0x00,
    0x00,0x00,0x7E,0x33,0x33,0x33,0x3E,0x30,0x30,0x30,0x30,0x78,0x00,0x00,0x00,0x00,
    0x00,0x00,0x3E,0x63,0x63,0x63,0x63,0x63,0x63,0x6B,0x6F,0x3E,0x06,0x07,0x00,0x00,
    0x00,0x00,0x7E,0x33,0x33,0x33,0x3E,0x36,0x36,0x33,0x33,0x73,0x00,0x00,0x00,0x00,
    0x00,0x00,0x3E,0x63,0x63,0x30,0x1C,0x06,0x03,0x63,0x63,0x3E,0x00,0x00,0x00,0x00,
    0x00,0x00,0xFF,0xDB,0x99,0x18,0x18,0x18,0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00,
    0x00,0x00,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x3E,0x00,0x00,0x00,0x00,
    0x00,0x00,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x36,0x1C,0x08,0x00,0x00,0x00,0x00,
    0x00,0x00,0x63,0x63,0x63,0x63,0x63,0x6B,0x6B,0x7F,0x36,0x36,0x00,0x00,0x00,0x00,
    0x00,0x00,0xC3,0xC3,0x66,0x3C,0x18,0x18,0x3C,0x66,0xC3,0xC3,0x00,0x00,0x00,0x00,
    0x00,0x00,0xC3,0xC3,0xC3,0x66,0x3C,0x18,0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00,
    0x00,0x00,0x7F,0x63,0x43,0x06,0x0C,0x18,0x30,0x61,0x63,0x7F,0x00,0x00,0x00,0x00,
    0x00,0x00,0x3C,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x3C,0x00,0x00,0x00,0x00,
    0x00,0x00,0x80,0xC0,0xE0,0x70,0x38,0x1C,0x0E,0x07,0x03,0x01,0x00,0x00,0x00,0x00,
    0x00,0x00,0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3C,0x00,0x00,0x00,0x00,
    0x08,0x1C,0x36,0x63,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,
    0x18,0x18,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x3C,0x46,0x06,0x3E,0x66,0x66,0x3B,0x00,0x00,0x00,0x00,
    0x00,0x00,0x70,0x30,0x30,0x3C,0x36,0x33,0x33,0x33,0x33,0x6E,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x3E,0x63,0x60,0x60,0x60,0x63,0x3E,0x00,0x00,0x00,0x00,
    0x00,0x00,0x0E,0x06,0x06,0x1E,0x36,0x66,0x66,0x66,0x66,0x3B,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x3E,0x63,0x63,0x7E,0x60,0x63,0x3E,0x00,0x00,0x00,0x00,
    0x00,0x00,0x1C,0x36,0x32,0x30,0x7C,0x30,0x30,0x30,0x30,0x78,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x3B,0x66,0x66,0x66,0x66,0x3E,0x06,0x66,0x3C,0x00,0x00,
    0x00,0x00,0x70,0x30,0x30,0x36,0x3B,0x33,0x33,0x33,0x33,0x73,0x00,0x00,0x00,0x00,
    0x00,0x00,0x0C,0x0C,0x00,0x1C,0x0C,0x0C,0x0C,0x0C,0x0C,0x1E,0x00,0x00,0x00,0x00,
    0x00,0x00,0x06,0x06,0x00,0x0E,0x06,0x06,0x06,0x06,0x06,0x66,0x66,0x3C,0x00,0x00,
    0x00,0x00,0x70,0x30,0x30,0x33,0x33,0x36,0x3C,0x36,0x33,0x73,0x00,0x00,0x00,0x00,
    0x00,0x00,0x1C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x1E,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x6E,0x7F,0x6B,0x6B,0x6B,0x6B,0x6B,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x6E,0x33,0x33,0x33,0x33,0x33,0x33,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x3E,0x63,0x63,0x63,0x63,0x63,0x3E,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x6E,0x33,0x33,0x33,0x33,0x3E,0x30,0x30,0x78,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x3B,0x66,0x66,0x66,0x66,0x3E,0x06,0x06,0x0F,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x6E,0x3B,0x33,0x30,0x30,0x30,0x78,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x3E,0x63,0x38,0x0E,0x03,0x63,0x3E,0x00,0x00,0x00,0x00,
    0x00,0x00,0x08,0x18,0x18,0x7E,0x18,0x18,0x18,0x18,0x1B,0x0E,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x66,0x66,0x66,0x66,0x66,0x66,0x3B,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x63,0x63,0x36,0x36,0x1C,0x1C,0x08,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x63,0x63,0x63,0x6B,0x6B,0x7F,0x36,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x63,0x36,0x1C,0x1C,0x1C,0x36,0x63,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x63,0x63,0x63,0x63,0x63,0x3F,0x03,0x06,0x3C,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x7F,0x66,0x0C,0x18,0x30,0x63,0x7F,0x00,0x00,0x00,0x00,
    0x00,0x00,0x0E,0x18,0x18,0x18,0x70,0x18,0x18,0x18,0x18,0x0E,0x00,0x00,0x00,0x00,
    0x00,0x00,0x18,0x18,0x18,0x18,0x18,0x00,0x18,0x18,0x18,0x18,0x18,0x00,0x00,0x00,
    0x00,0x00,0x70,0x18,0x18,0x18,0x0E,0x18,0x18,0x18,0x18,0x70,0x00,0x00,0x00,0x00,
    0x00,0x00,0x3B,0x6E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};


// array of font tables in program memory
const prog_uchar *FONTS[] PROGMEM =
{
    FONT6x8,
    FONT8x8,
    FONT8x16
}; 


//*****************************************************************************
//LCDPutChar.c
//Draws an ASCII character at the specified (x,y) address and color
//Inputs:
//c      = character to be displayed
//x      = row address (0 .. 131)
//y      = column address (0 .. 131)
//size   = font pitch (SMALL, MEDIUM, LARGE)
//fcolor = 12-bit foreground color value
//bcolor = 12-bit background color value
//Returns: nothing
// Author: James P Lynch, August 30, 2007
// Edited by Peter Davenport on August 23, 2010
//For more information on how this code does it's thing look at this
//"http://www.sparkfun.com/tutorial/Nokia%206100%20LCD%20Display%20Driver.pdf"
//*****************************************************************************
void LCDPutChar(char c, int x, int y, int fSize, int fColor, int bColor)
{
    int             i,j;
    unsigned int    nCols;
    unsigned int    nRows;
    unsigned int    nBytes;
    unsigned char   PixelRow;
    unsigned char   Mask;
    unsigned int    Word0;
    unsigned int    Word1;
    unsigned char   *pChar;
    unsigned char   character_pattern[16];
    prog_uchar      *pFont;

    // get pointer to font table in program memory
    pFont = (prog_uchar*)pgm_read_word((prog_uchar*)&FONTS[fSize]);
    
    // get columns, rows, and bytes info from font table
    nCols  = (unsigned int)pgm_read_byte(pFont);
    nRows  = (unsigned int)pgm_read_byte(pFont + 1);
	nBytes = (unsigned int)pgm_read_byte(pFont + 2);

	// get font pattern for character
	memcpy_P(character_pattern, pFont + (nBytes * (c - 0x1F)), nBytes);
    pChar = &character_pattern[0];

    // flip display to re-orient "up"
//    y = (COL_HEIGHT - 1) - (y + nCols);
//    x = (ROW_LENGTH - 1) - x;

    // Row address set (command 0x2B)
    LCDCommand(PASET);
    LCDData(x);
    LCDData(x + nRows - 1);

    // Column address set (command 0x2A)
    LCDCommand(CASET);
    LCDData(y);
    LCDData(y + nCols - 1);

    // WRITE MEMORY
    LCDCommand(RAMWR);

    // loop on each row, working backwards from the bottom to the top
    for (i = 0; i < nRows; i++)
    {
        // copy pixel row from font table and then decrement row
        PixelRow = *pChar++;
        // loop on each pixel in the row (left to right)
        // Note: we do two pixels each loop
        Mask = 0x80;
        for (j = 0; j < nCols; j += 2)
        {
            // if pixel bit set, use foreground color; else use the background color
            // now get the pixel color for two successive pixels
            if ((PixelRow & Mask) == 0)
                Word0 = bColor;
            else
                Word0 = fColor;
            Mask = Mask >> 1;
            if ((PixelRow & Mask) == 0)
                Word1 = bColor;
            else
                Word1 = fColor;
            Mask = Mask >> 1;
            // use this information to output three data bytes
            LCDData((Word0 >> 4) & 0xFF);
            LCDData(((Word0 & 0xF) << 4) | ((Word1 >> 8) & 0xF));
            LCDData(Word1 & 0xFF);
        }
    }
    // terminate the Write Memory command
    LCDCommand(NOP);
}

// *************************************************************************************************
//LCDPutStr.c
//
//Draws a null-terminates character string at the specified (x,y) address and color
//
// Inputs: pString = pointer to character string to be displayed
// x = row address (0 .. 131)
// y = column address (0 .. 131)
// Size = font pitch (SMALL, MEDIUM, LARGE)
// fColor = 12-bit foreground color value rrrrggggbbbb
// bColor = 12-bit background color value rrrrggggbbbb
//
// Returns: nothing
//
// Notes: Here's an example to display "Hello World!" at address (20,20)
// LCDPutChar("Hello World!", 20, 20, WHITE, BLACK);
//
// Author: James P Lynch, August 30, 2007
// Edited by Peter Davenport on August 23, 2010
//For more information on how this code does it's thing look at this
//"http://www.sparkfun.com/tutorial/Nokia%206100%20LCD%20Display%20Driver.pdf"
// *************************************************************************************************
void LCDPutStr(char *pString, int x, int y, int size, int fColor, int bColor)
{
    int  width = (SMALL == size) ? 6 : 8;
    
//    y = y + width;

    // loop until null-terminator is seen
    while (*pString != '\0')
    {
        // draw the character
        LCDPutChar(*pString++, x, y, size, fColor, bColor);
        // advance the y position
        y = y + width;
        // bail out if y exceeds 131
        if (y > 131) break;
    }
}



void LCDContrast(char setting)
{
    LCDCommand(VOLCTR);        // electronic volume, this is the contrast/brightness(EPSON)
    LCDData(setting);        // volume (contrast) setting - course adjustment,  -- original was 24
//     LCDData(0x03);            // internal resistor ratio - coarse adjustment
//     LCDData(0x30);   

    LCDCommand(NOP);        // nop(EPSON)
}

// *************************************************************************************************
//LCDDrawCircle
//
//Draws a full circle, half a circle, or a quarter of a circle
// Inputs:
// xCenter the x for the center of the circle
// yCenter the y for the center of the circle
// radius the radius for the circle
// color the color of the circle
// circleType the type of circle see LCD_driver.h for types of circles
//
//  This routine can be used to draw a complete circle, half circle or a quarter of a circle
//
// Returns: Nothing
//
// I found the original code at http://www.arduino.cc/playground/Code/GLCDks0108 and modified it for
// this library
//
// *************************************************************************************************
void LCDDrawCircle (int xCenter, int yCenter, int radius, int color, int circleType)
{
    int tSwitch, x1 = 0, y1 = radius;
    int Width = 2*radius, Height=Width;
    tSwitch = 3 - 2 * radius;

    while (x1 <= y1)
    {
        if (circleType == FULLCIRCLE||circleType == OPENSOUTH||circleType == OPENEAST||circleType == OPENSOUTHEAST)
        {
            LCDSetPixel(color,xCenter+radius - x1, yCenter+radius - y1);
            LCDSetPixel(color,xCenter+radius - y1, yCenter+radius - x1);
        }
        if (circleType == FULLCIRCLE||circleType == OPENNORTH||circleType == OPENEAST||circleType == OPENNORTHEAST)
        {
            LCDSetPixel(color,xCenter+Width-radius + x1, yCenter+radius - y1);
            LCDSetPixel(color,xCenter+Width-radius + y1, yCenter+radius - x1);
        }
        if (circleType == FULLCIRCLE||circleType == OPENNORTH||circleType == OPENWEST||circleType == OPENNORTHWEST)
        {
            LCDSetPixel(color,xCenter+Width-radius + x1, yCenter+Height-radius + y1);
            LCDSetPixel(color,xCenter+Width-radius + y1, yCenter+Height-radius + x1);
        }
        if (circleType == FULLCIRCLE||circleType == OPENSOUTH||circleType == OPENWEST||circleType == OPENSOUTHWEST)
        {
            LCDSetPixel(color,xCenter+radius - x1, yCenter+Height-radius + y1);
            LCDSetPixel(color,xCenter+radius - y1, yCenter+Height-radius + x1);
        }
        if (tSwitch < 0)
        {
            tSwitch += (4 * x1 + 6);
        }
        else
        {
            tSwitch += (4 * (x1 - y1) + 10);
            y1--;
        }
        x1++;
    }
}

