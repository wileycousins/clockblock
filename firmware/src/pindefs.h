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

// PIN DEFINES FOR V0.1
// SPI
#define SPI_MOSI_DDR  DDRB
#define SPI_MOSI_PIN  3
#define SPI_SCK_DDR   DDRB
#define SPI_SCK_PIN   5

// RTC
// RTC pins
#define RTC_CS_PORT   PORTC
#define RTC_CS_PIN    2
#define RTC_RST_PORT  PORTC
#define RTC_RST_PIN   3
#define RTC_INT_PORT  PORTD
#define RTC_INT_PIN   2
// avr external interrupt
#define RTC_EXT_INT       0
#define RTC_EXT_INT_vect  INT0_vect

// TLC5971
#define TLC_N          3
#define TLC_N_LEDS     (TLC_N * 12)
#define TLC_MOSI_PORT  PORTC
#define TLC_MOSI_PIN   4
#define TLC_SCK_PORT   PORTC
#define TLC_SCK_PIN    5

// switches
// switch pins
#define INPUT_PORT  PORTD
#define INPUT_HOUR  (1<<7)
#define INPUT_MIN   (1<<6)
#define INPUT_MASK  (INPUT_HOUR | INPUT_MIN)
//#define INPUT_MODE
// switch interrupts
#define INPUT_PCMSK       PCMSK2
#define INPUT_PCICR       PCICR
#define INPUT_PCIE        (1<<PCIE2)
#define INPUT_PCINT_vect  PCINT2_vect

// unused pins to be pulled up internally
#define UNUSED_PORTA_MASK
#define UNUSED_PORTB_MASK  ( (1<<7) | (1<<6) | (1<<2) | (1<<1) | (1<<0) )
#define UNUSED_PORTC_MASK  ( (1<<1) | (1<<0) )
#define UNUSED_PORTD_MASK  ( (1<<5) | (1<<4) | (1<<3) )

#endif

