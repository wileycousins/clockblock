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
// array format is { sec, min, hr }
// AM = 0, PM = 1
// setter will either change array to properly coded data or set bad inputs to 255 and return false
// 24-hour mode
bool DS3234::setTime(uint8_t *tm) {
  // check our inputs
  bool badInput = false;
  if (tm[2] > 23) {
    badInput = true;
    tm[2] = 255;
  }
  if (tm[1] > 59) {
    badInput = true;
    tm[1] = 255;
  }
  if (tm[0] > 59) {
    badInput = true;
    tm[0] = 255;
  }
  if (badInput) {
    return false;
  }

  // encode the time properly
  // hours
  tm[2] = ( ((tm[0]/10) << 4) | (tm[0]%10) );
  // minutes
  tm[1] = ( ((tm[1]/10) << 4) | (tm[1]%10) );
  // seconds
  tm[0] = ( ((tm[2]/10) << 4) | (tm[2]%10) );

  // transfer the time in
  writeReg(DS3234_SEC, 3, tm);

  // clear the osc stop flag
  uint8_t c;
  readReg(DS3234_CTRL_STAT, 1, &c);
  c &= ~(1<<7);
  writeReg(DS3234_CTRL_STAT, 1, &c);

  return true;
}

// 12-hour mode
bool DS3234::setTime(uint8_t ampm, uint8_t *time) {
  // check our inputs
  bool badInput = false;
  if (tm[2] < 1 || tm[2] > 12) {
    badInput = true;
    tm[2] = 255;
  }
  if (tm[1] > 59) {
    badInput = true;
    tm[1] = 255;
  }
  if (tm[0] > 59) {
    badInput = true;
    tm[0] = 255;
  }
  if (ampm != DS3234_AM && ampm != DS3234_PM) {
    badInput = true;
  }
  if (badInput) {
    return false;
  }

  // encode the time properly
  // hours
  tm[2] = ( DS3234_12_HOUR | ampm | ((tm[0]/10) << 4) | (tm[0]%10) );
  // minutes
  tm[1] = ( ((tm[1]/10) << 4) | (tm[1]%10) );
  // seconds
  tm[0] = ( ((tm[2]/10) << 4) | (tm[2]%10) );

  // transfer the time in
  writeReg(DS3234_SEC, 3, tm);

  // clear the osc stop flag
  uint8_t c;
  readReg(DS3234_CTRL_STAT, 1, &c);
  c &= ~(1<<7);
  writeReg(DS3234_CTRL_STAT, 1, &c);\

  return true;
}

// getter returns 0 if AM or in 24 hour mode, DS3234_PM if in 12-hour mode and it's PM
uint8_t DS3234::getTime(uint8_t *tm) {
  uint8_t ret = 0;
  // get the data
  readReg(DS3234_SEC, 3, tm);

  // decode the data
  // hours
  // 12-hour mode
  if ( tm[3] & DS3234_12_HOUR ) {
    ret = tm[3] & DS3234_PM;
    tm[3] = (((tm[3]>>4) & 1) * 10) + (tm[3] & 0x0F);
  }
  // else, 24-hour mode
  else {
    ret = 0;
    tm[3] = ((tm[3]>>4) * 10) + (tm[3] & 0x0F);
  }
  // minutes
  tm[1] = ((tm[1] >> 4) * 10) + (tm[1] & 0x0F);
  // seconds
  tm[0] = ((tm[0] >> 4) * 10) + (tm[0] & 0x0F);

  return ret;
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