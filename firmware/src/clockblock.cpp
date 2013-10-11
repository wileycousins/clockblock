// clockblock
// Copyright 2013 by Wiley Cousins, LLC.
// shared under the terms of the MIT License
//
// file: clockblock.cpp
// description: application file for the clockblock

// ************************************
// AVR includes necessary for this file
// ************************************
#include <util/delay.h>

// ********************
// application includes
// ********************
#include "clockblock.h"


// **************************
// INTERRUPT SERVICE ROUTINES
// **************************
// this ISR driver by a 1024 Hz squarewave from the RTC
ISR(INT0_vect) {

  
  ms++;
  // tick the display 32 times a second
  if ( !(ms%32) ) {
    tick = true;
  }
  // tock the time once a second
  if (ms > 1023) {
    tock = true;
    ms = 0;
  }
  
}


#ifdef BREADBOARD
// ISR for serial data input into TLC5940
ISR(TIMER0_COMPA_vect) {
  tlc.refreshGS();
}
#endif


// ***********
// application
// ***********
// main
int main() {
  // give those global vairables some values
  tick = false;
  ms = 0;

  // application variables
  // time vector - { seconds, minutes, hours}
  uint8_t tm[3] = {0, 58, 11};
  // last second measurement - used to sync up the milliseconds
  uint8_t lastSec = 0;

  // initialize the RTC
  rtc.init();
  // enable a 1024 Hz squarewave output on interrupt pin
  rtc.enableSquareWave(1);
  // check if the RTC has a good time
  if(rtc.hasLostTime()) {
    // if it has, assume it's 1:42 AM, because that's when people set up their clocks
    rtc.setTime(DS3234_AM, tm);
  }

  // initialize the LED drivers
  tlc.init();
  #ifdef BREADBOARD
  initTLCTimers();
  #endif

  // enable a falling edge interrupt on the square wave pin
  cli();
  EICRA = (0x2 << (2*RTC_INT));
  EIMSK = (1 << RTC_INT);
  sei();

  // set the display mode
  leds.setMode(DISPLAY_MODE_BLEND);

  // get lost
  for (;;) {
    // check the set time flag
    if (timeSet) {
      
    }

    // update the time on a tock
    if (tock) {
      tock = false;
      // get the time
      rtc.getTime(tm);
    }
    // update the arms on a tick
    if (tick) {
      tick = false;
      // update the clock arms
      updateArms(tm[2], tm[1], tm[0]);
    }
  }

  // one day we might get the answer
  return 42;
}

// update the clock arms
// dots array structure: { hr0, mn0, sc0, hr1, mn1, sc1, ... , hr11, mn11, sc11 }
void updateArms(uint8_t hour, uint8_t min, uint8_t sec) {
  uint16_t dots[DISPLAY_NUM_DOTS];

  leds.getDisplay(hour, min, sec, ms, dots);

  // update the LEDs
  for (uint8_t i=0; i<DISPLAY_NUM_DOTS; i++) {
    tlc.setGS(i, dots[i]);
  }
  tlc.update();
}
