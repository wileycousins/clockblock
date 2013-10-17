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
#include <util/atomic.h>

// ********************
// application includes
// ********************
#include "clockblock.h"


// **************************
// INTERRUPT SERVICE ROUTINES
// **************************
// this ISR driven by a 1024 Hz squarewave from the RTC
ISR(INT0_vect) {
  ms++;
  // tick the display 32 times a second
  if ( !(ms%32) ) {
    tick = true;
  }
}

#ifdef BREADBOARD
// ISR for serial data input into TLC5940
ISR(TIMER0_COMPA_vect, ISR_NOBLOCK) {
  // disable this ISR
  TIMSK0 &= ~(1 << OCIE0A);
  // refresh the data
  tlc.refreshGS();
  // re-enable this ISR
  TIMSK0 |= (1 << OCIE0A);
}
#endif

// ISR for switch inputs
ISR(PCINT1_vect, ISR_NOBLOCK) {
  // disable the pin change interrupt
  INPUT_PCICR &= ~INPUT_PCIE;
  // save the switch levels
  uint8_t hourSw = INPUT_PIN & INPUT_HOUR_SET;
  uint8_t minSw = INPUT_PIN & INPUT_MIN_MODE;
  // debounce
  _delay_ms(20);
  // compare levels and act appropriately
  if ( !(hourSw) && (hourSw == (INPUT_PIN & INPUT_HOUR_SET)) ) {
    set[1]++;
    timeSet = true;
  }
  if ( !(minSw) && (minSw == (INPUT_PIN & INPUT_MIN_MODE)) ) {
    set[0]++;
    timeSet = true;
  }
  // re-enable pin change interrupt
  INPUT_PCICR |= INPUT_PCIE;
}


// ***********
// application
// ***********
// main
int main(void) {
  // give those global vairables some values
  tick = false;
  ms = 0;

  // application variables
  // time vector - { seconds, minutes, hours}
  uint8_t tm[3] = {0, 58, 11};
  set[0] = 0;
  set[1] = 1;
  // animation frame
  uint8_t fr = 0;
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
  leds.setMode(DISPLAY_MODE_SET);

  // enable inputs
  initPins();

  // get lost
  for (;;) {
    // check the set time flag
    if (timeSet) {
      timeSet = false;
      tm[2] = (tm[2] + set[1]) % 12;
      tm[2] = (tm[2] == 0) ? 12 : tm[2];
      tm[1] = (tm[1] + set[0]) % 60;
      rtc.setTime(DS3234_AM, tm);
      set[0] = 0;
      set[1] = 0;
    }
    // update the arms on a tick
    if (tick) {
      tick = false;
      // increment the frame
      fr++;
      // get the time
      rtc.getTime(tm);
      // reset milliseconds if new second
      if (tm[0] != lastSec) {
        lastSec = tm[0];
        fr = 0;
        ATOMIC_BLOCK(ATOMIC_FORCEON) {
          ms = 0;
        }
      }
      // update the clock arms
      updateArms(tm[2], tm[1], tm[0], fr);
    }
  }

  // one day we might get the answer
  return 42;
}

// update the clock arms
// dots array structure: { hr0, mn0, sc0, hr1, mn1, sc1, ... , hr11, mn11, sc11 }
void updateArms(uint8_t hour, uint8_t min, uint8_t sec, uint8_t frame) {
  uint16_t dots[DISPLAY_NUM_DOTS];

  leds.getDisplay(hour, min, sec, frame, dots);

  // update the LEDs
  for (uint8_t i=0; i<DISPLAY_NUM_DOTS; i++) {
    tlc.setGS(i, dots[i]);
  }
  tlc.update();
}

// initialize input pins as inputs with pullups enabled
void initPins(void) {
  // clear pins in DDR to inputs
  INPUT_DDR &= ~( INPUT_HOUR_SET | INPUT_MIN_MODE );
  // enable pull up resistors
  INPUT_PORT |= ( INPUT_HOUR_SET | INPUT_MIN_MODE );

  // enable pin change interrupt on these pins
  INPUT_PCMSK |= ( INPUT_HOUR_SET | INPUT_MIN_MODE );
  INPUT_PCICR |= INPUT_PCIE;
}
