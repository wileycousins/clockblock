// clockblock
// Copyright 2013 by Wiley Cousins, LLC.
// shared under the terms of the MIT License
//
// file: display.cpp
// description: header for display class that handles computing the time display

#ifndef DISPLAY_H
#define DISPLAY_H

// avr includes
#include <stdint.h>

// display parameters
// LED brightness levels
#define DISPLAY_LVL_MAX 1000
// clock stuff
#define DISPLAY_NUM_DOTS 36

// effect modes
#define DISPLAY_NUM_MODES  4
#define DISPLAY_MODE_FILL  0
#define DISPLAY_MODE_BLEND 1
#define DISPLAY_MODE_PIE   2
#define DISPLAY_MODE_ARMS  3

class Display {
public:
  // constructor initializes the mode
  // parameters:
  //   ms - pointer to the application's millisecond timer for good smoothing
  Display(volatile uint16_t *millis);

  // get the dot display
  // parameters:
  //   hour, min, sec - time
  //   dots - dot array output
  void getDisplay(uint8_t hour, uint8_t min, uint8_t sec, uint16_t *dots);

  // set the display mode
  // parameters:
  //   m - mode as defined above
  void setMode(uint8_t m);

private:
  // display parameters (gets passed from getDisplay to the specific private mode method)
  // contains the current time to display as well as a pointer to the dots array the app will give the LED drivers
  typedef struct {
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint16_t *dots;
  } DisplayParams;

  // location of millisecond counter
  volatile uint16_t *ms;

  // display mode
  uint8_t mode;

  // different effects
  void displayFill(DisplayParams p);
  void displayBlend(DisplayParams p);
  void displayPie(DisplayParams p);
  void displayArms(DisplayParams p);
};

#endif