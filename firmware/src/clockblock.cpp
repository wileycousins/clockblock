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
ISR(INPUT_PCINT_vect, ISR_NOBLOCK) {
  buttons.handleChange();
  /*
  // diable the timer and switch interrupts and reset the switch timer counter
  disableSwitchTimer();
  disableSwitchInt();
  switchTimerCount = 0;
  // save the state that triggered the interrupt
  switchState = getSwitchState();
  // start the switch timer to debounce and time if necessary
  enableSwitchTimer();
  */
}

// switch debouncer / timer
ISR(TIMER2_OVF_vect, ISR_NOBLOCK) {
  buttons.handleTimer();
  /*
  // disable this interrupt
  disableSwitchTimer();
  // increment the counter
  switchTimerCount++;
  
  // debounce
  if (switchTimerCount < 3) {
    switchRelease = false;
    // keep the timer going
    enableSwitchTimer();
    return;
  }
  
  // check the values still match
  if (getSwitchState() == switchState) {
    // if one or both switches are down
    if (switchStatePushed()) {
      // check for a hold
      if (switchTimerCount >= 60) {
        switchTimerCount = 4;
        switchPress = false;
        switchHold = true;
        switchHoldState = switchState;
      }
      // check for a press
      else if (switchTimerCount == 3) {
        switchPress = true;
        switchPressState = switchState;
      }
      // re-enable the timer
      enableSwitchTimer();
    }
    // else switches were released
    else {
      switchRelease = true;
    }
  }
  // re-enable the pin change interrupt
  enableSwitchInt();
  */
}


// ***********
// application
// ***********
// main
int main(void) {
  // give those ISR volatile vairables some values
  ms = 0;
  tick = false;
  /*
  switchState = 0;
  switchPressState = 0;
  switchHoldState = 0;
  switchTimerCount = 0;
  switchRelease = false;
  switchPress = false;
  switchHold = 0;
  */

  // application variables
  // time vector - { seconds, minutes, hours}
  uint8_t tm[3] = {0, 58, 11};
  // set time vector - { minutes, hours }
  uint8_t set[2] = {0, 0};
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
    // if it has, assume it's 11:58 AM, because that's when people set up their clocks
    rtc.setTime(DS3234_AM, tm);
  }

  // initialize the LED drivers
  tlc.init();
  #ifdef BREADBOARD
  initTLCTimers();
  #else
  // set the TLC to autorepeat the pattern and to reset the GS counter whenever new data is latched in
  tlc.setFC(TLC5971_DSPRPT | TLC5971_TMGRST);
  #endif

  // enable a falling edge interrupt on the square wave pin
  cli();
  EICRA = (0x2 << (2*RTC_EXT_INT));
  EIMSK = (1 << RTC_EXT_INT);
  sei();

  // set the display mode
  leds.setMode(DISPLAY_MODE_BLEND);

  // enable inputs
  buttons.init();
  /*
  switchTimerCount = 0;
  initPins();
  initSwitchTimer();
  enableSwitchInt();
  */

  // set the operating mode
  uint8_t opMode = MODE_CLOCK;
  uint8_t dispMode = leds.getMode();

  // get lost
  for (;;) {

    uint8_t buttonState;
    // take care of any switch presses
    if (buttons.getPress(&buttonState)) {
      // switch press only matters in time and display set modes
      // time set mode
      if (opMode == MODE_TIME_SET) {
        // if the hour switch is low and the minute switch is high, increment the hours by 1
        if ( !(buttonState & INPUT_HOUR) && (buttonState & INPUT_MIN) ) {
          set[1] = (set[1] + 1) % 12;
          set[1] = (set[1] == 0) ? 12 : set[1];
        }
        // else if the hour switch is high and the minute switch is low, increment the minutes by 1
        else if ( (buttonState & INPUT_HOUR) && !(buttonState & INPUT_MIN) ) {
          set[0] = (set[0] + 1) % 60;
        }
      }
      // display set mode
      else if (opMode == MODE_DISPLAY_SET) {
        // if the hour switch is low and the minute switch is high, increment the mode
        if ( !(buttonState & INPUT_HOUR) && (buttonState & INPUT_MIN) ) {
          dispMode = (dispMode >= (DISPLAY_NUM_MODES-1)) ? 0 : (dispMode+1);
          leds.setMode(dispMode);
        }
        // else if the hour switch is high and the minute switch is low, decrement the mode
        else if ( (buttonState & INPUT_HOUR) && !(buttonState & INPUT_MIN) ) {
          dispMode = (dispMode == 0) ? (DISPLAY_NUM_MODES-1) : (dispMode-1);
          leds.setMode(dispMode);
        }
      }
    }

    // take care of any switch holds
    if (buttons.getHold(&buttonState)) {
      // figure out what to do with the hold
      // if we're currently operating as a clock: check for a mode command
      if (opMode == MODE_CLOCK) {
        // if the hour switch is low and the minute switch is high, go into time set mode
        if ( !(buttonState & INPUT_HOUR) && (buttonState & INPUT_MIN) ) {
          opMode = MODE_TIME_SET;
          set[0] = tm[1];
          set[1] = tm[2];
          dispMode = leds.getMode();
          leds.setMode(DISPLAY_MODE_SET);
        }
        // else if the hour switch is high and the minute switch is low, go into display set mode
        else if ( (buttonState & INPUT_HOUR) && !(buttonState & INPUT_MIN) ) {
          dispMode = leds.getMode();
          opMode = MODE_DISPLAY_SET;
          leds.setMode(DISPLAY_MODE_CHANGE);
        }
        // reset the buttons to prevent the hold from continuing to fire
        buttons.reset();
      }
      // if we're currently setting the time
      else if (opMode == MODE_TIME_SET) {
          // if the hour switch is low and the minute switch is high, increment the hours by 3
          if ( !(buttonState & INPUT_HOUR) && (buttonState & INPUT_MIN) ) {
            set[1] = (set[1] + 3) % 12;
            set[1] = (set[1] == 0) ? 12 : set[1];
          }
          // else if the hour switch is high and the minute switch is low, increment the minutes by 5
          else if ( (buttonState & INPUT_HOUR) && !(buttonState & INPUT_MIN) ) {
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
      }
      // if we're in display setting mode
      else if (opMode == MODE_DISPLAY_SET) {
          // both buttons were held down, so go back to clock mode
          if (!buttonState) {
            opMode = MODE_CLOCK;
            leds.setMode(DISPLAY_MODE_CHANGE_EXIT);
          }
      }
    }

    /*
    // take care of any switch presses
    if (switchPress && switchRelease) {
      // clear the flags
      switchPress = false;
      switchRelease = false;
      // switch press only matters in time and display set modes
      switch (opMode) {
        case MODE_TIME_SET:
          // if the hour switch is low and the minute switch is high, increment the hours by 1
          if ( !(switchPressState & INPUT_HOUR) && (switchPressState & INPUT_MIN) ) {
            set[1] = (set[1] + 1) % 12;
            set[1] = (set[1] == 0) ? 12 : set[1];
          }
          // else if the hour switch is high and the minute switch is low, increment the minutes by 1
          else if ( (switchPressState & INPUT_HOUR) && !(switchPressState & INPUT_MIN) ) {
            set[0] = (set[0] + 1) % 60;
          }
          break;

        case MODE_DISPLAY_SET:
          // if the hour switch is low and the minute switch is high, increment the mode
          if ( !(switchPressState & INPUT_HOUR) && (switchPressState & INPUT_MIN) ) {
            dispMode = (dispMode >= (DISPLAY_NUM_MODES-1)) ? 0 : (dispMode+1);
            leds.setMode(dispMode);
          }
          // else if the hour switch is high and the minute switch is low, decrement the mode
          else if ( (switchPressState & INPUT_HOUR) && !(switchPressState & INPUT_MIN) ) {
            dispMode = (dispMode == 0) ? (DISPLAY_NUM_MODES-1) : (dispMode-1);
            leds.setMode(dispMode);
          }
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
          if ( !(switchHoldState & INPUT_HOUR) && (switchHoldState & INPUT_MIN) ) {
            opMode = MODE_TIME_SET;
            set[0] = tm[1];
            set[1] = tm[2];
            dispMode = leds.getMode();
            leds.setMode(DISPLAY_MODE_SET);
          }
          // else if the hour switch is high and the minute switch is low, go into display set mode
          else if ( (switchHoldState & INPUT_HOUR) && !(switchHoldState & INPUT_MIN) ) {
            dispMode = leds.getMode();
            opMode = MODE_DISPLAY_SET;
            leds.setMode(DISPLAY_MODE_CHANGE);
          }
          // disable the timer and enable the pin change interrupt
          disableSwitchTimer();
          enableSwitchInt();
          break;

        case MODE_TIME_SET:
          // if the hour switch is low and the minute switch is high, increment the hours by 3
          if ( !(switchHoldState & INPUT_HOUR) && (switchHoldState & INPUT_MIN) ) {
            set[1] = (set[1] + 3) % 12;
            set[1] = (set[1] == 0) ? 12 : set[1];
          }
          // else if the hour switch is high and the minute switch is low, increment the minutes by 5
          else if ( (switchHoldState & INPUT_HOUR) && !(switchHoldState & INPUT_MIN) ) {
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
          // else both buttons were held down, so go back to clock mode
          if (!switchHoldState) {
            opMode = MODE_CLOCK;
            leds.setMode(DISPLAY_MODE_CHANGE_EXIT);
          }
          break;

        default:
          break;
      }
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
  #ifdef BREADBOARD
  // breadboard edition uses TLC5940
  for (uint8_t i=0; i<DISPLAY_NUM_DOTS; i++) {
    tlc.setGS(i, dots[i]);
  }
  tlc.update();
  #else
  // v0.1 uses TLC5971
  tlc.setGS(dots);
  #endif
}

// initialize input pins as inputs with pullups enabled
void initUnusedPins(void) {
  /*
  // clear pins in DDR to inputs
  DDR(INPUT_PORT) &= ~( INPUT_HOUR | INPUT_MIN );
  // enable pull up resistors
  INPUT_PORT |= ( INPUT_HOUR | INPUT_MIN );
  // set up pin change interrupt on these pins
  INPUT_PCMSK |= ( INPUT_HOUR | INPUT_MIN );
  */

  // handle unused pins in PCB version (set as inputs with pullups enabled)
  #ifndef BREADBOARD
  // PORTB
  DDRB &= ~UNUSED_PORTB_MASK;
  PORTB |= UNUSED_PORTB_MASK;
  // PORTC
  DDRC &= ~UNUSED_PORTC_MASK;
  PORTC |= UNUSED_PORTC_MASK;
  //PORTD
  DDRD &= ~UNUSED_PORTD_MASK;
  PORTD |= UNUSED_PORTD_MASK;
  #endif
}

/*
uint8_t getSwitchState(void) {
  return PIN(INPUT_PORT) & ( INPUT_HOUR | INPUT_MIN );
}
*/

/*
void initSwitchTimer(void) {
  // ensure timer2 settings are cleared out
  TCCR2A = 0;
  // set prescaler to 1024
  TCCR2B = ( (1 << CS22) | (1 << CS21) | (1 << CS20) );
}
*/

/*
void enableSwitchTimer(void) {
  // reload timer
  TCNT2 = 0;
  // enable the overflow interrupt
  TIMSK2 = (1 << TOIE2);
}
*/

/*
void disableSwitchTimer(void) {
  TIMSK2 = 0;
}
*/

/*
void enableSwitchInt(void) {
  // enable the pin change interrupt
  INPUT_PCICR |= INPUT_PCIE;
}
*/

/*
void disableSwitchInt(void) {
  // disable the pin change interrupt
  INPUT_PCICR &= ~INPUT_PCIE;
}
*/

/*
bool switchStatePushed(void) {
  return (switchState != ( INPUT_HOUR | INPUT_MIN ));
}
*/
