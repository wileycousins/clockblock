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

  // should probably malloc an the data buffer for sending shit. or figure out a way to do that without mallocing anything ever
}

// initialize pins as outputs and hang out
void TLC5971::init() {

}