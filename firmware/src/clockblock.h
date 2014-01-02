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
#include "PCF2129AT.h"
#include "TLC5971.h"


// ***********
// peripherals
// ***********
// SPI bus (in case you couldn't tell, SPI and I have our differences)
// parameters:
//   mosi DDR, pin - PB3
//   sck DDR, pin  - PB5
StuPId spi;

// USART bus for debugging - TO BE IMPLEMENTED

// PCF2129AT real-time-clock module
// parameters:
//   address of spi bus object,
//   chip select PORT, pin - PB2
//   reset PORT, pin       - PB0
//   interrupt PORT, pin   - PD2
PCF2129AT rtc(&spi);

// TLC5971 LED driver - TO BE IMPLEMENTED
// parameters:
//   3 drivers
//   serial clock PORT, pin - PC5
//   serial data PORT, pin  - PC4
TLC5971 tlc;

// Display class - contains all the different display modes of the clockblock
Display leds;

// Input class - handles the pushbuttons
Input buttons;


// *************
// ISR variables
// *************
// flag set by the ISR when it's time to update the clock arms
volatile bool tick;


// *******************
// function prototypes
// *******************
// main application
int main(void);
// set up async timer for clock signal
void initTicker(void);
// update clock arms
void updateArms(uint8_t *tm, uint8_t fr, uint16_t *dots);
// initialize unused pins to a safe state
void initUnusedPins(void);
// heartbeat for debugging
void beatHeart(void);
// button handling logic
void handleButtonPress(uint8_t state, uint8_t* tm);
