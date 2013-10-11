// clockblock
// Copyright 2013 by Wiley Cousins, LLC.
// shared under the terms of the MIT License
//
// file: display.cpp
// description: display class to handle computing the time display

#include "display.h"

Display::Display() {
  // set default mode to fill
  mode = DISPLAY_MODE_FILL;
}

void Display::getDisplay(uint8_t hour, uint8_t min, uint8_t sec, uint16_t *dots) {
  DisplayParams p = {
    hour,
    min,
    sec,
    dots
  };

  // display mode switch case
  switch (mode) {
    case DISPLAY_MODE_FILL:
      displayFill(p);
      break;

    case DISPLAY_MODE_BLEND:
      displayBlend(p);
      break;

    default:
      break;
  }
}

void Display::setMode(uint8_t m) {
  if (m < DISPLAY_NUM_MODES) {
    mode = m;
  }
}

// different effects
void Display::displayFill(DisplayParams p) {
  // hands
  uint8_t minHand = p.min/5;
  uint8_t secHand = p.sec/5;
  // fractions
  uint8_t minFrac = p.min/12;
  // mods
  uint8_t minMod = p.min%5;
  uint8_t secMod = p.sec%5;
  
  // fill the hour dots
  // all hours previous are full
  for (uint8_t i=0; i<p.hour; i++) {
    p.dots[i*3] = 5*DISPLAY_DISCRETE_LVL;
  }
  // current hour to fraction
  p.dots[p.hour*3] = DISPLAY_DISCRETE_LVL*(minFrac+1);
  // all other hours off
  for (uint8_t i=p.hour+1; i<12; i++) {
    p.dots[i*3] = 0;
  }

  // do the same with the minute dots
  // all minute dots previous get set to full
  for (uint8_t i=0; i<minHand; i++) {
    p.dots[(i*3)+1] = 5*DISPLAY_DISCRETE_LVL;
  }
  // current minute dot to fraction
  p.dots[(minHand*3)+1] = DISPLAY_DISCRETE_LVL*(minMod+1);
  // all other minute dots off
  for (uint8_t i=minHand+1; i<12; i++) {
    p.dots[(i*3)+1] = 0;
  }

  // finally, seconds
  // all second dots previous get set to full
  for (uint8_t i=0; i<secHand; i++) {
    p.dots[(i*3)+2] = 5*DISPLAY_DISCRETE_LVL;
  }
  // current second dot to fraction
  p.dots[(secHand*3)+2] = DISPLAY_DISCRETE_LVL*(secMod+1);
  // all other second dots off
  for (uint8_t i=secHand+1; i<12; i++) {
    p.dots[(i*3)+2] = 0;
  }
}

void Display::displayBlend(DisplayParams p) {

}


