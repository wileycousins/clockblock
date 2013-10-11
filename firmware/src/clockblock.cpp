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
ISR(INT0_vect) {
  tick = true;
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

  // application variables
  // time vector - { seconds, minutes, hours}
  uint8_t tm[3] = {0, 42, 1};

  // initialize the RTC
  rtc.init();
  // check if the RTC has a good time
  if(rtc.hasLostTime()) {
    // if it has, assume it's 1:42 AM, because that's when people set up their clocks
    rtc.setTime(DS3234_AM, tm);
  }
  // enable a 1 Hz squarewave output on interrupt pin
  rtc.enableSquareWave(0);

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

  // get lost
  for (;;) {
    // check the set time flag
    if (timeSet) {
      
    }

    // check the time update
    if (tick) {
      // clear the flag and get the time
      tick = false;
      rtc.getTime(tm);
    
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

  leds.getDisplay(hour, min, sec, dots);
  /*
  // hands
  uint8_t minHand = min/5;
  uint8_t secHand = sec/5;
  // fractions
  uint8_t minFrac = min/12;
  // mods
  uint8_t minMod = min%5;
  uint8_t secMod = sec%5;
  
  // fill the hour dots
  // all hours previous are full
  for (uint8_t i=0; i<hour; i++) {
    dots[i*3] = 5*LVL;
  }
  // current hour to fraction
  dots[hour*3] = LVL*(minFrac+1);
  // all other hours off
  for (uint8_t i=hour+1; i<12; i++) {
    dots[i*3] = 0;
  }

  // do the same with the minute dots
  // all minute dots previous get set to full
  for (uint8_t i=0; i<minHand; i++) {
    dots[(i*3)+1] = 5*LVL;
  }
  // current minute dot to fraction
  dots[(minHand*3)+1] = LVL*(minMod+1);
  // all other minute dots off
  for (uint8_t i=minHand+1; i<12; i++) {
    dots[(i*3)+1] = 0;
  }

  // finally, seconds
  // all second dots previous get set to full
  for (uint8_t i=0; i<secHand; i++) {
    dots[(i*3)+2] = 5*LVL;
  }
  // current second dot to fraction
  dots[(secHand*3)+2] = LVL*(secMod+1);
  // all other second dots off
  for (uint8_t i=secHand+1; i<12; i++) {
    dots[(i*3)+2] = 0;
  }
  */
  // update the LEDs
  for (uint8_t i=0; i<DISPLAY_NUM_DOTS; i++) {
    tlc.setGS(i, dots[i]);
  }
  tlc.update();
}
