// clockblock
// Copyright 2013 by Wiley Cousins, LLC.
// shared under the terms of the MIT License
//
// file: clockblock.cpp
// description: application file for the clockblock

// ************************************
// AVR includes necessary for this file
// ************************************
//#include <util/atomic.h>
#include <util/delay.h>

// ********************
// application includes
// ********************
#include "clockblock.h"


// **************************
// INTERRUPT SERVICE ROUTINES
// **************************
// frame counter ISR (timer 0 is clocked by the RTC)
ISR(TIMER0_OVF_vect) {
  tick = true;
}

// switch poller / debouncer / timer
ISR(INPUT_TIMER_vect, ISR_NOBLOCK) {
  // disable interrupt
  buttons.disableTimer();
  // handle the polling
  buttons.handleTimer();
  // re-enable the interrupt
  buttons.enableTimer();
}

// ***********
// application
// ***********
// main
int main(void) {
  // debugging
  #ifdef DEBUG
  // set up the heartbeat led
  DDRB |= (1<<3);
  PORTB |= (1<<3);

  // examine the last reset
  // watchdog reset - blink LED 4 times
  if (MCUSR & (1<<WDRF)) {
    for (uint8_t i=0; i<4; i++) {
      beatHeart();
      _delay_ms(250);
      beatHeart();
      _delay_ms(250);
    }
  }
  else if (MCUSR & (1<<BORF)) {
    for (uint8_t i=0; i<3; i++) {
      beatHeart();
      _delay_ms(250);
      beatHeart();
      _delay_ms(250);
    }
  }
  else if (MCUSR & (1<<EXTRF)) {
    for (uint8_t i=0; i<2; i++) {
      beatHeart();
      _delay_ms(250);
      beatHeart();
      _delay_ms(250);
    }
  }
  else if (MCUSR & (1<<PORF)) {
    for (uint8_t i=0; i<1; i++) {
      beatHeart();
      _delay_ms(250);
      beatHeart();
      _delay_ms(250);
    }
  }
  // clear the flags
  MCUSR = 0;

  // set UART for USB to serial
  // enable TX as output and RX as input with pullup inabled
  DDRA |= (1<<1);
  DDRA &= ~(1<<0);
  PORTA |= (1<<0);

  // set the baudrate to 9600
  // leave bit timing to default (LINBTR & 0x3F) = 0x20 = 32
  // also disable frame resync
  LINBTR |= (1<<LDISR);
  // LINBRR = (F_CPU/((LINBTR & 0x3F) * BAUDRATE)) - 1
  // LINBRR = (8000000/(32 * 9600)) - 1 = 25 + 0.167%
  LINBRR = 25;
  // enable lin/uart module, set to uart mode, set to 8 bit char, no parity, RX and TX enabled
  LINCR = ( (1<<LENA) | (1<<LCMD2) | (1<<LCMD1) | (1<<LCMD0) );

  uint8_t c;
  do {
    // make sure the line isn't busy
    while (LINSIR & (1<<LBUSY));
    // wait for a char recieve
    while (!(LINSIR & (1<<LRXOK)));
    // read the character
    c = LINDAT;
    // clear the flag
    //LINSIR = (1<<LRXOK);
    // make sure the line isn't busy
    while (LINSIR & (1<<LBUSY));
    // send an exclamation mark to confirm reciept of character
    //if (!LINSIR & (1<<LRXOK))
    LINDAT = c;
    // wait for transmit to finish
    while (!(LINSIR & (1<<LTXOK)));
    // clear the flag
    LINSIR = (1<<LTXOK);
  } while (c != 'g');

  // make sure the line isn't busy
  while (LINSIR & (1<<LBUSY));
  // send an exclamation mark to confirm reciept of  go character
  LINDAT = '!';
  // wait for transmit to finish
  while (!(LINSIR & (1<<LTXOK)));

  #endif


  // delay for a few ms to allow the RTC to take its initial temp measurement
  _delay_ms(1000);

  // begin setup - disable interrupts
  cli();

  // take care of unused pins
  initUnusedPins();

  // give those ISR volatile vairables some values
  tick = false;

  // application variables
  // time vector - { seconds, minutes, hours}
  uint8_t tm[3] = {0, 0, 12};
  // animation frame
  uint8_t fr = 0;
  // arms leds
  uint16_t dots[DISPLAY_NUM_DOTS];

  // initialize the LED driver
  // DO THIS BEFORE INITIALIZING RTC (to ensure proper SPI functionality for the RTC)
  tlc.init();
  // set the TLC to autorepeat the pattern and to reset the GS counter whenever new data is latched in
  tlc.setFC(TLC5971_DSPRPT);
  // set brightness to half
  tlc.setBC(63);

  // initialize the RTC
  rtc.init();
  // enable a 8192 Hz squarewave output on clock input pin
  rtc.setSquareWave(PCF2129AT_CLKOUT_8_kHz);

  // check the oscillator stop flag on the RTC and give it a new time if necessary
  if (rtc.hasLostTime()) {
    rtc.setTime(tm, PCF2129AT_AM);
  }
  // else get the good time from the RTC
  else {
    rtc.getTime(tm);
  }

  // initialize the ticker
  initTicker();

  // set the display mode
  leds.setMode(DISPLAY_MODE_BLEND);

  // enable inputs
  buttons.init();

  // end setup - enable interrupts
  sei();

  // get lost
  for (;;) {
    // take care of any switch presses
    uint8_t buttonState = 0;
    if (buttons.getPress(&buttonState)) {
      handleButtonPress(buttonState, tm);
    }

    // update the arms on a tick
    if (tick) {
      // clear the flag
      tick = false;
      // get the time
      if (++fr >= 32) {
        // beat the heart every second
        beatHeart();
        fr = 0;
        rtc.getTime(tm);
      }
      // update the clock arms
      updateArms(tm, fr, dots);
    }
  }

  // one day we might get the answer
  return 42;
}


// update the clock arms
// dots array structure: { hr0, mn0, sc0, hr1, mn1, sc1, ... , hr11, mn11, sc11 }
void updateArms(uint8_t *tm, uint8_t frame, uint16_t *dots) {
  // get the display
  leds.getDisplay(tm, frame, dots);
  // send to the LED driver
  tlc.setGS(dots);
}


// initialize unused pins as inputs with pullups enabled
void initUnusedPins(void) {
  #ifdef UNUSED_PORTB_MASK
  // PORTB
  DDRB &= ~UNUSED_PORTB_MASK;
  PORTB |= UNUSED_PORTB_MASK;
  #endif
  #ifdef UNUSED_PORTA_MASK
  // PORTA
  DDRA &= ~UNUSED_PORTA_MASK;
  PORTA |= UNUSED_PORTA_MASK;
  #endif
}


// button handling logic
void handleButtonPress(uint8_t state, uint8_t *tm) {
  // if hour switch, increment the hours by 1
  if (state == INPUT_HOUR) {
    rtc.getTime(tm);
    if (++tm[2] > 12) {
      tm[2] -= 12;
    }
    rtc.setTime(tm, PCF2129AT_AM);
  }
  // if minute switch, increment minutes
  else if (state == INPUT_MIN) {
    rtc.getTime(tm);
    if (++tm[1] > 59) {
      tm[1] -= 60;
    }
    rtc.setTime(tm, PCF2129AT_AM);
  }
  // if mode switch, increment mode
  else if (state == INPUT_MODE) {
    uint8_t m = leds.getMode();
    m = (m < DISPLAY_NUM_MODES-1) ? m+1 : 0;
    leds.setMode(m);
  }
}


void beatHeart(void) {
  #ifdef DEBUG
  PINB |= (1<<3);
  #endif
}


void initTicker(void) {
  // set timer 0 to run normally with no prescaler
  TCCR0A = 0;
  TCCR0B = (1<<CS00);
  // set timer 0 to clock from the RTC squarewave
  ASSR = (1<<EXCLK);
  // set timer 0 to clock asynchronously and reload the counter
  ASSR |= (1<<AS0);
  TCNT0 = 0;
  // enable timer 0 overflow interrupt
  TIMSK0 = (1<<TOIE0);
}
