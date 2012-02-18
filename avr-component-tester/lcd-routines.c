// Ansteuerung eines HD44780 kompatiblen LCD im 4-Bit-Interfacemodus
// http://www.mikrocontroller.net/articles/AVR-GCC-Tutorial
//
// The pin allocation is adjustable over defines in LCD routines.h
 
#include <avr/io.h>
#include "lcd-routines.h"
#include <util/delay.h>
#include <avr/eeprom.h>
 
void lcd_data(unsigned char temp1)
{
	LCD_PORT |= (1<<LCD_RS);        				// RS auf 1 setzen
	lcd_send(temp1);						// display the char
	switch(temp1) {
		case 0:
			uart_putc('#');
			break;
		case 225:						//ä
			uart_putc('ä');
			break;
		case 228:						//µ
			uart_putc('µ');
			break;
		case 244:						//Omega
			break;
		default:
			uart_putc(temp1);
	}
}

									// sends an instruction to the LCD
void lcd_command(unsigned char temp1)
{
	LCD_PORT &= ~(1<<LCD_RS);        				// R-S to 0 set
	lcd_send(temp1);
	
	if((temp1 == 0x80) || (temp1 == 0xC0)) 
	  uart_newline();
}

									//Actual one of LCD access - Function; 4-Bit-Modus
void lcd_send(unsigned char data) {
  
  _delay_us(500); 							// mick - needed for very slow Blue LCD
  
									//upper Nibble set
  LCD_PORT = (LCD_PORT & 0xF0) | ((data >> 4) & 0x0F);
  _delay_us(5);
  
  lcd_enable();
									//lower Nibble set
  LCD_PORT = (LCD_PORT & 0xF0) | (data & 0x0F);
  _delay_us(5);
  
  lcd_enable();
  _delay_us(60);  
  
  LCD_PORT &= 0xF0;
}

									// produces the Enable pulse
void lcd_enable(void)							// Toggle the bit ON 10uS OFF
{
	LCD_PORT |= (1<<LCD_EN1);
    _delay_us(10);                  					// short break
									// Extend break in accordance with data sheet LCD of the CONTROLLER for problems if necessary
									// http://www.mikrocontroller.net/topic/80900
   LCD_PORT &= ~(1<<LCD_EN1);
}
 
									// Initialisierung: 
									// Must be called completely at the beginning of the program.
 
void lcd_init(void)
{
	LCD_DDR = LCD_DDR | 0x0F | (1<<LCD_RS) | (1<<LCD_EN1);   	//port on exit scolded
									// 3 times must be sent one behind the other to the initialization
	_delay_ms(30);
	LCD_PORT = (LCD_PORT & 0xF0 & ~(1<<LCD_RS)) | 0x03;
	
	lcd_enable();
	_delay_ms(5);

	lcd_enable();
	_delay_ms(1);

	lcd_enable();
	_delay_ms(1);
	
	LCD_PORT = (LCD_PORT & 0xF0 & ~(1<<LCD_RS)) | 0x02;
	_delay_ms(1);
	
	lcd_enable();
	_delay_ms(1);

									// 4Bit / 2  Lines/ 5x7
	lcd_command(CMD_SetIFOptions | 0x08);

									//Display/cursors out/no flashing
	lcd_command(CMD_SetDisplayAndCursor | 0x04);

									//increment/no Scroll
	lcd_command(CMD_SetEntryMode | 0x02);	
	lcd_clear();
}
 
									// Sends the instruction for clearing of the display
 
void lcd_clear(void)
{
   lcd_command(CLEAR_DISPLAY);
   _delay_ms(5);
   uart_newline();
}

void uart_newline(void) {
  uart_putc('\r');
  uart_putc('\n');
}
 
 
									// Writes a string to the LCD
 
void lcd_string(char *data)
{
    while(*data) {
        lcd_data(*data);
        data++;
    }
}

									//EEPROM data to LCD
void lcd_eep_string(const unsigned char *data)
{	
unsigned char c;

    while(1) {
      c = eeprom_read_byte(data);
      
      if((c==0) || (c==128)) 						//0x00 end of string, 0x80 special - see top of main.c
	return;
        
      lcd_data(c);
      data++;
    }
}
