////////////////////////////////////////////
/////
////     AVR Component Tester
///       BrentBXR & MickM
// -----------------------------
// 
// LCD Routines ->
//     CID: 0x9938A4E
//     SID: 001
//
// $desc This file contains all the functions needed
//       to control a standard HDD chipset 2x16 char
//       LCD. 
//
//  ## This file is part of the original project and has ##
//  ##         been highly modified for our needs.       ##
//

#ifndef _LCDROUTINES_H_ 
#define _LCDROUTINES_H_



void lcd_data(unsigned char temp1);
void lcd_command(unsigned char temp1);
void lcd_send(unsigned char data);
void lcd_string(char *data);
void lcd_enable(void);
void lcd_clear(void);
void lcd_init(void);
void lcd_eep_string(const unsigned char *data);

/*
	#define lcd_data(x) LCD_PORT |= (1<<LCD_RS); lcd_send(x)
	#define lcd_command(x) LCD_PORT &= ~(1<<LCD_RS); lcd_send(x)
	#define lcd_clear() lcd_command(CLEAR_DISPLAY); _delay_ms(5)
*/

										//LCD-Instructions
#define CMD_SetEntryMode         0x04
#define CMD_SetDisplayAndCursor  0x08
#define CMD_SetIFOptions         0x20
#define CMD_SetCGRAMAddress      0x40    					// for Custom characters
#define CMD_SetDDRAMAddress      0x80    					// set to the cursor

										//Macros for LCD
#define Line1() SetCursor(1,0)							//jump to start of first line
#define Line2() SetCursor(2,0)							//jump to start of second line

#define SetCursor(y, x) lcd_command((uint8_t)(CMD_SetDDRAMAddress + (0x40*(y-1)) + x)) //To a certain position jump

#define UpdateProgress(x) SetCursor(2,13); lcd_string(x)

#define LCDLoadCustomChar() lcd_command(CMD_SetCGRAMAddress)			//Custom chars
#define LCD_CHAR_OMEGA  244							//Omega character
#define LCD_CHAR_U  228								//?-indication
#define LCD_CHAR_DIODE  0							//Diode icon; Custom character
  
										// LCD Instructions
#define CLEAR_DISPLAY 0x01
										// Pin allocation for the LCD, to used pins adapt
#define LCD_PORT      PORTD
#define LCD_DDR       DDRD
#define LCD_RS        PD4
#define LCD_EN1       PD5

#endif
/* eof */
