// clockblock
// Copyright 2013 by Wiley Cousins, LLC.
// shared under the terms of the MIT License
//
// file: Cereal.cpp
// description: basic class for handling UART serial communication

// avr includes
#include <stdlib.h>

// pindefs from main app with PORT and pin defines
#include "pindefs.h"
// check that necessary pins and ports are defined
#ifndef UART_TX_PORT
#error "UART_TX_PORT is not defined; please define in pindefs.h"
#endif
#ifndef UART_TX_PIN
#error "UART_TX_PIN is not defined; please define in pindefs.h"
#endif
#ifndef UART_RX_PORT
#error "UART_RX_PORT is not defined; please define in pindefs.h"
#endif
#ifndef UART_RX_PIN
#error "UART_RX_PIN is not defined; please define in pindefs.h"
#endif

// class definition
#include "Cereal.h"




// uart serial communication class
// contructor - um, like, probably does some stuff
Cereal::Cereal(void) {
  // actually nevermind
}

// initialization function
void Cereal::init(void) {
  // allocate some buffers
  inQ.setMaxLength(CEREAL_IN_Q_LEN);
  outQ.setMaxLength(CEREAL_OUT_Q_LEN);

  // set Tx as output and Rx as input with pullup enabled
  DDR(UART_TX_PORT) |= (1 << UART_TX_PIN);
  DDR(UART_RX_PORT) &= ~(1 << UART_RX_PIN);
  UART_RX_PORT |= (1 << UART_RX_PIN);

  // set baudrate to 9600
  // leave bit timing to default (LINBTR & 0x3F) = 0x20 = 32, but disable frame resync (unused by UART)
  LINBTR |= (1<<LDISR);
  // LINBRR = (F_CPU/((LINBTR & 0x3F) * BAUDRATE)) - 1
  // LINBRR = (8000000/(32 * 9600)) - 1 = 25 + 0.167%
  LINBRR = 25;
  // enable recieve interrupt
  LINENIR = (1<<LENRXOK);
  // enable lin/uart module, set to uart mode, set to 8 bit char, no parity, RX and TX enabled
  LINCR = ( (1<<LENA) | (1<<LCMD2) | (1<<LCMD1) | (1<<LCMD0) );
}

// number of characters recieved but not yet read
uint8_t Cereal::incoming(void) {
  return inQ.length();
}

// number of characters to be sent that have not been transmitted
uint8_t Cereal::outgoing(void) {
  return outQ.length();
}

// receive data (read data from incoming queue)
// returns number of bytes read from the incoming queue
uint8_t Cereal::recieve(uint8_t *b, uint8_t n) {
  for (uint8_t i=0; i<n; i++) {
    if (inQ.length()) {
      b[i] = inQ.pop();
    }
    else {
      // if q is empty, return number of bytes read
      return i;
    }
  }
  // if loop exits on its own, all requested bytes were received
  return n;
}

// send one character
void Cereal::send(uint8_t c) {
  send(&c, 1);
}

// send data (put data in the outgoing queue)
void Cereal::send(uint8_t *b, uint8_t n) {
  // push data to the outgoing queue
  for (uint8_t i=0; i<n; i++) {
    outQ.push(b[i]);
  }
  // if not currently transmitting
  if (!txFlag) {
    // enable the transmit complete interrupt
    LINENIR |= (1<<LENTXOK);
    // send the first character to start transfer
    LINDAT = outQ.pop();
    // set the flag
    txFlag = true;
  }
}

// handle transfer complete interrupt
void Cereal::handleTransfer(void) {
  // check recieve complete flag
  if (LINSIR & (1<<LRXOK)) {
    // push incoming data to the queue and clear flag
    inQ.push(LINDAT);
  }
  // check transmit complete flag
  if (LINSIR & (1<<LTXOK)) {
    // if there's still data to send
    if (outQ.length()) {
      LINDAT = outQ.pop();
    }
    // if there's no more data, disable the transmit interrupt
    else {
      LINENIR &= ~(1<<LENTXOK);
      txFlag = false;
    }
  }
}



// queue class for transmitting and recieving characters
// constructor allocates buffers 
CerealQueue::CerealQueue() {
  // set the max length of the array
  maxLen = 0;
  // set buffer pointer to the null pointer
  buff = 0;
  // set the head and the length to 0
  head = 0;
  len = 0;
}

// set the maximum length of the queue
void CerealQueue::setMaxLength(uint8_t qMaxLen) {
  maxLen = qMaxLen;
  // reallocate previous buffer to new length
  // if buffer is null, realloc performs as malloc
  buff = (uint8_t *)(realloc(buff, maxLen * sizeof(CerealQueueElement_t)));
}

// push a character onto the queue
// if the queue is full, the oldest char will be lost
void CerealQueue::push(uint8_t c) {
  // get the tail location of the queue
  uint8_t tail = head + len;
  if (tail >= maxLen) {
    tail -= maxLen;
  }
  // push the data onto the queue
  buff[tail] = c;
  // move the head if the queue was full
  if (++len > maxLen) {
    head += len - maxLen;
    len = maxLen;
  }
}

// pop the oldest character from the queue
uint8_t CerealQueue::pop(void) {
  // initialize the char to be returned at the null char
  uint8_t c = '\0';
  // check to make sure there's data in the queue
  if (len) {
    // get the char from the array
    c = buff[head];
    // increment the head and check for overflow
    if (++head > maxLen - 1) {
      head -= maxLen;
    }
    // decrement the length
    len--;
  }
  // return data (or the null chracter is the queue was empty)
  return c;
}

// get the length of the queue
uint8_t CerealQueue::length(void) {
  return len;
}
