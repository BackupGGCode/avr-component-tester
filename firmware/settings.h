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
	
	#define _PROJECT_NAME_ "ACT"
	#define _PROJECT_FULLNAME_ "AVR Component Tester"
	
	#define _FIRMWARE_VERSION_ 1
	#define _FIRMWARE_REVISION_ 2
	#define _HARDWARE_REVISION_ 2


//////// [ Override Pin# with Custom Alias ] ///////////////////////////////////////////////////////////////////

	//// ENABLE_PIN_ALIAS if defined then making a probe alias is enabled
	#define ENABLE_PIN_ALIAS
	
	//// Aliases /////////////////////////
	#ifdef ENABLE_PIN_ALIAS
		#define PIN1_ALIAS 'Y' // [pFLAG]
		#define PIN2_ALIAS 'B' 
		#define PIN3_ALIAS 'G'
		//#define PIN1_ALIAS '1'
		//#define PIN2_ALIAS '2'
		//#define PIN3_ALIAS '3'	
	#endif

//////// [ Override Pin# with Custom Alias ] ///////////////////////////////////////////////////////////////////

	//// LCD_SLOW_SEND if defined will put a small delay between LCD commands
	#define LCD_SLOW_SEND


//////// [ Probe and Input Registers ] /////////////////////////////////////////////////////////////////////////

	//// Defined Settings 
	#define ADC_PORT PORTC						// Use port C for A to D inputs
	#define ADC_DDR DDRC
	#define ADC_PIN PINC
	#define TP1 PC0								// AVR pin 23 probe.
	#define TP2 PC1								// AVR pin 24 probe.
	#define TP3 PC2								// AVR pin 25 probe.



//////// [ Internal Calculation Variables ] ////////////////////////////////////////////////////////////////////
	#define SMALL_CAP_VALUE 394						// 218  0xDA    Adjust for accuracy on big Caps with 750R, was 394
	#define LARGE_CAP_VALUE 283						// 167  0xA7    Adjust for accuracy, small Caps with 500K, was 283
	#define BAT_WEAK 850							// 930  0x03A2  7.7V weak battery, was 650
	#define BAT_DEAD 650							// 875  0x036B  7.2V dead Battery, was 600
	#define LARGE_R_VALUE 4700						// 
	#define SMALL_R_VALUE 680						// 
	#define ASCII_1 49								// Ascii one.
	#define ASCII_0 48								// Ascii zero.
	#define NORMAL_CAP_TESTS 1						// Just do normal Cap test, compare to CapTestMode in EEPROM, 2 to enable all
	#define HALF_ADC_RANGE 512						// midpoint of ADC
	#define MAX_ADC 1023							// Maximum ADC count
	#define WDT_enabled  							// Watchdog active for normal use, disable for debug
	
	#define V_CAPTESTMODE 0b00100010				// 34 0x22, last used EEPROM address 0x15A, Cap test type, pins to use
													// DEFAULT  0x22 = All 6 Cap Tests, 1st 0, 2nd 2


//////// [ Usfull Bit Control Defines ] ///////////////////////////////////////////////////////////////////////
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

//////// [ outputs to test resistors. ] ///////////////////////////////////////////////////////////////////////
	#define R_DDR DDRB							
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
									

//////// [ Power and Regulator Control Defines ] //////////////////////////////////////////////////////////////////////

	//// Pin Registers Defines //////////////////////
	#define ON_DDR 		DDRD
	#define ON_PORT 	PORTD
	#define ON_PIN_REG 	PIND
	#define ON_PIN 		PD6
	#define RST_PIN 	PD7
	
	//// Pin Control Defines ////////////////////////
	//POWER_ON() 	= Turn the regulator ON	
	#define POWER_ON() 	ON_DDR = (1<<ON_PIN); SETBIT(ON_PORT,ON_PIN); SETBIT(ON_PORT,RST_PIN)
	
	//POWER_OFF() 	= Turn the regulator OFF
	#define POWER_OFF() CLEARBIT(ON_PORT,ON_PIN); CLEARBIT(ON_DDR,ON_PIN); CLEARBIT(ON_PORT,ON_PIN)
	
	//POWER_GET() 	= Get the ON_PIN logic
	#define POWER_GET() CHECKBIT(ON_PIN_REG,ON_PIN)
	
	//RESET_GET()	= Get the reset's logic
	#define RESET_GET() CHECKBIT(ON_PIN_REG,RST_PIN)


//////// [ Part/Component Unique Identifying Defines ] /////////////////////////////////////////////////////////////////

	//// Supported Components ///////////////////////
	#define PART_NONE 			0
	#define PART_DIODE 			1
	#define PART_TRANSISTOR 	2
	#define PART_FET 			3
	#define PART_TRIAC 			4
	#define PART_THYRISTOR 		5
	#define PART_RESISTOR 		6
	#define PART_CAPACITOR 		7

	//// Special definitions for Devices ////////////
	#define PART_MODE_N_E_MOS	1
	#define PART_MODE_P_E_MOS 	2
	#define PART_MODE_N_D_MOS 	3
	#define PART_MODE_P_D_MOS 	4
	#define PART_MODE_N_JFET 	5
	#define PART_MODE_P_JFET 	6
	
	//// Transistor Ident ////////////////////////////
	#define PART_MODE_NPN 		1
	#define PART_MODE_PNP 		2


//////// [ Device Power Selection Jumper and Ident ] ///////////////////////////////////////////////////////////////////

	//// Pin Registers ///////////////////////////////
	#define PWRMODE_DDR 		DDRB
	#define PWRMODE_PIN 		PINB
	#define PWRMODE_PORT 		PORTB
	#define PWRMODE_BIT 		PB6

	//// Power Mode Ident ////////////////////////////
	#define PWR_5V 				0
	#define PWR_9V 				1
	
	//// Pin Control Defines ////////////////////////
	//PWRMODE_SETUP()	= Sets the jumper for the power selection into input with internal pull-up
	#define PWRMODE_SETUP()		CLEARBIT(PWRMODE_DDR,PWRMODE_BIT); SETBIT(PWRMODE_PORT,PWRMODE_BIT)
	//PWRMODE_GET()		= Gets the power jumper's logic
	#define PWRMODE_GET()		CHECKBIT(PWRMODE_PIN,PWRMODE_BIT)



#endif
/* eof */
