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
//#include "DS3234.h"
#include "PCF2129AT.h"
#include "TLC5971.h"


// *******************
// application defines
// *******************
// operating modes
//#define MODE_CLOCK        0
//#define MODE_TIME_SET     1
//#define MODE_DISPLAY_SET  2


// ***********
// peripherals
// ***********
// SPI bus (in case you couldn't tell, SPI and I have our differences)
// parameters:
//   mosi DDR, pin - PB3
//   sck DDR, pin  - PB5
StuPId spi(&SPI_MOSI_DDR, SPI_MOSI_PIN, &SPI_SCK_DDR, SPI_SCK_PIN);

// USART bus for debugging - TO BE IMPLEMENTED

// PCF2129AT real-time-clock module
// parameters:
//   address of spi bus object,
//   chip select PORT, pin - PB2
//   reset PORT, pin       - PB0
//   interrupt PORT, pin   - PD2
PCF2129AT rtc(&spi, &RTC_CS_PORT, RTC_CS_PIN, &RTC_SQW_PORT, RTC_SQW_PIN);

// TLC5971 LED driver - TO BE IMPLEMENTED
// parameters:
//   3 drivers
//   serial clock PORT, pin - PC5
//   serial data PORT, pin  - PC4
TLC5971 tlc(TLC_N, &TLC_SCK_PORT, TLC_SCK_PIN, &TLC_MOSI_PORT, TLC_MOSI_PIN);

// Display class - contains all the different display modes of the clockblock
Display leds;

// Input class - handles the pushbuttons
Input buttons;


// *************
// ISR variables
// *************
// millisecond counter incremented by the ~1 kHz squarewave from the RTC
volatile uint16_t ms;
// flag set by the ISR when it's time to update the clock arms
volatile bool tick;


// *******************
// function prototypes
// *******************
// main application
int main(void);
// update clock arms
void updateArms(uint8_t hour, uint8_t min, uint8_t sec, uint8_t fr, uint16_t *dots);
// initialize unused pins to a safe state
void initUnusedPins(void);
// heartbeat for debugginh
void beatHeart();

// button handling logic
void handleButtonPress(uint8_t state, uint8_t* tm);
void handleButtonHold(uint8_t state, uint8_t* tm);
