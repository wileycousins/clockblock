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
  holdState = INPUT_MASK;
  // switch timer counter
  //timerCount = 0;
  // switch event flags
  release = false;
  press = false;
  hold = false;
}

// get switch state
uint8_t Input::getState(void) {
  return PIN(INPUT_PORT) & INPUT_MASK;
}

// get the flag states and clear as necessary
// set uint8_t at pointer to switch state if flag is true
// outputted switch state is bit flipped for convenience in the app
bool Input::getHold(uint8_t *s) {
  if (hold) {
    hold = false;
    *s = INPUT_MASK & ~holdState;
    return true;
  }
  return false;
}

bool Input::getPress(uint8_t *s) {
  if (press && release) {
    press = false;
    release = false;
    *s = INPUT_MASK & ~pressState;
    return true;
  }
  return false;
}

// handle pin change
void Input::handleChange(void) {
  // disable the timer and switch interrupts and reset the switch timer counter
  disableTimer();
  disableInt();
  //timerCount = 0;
  // save the state that triggered the interrupt
  state = getState();
  // start the switch timer to debounce and time if necessary
  enableTimer();
}

// handle debouncing the pins and sensing presses vs holds
void Input::handleTimer(void) {
  // disable the timer
  disableTimer();
  // increment the counter
  //timerCount++;
  // clear the release flag
  release = false;

  // check the values still match (i.e. if true, it wasn't a bounce)
  if (getState() == state) {
    // if one or both switches are down (if both switches are up, both will read high)
    if ( state != INPUT_MASK) {
      // check for a press
      // if hold flag hasn't been set, this was just a press
      if (!hold) {
        press = true;
        pressState = state;
      }
      // check for a hold
      // if press flag has previously been set, it's a hold now
      else if (press) {
        press = false;
        hold = true;
        holdState = state;
        //timerCount = 1;
      }
      // re-enable the timer
      enableTimer();
    }
    // else switches were released
    else {
      release = true;
    }
  }

  // re-enable the pin change interrupt
  enableInt();
}

// disable the debounce timer and wait for a pin change (e.g. to exit a pin hold loop)
//void Input::reset(void) {
// disable the timer and enable the pin change interrupt
//  disableTimer();
//  enableInt();
//}

// initialization
void Input::init(void) {
  initPins();
  initInt();
  initTimer();
  enableInt();
}

// init switch pins as inputs with pullups enabled
void Input::initPins(void) {
  // clear pins in DDR to inputs
  DDR(INPUT_PORT) &= ~INPUT_MASK;
  // enable pull up resistors
  INPUT_PORT |= INPUT_MASK;
}

// init pin change interrupts
void Input::initInt(void) {
  // set up pin change interrupt on the pins
  INPUT_PCMSK |= INPUT_MASK;
}

// init timer for debouncing
// using an 16-bit timer on an 8MHz clock
// PS=1024 and OC1A=249 means an CTC will occur after about 32 ms
void Input::initTimer(void) {
  // using timer 0 (8=bit)
  // set to normal mode and disconnect OC pins
  TCCR0A = 0;
  // set prescaller to 1024
  TCCR0B = ( (1 << CS02) | (1 << CS01) | (1 << CS00) );

  // using timer 1 (16-bit)
  // ensure timer1 settings are cleared out
  // TCCR1A = 0;
  // set mode to CTC and prescaler to 1024
  // TCCR1B = ( (1 << WGM12) | (1 << CS12) | (1 << CS10) );
  // set top of timer to 249 (for 250 counts / cycle)
  // OCR1A = 249;
}

// interrupt helpers
void Input::enableInt(void) {
  // enable the pin change interrupt
  INPUT_PCICR |= INPUT_PCIE;
}

void Input::disableInt(void) {
  // disable the pin change interrupt
  INPUT_PCICR &= ~INPUT_PCIE;
}

void Input::enableTimer(void) {
  // using timer 0 (8-bit)
  // reload timer
  TCNT0 = 0;
  // enable overflow interrupt
  TIMSK0 |= (1 << TOIE0);

  // using timer 1 (16-bit)
  // reload timer
  // TCNT1 = 0;
  // enable the overflow interrupt
  // TIMSK1 = (1 << OCIE1A);
}

void Input::disableTimer(void) {
  // using timer 0 (8-bit)
  TIMSK0 = 0;

  // using timer 1 (16-bit)
  // TIMSK1 = 0;
}
