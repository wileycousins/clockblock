// clockblock
// Copyright 2013 by Wiley Cousins, LLC.
// shared under the terms of the MIT License
//
// file: clockblock.cpp
// description: application file for the clockblock

// defines
#define TIME_OVF (3600*12)
#define NUM_DOTS (3*12)
#define CHECK_TIME 1000;

// LED brightness levels
#define LVL 13000 

// AVR includes
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// application includes
#include "clockblock.h"
//#include "Cereal.h"
//#include "StuPId.h"

// global variables
// 12-hour time in seconds
volatile uint16_t time12 = 0;
// time has been updated by the RTC
volatile bool timeUpdate = false;
// time has been updated by the user
volatile bool timeSet = false;
// counter to periodically check that AVR time and RTC time match
uint16_t checkCounter = 0;
// array for LED brightness
uint16_t dots[NUM_DOTS];

// global objects
// USART interface at specified baudrate
//Cereal debug;
// SPI interface at specified frequency (MHz)
//StuPId spi;
// DS3234 Real-time clock module (spi channel, chip select on pin C2, reset on pin C3)
//DS3234 rtc(&spi, PORTC, 2, PORTC, 3);
// TLC5971 LED driver (3 drivers, serial clock on pin C5, serial data on pin C4)
//TLC5971 leds(3, PORTC, 5, PORTC, 4);

// main
int main() {
  // probably run some initializations
  // initialize the USART for debugging at 9600 bps
  //debug.init(9600);
  // initialize the SPI to operate at 100 MHz
  //spi.init(100);
  // initialize the led driver at 100% current
  //leds.init(255);

/*
  // initialize the clock
  // check the TXCO stop bit to see if the RTC has a good time stored
  if (!rtc.gotTime()) {
    // make a brash assumtion about what time it is
    time12 = 42;
    timeSet = true;
  }
  // otherwise, get the time from the RTC
  else {
    time12 = rtc.getTime12();
    timeUpdate = true;
  }
  // set the rtc to trigger an interrupt every second using alarm 1
  rtc.setSecondTrigger(1);
*/

  // get lost
  for (;;) {
    /*
    if (setTime) {
      rtc.setTime(time12);
      setTime = false;
      timeUpdate = true;
    }
    // if time has updated, update the LEDs
    if (timeUpdate) {
      // update the LEDs
      updateArms();
      // clear the flag
      timeUpdate = false;
      // periodic time check
      if (++checkCounter >= CHECK_TIME) {
        uint16 check = rtc.getTime12();
        if (check != time12) {
          time12 = check;
          timeUpdate = true;
        }
      }
    }
    */
  }

  // one day we might get the answer
  return 42;
}

// update the clock arms
void updateArms() {
  // break down the time
  uint8_t hourMod = time12%3600;
  uint8_t hour = time12/3600;
  uint8_t min = hourMod/60;
  uint8_t sec = hourMod%60;
  
  // fill the hour dots
  // all hours previous get set to full
  for (uint8_t i=0; i<hour; i++) {
    dots[((NUM_DOTS-1) - (i*3))] = 5*LVL;
  }
  // current hour to fraction
  dots[(NUM_DOTS-1) - (hour*3)] = LVL*(min/12+1);
  // all other hour dots off
  for (uint8_t i=0; i<(11-hour); i++) {
    dots[(i*3)+2] = 0;
  }

  // do the same with the minute dots
  // all minute dots previous get set to full
  for (uint8_t i=0; i<min/5; i++) {
    dots[((NUM_DOTS-2) - (i*3))] = 5*LVL;
  }
  // current minute dot to fraction
  dots[(NUM_DOTS-2) - ((min/5)*3)] = LVL*(sec/12+1);
  // all other minute dots off
  for (uint8_t i=0; i<(11-(min/5)); i++) {
    dots[(i*3)+1] = 0;
  }

  // finally, seconds
  // all second dots previous get set to full
  for (uint8_t i=0; i<sec/5; i++) {
    dots[((NUM_DOTS-3) - (i*3))] = 5*LVL;
  }
  // current second dot to fraction
  dots[(NUM_DOTS-3) - ((sec/5)*3)] = LVL*(sec%5+1);
  // all other second dots off
  for (uint8_t i=0; i<(11-(sec/5)); i++) {
    dots[(i*3)] = 0;
  }

  // send the data to the drivers
  //leds.setGS(dots);
}

// --------------------------------------------- //
// ISRs
// --------------------------------------------- //

// RTC interrupt on pin D2 - external interrupt 0
ISR(INT0_vect) {
  // increment and check for overflow
  if(++time12 >= TIME_OVF) {
    time12 = 0;
  }
}

// hour set interrupt on pin D7 - pin change interrupt 23
// minute set interrupt on pin D6 - pin change interrupt 22
ISR(PCINT2_vect) {
  // debounce the switch(es)
  _delay_ms(20);

  // check if hour pin is low
  if ( !(PIND & (1<<7)) ) {
    time += 3600;
    timeSet = true;
  }

  // check if minute pin is low
  if ( !(PIND & (1<<7)) ) {
    time += 60;
    timeSet = true;
  }

  // check for overflow
  if (time12 >= TIME_OVF) {
    time12 = time12 - TIME_OVF;
  }
}