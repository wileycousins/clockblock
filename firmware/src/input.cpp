// clockblock
// Copyright 2013 by Wiley Cousins, LLC.
// shared under the terms of the MIT License
//
// file: input.cpp
// description: class for handling user input into the clockblock

#include "input.h"
#include "pindefs.h"

// contructor - gives private variables default values
Input::Input(void) {
  // switch states
  state = INPUT_MASK;
  pressState = INPUT_MASK;
  // switch timer counter
  timerCount = 0;
  // switch event flags
  release = false;
  press = false;
}

// get switch state
uint8_t Input::getState(void) {
  return PIN(INPUT_PORT) & INPUT_MASK;
}

// get the flag states and clear as necessary
// set uint8_t at pointer to switch state if flag is true
// outputted switch state is bit flipped for convenience in the app
bool Input::getPress(uint8_t *s) {
  if (press && release) {
    press = false;
    release = false;
    *s = INPUT_MASK & ~pressState;
    return true;
  }
  return false;
}

// handle debouncing the pins and sensing presses
void Input::handleTimer(void) {
  // shift the old state over and read in the new one
  state = (state << 4) | getState();
  // compare and increment count if they match
  if( (state >> 4) == (state & 0x0F) ) {
    timerCount++;
  }
  else {
    timerCount = 0;
  }
  // after 3 matches, consider switch debounced
  if (timerCount >= INPUT_DEBOUNCE_COUNT) {
    // if all switches are up, it's a release
    if ( (state & 0x0F) == INPUT_MASK ) {
      release = true;
      timerCount = 0;
    }
    else {
      // else it's not a release
      release = false;
      press = true;
      pressState = state & 0x0F;
      // after INPUT_HOLD_COUNT, start faking releases
      if (timerCount > INPUT_HOLD_COUNT) {
        release = true;
        timerCount = INPUT_REPEAT_COUNT;
      }
    }
  }
}


// initialization
void Input::init(void) {
  initPins();
  initTimer();
  enableTimer();
}

// init switch pins as inputs with pullups enabled
void Input::initPins(void) {
  // clear pins in DDR to inputs
  DDR(INPUT_PORT) &= ~INPUT_MASK;
  // enable pull up resistors
  //INPUT_PORT |= INPUT_MASK;
}

// init timer for debouncing
// using an 16-bit timer on an 8MHz clock
// PS=1024 and OC1A=249 means an CTC will occur after about 32 ms
void Input::initTimer(void) {
  // using timer 1 (16-bit)
  // ensure timer1 settings are cleared out
  TCCR1A = 0;
  // set mode to CTC and prescaler to 1024
  TCCR1B = ( (1 << WGM12) | (1 << CS12) | (1 << CS10) );
  // set top of timer to 99 (for 100 counts / cycle)
  OCR1A = 99;
}


void Input::enableTimer(void) {
  // enable the interrupt
  TIMSK1 = (1 << OCIE1A);
}

void Input::disableTimer(void) {
  // using timer 1 (16-bit)
  TIMSK1 = 0;
}
