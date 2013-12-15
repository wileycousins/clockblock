// clockblock
// Copyright 2013 by Wiley Cousins, LLC.
// shared under the terms of the MIT License
//
// file: Cereal.h
// description: header for basic UART class

#ifndef LIB_CEREAL_H
#define LIB_CEREAL_H

// necessary avr includes
#include <avr/io.h>
#include <avr/interrupt.h>

// length of input queue
#define CEREAL_IN_Q_LEN  20
// length of output queue
#define CEREAL_OUT_Q_LEN  20


// queue for buffering incoming and outgoing serial data
// queue = FIFO
class CerealQueue {
public:
  // constructor allocates buffers 
  CerealQueue();
  // set the maximum length of the queue
  void setMaxLength(uint8_t qMaxLen);
  // push a character onto the queue
  void push(uint8_t c);
  // pop a character from the queue
  uint8_t pop(void);
  // get the length of the queue
  uint8_t length(void);
private:
  // buffer holds unsigned chars
  typedef uint8_t CerealQueueElement_t; 
  // pointer to array used as queue storage
  CerealQueueElement_t *buff;
  // maximum length of the queue, queue head, and length of the queue
  uint8_t maxLen;
  uint8_t head;
  uint8_t len;
};

// UART serial methods and buffers
class Cereal {
public:
  // contructor - um, like, probably does some stuff
  Cereal(void);
  // initialization function
  void init(void);
  // number of characters recieved but not yet read
  uint8_t incoming(void);
  // number of characters to be sent that have not been transmitted
  uint8_t outgoing(void);
  // receive data (read data from incoming queue)
  // returns number of bytes read from the incoming queue
  uint8_t recieve(uint8_t *b, uint8_t n);
  // send a single character
  void send(uint8_t c);
  // send data (put data in the outgoing queue)
  void send(uint8_t *b, uint8_t n);
  // handle transfer complete interrupt
  void handleTransfer(void);

private:
  // currently transmitting flag
  volatile bool txFlag;
  // incoming data queue
  CerealQueue inQ;
  // outgoing data queue
  CerealQueue outQ;
};

#endif