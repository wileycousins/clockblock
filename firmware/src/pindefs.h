// clockblock
// Copyright 2013 by Wiley Cousins, LLC.
// shared under the terms of the MIT License
//
// file: pindefs.h
// description: port and pin definitions

#ifndef CLOCKBLOCK_PINDEFS_H
#define CLOCKBLOCK_PINDEFS_H

// include for AVR register addresses
#include <avr/io.h>

// DDR and PIN from port register macros
#define DDR(port) (*(&port-1))
#define PIN(port) (*(&port-2))

// SPI
#define SPI_MOSI_DDR  DDRB
#define SPI_MOSI_PIN  3
#define SPI_SCK_DDR   DDRB
#define SPI_SCK_PIN   5

// RTC
// RTC pins
#define RTC_CS_PORT   PORTB
#define RTC_CS_PIN    2
#define RTC_RST_PORT  PORTB
#define RTC_RST_PIN   0
#define RTC_INT_PORT  PORTD
#define RTC_INT_PIN   2
// avr external interrupt
#define RTC_EXT_INT       0
#define RTC_EXT_INT_vect  INT0_vect

// TLC5971
#define TLC_MOSI_PORT  PORTC
#define TLC_MOSI_PIN   4
#define TLC_SCK_PORT   PORTC
#define TLC_SCK_PIN    5

// switches
// switch pins
#define INPUT_PORT  PORTC
#define INPUT_HOUR  (1<<0)
#define INPUT_MIN   (1<<1)
//#define INPUT_MODE
// switch interrupts
#define INPUT_PCMSK  PCMSK1
#define INPUT_PCICR  PCICR
#define INPUT_PCIE   (1<<PCIE1)

#endif
