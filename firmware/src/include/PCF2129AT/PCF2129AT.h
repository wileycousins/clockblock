// clockblock
// Copyright 2013 by Wiley Cousins, LLC.
// shared under the terms of the MIT License
//
// file: PCF2129AT.h
// description: header for NXP PCF2129AT real-time clock class

#ifndef LIB_PCF2129AT_H
#define LIB_PCF2129AT_H

// woo! typedefs!
#include <stdint.h>
// woo! SPI!
#include "StuPId.h"

// let's define some register addresses
#define PCF2129AT_CTRL_1           0x00
#define PCF2129AT_CTRL_2           0x01
#define PCF2129AT_CTRL_3           0x02
#define PCF2129AT_SEC              0x03
#define PCF2129AT_MIN              0x04
#define PCF2129AT_HOUR             0x05
#define PCF2129AT_DAY              0x06
#define PCF2129AT_WEEKDAY          0x07
#define PCF2129AT_MONTH            0x08
#define PCF2129AT_YEAR             0x09
#define PCF2129AT_SEC_ALARM        0x0A
#define PCF2129AT_MIN_ALARM        0x0B   
#define PCF2129AT_HOUR_ALARM       0x0C
#define PCF2129AT_DAY_ALARM        0x0D
#define PCF2129AT_WEEKDAY_ALARM    0x0E
#define PCF2129AT_CLKOUT_CTRL      0x0F
#define PCF2129AT_WATCHGDOG_CTRL   0x10
#define PCF2129AT_WATCHDOG_VAL     0x11
#define PCF2129AT_TIMESTAMP_CTRL   0x12
#define PCF2129AT_SEC_TIMESTAMP    0x13
#define PCF2129AT_MIN_TIMESTAMP    0x14
#define PCF2129AT_HOUR_TIMESTAMP   0x15
#define PCF2129AT_DAY_TIMESTAMP    0x16
#define PCF2129AT_MONTH_TIMESTAMP  0x17
#define PCF2129AT_YEAR_TIMESTAMP   0x18
#define PCF2129AT_AGE_OFFSET       0x19

// control register 1 (PCF2129AT_CTRL_1) bits
#define PCF2129AT_EXT_TEST  (1<<7)
#define PCF2129AT_STOP      (1<<5)
#define PCF2129AT_TSF1      (1<<4)
#define PCF2129AT_POR_OVRD  (1<<3)
#define PCF2129AT_12_24     (1<<2)
#define PCF2129AT_MI        (1<<1)
#define PCF2129AT_SI        (1<<0)

// control register 2 (PCF2129AT_CTRL_2) bits
#define PCF2129AT_MSF   (1<<7)
#define PCF2129AT_WDTF  (1<<6)
#define PCF2129AT_TSF2  (1<<5)
#define PCF2129AT_AF    (1<<4)
#define PCF2129AT_TSIE  (1<<2)
#define PCF2129AT_AIE   (1<<1)

// control register 3 (PCF2129AT_CTRL_3) bits
#define PCF2129AT_PWRMNG2  (1<<7)
#define PCF2129AT_PWRMNG1  (1<<6)
#define PCF2129AT_PWRMNG0  (1<<5)
#define PCF2129AT_BTSE     (1<<4)
#define PCF2129AT_BF       (1<<3)
#define PCF2129AT_BLF      (1<<2)
#define PCF2129AT_BIE      (1<<1)
#define PCF2129AT_BLIE     (1<<0)

// clkout control register bits
#define PCF2129AT_TCR1  (1<<7)
#define PCF2129AT_TCR0  (1<<6)
#define PCF2129AT_COF2  (1<<2)
#define PCF2129AT_COF1  (1<<1)
#define PCF2129AT_COF0  (1<<0)

// clkout modes
#define PCF2129AT_CLKOUT_32_kHz  0
#define PCF2129AT_CLKOUT_16_kHz  (PCF2129AT_COF0)
#define PCF2129AT_CLKOUT_8_kHz   (PCF2129AT_COF1)
#define PCF2129AT_CLKOUT_4_kHz   (PCF2129AT_COF1 | PCF2129AT_COF0)
#define PCF2129AT_CLKOUT_2_kHz   (PCF2129AT_COF2)
#define PCF2129AT_CLKOUT_1_kHz   (PCF2129AT_COF2 | PCF2129AT_COF0)
#define PCF2129AT_CLKOUT_1_Hz    (PCF2129AT_COF2 | PCF2129AT_COF1)
#define PCF2129AT_CLKOUT_OFF     (PCF2129AT_COF2 | PCF2129AT_COF1 | PCF2129AT_COF0)

// AM / PM - bit 5 in the hour registers
#define PCF2129AT_AM  0
#define PCF2129AT_PM  (1<<5)

// oscillator stopped flag - bit 7 in the seconds register
#define PCF2129AT_OSF  (1<<7)

// write command
#define PCF2129AT_SPI_READ   (1<<7)
#define PCF2129AT_SPI_WRITE  0
#define PCF2129AT_SPI_SA     0x20




class PCF2129AT {
public:
  // constructor
  PCF2129AT(StuPId *s, volatile uint8_t *csPo, uint8_t csPi, volatile uint8_t *sqwPo, uint8_t sqwPi);
  // initialize (sets pins up and stuff), returns false if the osc has stopped and the time is therefore suspect, true otherwise
  void init();

  // check to see if the osc has stopped
  bool hasLostTime();

  // square wave control
  void setSquareWave(uint8_t mode);

  // time getter and setter
  // setter returns true if given a proper time
  // overloaded for AM/PM mode or 24 hour mode
  bool setTime(uint8_t *time);
  bool setTime(uint8_t ampm, uint8_t *time);
  uint8_t getTime(uint8_t *time);

private:
  // pointer to SPI object
  StuPId *spi;
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
  volatile uint8_t *sqwPort;
  uint8_t sqwPin;
};

#endif