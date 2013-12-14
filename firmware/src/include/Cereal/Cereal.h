// clockblock
// Copyright 2013 by Wiley Cousins, LLC.
// shared under the terms of the MIT License
//
// file: Cereal.h
// description: header for basic UART class

#ifndef LIB_CEREAL_H
#define LIB_CEREAL_H

// UART ISRs
ISR(LIN_TC_vect, ISR_NOBLOCK);

// UART serial methods and buffers
class Cereal {
public:
  // contructor - um, like, probably does some stuff
  Cereal();
  // initialization function
  void init(void);
  // number of characters recieved but not yet read
  uint8_t incoming(void);
  // number of characters to be sent that have not been transmitted
  uint8_t outgoing(void);
  // receive data (read data from incoming queue)
  // returns number of bytes read from the incoming queue
  uint8_t recieve(uint8_t *b, uint8_t n);
  // send data (put data in the outgoing queue)
  void send(uint8_t *b, uint8_t n);

private:
  // incoming data queue
  CerealQueue inQ(CEREAL_IN_Q_LEN);
  // outgoing data queue
  CerealQueue outQ(CEREAL_OUT_Q_LEN);
  // declare ISR as a friend of the class so it can access shit
  friend void LIN_TC_vect(void);
};

// queue for buffering incoming and outgoing serial data
// queue = FIFO
class CerealQueue {
public:
  // constructor allocates buffers 
  CerealQueue(uint8_t qMaxLen);
  // push a character onto the queue
  void push(uint8_t c);
  // pop a character from the queue
  uint8_t pop(void);
  // get the length of the queue
  uint8_t length(void);
private:
  // pointer to array used as queue storage
  uint8_t *buff;
  // maximum length of the queue, queue head, and length of the queue
  uint8_t maxLen;
  uint8_t head;
  uint8_t len;
};

#endif