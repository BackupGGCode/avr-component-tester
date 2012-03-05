/*

    ///////////////////////////////////////////////////
   ///////////////// AVR Component Tester ////////////
  //////////////////    Version 1 beta    ///////////
 ///////////////////////////////////////////////////

 AVR Component Tester
 Version 1.0 BETA

 This project is based on the original 'AVR Transistor Tester' and will be
 modified for a small group of people interested in expanding this project.
 The original project can be found:
 
 http://www.mikrocontroller.net/articles/AVR-Transistortester
 
 This new project (ACT for short) does not have its own website; but it does
 have its own google project page which includes news, wiki, bug tracking,
 and a publicly viewable SVN; where you can always find the latest source
 files and other resources such as: Schematics, PCB files; and even pre-
 compiled gerbers, if you want to get your own PCB(s) designed. The gerbers
 are already completed and designed to work with SEEED Studio's PCB rules and
 formats. See www.seeedstudios.com for more information on there 'Fusion PCB
 service'.

 Main Project Developers:
 BrentBXR (@Gmail.com)
 MickM
 
 We also have a related forum thread at DangerousPrototypes, there you can see
 where it all started, and where we are now with this project. Find it here:
 
 http://dangerousprototypes.com/forum/viewtopic.php?f=19&t=3260
 
 This is a open hardware project; which means everything about it from the source
 files, down to the schematics and pcb files are all open source and public domain
 you are welcome to use, modify, sell, or anything without issue. It is common
 curtosy to atleast let us know what your doing with our project if you use it.
 it is also common that if you decide to replicate and sell; that you atleast tell
 us... that said you dont have to tell us anything :P
 
                                                                   [BrentBXR]
*/

#include <avr/io.h>
#include "lcd-routines.h"
#include "swuart.h"
#include <util/delay.h>
#include <avr/sleep.h>
#include <stdlib.h>
#include <string.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <math.h>

#include "settings.h"


	uint8_t CapTestMode EEMEM = 0b00100010;				// 34 0x22, last used EEPROM address 0x15A, Cap test type, pins to use
														// DEFAULT  0x22 = All 6 Cap Tests, 1st 0, 2nd 2

	uint8_t RFU1 EEMEM = 0;						// 0x00 [RESERVED EEPROM SPACE]
	uint8_t RFU2 EEMEM = 0;						// 0x00 [RESERVED EEPROM SPACE]
	uint8_t RFU3 EEMEM = 0;						// 0x00 [RESERVED EEPROM SPACE]

	unsigned int R_L_VAL EEMEM = SMALL_R_VALUE;			// R_L; 750 0x2EE
	unsigned int R_H_VAL EEMEM = LARGE_R_VALUE;			// R_H; 5000 0x1388
	
	unsigned int H_CAPACITY_FACTOR EEMEM = SMALL_CAP_VALUE;		// 218 0xDA Capacitor calibration numbers for H and L value accuracy
	unsigned int L_CAPACITY_FACTOR EEMEM = LARGE_CAP_VALUE;		// 167 0xA7



// *########################################################################################

	// Words, messages, and strings:        
	unsigned char StartupMessage[]	EEMEM = "ACT v1.0   ";
	unsigned char BatMode[] 		EEMEM = "[BAT]";
	unsigned char PwrMode[]			EEMEM = "[PWR]";
	unsigned char TestRunning[]     EEMEM = "Testing...";
	unsigned char Bat[]             EEMEM = "Battery ";
	unsigned char BatWeak[]         EEMEM = "weak";
	unsigned char BatEmpty[]        EEMEM = "empty!";
	unsigned char TestFailed1[]     EEMEM = "No, unknown, or";
	unsigned char TestFailed2[]     EEMEM = "damaged ";
	unsigned char Bauteil[]         EEMEM = "part";
	unsigned char Unknown[]         EEMEM = " unknown";
	unsigned char OrBroken[] 		EEMEM = "or damaged ";
	unsigned char TestTimedOut[] 	EEMEM = "Timeout!";
	
	// Components
	unsigned char Diode[]           EEMEM = "Diode: ";
	unsigned char DualDiode[]       EEMEM = "Double diode ";
	unsigned char TwoDiodes[]       EEMEM = "2 diodes";
	unsigned char Antiparallel[]    EEMEM = "anti-parallel";
	unsigned char Resistor[] 		EEMEM = "Resistor: ";
	unsigned char Capacitor[] 		EEMEM = "Capacitor: ";
	unsigned char InSeries[]        EEMEM = "serial A=";
	unsigned char Triac[]           EEMEM = "Triac";
	unsigned char Thyristor[]       EEMEM = "Thyristor";

	// Codes and Values
	unsigned char K1[]          EEMEM = ";C1=";
	unsigned char K2[]          EEMEM = ";C2=";
	unsigned char GAK[]         EEMEM = "GAC=";
	unsigned char NextK[]       EEMEM = ";C=";
	unsigned char K[]           EEMEM = "C=";
	unsigned char mosfet[]  	EEMEM = "-MOS";
	unsigned char emode[]   	EEMEM = "-E";
	unsigned char dmode[]   	EEMEM = "-D";
	unsigned char jfet[]    	EEMEM = "-JFET";
	unsigned char A1[]      	EEMEM = ";A1=";
	unsigned char A2[]      	EEMEM = ";A2=";
	unsigned char GateCap[] 	EEMEM = " C=";
	unsigned char hfestr[]  	EEMEM = "hFE=";
	unsigned char NPN[]     	EEMEM = "NPN";
	unsigned char PNP[]     	EEMEM = "PNP";
	unsigned char bstr[]    	EEMEM = " B=";
	unsigned char cstr[]    	EEMEM = ";C=";
	unsigned char estr[]    	EEMEM = ";E=";
	unsigned char gds[]     	EEMEM = "GDS=";
	unsigned char Uf[]      	EEMEM = "Uf=";
	unsigned char vt[]      	EEMEM = "Vt=";
	unsigned char mV[]      	EEMEM = "mV";
	unsigned char Anode[]   	EEMEM = "A=";
	unsigned char Gate[]    	EEMEM = "G=";
	unsigned char CA[]      	EEMEM = "CA";
	unsigned char CC[]     		EEMEM = "CC";

	// LCD Icons
	unsigned char DiodeIcon[] EEMEM = {4,31,31,14,14,4,31,4,0};	// Diode icon

struct Diode {
	uint8_t Anode;
	uint8_t Cathode;
	int Voltage;
};
									// Function prototypes

void 			CheckPins			(uint8_t HighPin, uint8_t LowPin, uint8_t TristatePin);
void 			DischargePin		(uint8_t PinToDischarge, uint8_t DischargeDirection);
unsigned int 	ReadADC				(uint8_t mux);
void 			lcd_show_format_cap	(char outval[], uint8_t strlength, uint8_t CommaPos);
void 			ReadCapacity		(uint8_t HighPin, uint8_t LowPin);		


volatile unsigned int PowerMode=PWR_5V;


struct 			Diode diodes[6];

uint8_t 		NumOfDiodes;
uint8_t 		b;						// pins of transistor
uint8_t 		c;						// pins of transistor
uint8_t 		e;						// pins of transistor
unsigned long 	lhfe;					// Amplification factor
uint8_t 		PartReady;				// Device recognized, Transistor, FET, Triac
unsigned int 	hfe[2];					// Amplification factors
unsigned int 	uBE[2];					// B-E Covering for transistors
uint8_t 		PartMode;				// See defines PART_MODE_
uint8_t 		tmpval;
uint8_t 		tmpval2;
uint8_t 		ra;						// Resistance pin
uint8_t 		rb;						// Resistance pin
unsigned int 	rv[2];					// Voltage drop at the resistance
unsigned int 	radcmax[2];	       		// Max ADC value (smaller than 1023, comes close but does not get to zero)
uint8_t 		ca;						// Condenser-Pins
uint8_t 		cb;						// Condenser-Pins
uint8_t 		cp1;					// Testing condenser pins, if measurement for individual pins selected
uint8_t 		cp2;					// Testing condenser pins, if measurement for individual pins sel
uint8_t 		ctmode;					// Condenser test mode
unsigned long 	cv;
uint8_t 		tmpPartFound;			// temp found Device, used for Resistor
uint8_t 		PartFound;				// the found Device numeric ID see defines PART_
char 			outval[8];
unsigned int 	adcv[4];
unsigned int 	gthvoltage;				// Gate threshold voltage
char 			outval2[6];


uint8_t GetPinAlias(uint8_t nPin)
{
   switch(nPin) {
      case (unsigned char)'1':
         nPin = PIN1_ALIAS;
         break;
      case (unsigned char)'2':
         nPin = PIN2_ALIAS;
         break;
      case (unsigned char)'3':
         nPin = PIN3_ALIAS;
         break;
   }
   return nPin;
}



int main(void) {
  ON_DDR = (1<<ON_PIN);							// Switch on
  ON_PORT = (1<<ON_PIN) | (1<<RST_PIN);					// Keep power on and set -RESET pin High

  PWRMODE_DDR &= ~(1<<PWRMODE_BIT); // Battery mode jumper; set to input
  PWRMODE_PORT |= (1<<PWRMODE_BIT); // internal pull-up
  
  uint8_t tmp;
  ADCSRA = (1<<ADEN) | (1<<ADPS1) | (1<<ADPS0);				// Enable ADC, set Prescale to 8
  lcd_init();								// init LCD
									// Load EEPROM constants
  unsigned int rhval = eeprom_read_word(&R_H_VAL);			// R_H
  unsigned int rlval = eeprom_read_word(&R_L_VAL);			// R_L
  
  ctmode = eeprom_read_byte(&CapTestMode);				// Compile time choice of test modes (0x22)
  cp1 = (ctmode & 12) >> 2;						// Capacitor pin 1, DEFAULT 0
  cp2 = ctmode & 3;							// Capacitor pin 2, DEFAULT 2
  ctmode = (ctmode & 48) >> 4;						// Capacitor test mode, DEFAULT is 0x02 for all 6 cap tests.
  
  wdt_disable();							// Disable watch dog timer.
  
  if(MCU_STATUS_REG & (1<<WDRF)) {					// Examine for Watchdog RESETs That enters, if the Watchdog 2s were not put back Can occur, 
                                                                        // if the program in a continuous loop " itself; tangled" has.
    lcd_eep_string(TestTimedOut);					// Message - "Timeout!"
    _delay_ms(3000);                                                    // Wait 3 sec
    ON_PORT = 0;							// Switch off! 
    return 0;
  }
  
  LCDLoadCustomChar();							// Custom indication Diode symbol into LCD load
  lcd_eep_string(DiodeIcon);						// Message - diode icon
  Line1();								// jump to start of first line

start:									// re-entry point, if button is re-pressed
  #ifdef WDT_enabled
    wdt_enable(WDTO_2S);						// Watchdog Timer on, 2 seconds?
  #endif

  PartFound 	= PART_NONE;
  tmpPartFound 	= PART_NONE;
  NumOfDiodes 	= 0;
  PartReady 	= 0;
  PartMode 		= 0;
  ca 			= 0;
  cb 			= 0;

  lcd_clear();
  ADC_DDR = (1<<TxD);							// Enable the Software-UART
  uart_newline();
                                                // Measure the 9V battery Supply ( - diode drop)
  ReadADC(5 | (1<<REFS1));						// Dummy-Readout
  hfe[0] = ReadADC(5 | (1<<REFS1));			// OR with internal reference

  lcd_clear();
  lcd_eep_string(StartupMessage);
  

  // Check GND&BATMODE_BIT is jumped to ground; if its not then test for battery.
  if(PWRMODE_PIN & (1<<PWRMODE_BIT)) {
	PowerMode = PWR_9V;
	lcd_eep_string(BatMode);
	Line2();
	if (hfe[0] < BAT_WEAK) {						// 930 was 650 Goes weak at 7.7v Input.
		//lcd_clear();
		lcd_eep_string(Bat);						// Message - "Battery €"
		if(hfe[0] < BAT_DEAD) {					// 875 was 600, Vcc < 7.2V
			lcd_eep_string(BatEmpty);				// Message - "empty!€€" - Battery Dead
			_delay_ms(1000);
			PORTD = 0;								// switch off
			return 0;
		}
		lcd_clear();
		lcd_eep_string(BatWeak);					// Message - "weak€€€", Battery weak
		Line2();									// Start second line
	  }
  } else {
    lcd_eep_string(PwrMode);
	Line2();
  }
 
  lcd_eep_string(TestRunning);						// Message - "Testing"
  UpdateProgress("00%");
  //lcd_data((unsigned char)'.');						// Test and Progress
  CheckPins(TP1, TP2, TP3);
  UpdateProgress("16%");
  //lcd_data((unsigned char)'.');
  CheckPins(TP1, TP3, TP2);
  UpdateProgress("33%");
  //lcd_data((unsigned char)'.');
  CheckPins(TP2, TP1, TP3);
  UpdateProgress("50%");
  //lcd_data((unsigned char)'.');
  CheckPins(TP2, TP3, TP1);
  UpdateProgress("66%");
  //lcd_data((unsigned char)'.');
  CheckPins(TP3, TP2, TP1);
  UpdateProgress("83%");
  //lcd_data((unsigned char)'.');
  CheckPins(TP3, TP1, TP2);
  UpdateProgress("99%");
  //lcd_data((unsigned char)'.');
  

//---------------------------------------------CAPACITOR---------------------------------------
									// Separate measurement to the test on condenser
  if(((PartFound == PART_NONE) || (PartFound == PART_RESISTOR) || (PartFound == PART_DIODE)) && (ctmode > 0)) {
									// Condenser unload; otherwise possibly no measurement is possible
    R_PORT = 0;
    R_DDR = (1<<(TP1 * 2)) | (1<<(TP2 * 2)) | (1<<(TP3 * 2));
    _delay_ms(10);
    R_DDR = 0;

    if(ctmode == NORMAL_CAP_TESTS) {					// see if we want to do all 6 Cap Tests
      ReadCapacity(cp1, cp2);						// No - just read the pins both ways.
      ReadCapacity(cp2, cp1);
    } else {								// DEFAULT ctmode == 0x02  to do all tests
	ReadCapacity(TP3, TP1);
	ReadCapacity(TP3, TP2);
	ReadCapacity(TP2, TP3);
	ReadCapacity(TP2, TP1);
	ReadCapacity(TP1, TP3);
	ReadCapacity(TP1, TP2);
      }
   }

  lcd_clear();								// Finished, now evaluate, the results

//---------------------------------------------DIODE------------------------------------------------  
  if(PartFound == PART_DIODE) {
    if(NumOfDiodes == 1) {						// Standard-Diode
      lcd_eep_string(Diode);						// Message - "Diode: "
      lcd_eep_string(Anode);						// Message - "A="
      lcd_data(GetPinAlias(diodes[0].Anode + ASCII_1));				// Display 1, 2, or 3
      lcd_eep_string(NextK);						// Message - ";C="
      lcd_data(GetPinAlias(diodes[0].Cathode + ASCII_1));				// Display 1, 2, or 3
      Line2();								// Start second line
      lcd_eep_string(Uf);						// Message - "Uf="
      lcd_string(itoa(diodes[0].Voltage, outval, 10));
      lcd_eep_string(mV);						// Message - "mV"
      goto end;
    } else if(NumOfDiodes == 2) {					// dual diode
	if(diodes[0].Anode == diodes[1].Anode) {			// Common Anode
	  lcd_eep_string(DualDiode);					// Message - "Double diode €"
	  lcd_eep_string(CA);						// Message - "CA"
	  Line2();							// Start second line
	  lcd_eep_string(Anode);					// Message - "A="
	  lcd_data(GetPinAlias(diodes[0].Anode + ASCII_1));				// Display 1, 2, or 3
	  lcd_eep_string(K1);						// Message - ";C1="
	  lcd_data(GetPinAlias(diodes[0].Cathode + ASCII_1));				// Display 1, 2, or 3
	  lcd_eep_string(K2);						// Message - ";C2="
	  lcd_data(GetPinAlias(diodes[1].Cathode + ASCII_1));			// Display 1, 2, or 3
	  goto end;
	} else if(diodes[0].Cathode == diodes[1].Cathode) {		// Common Cathode
	    lcd_eep_string(DualDiode);					// Message - "Double diode €"
	    lcd_eep_string(CC);						// Message - "CC"
	    Line2(); 							// Start second line
	    lcd_eep_string(K);						// Message - "C="
	    lcd_data(GetPinAlias(diodes[0].Cathode + ASCII_1));			// Display 1, 2, or 3
	    lcd_eep_string(A1);						// Message - ";A1="
	    lcd_data(GetPinAlias(diodes[0].Anode + ASCII_1));			// Display 1, 2, or 3
	    lcd_eep_string(A2);						// Message - ";A2="
	    lcd_data(GetPinAlias(diodes[1].Anode + ASCII_1));			// Display 1, 2, or 3
	    goto end;
	  } else if ((diodes[0].Cathode == diodes[1].Anode) && \
		     (diodes[1].Cathode == diodes[0].Anode)) {		// Antiparallel
	      lcd_eep_string(TwoDiodes);				// Message - "2 diodes"
	      Line2(); 							// Start second line
	      lcd_eep_string(Antiparallel);				// Message - "anti-parallel"
	      goto end;
	    }
    } else if(NumOfDiodes == 3) { 					// Series connection from 2 diodes; as 3 diodes one recognizes
	b = 3;
	c = 3;
									// Check to see if it is series connection of 2 diodes.
									// But 2 cathodes, and 2 anodes must agree.
									// Then the 2 diodes are a single dual-diode.
	if((diodes[0].Anode == diodes[1].Anode) || (diodes[0].Anode == diodes[2].Anode)) 
	  b = diodes[0].Anode;

	if(diodes[1].Anode == diodes[2].Anode) 
	  b = diodes[1].Anode;

	if((diodes[0].Cathode == diodes[1].Cathode) || (diodes[0].Cathode == diodes[2].Cathode)) 
	  c = diodes[0].Cathode;

	if(diodes[1].Cathode == diodes[2].Cathode) 
	  c = diodes[1].Cathode;

	if((b<3) && (c<3)) {
	  lcd_eep_string(TwoDiodes);					// Message - "2 diodes"
	  Line2();							// Start second line
	  lcd_eep_string(InSeries);					// Message - "serial A=€€"
	  lcd_data(GetPinAlias(b + ASCII_1));					// Display 1, 2, or 3
	  lcd_eep_string(NextK);					// Message - ";C="
	  lcd_data(GetPinAlias(c + ASCII_1));					// Display 1, 2, or 3
	  goto end;
	}
      }
  } 
  	
//---------------------------------------------TRANSISTOR--------------------------------------------
    else if (PartFound == PART_TRANSISTOR) {
      if(PartReady == 0) {						// 2nd examination never made, e.g. a transistor with protection diode.
	hfe[1] = hfe[0];
	uBE[1] = uBE[0];
      }

      if((hfe[0]>hfe[1])) {						// If the amplification factor with the first test was higher: swap C and E
	hfe[1] = hfe[0];
	uBE[1] = uBE[0];
	tmp = c;
	c = e;
	e = tmp;
      }

      if(PartMode == PART_MODE_NPN) 
	lcd_eep_string(NPN);						// Message - "NPN"
      else 
	lcd_eep_string(PNP);						// Message - "PNP"

      lcd_eep_string(bstr);						// Message - " B="
      lcd_data(GetPinAlias(b + ASCII_1));						// Display 1, 2, or 3
      
      lcd_eep_string(cstr);						// Message - ";C="
      lcd_data(GetPinAlias(c + ASCII_1));						// Display 1, 2, or 3
      
      lcd_eep_string(estr);						// Message - ";E="
      lcd_data(GetPinAlias(e + ASCII_1));						// Display 1, 2, or 3
      
      Line2(); 								// Start second line
									// Amplification factor compute, hFE = Emitter current/base current
      lhfe = hfe[1];
      lhfe *= (((unsigned long)rhval * 100) / (unsigned long)rlval);	// 500000/750 = 666.666r
      

      if(uBE[1]<11) 
	uBE[1] = 11;

      lhfe /= uBE[1];
      hfe[1] = (unsigned int) lhfe;
      lcd_eep_string(hfestr);						// Message - "hFE="
      lcd_string(utoa(hfe[1], outval, 10));
      SetCursor(2,7);							// Cursor on line 2, character 7

      if(NumOfDiodes > 2) 						// Transistor with protection diode
	lcd_data(LCD_CHAR_DIODE);					// Diode indicate
      else
	lcd_data(' ');

      for(c=0;c<NumOfDiodes;c++) {
	if(( (diodes[c].Cathode == e) && (diodes[c].Anode == b) && \
	     (PartMode == PART_MODE_NPN)) || ((diodes[c].Anode == e) && \
	     (diodes[c].Cathode == b) && (PartMode == PART_MODE_PNP))) {
	  lcd_eep_string(Uf);						// Message - "Uf="
	  lcd_string(itoa(diodes[c].Voltage, outval, 10));
	  lcd_data('m');
	  goto end;
	}
      }

      goto end;
      } 

//---------------------------------------------FET---------------------------------------------------     
	else if (PartFound == PART_FET) {				// JFET or MOSFET
	  if(PartMode & 1)						// N-channel
	    lcd_data('N');
	  else 
	    lcd_data('P');						// P-channel

	  if((PartMode == PART_MODE_N_D_MOS) || (PartMode == PART_MODE_P_D_MOS)) {
	    lcd_eep_string(dmode);					// Message - "-D"
	    lcd_eep_string(mosfet);					// Message - "-MOS"
	    } else {
		if((PartMode == PART_MODE_N_JFET) || (PartMode == PART_MODE_P_JFET)) 
		  lcd_eep_string(jfet);					// Message - "-JFET"
		else {
		  lcd_eep_string(emode);				// Message - "-E"
		  lcd_eep_string(mosfet);				// Message - "-MOS"
		}
	    }
									// Gate capacity
	  if(PartMode < 3) {						// Enrichment MOSFET
	    lcd_eep_string(GateCap);					// Message - " C="
	    ReadCapacity(b,e);						// Measurement
	    hfe[0] = (unsigned int)cv;

	    if(hfe[0]>2) 
	      hfe[0] -= 3;

	    utoa(hfe[0], outval2, 10);
	    tmpval = strlen(outval2);
	    tmpval2 = tmpval;

	    if(tmpval>4) 
	      tmpval = 4;						// If capacity > 100nF drop fractional part to fit on the LCD

	    lcd_show_format_cap(outval2, tmpval, tmpval2);
	    lcd_data('n');
	  }

	  Line2();							// Start second line
	  lcd_eep_string(gds);						// Message - "GDS="
	  lcd_data(GetPinAlias(b + ASCII_1));					// Display 1, 2, or 3
	  lcd_data(GetPinAlias(c + ASCII_1));					// Display 1, 2, or 3
	  lcd_data(GetPinAlias(e + ASCII_1));					// Display 1, 2, or 3

	  if((NumOfDiodes > 0) && (PartMode < 3))			// MOSFET with protection diode; it gives only with enrichment FETs 
	    lcd_data(LCD_CHAR_DIODE);					// Diode indicate
	  else 
	    lcd_data(' ');						// Blank

	  if(PartMode < 3) {						// Enrichment MOSFET
	    gthvoltage=(gthvoltage/8);
	    lcd_eep_string(vt);						// Message - "Vt="
	    lcd_string(utoa(gthvoltage, outval, 10));			// Gate threshold voltage, was determined before
	    lcd_data('m');
	  }

	  goto end;


      } 

//---------------------------------------------THYRISTOR---------------------------------------------     
	else if (PartFound == PART_THYRISTOR) {
	  lcd_eep_string(Thyristor);			 		// Message - "Thyristor"
	  Line2();						 	// Start second line
	  lcd_eep_string(GAK);				 		// Message - "GAC="
	  lcd_data(GetPinAlias(b + ASCII_1));					// Display 1, 2, or 3
	  lcd_data(GetPinAlias(c + ASCII_1));					// Display 1, 2, or 3
	  lcd_data(GetPinAlias(e + ASCII_1));					// Display 1, 2, or 3
	  goto end;

	} 

//---------------------------------------------TRIAC-------------------------------------------------	
	  else if (PartFound == PART_TRIAC) {
	    lcd_eep_string(Triac);					// Message - "Triac"
	    Line2();							// Start second line
	    lcd_eep_string(Gate);					// Message - "G="
	    lcd_data(GetPinAlias(b + ASCII_1));					// Display 1, 2, or 3
	    lcd_eep_string(A1);						// Message - ";A1="
	    lcd_data(GetPinAlias(e + ASCII_1));					// Display 1, 2, or 3
	    lcd_eep_string(A2);						// Message - ";A2="
	    lcd_data(GetPinAlias(c + ASCII_1));					// Display 1, 2, or 3
	    goto end;

	  } 

//---------------------------------------------RESISTOR----------------------------------------------	  
	    else if(PartFound == PART_RESISTOR) {
	      lcd_eep_string(Resistor);					// Message - "Resistor: €€"
	      lcd_data(GetPinAlias(ra + ASCII_1));					// Display 1, 2, or 3 Pin data
	      lcd_data('-');
	      lcd_data(GetPinAlias(rb + ASCII_1));					// Display 1, 2, or 3
	      Line2();							// Start second line

	      if(rv[0] > HALF_ADC_RANGE) 				// Examine, how far the Voltages across the test resistances deviate from 512 
		hfe[0] = (rv[0] - HALF_ADC_RANGE);
	      else 
		hfe[0] = (HALF_ADC_RANGE - rv[0]);

	      if(rv[1] > HALF_ADC_RANGE) 
		hfe[1] = (rv[1] - HALF_ADC_RANGE);
	      else 
		hfe[1] = (HALF_ADC_RANGE - rv[1]);

	      if(hfe[0] > hfe[1])  {
		radcmax[0] = radcmax[1];
		rv[0] = rv[1];						// Result use, which is more near because of 512 (accuracy improves)
		rv[1] = rhval;						// High - Test resistance
	      } else 
		  rv[1] = rlval;					// Low - Test resistance

	      if(rv[0] == 0) 
		rv[0] = 1;

	      lhfe = (unsigned long)((unsigned long)((unsigned long)rv[1] * \
	                             (unsigned long)rv[0]) / (unsigned long)((unsigned long)radcmax[0] - (unsigned long)rv[0]));	// Resistance compute
	      ultoa(lhfe,outval,10);

	      if(rv[1] == rhval) {					// 470k- Resisted?
		ra = strlen(outval);					// Necessarily, in order to indicate comma

		for(rb=0;rb<ra;rb++) {
		  lcd_data(outval[rb]);

		  if(rb == (ra-2)) 
		    lcd_data(',');					// comma
		}

		lcd_data ('K');						// Kilo ohm, if 470k uses resistance
	      } else 
		  lcd_string(outval);
		    
	      lcd_data(LCD_CHAR_OMEGA);					// Omega for ohms 
	      goto end;

	    } 

//---------------------------------------------CAPACITOR---------------------------------------------	    
	      else if(PartFound == PART_CAPACITOR) {			// Capacitor measurement
		lcd_eep_string(Capacitor);				// Message - "Capacitor: €€"
		lcd_data(GetPinAlias(ca + ASCII_1));					// Display 1, 2, or 3 Pin - Data
		lcd_data('-');
		lcd_data(GetPinAlias(cb + ASCII_1));					// Display 1, 2, or 3
		Line2();						// Start second line
		tmpval2 = 'n';						// n for nF
		    
		if(cv > 99999) {					// Too big
		  cv /= 1000;						// convert to Micro Farads

		  tmpval2 = LCD_CHAR_U;					// change n to greek char for micro
		}

		ultoa(cv, outval, 10);					// outval now a string version of cv
		tmpval = strlen(outval); 
		lcd_show_format_cap(outval, tmpval, tmpval); 
		lcd_data(tmpval2);					// display the SI Suffix
		lcd_data('F');						// F for Farads
		goto end;
	      }

//---------------------------------------------NOT-FOUND-OR-DAMAGED---------------------------------------------------------	
		if(NumOfDiodes == 0) {					// No diodes found
		lcd_eep_string(TestFailed1);				// Message - "No, unknown, or€" 
		Line2();						// Start second line
		lcd_eep_string(TestFailed2);				// Message - "damaged €€€€"
		lcd_eep_string(Bauteil);				// Message - "part€€€€€€"
		} else {
		  lcd_eep_string(Bauteil);				// Message - "part€€€€€€"
		  lcd_eep_string(Unknown);				// Message - " unknown€"
		  Line2();						// Start second line
		  lcd_eep_string(OrBroken);				// Message - "or damaged €€"
		  lcd_data(NumOfDiodes + 48);
		  lcd_data(LCD_CHAR_DIODE);
		}

		end:

		while(!(ON_PIN_REG & (1<<RST_PIN)));			// wait, to tracers released
		  _delay_ms(200);

		for(hfe[0] = 0;hfe[0]<10000;hfe[0]++) {			// 10 Seconds untill power off.

		  if(!(ON_PIN_REG & (1<<RST_PIN)))			// if the button is pressed, start all over
		    goto start;

		  wdt_reset();						// We want to wait the full 10 Seconds
		  _delay_ms(1);						// 1mS 10,000 times = 10 seconds
		}

	ON_PORT &= ~(1<<ON_PIN);					// Switch off
	wdt_disable();							// Watchdog out
									// Continuous loop, no timer
  while(1) {
    if(!(ON_PIN_REG & (1<<RST_PIN)))					// only one reaches, if the automatic disconnection was not inserted
      goto start;
  }
  
  return 0;
}									// End of main()

void CheckPins(uint8_t HighPin, uint8_t LowPin, uint8_t TristatePin) {	// Do the tests on the Probe pins, get device characteristics
									// HighPin   - Starts at H, logic 1.
									// LowPin    - Starts at L, logic 0.
									// TriState  - Starts Hi-Z, is put both H and L during the tests.
unsigned int adcv[6];
uint8_t tmpval, tmpval2;
									// HighPin is Vcc, LowPin is Gnd, TriState is HiZ
  wdt_reset();
									// Pin set
  tmpval = (LowPin * 2);						// necessarily because of the arrangement of the resistances
  R_DDR = (1<<tmpval);							// Low pin on exit and over R_L to ground
  R_PORT = 0;
  ADC_DDR = (1<<HighPin) | (1<<TxD);					// High pin on exit
  ADC_PORT = (1<<HighPin) | TXD_VAL;					// High pin to Vcc
  _delay_ms(5);
									// With some MOSFETs the gate (TriState act pin) must be unloaded first
									// N-channel:
  DischargePin(TristatePin,0);						
  adcv[0] = ReadADC(LowPin);						// Read the Voltage at the Low pin
  
  if(adcv[0] < 200) 
    goto next;								// Does the Device close now?
									
  DischargePin(TristatePin,1);						// otherwise: Unloaded for p-channel (gate on pluses)
  adcv[0] = ReadADC(LowPin);						// Read the Voltage at the Low pin

  next:

  if(adcv[0] > 19) {							// Device leads something without control current
									// Test on N-JFET or leading N-MOSFET
    R_DDR |= (2<<(TristatePin*2));					// Tristate pin (assumed gate) over R_H to ground
    _delay_ms(20);
    adcv[1] = ReadADC(LowPin);						// Voltage at the assumed SOURCE measure
    R_PORT |= (2<<(TristatePin*2));					// Tristate pin (assumed gate) over R_H on pluses
    _delay_ms(20);
    adcv[2] = ReadADC(LowPin);						// Voltage at the assumed SOURCE measure again
									// If it concerns a leading MOSFET or JFET, would have adcv [1] > adcv [0] its
    if(adcv[2]>(adcv[1]+100)) {						// Voltage at the gate measure, to the distinction between MOSFET and JFET
      ADC_PORT = TXD_VAL;
      ADC_DDR = (1<<LowPin) | (1<<TxD);					// Low pin to ground
      tmpval = (HighPin * 2);						// necessarily because of the arrangement of the resistances
      R_DDR |= (1<<tmpval);						// High pin on exit
      R_PORT |= (1<<tmpval);						// High pin over R_L on Vcc
      _delay_ms(20);
      adcv[2] = ReadADC(TristatePin);					// Voltage at the assumed gate measure

      if(adcv[2]>800) {							// MOSFET
	PartFound = PART_FET;						// N-channel-MOSFET
	PartMode = PART_MODE_N_D_MOS;					// Depletion MOSFET
      } else {								// JFET (pn transition between G and S leads)
	PartFound = PART_FET;						// N-channel-JFET
	PartMode = PART_MODE_N_JFET;
      }
      
      b = TristatePin;
      c = HighPin;
      e = LowPin;
    }
    
    ADC_PORT = TXD_VAL;

									// Test on P-JFET or leading P-MOSFET
    ADC_DDR = (1<<LowPin) | (1<<TxD);					// Low pin (assumed drain) to ground, Tristate pin (assumed gate) still is over R_H on pluses
    tmpval = (HighPin * 2);						// necessarily because of the arrangement of the resistances
									
    R_DDR |= (1<<tmpval);						// High pin on exit
    R_PORT |= (1<<tmpval);						// High pin over R_L on Vcc
    _delay_ms(20);
    adcv[1] = ReadADC(HighPin);						// Voltage at the assumed SOURCE measure
									
    R_PORT = (1<<tmpval);						// Tristate pin (assumed gate) over R_H to ground
    _delay_ms(20);
    adcv[2] = ReadADC(HighPin);						// Voltage at the assumed SOURCE measure again
									// - If it concerns a leading P-MOSFET or P-JFET, would have adcv [0] > adcv [1] its
    if(adcv[1]>(adcv[2]+100)) {
									// - Voltage at the gate measure, for distinction between MOSFET and JFET
      ADC_PORT = (1<<HighPin) | TXD_VAL;				// High pin firmly on pluses
      ADC_DDR = (1<<HighPin) | (1<<TxD);				// High pin on exit
      _delay_ms(20);
      adcv[2] = ReadADC(TristatePin);					// Voltage at the assumed gate measure
      
      if(adcv[2]<200) {							// MOSFET
	PartFound = PART_FET;						// P-channel-MOSFET 
	PartMode = PART_MODE_P_D_MOS;					// Depletion MOSFET
      } else {								// JFET (pn transition between G and S leads)
	PartFound = PART_FET;						// P-channel-JFET
	PartMode = PART_MODE_P_JFET;
      }

      b = TristatePin;
      c = LowPin;
      e = HighPin;
    }
  }
									// Pins erneut setzen - Pin set again
  tmpval = (LowPin * 2);						// necessarily because of the arrangement of the resistances
  R_DDR = (1<<tmpval);							// Low pin on exit and over R_L to ground
  R_PORT = 0;
  ADC_DDR = (1<<HighPin) | (1<<TxD);					// High pin on exit
  ADC_PORT = (1<<HighPin) | TXD_VAL;					// High pin to Vcc
  _delay_ms(5);	

  if(adcv[0] < 200) {							// If the Device does not have a passage between HighPin and LowPin
									// Test auf pnp
    tmpval2 = (TristatePin * 2);					// necessarily because of the arrangement of the resistances
    R_DDR |= (1<<tmpval2);						// Tristate pin over R_L to ground, to the test on pnp
    _delay_ms(2);
    adcv[1] = ReadADC(LowPin);						// Voltage measure
    
    if(adcv[1] > 700) {
									// Device leads => Pnp transistor or the like.
									// Amplification factor in both directions measure
      R_DDR = (1<<tmpval);						// Tristate pin (basis) high impedance
      tmpval2++;
      R_DDR |= (1<<tmpval2);						// Tristate pin (basis) over R_H to ground

      _delay_ms(10);
      adcv[1] = ReadADC(LowPin);					// Voltage at the Low pin (assumed collector) measure
      adcv[2] = ReadADC(TristatePin);					// Base voltage measure
									// Examine whether test already times run
      if((PartFound == PART_TRANSISTOR) || (PartFound == PART_FET)) 
	PartReady = 1;

      hfe[PartReady] = adcv[1];
	uBE[PartReady] = adcv[2];

      if(PartFound != PART_THYRISTOR) {
	if(adcv[2] > 200) {
	  PartFound = PART_TRANSISTOR;					// PNP transistor found (basis is " " upward; pulled)
	  PartMode = PART_MODE_PNP;
	} else {
	    if(adcv[0] < 20) {						// - Durchlassspannung in the closed condition small enough? (otherwise D-mode-FETs are falsely recognized as E-mode)
	      PartFound = PART_FET;					// P-channel-MOSFET found (basis/gate is not " " upward; pulled)
	      PartMode = PART_MODE_P_E_MOS;
									// Measurement of the gate threshold voltage
	      tmpval = (1<<LowPin);
	      tmpval2 = R_DDR;
	      ADMUX = TristatePin | (1<<REFS0);
	      gthvoltage = 0;

	      for(b=0;b<13;b++) {
		wdt_reset();
		DischargePin(TristatePin,1);

		while (!(ADC_PIN&tmpval));				// Control rooms, until the MOSFET scolded and on high goes to drain

		R_DDR = 0;
		ADCSRA |= (1<<ADSC);

		while (ADCSRA&(1<<ADSC));
		
		gthvoltage += (MAX_ADC - ADCW);
		R_DDR = tmpval2;
	      }

	      gthvoltage *= 3;						// Conversion in mV, together with the division by 8 (with the LCD announcement)
	    }
	  }
	  b = TristatePin;
	  c = LowPin;
	  e = HighPin;
	}
      }

									// Tristate (basis assumed) on pluses, to the test on npn
      ADC_PORT = TXD_VAL;						// Low pin to ground
      tmpval = (TristatePin * 2);					// necessarily because of the arrangement of the resistances
      tmpval2 = (HighPin * 2);						// necessarily because of the arrangement of the resistances
      R_DDR = (1<<tmpval) | (1<<tmpval2);				// High pin and Tristate pin on exit
      R_PORT = (1<<tmpval) | (1<<tmpval2);				// High pin and Tristate pin over R_L on Vcc
      ADC_DDR = (1<<LowPin) | (1<<TxD);					// Low pin on exit
      _delay_ms(10);
      adcv[1] = ReadADC(HighPin);					// Voltage at the High pin measure
      
      if(adcv[1] < 500) {
	if(PartReady == 1) goto testend;
									// Device leads => NPN transistor or the like.
									// Test on thyristor:
									// Gate unload
	R_PORT = (1<<tmpval2);						// Tristate pin (gate) over R_L to ground
	_delay_ms(10);
	R_DDR = (1<<tmpval2);						// Tristate pin (gate) high impedance
									
	_delay_ms(5);
	adcv[3] = ReadADC(HighPin);					// Again Voltage at the High pin (anode assumed) measure

	R_PORT = 0;							// High pin (anode assumed) to ground
	_delay_ms(5);
	R_PORT = (1<<tmpval2);						// High pin (anode assumed) pluses
	_delay_ms(5);
	adcv[2] = ReadADC(HighPin);					// Again Voltage at the High pin (anode assumed) measure

	if((adcv[3] < 500) && (adcv[2] > 900)) {			// After switching the holding current off the thyristor must close
									// was switched before disconnection of triggering Rome and is still switched although gate out => Thyristor
	  PartFound = PART_THYRISTOR;
									//  Test on Triac
	  R_DDR = 0;
	  R_PORT = 0;
	  ADC_PORT = (1<<LowPin) | TXD_VAL;				// Low-Pin fest auf Plus - Low pin firmly on pluses
	  _delay_ms(5);
	  R_DDR = (1<<tmpval2);						// HighPin over R_L to ground
	  _delay_ms(5);

	  if(ReadADC(HighPin) > 50) 
	    goto savenresult;						// - Voltage at the High pin (more assumed a2) measure; if too highly: Device leads now => no triac

	  R_DDR |= (1<<tmpval);						// Gate also over R_L to ground => Triac would have to ignite
	  _delay_ms(5);

	  if(ReadADC(TristatePin) < 200) 
	    goto savenresult;						// - Voltage at the Tristate pin (assumed gate) measure; Abort if Voltage too small

	  if(ReadADC(HighPin) < 150) 
	    goto savenresult;						// Device does not lead now => no triac => Abort

	  R_DDR = (1<<tmpval2);						// TriState act pin (gate) again high impedance
	  _delay_ms(5);

	  if(ReadADC(HighPin) < 150) 
	    goto savenresult;    					// - Device does not lead after switching the gate stream off any longer => no triac => Abort

	  R_PORT = (1<<tmpval2);					// HighPin over R_L on pluses => Holding current out
	  _delay_ms(5);
	  R_PORT = 0;							// HighPin again over R_L to ground; Triac would have to now close
	  _delay_ms(5);

	  if(ReadADC(HighPin) > 50) 
	    goto savenresult;						// - Voltage at the High pin (more assumed a2) measure; if too highly: Device leads now => no triac

	  PartFound = PART_TRIAC;
	  PartReady = 1;
	  goto savenresult;
	}
									// Test on transistor or MOSFET
	tmpval++;
	R_DDR |= (1<<tmpval);						// Tristate pin (basis) on exit
	R_PORT |= (1<<tmpval);						// Tristate pin (basis) over R_H on pluses
	_delay_ms(50);
	adcv[1] = ReadADC(HighPin);					// Voltage at the High pin (assumed collector) measure
	adcv[2] = ReadADC(TristatePin);					// Base voltage measure
  
	if((PartFound == PART_TRANSISTOR) || (PartFound == PART_FET)) 
	  PartReady = 1;						// examine whether test already times run

	hfe[PartReady] = MAX_ADC - adcv[1];
	uBE[PartReady] = MAX_ADC - adcv[2];

	if(adcv[2] < 500) {
	  PartFound = PART_TRANSISTOR;					// NPN transistor found (basis is " " downward; pulled)
	  PartMode = PART_MODE_NPN;
	} else {
	    if(adcv[0] < 20) {						// - Durchlassspannung in the closed condition small enough? (otherwise D-mode-FETs are falsely recognized as E-mode)
	      PartFound = PART_FET;					// N-channel-MOSFET found (basis/gate is not " " downward; pulled)
	      PartMode = PART_MODE_N_E_MOS;
	      tmpval2 = R_DDR;						// Gate threshold voltage measure
	      tmpval=(1<<HighPin);
	      ADMUX = TristatePin | (1<<REFS0);
	      gthvoltage = 0;

	      for(b=0;b<13;b++) {
		wdt_reset();
		DischargePin(TristatePin,0);

		while ((ADC_PIN&tmpval));	 			// Control rooms, until the MOSFET scolded and on low goes to drain 

		R_DDR = 0;
		R_PORT = 0;
		ADCSRA |= (1<<ADSC);

		while (ADCSRA&(1<<ADSC));

		gthvoltage += ADCW;

		R_DDR = tmpval2;
		R_PORT = tmpval2;
	      }

	      gthvoltage *= 3;						// Conversion in mV, together with the division by 8 (with the LCD announcement)
	    }
	  }

	  savenresult:
	  b = TristatePin;
	  c = HighPin;
	  e = LowPin;
	}

	ADC_DDR = (1<<TxD);
	ADC_PORT = TXD_VAL;
									// Finished
	} else {							// Passage
									// Test auf Diode
	    tmpval2 = (2<<(2*HighPin));					// R_H
	    tmpval = (1<<(2*HighPin));					// R_L
	    ADC_PORT = TXD_VAL;
	    ADC_DDR = (1<<LowPin) | (1<<TxD);				// Low pin to ground, High pin still is over R_L on Vcc
	    DischargePin(TristatePin,1);				// Unloaded for P-channel-MOSFET
	    _delay_ms(5);
	    adcv[0] = ReadADC(HighPin) - ReadADC(LowPin);
	    R_DDR = tmpval2;						// High pin over R_H on pluses
	    R_PORT = tmpval2;
	    _delay_ms(5);
	    adcv[2] = ReadADC(HighPin) - ReadADC(LowPin);
	    R_DDR = tmpval;						// High pin over R_L on pluses
	    R_PORT = tmpval;
	    DischargePin(TristatePin,0);				// Unloaded for N-channel-MOSFET
	    _delay_ms(5);
	    adcv[1] = ReadADC(HighPin) - ReadADC(LowPin);
	    R_DDR = tmpval2;						// High pin over R_H on pluses
	    R_PORT = tmpval2;
	    _delay_ms(5);
	    adcv[3] = ReadADC(HighPin) - ReadADC(LowPin);
		/*      Without unloading it can come to wrong identifications, since the gate of a MOSFETs can be still loaded. 
		        The additional measurement with " großen" R_H resisted is accomplished, around antiparallel diodes of 
		        Resistances differentiate to be able.
		        A diode has a Durchlassspg relatively independent of the passage stream. 
		        With a resistance the voltage drop changes strongly (linear) with the river. 
		*/
	    if(adcv[0] > adcv[1]) {
	      adcv[1] = adcv[0];					// the higher value wins
	      adcv[3] = adcv[2];
	    }

	    if((adcv[1] > 30) && (adcv[1] < 950)) {			// Voltage is over 0,15V and under 4,64V => Ok one 
	      if((PartFound == PART_NONE) || (PartFound == PART_RESISTOR)) 
		PartFound = PART_DIODE;					// Diode by default.
									// Otherwise there would be problems with transistors with protection diode 
	      diodes[NumOfDiodes].Anode = HighPin;
	      diodes[NumOfDiodes].Cathode = LowPin;
	      diodes[NumOfDiodes].Voltage = (adcv[1]*54/11);		// - Multiply by approx 4,9, in order to receive from the ADC the Voltage in millivolts
	      NumOfDiodes++;

	      for(uint8_t i=0;i<NumOfDiodes;i++) {
		if((diodes[i].Anode == LowPin) && (diodes[i].Cathode == HighPin)) {	// two antiparallel diodes: Defectively or duo LED 
		  if((adcv[3]*64) < (adcv[1] / 5)) {			// Durchlassspannung falls with smaller test stream strongly off => Defectively
		    if(i<NumOfDiodes) {
		      for(uint8_t j=i;j<(NumOfDiodes-1);j++) {diodes[j].Anode = diodes[j+1].Anode;\
			                                      diodes[j].Cathode = diodes[j+1].Cathode;\
			                                      diodes[j].Voltage = diodes[j+1].Voltage;
		      }
		    }

		    NumOfDiodes -= 2;
	          }
	        }
	      }
	    }
	}

									// Test on resistance
	tmpval2 = (2<<(2*HighPin));					// R_H
	tmpval = (1<<(2*HighPin));					// R_L
	ADC_PORT = TXD_VAL;
	ADC_DDR = (1<<LowPin) | (1<<TxD);				// Low pin to ground
	R_DDR = tmpval;							// High pin over R_L on pluses
	R_PORT = tmpval;
	adcv[2] = ReadADC(LowPin);
	adcv[0] = ReadADC(HighPin) - adcv[2];
	R_DDR = tmpval2;						// High pin over R_H on pluses
	R_PORT = tmpval2;
	adcv[3] = ReadADC(LowPin);
	adcv[1] = ReadADC(HighPin) - adcv[3];

									// Measurement of the voltage difference between the positive terminal of R_L and R_H and Vcc
	tmpval2 = (2<<(2*LowPin));					// R_H
	tmpval = (1<<(2*LowPin));					// R_L
	ADC_DDR = (1<<HighPin) | (1<<TxD);				// High pin on exit
	ADC_PORT = (1<<HighPin) | TXD_VAL;				// High pin firmly on pluses
	R_PORT = 0;
	R_DDR = tmpval;							// Low pin over R_L to ground
	adcv[2] += (MAX_ADC - ReadADC(HighPin));
	R_DDR = tmpval2;						// Low pin over R_H to ground
	adcv[3] += (MAX_ADC - ReadADC(HighPin));

	if(((adcv[0] - adcv[2]) < 900) && ((adcv[1] - adcv[3]) > 20)) goto testend; 	// Voltage drops with small test stream not far enough

	if(((adcv[1] * 32) / 31) < adcv[0]) {				// - Sloping Voltage does not drop with smaller test stream strongly and it exists " Almost Kurzschluss" => Resistance
	  if((PartFound == PART_DIODE) || (PartFound == PART_NONE) || (PartFound == PART_RESISTOR)) {
	    if((tmpPartFound == PART_RESISTOR) && (ra == LowPin) && (rb == HighPin)) {
					/* The Device was tested already once with reverse polarity. Now compare both results with one another.
					If they are quite similar, it concerns (in all probability) a resistance. 
					*/
	      if(!((((adcv[0] + 100) * 6) >= ((rv[0] + 100) * 5)) &&  \
		   (((rv[0] + 100) * 6) >= ((adcv[0] + 100) * 5)) &&  \
		   (((adcv[1] + 100) * 6) >= ((rv[1] + 100) * 5)) &&  \
		   (((rv[1] + 100) * 6) >= ((adcv[1] + 100) * 5)))) {	// min. 20% deviation => no resistance
		tmpPartFound = PART_NONE;
		goto testend;
	      }

	      PartFound = PART_RESISTOR;
	    }

	    rv[0] = adcv[0];
	    rv[1] = adcv[1];

	    radcmax[0] = MAX_ADC - adcv[2];				// - V at the Low pin is not completely zero, but approximately 0,1V (however one measures).
	    radcmax[1] = MAX_ADC - adcv[3];
	    ra = HighPin;
	    rb = LowPin;
	    tmpPartFound = PART_RESISTOR;
	  }
	}

	testend:
	ADC_DDR = (1<<TxD);
	ADC_PORT = TXD_VAL;
	R_DDR = 0;
	R_PORT = 0;
}									// End of CheckPins()


void ReadCapacity(uint8_t HighPin, uint8_t LowPin) {
  if(PartFound == PART_CAPACITOR) 
    goto end;								// Already a condenser found
  
  unsigned long gcval = 0;
  unsigned int tmpint = 0;
  uint8_t extcnt = 0;
  uint8_t tmpx = 0;

  tmpval2 = (2<<(2*HighPin));						// R_H
  tmpval = (1<<(2*HighPin));						// R_L
  ADC_PORT = TXD_VAL;
  R_PORT = 0;
  R_DDR = 0;
  ADC_DDR = (1<<LowPin) | (1<<TxD);					// Low pin to ground
  R_DDR = tmpval2;							// HighPin over R_H to ground
  _delay_ms(5);
  adcv[0] = ReadADC(HighPin);
  DischargePin(HighPin,1);
  adcv[2] = ReadADC(HighPin);
  _delay_ms(5);
  adcv[1] = ReadADC(HighPin);
  wdt_reset();

  if((adcv[1] > (adcv[0] + 1)) || (adcv[2] > (adcv[0] + 1))) {		// Voltage rose
    R_DDR = tmpval;							// High pin over R_L to gnd
    
    while(ReadADC(HighPin) > (ReadADC(LowPin) + 10)) {
      wdt_reset();
      tmpint++;

      if(tmpint == 0) {
	extcnt++;

	if(extcnt == 30) 
	  break;							// High pin over R_L to gnd
      }
    }
    
    tmpint = 0;
    extcnt = 0;
    R_PORT = tmpval;							// High pin over R_L on pluses
    _delay_ms(5);
    adcv[2] = ReadADC(HighPin);
    _delay_ms(80);
    adcv[3] = ReadADC(HighPin);
    
    if((adcv[3] < (adcv[2] + 3)) && (adcv[3] < 850)) 
      goto end;								// Voltage is not considerably increasedly => Abort
    
    if((NumOfDiodes > 0) && (adcv[3] > 950) && (PartFound != PART_FET)) 
      goto end; 							// in all probability (or several) a diode (n) in check direction, which is otherwise falsely recognized as condenser
    	
    R_PORT = 0;
    
    while(ReadADC(HighPin) > (ReadADC(LowPin) + 10)) {
      wdt_reset();
      tmpint++;
      if(tmpint == 0) {
	extcnt++;

	if(extcnt == 30) 
	  break;							// Timeout for unloading
      }
    }
    
    tmpint = 0;
    extcnt = 0;
    ADC_DDR = 7 | (1<<TxD);						// all pins on exit and from gnd
    R_PORT = tmpval;							// HighPin over R_L on pluses
    tmpval=(1<<HighPin);
    _delay_ms(2);
    ADC_DDR = (1<<LowPin) | (1<<TxD);					// Condenser over R_L slowly load
    
    while (!(ADC_PIN & tmpval)) {					// Control rooms, until HighPin goes on High; Loop lasts 7 cycles
      wdt_reset();
      tmpint++;
      
      if(tmpint == 0) {
	extcnt++;

	if(extcnt == 30) 
	  break;							// Timeout for load
      }
    }

    if((extcnt == 0) && (tmpint<256)) {					// Low capacity
      ADC_DDR = (1<<LowPin) | (1<<TxD);	
									// with R_H measure again
      R_PORT = 0;
      tmpint = 0;
      extcnt = 0;

      while(ReadADC(HighPin) > (ReadADC(LowPin) + 10)) {
	wdt_reset();
	tmpint++;

	if(tmpint == 0) {
	  extcnt++;

	  if(extcnt == 30) 
	    break;							// Timeout for unloading
	}
      }

      tmpint = 0;
      extcnt = 0;
      ADC_DDR = 7 | (1<<TxD);						// all pins on exit
      ADC_PORT = TXD_VAL;						// all pins to ground
      R_DDR = tmpval2;							// HighPin over R_H on exit
      R_PORT = tmpval2;							// HighPin over R_H on pluses
      _delay_ms(2);

      if(PartFound == PART_FET) 
	ADC_DDR = (7 & ~tmpval) | (1<<TxD);				// - Condenser over R_H slowly load, free pin (drain) for gate capacity measurement on gnd
      else 
	ADC_DDR = (1<<LowPin) | (1<<TxD);				// Condenser over R_H slowly load

      while (!(ADC_PIN & tmpval)) {					// Control rooms, until HighPin goes on High; Loop lasts 7 cycles
	wdt_reset();
	tmpint++;

	if(tmpint == 0) {
	  extcnt++;

	  if(extcnt == 30) 
	    break;							// Timeout for capacity measurement
	}
      }

      tmpx = 1;
    }

    if(tmpx) {
      gcval = eeprom_read_word(&H_CAPACITY_FACTOR);

      if((extcnt == 0) && (tmpint < 5)) 
	goto end;							// Capacity too small

      cv = 1;
    } else {
	gcval = eeprom_read_word(&L_CAPACITY_FACTOR);
	cv = 1000;
      }

    gcval *= (unsigned long)(((unsigned long)extcnt * 65536) + (unsigned long)tmpint);	// Unrechnen worth and store
    gcval /= 100;
    cv *= gcval;

    PartFound = PART_CAPACITOR;						// Condenser found

    ca = HighPin;
    cb = LowPin;
									// Condenser again unload
    tmpint = 0;
    extcnt = 0;
    R_DDR = (1<<(2*HighPin));						// High pin over R_L to ground
    R_PORT = 0;

    while(ReadADC(HighPin) > (ReadADC(LowPin) + 10)) {
      wdt_reset();
      tmpint++;

      if(tmpint == 0) {
	extcnt++;

	if(extcnt == 30) break;						// Timeout for unloading
      }
    }

    ADC_DDR = 7 | (1<<TxD);						// Timeout for unloading
    ADC_PORT = 7 | TXD_VAL;
    _delay_ms(10);
									// Finished
  }
  
  end:
  ADC_DDR =  (1<<TxD);
  ADC_PORT = TXD_VAL;
  R_DDR = 0;
  R_PORT = 0; 
}									// End of ReadCapacity()


unsigned int ReadADC(uint8_t mux) {					// - ADC value of the indicated channel pick out and as unsigned int back against
unsigned int adcx = 0;

  ADMUX = mux | (1<<REFS0);
  
  for(uint8_t j=0;j<20;j++) {						// 20 measurements; for better accuracy
    ADCSRA |= (1<<ADSC);
    
    while (ADCSRA&(1<<ADSC))
      ;

    adcx += ADCW;
  }

  adcx /= 20;
  return adcx;
}


void DischargePin(uint8_t PinToDischarge, uint8_t DischargeDirection) {
	/* - Connection of an element briefly (10ms) on a certain potential put 
		 This function is intended to unloading of MOSFET Gates, 
		 in order to be able to recognize protection diodes etc. in MOSFETs Parameter: 
		 PinToDischarge: unloading pin DischargeDirection: 
		 0 =  gnd  (N-channel-FET), 
		 1 =  plus (P-channel-FET)
	*/
uint8_t tmpval;
  
  tmpval = (PinToDischarge * 2);					// necessarily because of the arrangement of the resistances

  if(DischargeDirection) 
    R_PORT |= (1<<tmpval);						// R_L out

  R_DDR |= (1<<tmpval);							// Pin on exit and over R_L to ground
  _delay_ms(10);
  R_DDR &= ~(1<<tmpval);						// Pin entrance
  
  if(DischargeDirection) 
    R_PORT &= ~(1<<tmpval);						// R_L out
}


void lcd_show_format_cap(char outval[], uint8_t strlength, uint8_t CommaPos) {
  if(strlength < 3) {
    if(strlength == 1) {
      lcd_string("0.");
      lcd_data('0');
      lcd_data(outval[0]);
    } else {								// strlength == 0, or strlength == 2
	lcd_string("0.");
	lcd_data(outval[0]);
	lcd_data(outval[1]);
      }
  } else {								// strlength >= 3
      for(PartReady = 0; PartReady < strlength; PartReady++) {
	if((PartReady + 2) == CommaPos) 
	  lcd_data('.');						// Should this be a comma?
	
	lcd_data(outval[PartReady]);
      }
    }
}

