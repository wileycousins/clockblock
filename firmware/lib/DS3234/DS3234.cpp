// clockblock
// Copyright 2013 by Wiley Cousins, LLC.
// shared under the terms of the MIT License
//
// file: clockblock.cpp
// description: class for DS3234 real-time clock

#include "DS3234.h"
#include "SPI.h"

// can we build it? yes we can!
// parameters:
//   SPI channel (implement later when I've written an SPI library)
//   chip select port, chip select pin
//   reset port, reset pin
//   interrupt port, interrupt pin
DS3234::DS3234( /* StuPId *spi, */ uint8_t csPo, uint8_t csPi, uint8_t rstPo, uint8_t rstPi, uint8_t intPo, uint8_t intPi) {
  // save this shit
  // SPI channel
  // deal with this later
  // chip select pin
  csPort = csPo;
  csPin = csPi;
  // reset
  rstPort = rstPo;
  rstPin = rstPi;
  // interrupt
  intPort = intPo;
  intPin = intPi;
}

// initialization - sets pin modes and whatnot
// returns false if osc has stopped, true otherwise (true means the RTC has a good time)
bool DS3234::init() {
  // sets chip select pin to output and pulls it high
  // DDR is PORT + 1
  *(csPort+1) |= (1 << csPin);
  *csPort |= (1 << csPin);
  // do the same for the reset pin
  *(rstPort+1) |= (1 << rstPin);
  *rstPort |= (1 << rstPin);
  // ensure interrupt pin is set to an input
  *(intPort+1) &= ~(1 << rstPin);

  // check the status register for an oscilator stop
  // oscstopflag is bit 7 in the crtl_stat register
  uint8_t stat;
  readReg(DS3234_CTRL_STAT, 1, &stat);
  // return false if bit is set (osc has stopped), true otherwise
  return !(stat & (1<<7))
}

// set and get time
void DS3234::setTime(uint8_t *time) {

}

void DS3234::getTime(uint8_t *time) {
  
}

// SPI helpers
// start / end
// ensures SPI options are set correctly and pulls the chip select line low / high to start / end a transfer
void DS3234::spiStart() {
  // DS3234 is MSB first
  SPI.setBitOrder(MSBFIRST);
  // runs at up to 4MHz. dividing clock by 4 will ensure this isn't exceeded
  SPI.setClockDivider(SPI_CLOCK_DIV4);
  // supports SPI modes 1 and 3 (autodetects). let's use 1 (CPOL = 0, CPHA = 1)
  SPI.setDataMode(SPI_MODE1);
  // finally pull the chip select line low to begin the transfer
  *csPort &= ~(1 << csPin);
}

void DS3234::spiEnd() {
  // pull the chip select line high to end the transfer
  *csPort |= (1 << csPin);
}

// read / write
// parameters:
//   register address to start at
//   number of bytes to transfer
//   buffer to read to / write from
void DS3234::readReg(uint8_t reg, uint8_t n, uint8_t *data) {
  spiStart();
  SPI.transfer(reg);
  for (uint8_t i=0; i<n; i++) {
    data[i] = SPI.transfer(0x00);
  }
  spiEnd();
}

void DS3234::writeReg(uint8_t reg, uint8_t n, uint8_t *data) {
  spiStart();
  SPI.transfer(DS3234_SPI_WRITE | reg);
  for (uint8_t i=0; i<n; i++) {
    SPI.transfer(data[i]);
  }
  spiEnd();
}