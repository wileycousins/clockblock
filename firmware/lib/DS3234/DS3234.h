// clockblock
// Copyright 2013 by Wiley Cousins, LLC.
// shared under the terms of the MIT License
//
// file: clockblock.cpp
// description: header for DS3234 real-time clock class

#ifndef LIB_DS3234_H
#define LIB_DS3234_H

// woo! typedefs!
#include <stdint.h>
// woo! SPI!
#include "StuPId.h"

// let's define some register addresses
#define DS3234_SEC               0x00
#define DS3234_MIN               0x01
#define DS3234_HOUR              0x02
#define DS3234_DAY               0x03
#define DS3234_DATE              0x04
#define DS3234_MONTH_CENT        0x05
#define DS3234_YEAR              0x06
#define DS3234_ALARM_1_SEC       0x07
#define DS3234_ALARM_1_MIN       0x08   
#define DS3234_ALARM_1_HOUR      0x09
#define DS3234_ALARM_1_DAY_DATE  0x0A
#define DS3234_ALARM_2_MIN       0x0B
#define DS3234_ALARM_2_HOUR      0x0C
#define DS3234_ALARM_2_DAY_DATE  0x0D
#define DS3234_CTRL              0x0E
#define DS3234_CTRL_STAT         0x0F
#define DS3234_XOSC_AGE_OFF      0x10
#define DS3234_TEMP_MSB          0x11
#define DS3234_TEMP_LSB          0x12
#define DS3234_DISABLE_TEMP      0x13
#define DS3234_SRAM_ADDR         0x18
#define DS3234_SRAM_DATA         0x19

// write command
#define DS3234_SPI_WRITE         0x80

// AM / PM and 12 / 24 hour modes
// bits 5 and 6 respectively in the hour registers
#define DS3234_AM       0
#define DS3234_PM       (1<<5)
#define DS3234_24_HOUR  0
#define DS3234_12_HOUR  (1<<6)

// day / date modes
// bit 6 in the day / date registers
#define DS3234_DAY_MODE   (1<<6)
#define DS3234_DATE_MODE  0

class DS3234 {
public:
  // constructor
  DS3234( /* StuPId *spi, */ volatile uint8_t *csPo, uint8_t csPi, volatile uint8_t *rstPo, uint8_t rstPi, volatile uint8_t *intPo, uint8_t intPi);
  // initialize (sets pins up and stuff), returns false if the osc has stopped and the time is therefore suspect, true otherwise
  bool init();
  // time getter and setter
  // setter returns true if given a proper time
  // overloaded for AM/PM mode or 24 hour mode
  bool setTime(uint8_t *time);
  bool setTime(uint8_t ampm, uint8_t *time);
  uint8_t getTime(uint8_t *time);

//private:
  // SPI helpers
  void spiStart();
  void spiEnd();
  void readReg(uint8_t reg, uint8_t n, uint8_t *data);
  void writeReg(uint8_t reg, uint8_t n, uint8_t *data);

  // pin assignments
  // chip select
  volatile uint8_t *csPort;
  uint8_t csPin;
  // reset pin
  volatile uint8_t *rstPort;
  uint8_t rstPin;
  // interrupt pin
  volatile uint8_t *intPort;
  uint8_t intPin;

};

#endif