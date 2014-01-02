// clockblock
// Copyright 2013 by Wiley Cousins, LLC.
// shared under the terms of the MIT License
//
// file: StuPId.cpp
// description: stupid little SPI master class

// avr include
#include "avr/io.h"

// pindefs from main app with DDR and pin defines
#include "pindefs.h"
// check that necessary pins and ports are defined
#ifndef SPI_MOSI_DDR
#error "SPI_MOSI_DDR is not defined; please define in pindefs.h"
#endif
#ifndef SPI_MOSI_PIN
#error "SPI_MOSI_PIN is not defined; please define in pindefs.h"
#endif
#ifndef SPI_SCK_DDR
#error "SPI_SCK_DDR is not defined; please define in pindefs.h"
#endif
#ifndef SPI_SCK_PIN
#error "SPI_SCK_PIN is not defined; please define in pindefs.h"
#endif

// class definition
#include "StuPId.h"

// constructor takes in data direction registers for MOSI and SCK
StuPId::StuPId() {
  /*
  // save mosi pin
  ddrMosi = mosiDdr;
  pinMosi = mosiPin;
  // save sck pin
  ddrSck = sckDdr;
  pinSck = sckPin;
  */
}

// set pin directions and enable SPI
void StuPId::enable() {
  SPI_MOSI_DDR |= (1 << SPI_MOSI_PIN);
  SPI_SCK_DDR |= (1 <<SPI_SCK_PIN);

  // set the SPI enable bit and the master mode bit
  SPCR |= ( (1 << SPE) | (1 << MSTR) );
  enabled = true;
}

// diable SPI
void StuPId::disable() {
  SPCR &= ~(1 << SPE);
  enabled = false;
}

// returns if enabled
bool StuPId::isEnabled() {
  if (SPCR & (1<<SPE)) {
    return true;
  }
  else {
    return false;
  }
}

// set data mode, overloaded to either take modes 0-3 or the polarity phase pair
void StuPId::setDataMode(uint8_t mode) {
  if (mode < 4) {
    SPCR = ( (SPCR & ~((1<<CPHA)|(1<<CPOL))) | (mode << CPHA) );
  }
}

void StuPId::setDataMode(uint8_t pol, uint8_t pha) {
  if (pol>1) {
    pol = 1;
  }
  if (pha>1) {
    pha = 1;
  }
  setDataMode( (pol<<1) | pha );
}

// set the data direction (MSB_FIRST or LSB_FIRST)
void StuPId::setDataOrder(uint8_t order) {
  if (order == SPI_MSB_FIRST || order == SPI_LSB_FIRST) {
    SPCR = ( (SPCR & ~(SPI_LSB_FIRST)) | order );
  }
}

// set the interface speed
// takes in the clock divider and whether or not to run at double speed
void StuPId::setDataRate(uint8_t divide, uint8_t dbl) {
  if (divide <= SPI_DIV_128) {
    SPCR = ( (SPCR & ~(SPI_DIV_128)) | divide);
  }
  if (dbl <= SPI_SPEED_DOUBLE) {
    SPSR = ( (SPSR & ~(SPI_SPEED_DOUBLE)) | dbl );
  }
}

// transfer data
// synchronous protocol, so it writes the parameter and returns what it recieves
uint8_t StuPId::transfer(uint8_t data) {
  if (enabled) {
    SPDR = data;
    while(!(SPSR & (1<<SPIF)));
    return SPDR;
  }
  else {
    return 0;
  }
}
