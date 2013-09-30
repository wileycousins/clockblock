// clockblock
// Copyright 2013 by Wiley Cousins, LLC.
// shared under the terms of the MIT License
//
// file: TLC5971.h
// description: header for TLC5971 LED-driver

#ifndef LIB_TLC5971_H
#define LIB_TLC5971_H

// typedefs (uint8_t's all day long baby)
#include <stdint.h>

class TLC5971 {
public:
  // constructor takes number of drivers daisychained on one serial line and the pindefs
  TLC5971(uint8_t n, volatile uint8_t *sckPo, uint8_t sckPi, volatile uint8_t *mosiPo, uint8_t mosiPi);
  // initialization routine
  init();

private:
  // serial helpers
  // pulse the serial clock
  void pulseSck();
  // set mosi and pulse the clock
  void sendBit();

  // number of drivers
  uint8_t num;

  // pin defs
  volatile uint8_t *sckPort;
  uint8_t sckPin;
  volatile uint8_t *mosiPort;
  uint8_t mosiPin;
};

#endif