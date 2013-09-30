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

// function control defines (function control is the 5 MSb's of the data package)
#define TLC5971_BLANK   (1<<0)
#define TLC5971_DSPRPT  (1<<1)
#define TLC5971_TMGRST  (1<<2)
#define TLC5971_EXTGCK  (1<<3)
#define TLC5971_OUTTMG  (1<<4)



class TLC5971 {
public:
  // constructor takes number of drivers daisychained on one serial line and the pindefs
  TLC5971(uint8_t n, volatile uint8_t *sckPo, uint8_t sckPi, volatile uint8_t *mosiPo, uint8_t mosiPi);
  // initialization routine
  void init();
  // control the chip
  void setLeds(uint8_t *)

private:
  // serial helpers
  // pulse the serial clock
  void pulseSck();
  // set mosi and pulse the clock
  void sendBit();
  // send the write command to the device (6-bits = 0x25 = 0b100101)
  void sendWriteCommand();
  // send the function control packet
  void sendFC();
  // send the brightness control
  void sendBC();
  // send the greyscale
  void sendGS();
  // send a full 224 bit package
  void sendDataPackage();

  // buffer pointers
  // function control
  uint8_t *fc;
  // brightness control
  uint8_t *bc;
  // greyscale control
  uint8_t *gs;

  // number of drivers
  uint8_t num;

  // pin defs
  volatile uint8_t *sckPort;
  uint8_t sckPin;
  volatile uint8_t *mosiPort;
  uint8_t mosiPin;
};

#endif