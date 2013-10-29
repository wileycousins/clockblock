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
ISR(TIMER2_OVF_vect, ISR_NOBLOCK) {
  buttons.handleTimer();
}

uint16_t dots[DISPLAY_NUM_DOTS];

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
  uint8_t tm[3] = {0, 58, 11};
  // set time vector - { minutes, hours }
  uint8_t set[2] = {0, 0};
  // animation frame
  uint8_t fr = 0;
  // last second measurement - used to sync up the milliseconds
  uint8_t lastSec = 0;
  // set the operating mode
  uint8_t opMode = MODE_CLOCK;
  // arms leds
  uint16_t dots[DISPLAY_NUM_DOTS];

  // initialize the RTC
  rtc.init();
  // enable a 1024 Hz squarewave output on interrupt pin
  rtc.enableSquareWave(1);
  // check if the RTC has a good time
  // if(rtc.hasLostTime()) {
  //   // if it has, assume it's 1:15 AM, because that's when people set up their clocks
  //   tm[2] = 1;
  //   tm[1] = 15;
  //   //rtc.setTime(DS3234_AM, tm);
  // }

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
      opMode = handleButtonPress(buttonState, opMode, set, tm);
    }

    // take care of any switch holds
    if (buttons.getHold(&buttonState)) {
      opMode = handleButtonHold(buttonState, opMode, set, tm);
    }

    // update the arms on a tick
    if (tick) {
      // clear the flag
      tick = false;

      // get the time
      if (fr++ >= 32) {
        fr = 0;
        if (tm[0]++ >= 60) {
          tm[0] = 0;
          if (tm[1]++ >= 60) {
            tm[1] = 0;
            if (tm[2]++ >= 13) {
              tm[2] = 1;
            }
          }
        }
      }

      // update the clock arms
      if (opMode == MODE_CLOCK) {
        updateArms(tm[2], tm[1], tm[0], fr, dots);
      }
      else if (opMode == MODE_TIME_SET) {
        updateArms(set[1], set[0], tm[0], fr, dots);
      }
      else {
        updateArms(tm[2], tm[1], tm[0], fr, dots);
      } 
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

// initialize input pins as inputs with pullups enabled
void initUnusedPins(void) {
  // PORTB
  DDRB &= ~UNUSED_PORTB_MASK;
  PORTB |= UNUSED_PORTB_MASK;
  // PORTC
  DDRC &= ~UNUSED_PORTC_MASK;
  PORTC |= UNUSED_PORTC_MASK;
  //PORTD
  DDRD &= ~UNUSED_PORTD_MASK;
  PORTD |= UNUSED_PORTD_MASK;
}

// button handling logic
uint8_t handleButtonPress(uint8_t state, uint8_t opMode, uint8_t *set, uint8_t* tm) {
  // switch press only matters in time and display set modes
  // time set mode
  if (opMode == MODE_TIME_SET) {
    // if hour switch, increment the hours by 1
    if (state == INPUT_HOUR) {
      set[1] = (set[1] + 1) % 12;
      set[1] = (set[1] == 0) ? 12 : set[1];
    }
    // if minute switch, increment minutes
    else if (state == INPUT_MIN) {
      set[0] = (set[0] + 1) % 60;
    }
  }
  // display set mode
  else if (opMode == MODE_DISPLAY_SET) {
    // if hour switch, increment the mode
    if (state == INPUT_HOUR) {
      uint8_t dispMode = leds.getMode();
      dispMode = (dispMode >= (DISPLAY_NUM_MODES-1)) ? 0 : (dispMode+1);
      leds.setMode(dispMode);
    }
    // if minute switch, decrement the mode
    else if (state == INPUT_MIN) {
      uint8_t dispMode = leds.getMode();
      dispMode = (dispMode == 0) ? (DISPLAY_NUM_MODES-1) : (dispMode-1);
      leds.setMode(dispMode);
    }
  }
  // return the opMode
  return opMode;
}

uint8_t handleButtonHold(uint8_t state, uint8_t opMode, uint8_t *set, uint8_t* tm) {
  // figure out what to do with the hold
  // if we're currently operating as a clock: check for a mode command
  if (opMode == MODE_CLOCK) {
    // if hour switch is held, go into time set mode
    if (state == INPUT_HOUR) {
      opMode = MODE_TIME_SET;
      set[0] = tm[1];
      set[1] = tm[2];
      leds.setMode(DISPLAY_SET_TIME);
    }
    // if minute switch is held, go into display set mode
    else if (state == INPUT_MIN) {
      opMode = MODE_DISPLAY_SET;
      leds.setMode(DISPLAY_SET_MODE);
    }
    // reset the buttons to prevent the hold from continuing to fire
    buttons.reset();
  }
  // if we're currently setting the time
  else if (opMode == MODE_TIME_SET) {
    // if the hour switch is held, increment the hours by 3
    if (state == INPUT_HOUR) {
      set[1] = (set[1] + 3) % 12;
      set[1] = (set[1] == 0) ? 12 : set[1];
    }
    // else if the minute switch is held, increment the minutes by 5
    else if (state == INPUT_MIN) {
      set[0] = (set[0] + 5) % 60;
    }
    // else both buttons were held down, so go back to clock mode
    else {
      opMode = MODE_CLOCK;
      tm[2] = set[1];
      tm[1] = set[0];
      tm[0] = 0;
      //rtc.setTime(tm);
      leds.setMode(DISPLAY_SET_EXIT);
    }
  }
  // if we're in display setting mode
  else if (opMode == MODE_DISPLAY_SET) {
    // both buttons were held down, so go back to clock mode
    if (state == INPUT_MASK) {
      opMode = MODE_CLOCK;
      leds.setMode(DISPLAY_SET_EXIT);
    }
  }
  // return the opMode
  return opMode;
}
