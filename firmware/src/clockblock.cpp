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
  // diable the timer and switch interrupts and reset the switch timer counter
  disableSwitchTimer();
  disableSwitchInt();
  switchTimerCount = 0;
  // save the state that triggered the interrupt
  switchState = getSwitchState();
  // start the switch timer to debounce and time if necessary
  enableSwitchTimer();
}

// switch debouncer / timer
ISR(TIMER2_OVF_vect, ISR_NOBLOCK) {
  // disable this interrupt
  disableSwitchTimer();
  // check that the state still matches and that it's some thing we care about
  if (switchStateValid() && (getSwitchState() == switchState)) {
    // increment the timer
    switchTimerCount++;
    // check for a hold event
    if (switchTimerCount >= 60) {
      // set the hold flag
      switchHold = true;
      switchHoldState = switchState;
    }
    // else, re-enable the timer interrupt
    else {
      enableSwitchTimer();
      // check for a normal press event
      if (switchTimerCount == 3) {
        switchPress = true;
        switchPressState = switchState;
        enableSwitchInt();
      }
    }
  }
  else {
    // if it was a bounce or if it was something we don't care about, re-enable pin change interrupt
    enableSwitchInt();
  }
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
  // set time vector - { minutes, hours }
  uint8_t set[2] = {0, 0};
  // animation frame
  uint8_t fr = 0;
  // last second measurement - used to sync up the milliseconds
  uint8_t lastSec = 0;
  // if we're in a set-time routine
  bool settingTime = false;

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

  // enable inputs
  switchTimerCount = 0;
  initPins();
  initSwitchTimer();
  enableSwitchInt();

  // set the operating mode
  uint8_t opMode = MODE_CLOCK;
  uint8_t dispMode = leds.getMode();

  // get lost
  for (;;) {

    // take care of any switch presses
    if (switchPress) {
      // clear the flag
      switchPress = false;
      // switch press only matters in time and display set modes
      switch (opMode) {
        case MODE_TIME_SET:
          // if the hour switch is low and the minute switch is high, increment the hours by 1
          if ( !(switchPressState & INPUT_HOUR_SET) && (switchPressState & INPUT_MIN_MODE) ) {
            set[1] = (set[1] + 1) % 12;
            set[1] = (set[1] == 0) ? 12 : set[1];
          }
          // else if the hour switch is high and the minute switch is low, increment the minutes by 1
          else if ( (switchPressState & INPUT_HOUR_SET) && !(switchPressState & INPUT_MIN_MODE) ) {
            set[0] = (set[0] + 1) % 60;
          }
          break;

        case MODE_DISPLAY_SET:
          break;

        default:
          break;
      }
    }

    // take care of any switch holds
    if (switchHold) {
      // clear the flag
      switchHold = false;
      // figure out what to do with the hold
      switch (opMode) {
        // we're currently operating as a clock: check for a mode command
        case MODE_CLOCK:
          // if the hour switch is low and the minute switch is high, go into time set mode
          if ( !(switchHoldState & INPUT_HOUR_SET) && (switchHoldState & INPUT_MIN_MODE) ) {
            opMode = MODE_TIME_SET;
            set[0] = tm[1];
            set[1] = tm[2];
            dispMode = leds.getMode();
            leds.setMode(DISPLAY_MODE_SET);
          }
          // else if the hour switch is high and the minute switch is low, go into display set mode
          //else if ( (switchHoldState & INPUT_HOUR_SET) && !(switchHoldState & INPUT_MIN_MODE) ) {
            //opMode = MODE_DISPLAY_SET;
            //leds.setMode(DISPLAY_MODE_SET);
          //}
          break;

        case MODE_TIME_SET:
          // if the hour switch is low and the minute switch is high, increment the hours by 3
          if ( !(switchHoldState & INPUT_HOUR_SET) && (switchHoldState & INPUT_MIN_MODE) ) {
            set[1] = (set[1] + 3) % 12;
            set[1] = (set[1] == 0) ? 12 : set[1];
          }
          // else if the hour switch is high and the minute switch is low, increment the minutes by 5
          else if ( (switchHoldState & INPUT_HOUR_SET) && !(switchHoldState & INPUT_MIN_MODE) ) {
            set[0] = (set[0] + 5) % 60;
          }
          // else both buttons were held down, so go back to clock mode
          else {
            opMode = MODE_CLOCK;
            tm[2] = set[1];
            tm[1] = set[0];
            tm[0] = 0;
            rtc.setTime(tm);
            leds.setMode(dispMode);
          }
          break;

        case MODE_DISPLAY_SET:
          // if the hour switch is low and the minute switch is high, increment the hours by 3
          if ( !(switchHoldState & INPUT_HOUR_SET) && (switchHoldState & INPUT_MIN_MODE) ) {
             
          }
          // else if the hour switch is high and the minute switch is low, increment the minutes by 5
          else if ( (switchHoldState & INPUT_HOUR_SET) && !(switchHoldState & INPUT_MIN_MODE) ) {
            
          }
          // else both buttons were held down, so go back to clock mode
          else {
            opMode = MODE_CLOCK;
          }
          break;

        default:
          break;
      }
    }

    /*
    // check the set time flag
    if (timeSet) {
      // clear the ISR flag
      timeSet = false;
      // set the application flag
      if (!settingTime) {
        settingTime = true;
        // get the current set time
        set[0] = tm[1];
        set[1] = tm[2];
        // set the display mode
        leds.setMode(DISPLAY_MODE_SET);
      }
      else {
        settingTime = false;
        tm[2] = set[1];
        tm[1] = set[0];
        rtc.setTime(tm);
        leds.setMode(DISPLAY_MODE_BLEND);
      }
      // re-enable pin change interrupt
      INPUT_PCICR |= INPUT_PCIE;
    }
    */

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
      if (opMode == MODE_CLOCK) {
        updateArms(tm[2], tm[1], tm[0], fr);
      }
      else if (opMode == MODE_TIME_SET) {
        updateArms(set[1], set[0], tm[0], fr);
      }
      else {
        updateArms(tm[2], tm[1], tm[0], fr);
      } 
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

  // set up pin change interrupt on these pins
  INPUT_PCMSK |= ( INPUT_HOUR_SET | INPUT_MIN_MODE );
}

uint8_t getSwitchState(void) {
  return INPUT_PIN & ( INPUT_HOUR_SET | INPUT_MIN_MODE );
}

void initSwitchTimer(void) {
  // ensure timer2 settings are cleared out
  TCCR2A = 0;
  // set prescaler to 1024
  TCCR2B = ( (1 << CS22) | (1 << CS21) | (1 << CS20) );
}

void enableSwitchTimer(void) {
  // reload timer
  TCNT2 = 0;
  // enable the overflow interrupt
  TIMSK2 = (1 << TOIE2);
}

void disableSwitchTimer(void) {
  TIMSK2 = 0;
}

void enableSwitchInt(void) {
  // enable the pin change interrupt
  INPUT_PCICR |= INPUT_PCIE;
}

void disableSwitchInt(void) {
  // disable the pin change interrupt
  INPUT_PCICR &= ~INPUT_PCIE;
}

bool switchStateValid(void) {
  return (switchState != ( INPUT_HOUR_SET | INPUT_MIN_MODE ));
}