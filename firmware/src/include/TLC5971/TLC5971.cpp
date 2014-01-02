// clockblock
// Copyright 2013 by Wiley Cousins, LLC.
// shared under the terms of the MIT License
//
// file: TLC5971.cpp
// description: TLC5971 LED-driver class

// pin definitions from main app
#include "pindefs.h"
// check that necessary stuff is defined
#ifndef TLC_N
#error "TLC_N undefined; please define in pindefs.h"
#endif
#ifndef TLC_MOSI_PORT
#error "TLC_MOSI_PORT undefined; please define in pindefs.h"
#endif
#ifndef TLC_MOSI_PIN
#error "TLC_MOSI_PIN undefined; please define in pindefs.h"
#endif
#ifndef TLC_SCK_PORT
#error "TLC_SCK_PORT undefined; please define in pindefs.h"
#endif
#ifndef TLC_SCK_PIN
#error "TLC_SCK_PIN undefined; please define in pindefs.h"
#endif

// DDR from PORT macro
#ifndef DDR
#define DDR(port) (*(&port-1))
#endif

// class definition
#include "TLC5971.h"

TLC5971::TLC5971() {
  // save pin defs
  // sckPort = sckPo;
  // sckPin = sckPi;
  // mosiPort = mosiPo;
  // mosiPin = mosiPi;

  // number of drivers
  // num = n;

  // function control to blank outputs and brightness control to full by default
  fc = TLC5971_BLANK;
  bc[0] = 127;
  bc[1] = 127;
  bc[2] = 127;
}

// initialize pins as outputs and hang out
void TLC5971::init() {
  // serial clock - set to output and output low
  DDR(TLC_SCK_PORT) |= (1 << TLC_SCK_PIN);
  TLC_SCK_PORT &= ~(1 << TLC_SCK_PIN);

  // serial data - set to output and output low
  DDR(TLC_MOSI_PORT) |= (1 << TLC_MOSI_PIN);
  TLC_MOSI_PORT &= ~(1 << TLC_MOSI_PIN);
}

// set the LEDz
// parameters
//   - *fc: array of N uint8_t for function control
//     - { fc0, fc1, ... , fcN }
//   - *bc: array of N*3 uint8_t for brightness correction
//     - { bcR0, bcG0, bcB0, bcR1, bcG1, ... , bcBN }
//   - *gs: array of N*12 uint16_t for greyscale data (PWM)  
//     - { gs0.0, gs0.1, gs0.2, gs0.3, gs0.4, ... , gs1.0, gs1.1, ... , gsN.12 }
void TLC5971::setGS(uint16_t *g) {
  // do this for all the drivers
  for (int8_t i=TLC_N-1; i>=0; i--) {
    // greyscale data - 12x 16-bit words per driver
    gs = g + (12*i);
    // send the write command
    sendWriteCommand();
    // function control data
    sendFC();
    // brightness correction
    sendBC();
    // greyscale data
    sendGS();
  }
}

void TLC5971::setBC(uint8_t b) {
  for (uint8_t i=0; i<3; i++) {
    if (b < 128) {
      bc[i] = b;
    }
    else {
      bc[i] = 127;
    }
  }
}

void TLC5971::setBC(uint8_t *b) {
  for (uint8_t i=0; i<3; i++) {
    if (b[i] < 128) {
      bc[i] = b[i];
    }
    else {
      bc[i] = 127;
    }
  }
}

void TLC5971::setFC(uint8_t f) {
  fc = f;
}

// serial helpers
// set mosi and pulse the clock
// data is a byte with the data right-aligned, MSb left, nBits is the number of bits to send
// sends data MSb first
void TLC5971::sendData(uint8_t data, uint8_t n) {
  // for n bits
  for (int8_t i=n-1; i>=0; i--) {
    /*
    // if MOSI and SCK are on the same port
    #if TLC_MOSI_PORT = TLC_SCK_PORT
    // pull the clock line low and set the data bit
    TLC_SCK_PORT = ((TLC_SCK_PORT & ~((1<<TLC_SCK_PIN) | (1<<TLC_MOSI_PIN))) | (((data>>i) & 1) << TLC_MOSI_PIN));
    // pull the clock line high (data clocked in on rising edge)
    TLC_SCK_PORT |= (1<<TLC_SCK_PIN);

    // else, the pins are on different ports
    #else
    */
    // pull the clock line low
    TLC_SCK_PORT &= ~(1 << TLC_SCK_PIN);
    // set the data bit
    TLC_MOSI_PORT = ((TLC_MOSI_PORT & ~(1<<TLC_MOSI_PIN)) | (((data>>i) & 1) << TLC_MOSI_PIN));
    // pull the clock line high to clock in data
    TLC_SCK_PORT |= (1 << TLC_SCK_PIN);

    //#endif
  }



  /*
  // ensure the clock line is low
  // *sckPort &= ~(1 << sckPin);
  // for nBits
  for (int8_t i=n-1; i>=0; i--) {
    // set or clear the MOSI pin
    if (data & (1 << i)) {
      *mosiPort |= (1 << mosiPin);
    }
    else {
      *mosiPort &= ~(1 << mosiPin);
    }
    // pulse the serial clock
    *sckPort |= (1 << sckPin);
    *sckPort &= ~(1 << sckPin);
  }
  */
}

// send the write command to the device (6-bits = 0x25 = 0b100101)
void TLC5971::sendWriteCommand() {
  sendData(0x25, 6);
}

// send the function control packet
void TLC5971::sendFC() {
  sendData(fc, 5);
}

// send the brightness control
void TLC5971::sendBC() {
  for (int8_t j=2; j>=0; j--) {
    sendData(bc[j], 7);
  }
}

// send the greyscale
void TLC5971::sendGS() {
  for (int8_t j=11; j>=0; j--) {
    uint8_t hi = (uint8_t)(gs[j] >> 8);
    uint8_t lo = (uint8_t)(gs[j] & 0xFF);
    sendData(hi, 8);
    sendData(lo, 8);
  }
}
