// clockblock
// Copyright 2013 by Wiley Cousins, LLC.
// shared under the terms of the MIT License
//
// file: clockblock.cpp
// description: application file for the clockblock

// ************************************
// AVR includes necessary for this file
// ************************************
#include <util/atomic.h>
#include <util/delay.h>

// ********************
// application includes
// ********************
#include "clockblock.h"


// **************************
// INTERRUPT SERVICE ROUTINES
// **************************
// this ISR driven by a 1024 Hz squarewave from the RTC
ISR(RTC_EXT_INT_vect) {
  ms++;
  // tick the display 32 times a second
  if ( !(ms%32) ) {
    tick = true;
    if (ms >= 1024) {
      ms = 0;
    }
  }
}

// ISR for switch inputs
ISR(INPUT_PCINT_vect, ISR_NOBLOCK) {
  buttons.handleChange();
}

// switch debouncer / timer
ISR(TIMER1_COMPA_vect, ISR_NOBLOCK) {
  buttons.handleTimer();
}

// ***********
// application
// ***********
// main
int main(void) {
  // give those ISR volatile vairables some values
  ms = 0;
  tick = false;

  // application variables
  // time vector - { seconds, minutes, hours}
  uint8_t tm[3] = {0, 0, 12};
  // animation frame
  uint8_t fr = 0;
  // arms leds
  uint16_t dots[DISPLAY_NUM_DOTS];

  // initialize the RTC
  //rtc.init();
  // enable a 1024 Hz squarewave output on interrupt pin
  //rtc.enableSquareWave(1);

  // initialize the LED driver
  tlc.init();
  // set the TLC to autorepeat the pattern and to reset the GS counter whenever new data is latched in
  tlc.setFC(TLC5971_DSPRPT);

  // enable a falling edge interrupt on the square wave pin
  cli();
  EICRA = (0x2 << (2*RTC_EXT_INT));
  EIMSK = (1 << RTC_EXT_INT);
  sei();

  // set the display mode
  leds.setMode(DISPLAY_MODE_BLEND);

  // enable inputs
  buttons.init();

  // get lost
  for (;;) {
    uint8_t buttonState;
    // take care of any switch presses
    if (buttons.getPress(&buttonState)) {
      handleButtonPress(buttonState, tm);
    }

    // take care of any switch holds
    if (buttons.getHold(&buttonState)) {
      handleButtonHold(buttonState, tm);
    }

    // update the arms on a tick
    if (tick) {
      // clear the flag
      tick = false;
      // get the time
      if (++fr >= 32) {
        fr = 0;
        if (++tm[0] >= 60) {
          tm[0] = 0;
          if (++tm[1] >= 60) {
            tm[1] = 0;
            if (++tm[2] >= 13) {
              tm[2] = 1;
            }
          }
        }
      }

      // update the clock arms
      updateArms(tm[2], tm[1], tm[0], fr, dots);
    }
  }

  // one day we might get the answer
  return 42;
}


// update the clock arms
// dots array structure: { hr0, mn0, sc0, hr1, mn1, sc1, ... , hr11, mn11, sc11 }
void updateArms(uint8_t hour, uint8_t min, uint8_t sec, uint8_t frame, uint16_t *dots) {
  // get the display
  leds.getDisplay(hour, min, sec, frame, dots);
  // send to the LED driver
  tlc.setGS(dots);
}

// initialize unused pins as inputs with pullups enabled
void initUnusedPins(void) {
  // PORTB
  DDRB &= ~UNUSED_PORTB_MASK;
  PORTB |= UNUSED_PORTB_MASK;
  // PORTC
  //DDRC &= ~UNUSED_PORTC_MASK;
  //PORTC |= UNUSED_PORTC_MASK;
  //PORTD
  //DDRD &= ~UNUSED_PORTD_MASK;
  //PORTD |= UNUSED_PORTD_MASK;
}

// button handling logic
void handleButtonPress(uint8_t state, uint8_t *tm) {
  // if hour switch, increment the hours by 1
  if (state == INPUT_HOUR) {
    tm[2] = (tm[2] + 1) % 12;
    tm[2] = (tm[2] == 0) ? 12 : tm[2];
  }
  // if minute switch, increment minutes
  else if (state == INPUT_MIN) {
    tm[1] = (tm[1] + 1) % 60;
  }
}

void handleButtonHold(uint8_t state, uint8_t *tm) {
  // if the hour switch is held, increment the hours by 3
  if (state == INPUT_HOUR) {
    tm[2] = (tm[2] + 3) % 12;
    tm[2] = (tm[2] == 0) ? 12 : tm[2];
  }
  // else if the minute switch is held, increment the minutes by 5
  else if (state == INPUT_MIN) {
    tm[1] = (tm[1] + 5) % 60;
  }
  // else both buttons were held down, so increment the display mode
  else {
    uint8_t mode = leds.getMode();
    mode = (mode == (DISPLAY_NUM_MODES-1)) ? 0 : (mode + 1);
    leds.setMode(mode);
  }
}
