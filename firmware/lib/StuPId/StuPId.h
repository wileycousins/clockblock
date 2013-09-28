// clockblock
// Copyright 2013 by Wiley Cousins, LLC.
// shared under the terms of the MIT License
//
// file: StuPId.h
// description: header for stupid little SPI master class

#ifndef LIB_STUPID_H
#define LIB_STUPID_H

#include <stdint.h>

// data order defines
#define SPI_MSB_FIRST 0
#define SPI_LSB_FIRST (1<<5)

// clock divider defines
#define SPI_DIV_4    0
#define SPI_DIV_16   1
#define SPI_DIV_64   2
#define SPI_DIV_128  3

#define SPI_SPEED_NORMAL 0
#define SPI_SPEED_DOUBLE 1


class StuPId {
public:
  // constructor takes in data direction registers for MOSI and SCK
  StuPId(volatile uint8_t *mosiDdr, uint8_t mosiPin, volatile uint8_t *sckDdr, uint8_t sckPin);
  // set pin directions and enable SPI
  void enable();
  // diable SPI
  void disable();
  // set data mode, overloaded to either take modes 0-3 or the polaity phase pair
  void setDataMode(uint8_t mode);
  void setDataMode(uint8_t pol, uint8_t pha);
  // set the data direction (MSB_FIRST or LSB_FIRST)
  void setDataOrder(uint8_t order);
  // set the interface speed
  // takes in the clock divider and whether or not to run at double speed
  void setDataRate(uint8_t divide, uint8_t dbl);
  // transfer data
  // synchronous protocol, so it writes the parameter and returns what it recieves
  uint8_t transfer(uint8_t data);

private:
  bool enabled;
  volatile uint8_t *ddrMosi;
  uint8_t pinMosi;
  volatile uint8_t *ddrSck;
  uint8_t pinSck;
};

#endif