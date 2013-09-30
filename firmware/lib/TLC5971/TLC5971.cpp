// clockblock
// Copyright 2013 by Wiley Cousins, LLC.
// shared under the terms of the MIT License
//
// file: TLC5971.cpp
// description: TLC5971 LED-driver class

#include "TLC5971.h"

TLC5971::TLC5971(uint8_t n, volatile uint8_t *sckPo, uint8_t sckPi, volatile uint8_t *mosiPo, uint8_t mosiPi) {
  // save pin defs
  sckPort = sckPo;
  sckPin = sckPi;
  mosiPort = mosiPo;
  mosiPin = mosiPi;

  // number of drivers
  num = n;
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

// serial helpers
// set mosi and pulse the clock
// data is a byte with the data right-aligned, MSb left, nBits is the number of bits to send
// sends data MSb first
void TLC5971::sendData(uint8_t data, uint8_t n) {
  // ensure the clock line is low
  *sckPort &= ~(1 << sckPin);
  // for nBits
  for (int8_t i=n-1; i>=0; i--) {
    // set or clear the MOSI pin
    if (data & (1 << i)) {
      mosiPort |= (1 << mosiPin);
    }
    else {
      mosiPort &= ~(1 << mosiPin);
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
