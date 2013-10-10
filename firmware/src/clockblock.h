// clockblock
// Copyright 2013 by Wiley Cousins, LLC.
// shared under the terms of the MIT License
//
// file: clockblock.cpp
// description: header file for the clockblock application

// ************************************
// AVR includes necessary for this file
// ************************************
#include <avr/io.h>
#include <avr/interrupt.h>


// ********************
// application includes
// ********************
#include "StuPId.h"
#include "DS3234.h"
//#include "TLC5971.h"


// *******************
// application defines
// *******************
// LED brightness levels
#define LVL 800
// clock stuff
#define NUM_DOTS 36
// interrupt pin for RTC
#define RTC_INT_PORT PORTD
#define RTC_INT_PIN  2
// avr external interrupt (0 or 1, INT0_vect or INT1_vect)
#define RTC_INT      0
#define RTC_INT_vect INTO_vect


// *************
// ISR variables
// *************
// flag set by the 1 Hz square wave interrupt from the RTC
volatile bool tick;
// flag set by user input to change the time
volatile bool timeSet;


// ***********
// peripherals
// ***********
// SPI bus (in case you couldn't tell, SPI and I have our differences)
// parameters:
//   mosi DDR, pin - PB3
//   sck DDR, pin  - PB5
StuPId spi(&DDRB, 3, &DDRB, 5);

// USART bus for debugging - TO BE IMPLEMENTED

// DS3234 real-time-clock module
// parameters:
//   address of spi bus object,
//   chip select PORT, pin - PB2
//   reset PORT, pin       - PB0
//   interrupt PORT, pin   - PD2
DS3234 rtc(&spi, &PORTB, 2, &PORTB, 0, &RTC_INT_PORT, RTC_INT_PIN);

// TLC5971 LED driver - TO BE IMPLEMENTED
// parameters:
//   3 drivers
//   serial clock PORT, pin - PC5
//   serial data PORT, pin  - PC4
//TLC5971 leds(3, &PORTC, 5, &PORTC, 4);


// ****************************
// BREADBOARD EDITION EXCLUSIVE
// TLC5940 LED driver from github.com/mcous/TLC5940
#ifdef BREADBOARD
#include "TLC5940.h"
TLC5940 tlc;
void initTLCTimers(void) {
  cli();
  // user timer 1 to toggle the gs clock pin
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1C = 0;
  TIMSK1 = 0;
  // toggle OC1A (pin B1) on compare match event
  TCCR1A |= (1 << COM1A0);
  // set the top of the timer
  // PS = 1, F_CPU = 16 MHz, F_OC = F_CPU/(2 * PS * (OCR1A+1)
  // gs edge gets sent every 32*2=64 clock ticks
  OCR1A = 31;
  // put the timer in CTC mode and start timer with no prescaler
  TCCR1B |= ( (1 << WGM12) | (1 << CS10) );

  // set up an isr for the serial cycle to live in
  // let it live in timer 0
  TCCR0A = 0;
  TCCR0B = 0;
  TIMSK0 = 0;
  // set waveform generation bit to put the timer into CTC mode
  TCCR0A |= (1 << WGM01);
  // set the top of the timer - want this to happen every 4096 * gs clocks = every 8192 clock ticks
  // set top to 255 for an interrupt every 256 * 1024 = 64 * 4096 clock ticks
  OCR0A = 255;
  // start the timer with a 1024 prescaler
  TCCR0B |= ( (1 << CS02) | (1 << CS00) );
  // enable the interrupt of output compare A match
  TIMSK0 |= (1 << OCIE0A);
  sei();
}
#endif
// BREADBOARD YOU'RE MY HERO
// ****************************

// *******************
// function prototypes
// *******************
// main application
int main();
// update clock arms
void updateArms(uint8_t hour, uint8_t min, uint8_t sec);
