////////////////////////////////////////////
/////
////     AVR Component Tester
///       BrentBXR & MickM
// -----------------------------
// 
// ACT Settings Definitions ->
//     CID: 0x9938A4E
//     SID: 001
//
// $desc This file holds all the customizable settings as
//       defines. Be very carefull when editing this file
//       only modify this  file if you know what  you are
//       doing.  You  have  been  warned.
//
//


#ifndef _SETTINGS_H_ 
#define _SETTINGS_H_


#define MCU_STATUS_REG MCUCR


// Define the probe aliases.
#define PIN1_ALIAS 'Y' //ASCII
#define PIN2_ALIAS 'B' //ASCII
#define PIN3_ALIAS 'G' //ASCII

// Misc. LCD Settings
// LCD_SLOW_SEND if defined will put a small delay between LCD commands
#define LCD_SLOW_SEND


// Defined Settings 
#define ADC_PORT PORTC							// Use port C for A to D inputs
#define ADC_DDR DDRC
#define ADC_PIN PINC
#define TP1 PC0								// AVR pin 23 probe.
#define TP2 PC1								// AVR pin 24 probe.
#define TP3 PC2								// AVR pin 25 probe.

#define WDT_enabled  							// Watchdog active for normal use, disable for debug
									// MickM defines, set with 9.00V DC input from a bench PS.
#define SMALL_CAP_VALUE 394						// 218  0xDA    Adjust for accuracy on big Caps with 750R, was 394
#define LARGE_CAP_VALUE 283						// 167  0xA7    Adjust for accuracy, small Caps with 500K, was 283
#define BAT_WEAK 850							// 930  0x03A2  7.7V weak battery, was 650
#define BAT_DEAD 650							// 875  0x036B  7.2V dead Battery, was 600
#define LARGE_R_VALUE 4700						// 
#define SMALL_R_VALUE 680						// 
#define ASCII_1 49							// Ascii one.
#define NORMAL_CAP_TESTS 1						// Just do normal Cap test, compare to CapTestMode in EEPROM, 2 to enable all
#define HALF_ADC_RANGE 512						// midpoint of ADC
#define MAX_ADC 1023							// Maximum ADC count





#define R_DDR DDRB							// Port B for outputs to test resistors.
#define R_PORT PORTB

									// Physical and logical hookup:
									// 
									// Original usd Mega8, mine is Mega328P, same pinout in dip28
									// 
									// Output ----->Rlow----->
									//                          join here , probe ----> Input
									// Output ----->RHigh----->
									// 
									// two outputs, two R's, one probe, one input
									// repeat for 3 probes
									// 
									// 14-PB0	Rx-750R	    23-PC0 -> Probe		RL1
									// 15-PB1      	Rx 500K     23-PC0			RH1
									// 
									// 16-PB2	Rx-750R	    24-PC1 -> Probe		RL2
									// 17-PB3	Rx 500K     24-PC1			RH2
									// 
									// 18-PB4	Rx-750R	    25-PC2 -> Probe		RL3
									// 19-PB5	Rx 500K     25-PC2			RH3
									// 
									// 26-PC3-x
									// 27-PC4-x
									// 
									// Voltage divider for the battery monitor.
									// Pin 28, PC5(ADC5) monitors it.
									// 
									// Vin (v) --->  diode ----> 10K ---28-PC5(ADC5) and ---3K3 ---> GND
									// ----> Vreg ---. +5V
									// There is a 5V1 Zener across the 3K3 to ground, this is for ADC input protection.
									// The Battery/PS input is not regulated at all, so could be high or spiky.
									// 
									// It will never set 5V1 as the input is too small.
									// 
									

#define ON_DDR DDRD
#define ON_PORT PORTD
#define ON_PIN_REG PIND
#define ON_PIN PD6							// Must be set high immediately, to stay powered on
#define RST_PIN PD7							// "push to test" button

#define PART_NONE 0							// Types of Devices
#define PART_DIODE 1
#define PART_TRANSISTOR 2
#define PART_FET 3
#define PART_TRIAC 4
#define PART_THYRISTOR 5
#define PART_RESISTOR 6
#define PART_CAPACITOR 7

									// Special definitions for Devices
#define PART_MODE_N_E_MOS 1						// FETs
#define PART_MODE_P_E_MOS 2
#define PART_MODE_N_D_MOS 3
#define PART_MODE_P_D_MOS 4
#define PART_MODE_N_JFET 5
#define PART_MODE_P_JFET 6


#define PART_MODE_NPN 1							// Bipolar
#define PART_MODE_PNP 2


// Select power source; If BATMODE_BIT is jumped to ground then power source = 5v; else 9v battery
#define PWRMODE_DDR DDRB
#define PWRMODE_PIN PINB
#define PWRMODE_PORT PORTB
#define PWRMODE_BIT PB6

#define PWR_5V 0
#define PWR_9V 1



	// Simple Bit Control Defines
	#define SETBIT(ADDRESS,BIT) (ADDRESS |= (1<<BIT)) 
	#define CLEARBIT(ADDRESS,BIT) (ADDRESS &= ~(1<<BIT)) 
	#define FLIPBIT(ADDRESS,BIT) (ADDRESS ^= (1<<BIT)) 
	#define CHECKBIT(ADDRESS,BIT) (ADDRESS & (1<<BIT)) 

	#define SETBITMASK(x,y) (x |= (y)) 
	#define CLEARBITMASK(x,y) (x &= (~y)) 
	#define FLIPBITMASK(x,y) (x ^= (y)) 
	#define CHECKBITMASK(x,y) (x & (y)) 

	#define VARFROMCOMB(x, y) x 
	#define BITFROMCOMB(x, y) y 

	#define C_SETBIT(comb) SETBIT(VARFROMCOMB(comb), BITFROMCOMB(comb)) 
	#define C_CLEARBIT(comb) CLEARBIT(VARFROMCOMB(comb), BITFROMCOMB(comb)) 
	#define C_FLIPBIT(comb) FLIPBIT(VARFROMCOMB(comb), BITFROMCOMB(comb)) 
	#define C_CHECKBIT(comb) CHECKBIT(VARFROMCOMB(comb), BITFROMCOMB(comb)) 

	#define nop()  __asm__ __volatile__("nop")


#endif
/* eof */
