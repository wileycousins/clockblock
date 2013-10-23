// clockblock
// Copyright 2013 by Wiley Cousins, LLC.
// shared under the terms of the MIT License
//
// file: clockblock.cpp
// description: header file for the clockblock application

// ************************************
// AVR includes necessary for this file
// ************************************
#include <avr/interrupt.h>


// ********************
// application includes
// ********************
#include "pindefs.h"
#include "display.h"
#include "input.h"
#include "StuPId.h"
#include "DS3234.h"
#include "TLC5971.h"


// *******************
// application defines
// *******************
// operating modes
#define MODE_CLOCK        0
#define MODE_TIME_SET     1
#define MODE_DISPLAY_SET  2

// *************
// ISR variables
// *************
// millisecond counter incremented by the ~1 kHz squarewave from the RTC
volatile uint16_t ms;
// flag set by the ISR when it's time to update the clock arms
volatile bool tick;

// ***********
// peripherals
// ***********
// SPI bus (in case you couldn't tell, SPI and I have our differences)
// parameters:
//   mosi DDR, pin - PB3
//   sck DDR, pin  - PB5
StuPId spi(&SPI_MOSI_DDR, SPI_MOSI_PIN, &SPI_SCK_DDR, SPI_SCK_PIN);

// USART bus for debugging - TO BE IMPLEMENTED

// DS3234 real-time-clock module
// parameters:
//   address of spi bus object,
//   chip select PORT, pin - PB2
//   reset PORT, pin       - PB0
//   interrupt PORT, pin   - PD2
DS3234 rtc(&spi, &RTC_CS_PORT, RTC_CS_PIN, &RTC_RST_PORT, RTC_RST_PIN, &RTC_INT_PORT, RTC_INT_PIN);

#ifndef BREADBOARD
// TLC5971 LED driver - TO BE IMPLEMENTED
// parameters:
//   3 drivers
//   serial clock PORT, pin - PC5
//   serial data PORT, pin  - PC4
TLC5971 tlc(TLC_N, &TLC_SCK_PORT, TLC_SCK_PIN, &TLC_MOSI_PORT, TLC_MOSI_PIN);
#endif

// Display class - contains all the different display modes of the clockblock
Display leds;

// Input class - handles the pushbuttons
Input buttons;

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
// BREADBOARD YOU'RE MY HERO!!1
// ****************************

// *******************
// function prototypes
// *******************
// main application
int main(void);
// update clock arms
void updateArms(uint8_t hour, uint8_t min, uint8_t sec, uint8_t fr);
// initialize unused pins to a safe state
void initUnusedPins(void);

// button handling logic
uint8_t handleButtonPress(uint8_t state, uint8_t opMode, uint8_t *set, uint8_t* tm);
uint8_t handleButtonHold(uint8_t state, uint8_t opMode, uint8_t *set, uint8_t* tm);
