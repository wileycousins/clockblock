// clockblock
// Copyright 2013 by Wiley Cousins, LLC.
// shared under the terms of the MIT License
//
// file: input.cpp
// description: class for handling user input into the clockblock

#include "input.h"
#include "pindefs.h"

// contructor - gives private variables default values
Input::Input(void) {

}

// get switch state
uint8_t Input::getState(void){

}

// get the flag states and clear as necessary
bool Input::getHold(void){

}

bool Input::getPress(void){

}

// handle pin change
void Input::handleChange(void){

}

// handle debouncing
void Input::handleDebounce(void){

}

// disable the debounce timer and wait for a pin change (e.g. to exit a pin hold loop)
void Input::reset(void){

}

// initialization
void Input::init(void){

}

// init switch pins as inputs with pullups enabled
void Input::initPins(void){

}

// init pin change interrupts
void Input::initInt(void){

}

// init timer for debouncing
void Input::initTimer(void){

}

// interrupt helpers
void Input::enableInt(void){

}

void Input::disableInt(void){

}

void Input::enableTimer(void){

}

void Input::disableTimer(void){

}

