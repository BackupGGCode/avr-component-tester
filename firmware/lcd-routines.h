//Control of a HD44780 compatible LCD in the 4-Bit-Interface
//
// http://www.mikrocontroller.net/articles/AVR-GCC-Tutorial
//
//LCD
void lcd_data(unsigned char temp1);
void lcd_command(unsigned char temp1);
void lcd_send(unsigned char data);
void lcd_string(char *data);
void lcd_enable(void);
void lcd_init(void);
void lcd_clear(void);
void lcd_eep_string(const unsigned char *data);
										//Software-UART
extern void uart_putc(uint8_t data);
void uart_newline(void);
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
