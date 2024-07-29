
// Include WDT libraries
//----------------------
#include <avr/sleep.h>
#include <avr/wdt.h>

// Makes sure A/D registers are defined if not done in libraries
//--------------------------------------------------------------
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

// Define I/Os
//------------
#define Led   0     //Led Pin  PB0

// Define variables used in ISR
//-----------------------------
volatile boolean f_wdt = 1;

//===================================================
// Setup
//===================================================
void setup() {
  // Setup I/Os
  //-----------
  pinMode(Led, OUTPUT);
  
  // Set initial state to 8 sec interrupts
//===================================================
// Set up watchdog interrupt time
// 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms
// 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec
//===================================================

  setup_watchdog(9);        //Change
}

//===================================================
// Main Loop
//===================================================
void loop() {

  // // wait for timed out watchdog / flag is set when a watchdog timeout occurs

  if (f_wdt == 1) {
    // Reset flag
    f_wdt = 0;

    // Flash LED once every 8second for 10ms
  
    digitalWrite(Led, HIGH);
    delay(10);               //Change
    digitalWrite(Led, LOW);

    // Set all used port to intput to save power

    pinMode(Led, INPUT);

    // Go to sleep

    system_sleep();

    // Set all ports into state before sleep

    pinMode(Led, OUTPUT);
  }
}

//===================================================================
// Watchdog Interrupt Service / is executed when watchdog timed out
//===================================================================

ISR(WDT_vect) {
  f_wdt = 1;      // set global flag
}


void setup_watchdog(int ii) {
  byte bb;
  int ww;
  if (ii > 9 ) ii = 9;
  bb = ii & 7;
  if (ii > 7) bb |= (1 << 5);
  bb |= (1 << WDCE);
  ww = bb;

  MCUSR &= ~(1 << WDRF);
  // start timed sequence
  WDTCR |= (1 << WDCE) | (1 << WDE);
  // set new watchdog timeout value
  WDTCR = bb;
  WDTCR |= _BV(WDIE);
}

//===================================================================
// Set system into the sleep state
// System wakes up when watchdog is timed out
//===================================================================
void system_sleep() {
  cbi(ADCSRA, ADEN);                   // switch Analog to Digital converter OFF
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();
  sleep_mode();                        // System sleeps here

  // Wake-up point after watchdog timed out
  sleep_disable();                     // System continues execution here when watchdog timed out
  //sbi(ADCSRA,ADEN);                  // switch Analog to Digitalconverter ON
}

//------------END-----------------
