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
#define SPI_MOSI_DDR  DDRA
#define SPI_MOSI_PIN  4
#define SPI_SCK_DDR   DDRA
#define SPI_SCK_PIN   5

// RTC
// RTC pins
#define RTC_CS_PORT   PORTA
#define RTC_CS_PIN    3
//#define RTC_RST_PORT  PORTC
//#define RTC_RST_PIN   3
#define RTC_SQW_PORT  PORTB
#define RTC_SQW_PIN   6
// avr external interrupt
#define RTC_EXT_INT       0
#define RTC_EXT_INT_vect  INT0_vect

// TLC5971
#define TLC_N          3
#define TLC_N_LEDS     (TLC_N * 12)
#define TLC_MOSI_PORT  PORTA
#define TLC_MOSI_PIN   6
#define TLC_SCK_PORT   PORTA
#define TLC_SCK_PIN    7

// switches
// switch pins
#define INPUT_PORT  PORTB
#define INPUT_MODE  (1<<2)
#define INPUT_HOUR  (1<<1)
#define INPUT_MIN   (1<<0)
#define INPUT_MASK  (INPUT_MODE | INPUT_HOUR | INPUT_MIN)

// switch interrupts
#define INPUT_PCMSK       PCMSK1
#define INPUT_PCICR       PCICR
#define INPUT_PCIE        (1<<PCIE1)
#define INPUT_PCINT_vect  PCINT1_vect
// switch debounce timer
#define INPUT_TIMER_vect  TIMER1_COMPA_vect

// unused pins to be pulled up internally
#define UNUSED_PORTB_MASK  ( (1<<6) | (1<<5) | (1<<4) | (1<<2) | (1<<1) | (1<<0) )
//#define UNUSED_PORTA_MASK  ()

#endif

