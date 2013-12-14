// clockblock
// Copyright 2013 by Wiley Cousins, LLC.
// shared under the terms of the MIT License
//
// file: Cereal.cpp
// description: basic class for handling UART serial communication

// avr includes
#include <stdlib.h>

// class definition
#include "Cereal.h"




// constructor allocates buffers 
CerealQueue::CerealQueue(uint8_t qMaxLen) {
  // set the max length of the array
  maxLen = qMaxLen;
  // allocate space in the heap for the buffer
  buff = (int *)(malloc(maxLen * sizeof(uint8_t)));
  // set the head and the length to 0
  head = 0;
  len = 0;
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
