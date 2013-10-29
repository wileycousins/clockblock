// clockblock
// Copyright 2013 by Wiley Cousins, LLC.
// shared under the terms of the MIT License
//
// file: TLC5971.cpp
// description: TLC5971 LED-driver class

#include "TLC5971.h"
//#include "avr/io.h"

TLC5971::TLC5971(uint8_t n, volatile uint8_t *sckPo, uint8_t sckPi, volatile uint8_t *mosiPo, uint8_t mosiPi) {
  // save pin defs
  sckPort = sckPo;
  sckPin = sckPi;
  mosiPort = mosiPo;
  mosiPin = mosiPi;

  // number of drivers
  num = n;

  // function control to blank outputs and brightness control to full by default
  fc = TLC5971_BLANK;
  bc[0] = 127;
  bc[1] = 127;
  bc[2] = 127;
}

// initialize pins as outputs and hang out
void TLC5971::init() {
  // serial clock - set to output and output low
  *(sckPort-1) |= (1 << sckPin);
  *sckPort &= ~(1 << sckPin);

  // serial data - set to output and output low
  *(mosiPort-1) |= (1 << mosiPin);
  *mosiPort &= ~(1 << mosiPin);
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
  for (int8_t i=num-1; i>=0; i--) {
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
  // ensure the clock line is low
  //*sckPort &= ~(1 << sckPin);
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
