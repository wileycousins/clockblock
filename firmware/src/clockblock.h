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
#include "Cereal.h"
#include "StuPId.h"
#include "PCF2129AT.h"
#include "TLC5971.h"


// ***********
// peripherals
// ***********
// SPI bus (in case you couldn't tell, SPI and I have our differences)
StuPId spi;

// UART bus for debugging - TO BE IMPLEMENTED
Cereal uart;

// PCF2129AT real-time-clock module
// parameters:
//   address of spi bus object,
PCF2129AT rtc(&spi);

// TLC5971 LED driver
TLC5971 tlc;

// Display class - contains all the different display modes of the clockblock
Display leds;

// Input class - handles the push-buttons
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
