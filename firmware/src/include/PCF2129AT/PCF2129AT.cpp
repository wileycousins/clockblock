// clockblock
// Copyright 2013 by Wiley Cousins, LLC.
// shared under the terms of the MIT License
//
// file: PCF2129AT.cpp
// description: class for NXP PCF2129AT real-time clock

#include "PCF2129AT.h"

// can we build it? yes we can!
// parameters:
//   SPI channel (implement later when I've written an SPI library)
//   chip select port, chip select pin
//   reset port, reset pin
//   interrupt port, interrupt pin
PCF2129AT::PCF2129AT(StuPId *s, volatile uint8_t *csPo, uint8_t csPi, volatile uint8_t *sqwPo, uint8_t sqwPi) {
  // save this shit
  // SPI channel
  spi = s;
  // chip select pin
  csPort = csPo;
  csPin = csPi;
  // square wave
  sqwPort = sqwPo;
  sqwPin = sqwPi;
}

// initialization - sets pin modes and whatnot
// returns false if osc has stopped, true otherwise (true means the RTC has a good time)
void PCF2129AT::init() {
  // sets chip select pin to output and pulls it high
  // DDR is PORT - 1
  *(csPort-1) |= (1 << csPin);
  *csPort |= (1 << csPin);
  // ensure squarewave pin is set to an input and its pullup resistor is activated
  *(sqwPort-1) &= ~(1 << sqwPin);
  *sqwPort |= (1 << sqwPin);

  // set up SPI
  spi->disable();
  // PCF2129AT is MSB first
  spi->setDataOrder(SPI_MSB_FIRST);
  // runs at up to 2MHz. dividing clock by 4 will ensure this isn't exceeded
  spi->setDataRate(SPI_DIV_4, SPI_SPEED_NORMAL);
  // supports SPI mode 0 (CPOL = 0, CPHA = 0)
  spi->setDataMode(0);
  // enable
  spi->enable();
}

bool PCF2129AT::hasLostTime() {
  uint8_t stat;
  readReg(PCF2129AT_SEC, 1, &stat);
  return (stat & PCF2129AT_OSF);
}

// set the square wave output on the CLKOUT pin at given mode
void PCF2129AT::setSquareWave(uint8_t mode) {
  uint8_t c;
  readReg(PCF2129AT_CLKOUT_CTRL, 1, &c);
  // clear the rate select bits
  c &= ~( PCF2129AT_COF2 | PCF2129AT_COF1 | PCF2129AT_COF0 );
  // set the new rate bits
  if (mode <= PCF2129AT_CLKOUT_OFF) {
    c |= ( mode );
    writeReg(PCF2129AT_CLKOUT_CTRL, 1, &c);
  }
}


// set and get time
// array format is { sec, min, hr }
// AM = 0, PM = 1
// setter will either change array to properly coded data or set bad inputs to 255 and return false
// 24-hour mode
bool PCF2129AT::setTime(uint8_t *tm) {
  uint8_t set[3];

  // check our inputs
  if (tm[2] > 23 || tm[1] > 59 || tm[0] > 59) {
    return false;
  }

  // encode the time properly
  // hours
  set[2] = tm[2] = ( ((tm[2]/10) << 4) | (tm[2]%10) );
  // minutes
  set[1] = tm[1] = ( ((tm[1]/10) << 4) | (tm[1]%10) );
  // seconds
  set[0] = tm[0] = ( ((tm[0]/10) << 4) | (tm[0]%10) );

  // set 24 hour mode
  uint8_t c;
  readReg(PCF2129AT_CTRL_1, 1, &c);
  c &= ~PCF2129AT_12_24;
  writeReg(PCF2129AT_CTRL_1, 1, &c);
  // transfer the time in and clear the osc stop flag
  writeReg(PCF2129AT_SEC, 3, set);

  return true;
}

// 12-hour mode
bool PCF2129AT::setTime(uint8_t *tm, uint8_t ampm) {
  uint8_t set[3];


  // check our inputs
  if (tm[2] < 1 || tm[2] > 12 || tm[1] > 59 || tm[0] > 59 || (ampm != PCF2129AT_AM && ampm != PCF2129AT_PM)) {
    return false;
  }

  // encode the time properly
  // hours
  set[2] = ( ampm | ((tm[2]/10) << 4) | (tm[2]%10) );
  // minutes
  set[1] = ( ((tm[1]/10) << 4) | (tm[1]%10) );
  // seconds
  set[0] = ( ((tm[0]/10) << 4) | (tm[0]%10) );

  // set 12-hour mode
  uint8_t c;
  readReg(PCF2129AT_CTRL_1, 1, &c);
  c |= PCF2129AT_12_24;
  writeReg(PCF2129AT_CTRL_1, 1, &c);
  // transfer the time in and clear the osc stop flag
  writeReg(PCF2129AT_SEC, 3, set);

  return true;
}

// getter returns 0 if AM or in 24 hour mode, PCF2129AT_PM if in 12-hour mode and it's PM
uint8_t PCF2129AT::getTime(uint8_t *tm) {
  uint8_t ret = 0;
  // get mode
  readReg(PCF2129AT_CTRL_1, 1, &ret);
  ret &= PCF2129AT_12_24;
  // get the data
  readReg(PCF2129AT_SEC, 3, tm);

  // decode the data
  // hours
  // 12-hour mode
  if ( ret ) {
    ret = tm[2] & PCF2129AT_PM;
    tm[2] = (((tm[2]>>4) & 1) * 10) + (tm[2] & 0x0F);
  }
  // else, 24-hour mode
  else {
    tm[2] = ((tm[2]>>4) * 10) + (tm[2] & 0x0F);
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
void PCF2129AT::spiStart() {
  // pull the chip select line low to begin the transfer
  *csPort &= ~(1 << csPin);
}

void PCF2129AT::spiEnd() {
  // pull the chip select line high to end the transfer
  *csPort |= (1 << csPin);
}

// read / write
// parameters:
//   register address to start at
//   number of bytes to transfer
//   buffer to read to / write from
void PCF2129AT::readReg(uint8_t reg, uint8_t n, uint8_t *data) {
  spiStart();
  spi->transfer(PCF2129AT_SPI_READ | PCF2129AT_SPI_SA | reg);
  for (uint8_t i=0; i<n; i++) {
    data[i] = spi->transfer(0);
  }
  spiEnd();
}

void PCF2129AT::writeReg(uint8_t reg, uint8_t n, uint8_t *data) {
  spiStart();
  spi->transfer(PCF2129AT_SPI_WRITE | PCF2129AT_SPI_SA | reg);
  for (uint8_t i=0; i<n; i++) {
    spi->transfer(data[i]);
  }
  spiEnd();
}