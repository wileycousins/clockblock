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
#define DISPLAY_DISCRETE_LVL 100
// clock stuff
#define DISPLAY_NUM_DOTS 36

// effect modes
#define DISPLAY_NUM_MODES  2
#define DISPLAY_MODE_FILL  0
#define DISPLAY_MODE_BLEND 1


// display parameters (gets passed from getDisplay to the specific private mode method)
// contains the current time to display as well as a pointer to the dots array the app will give the LED drivers
typedef struct {
  uint8_t hour;
  uint8_t min;
  uint8_t sec;
  uint16_t *dots;
} DisplayParams;

class Display {
public:
  // constructor initializes the mode to something
  Display();
  
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
  // display mode
  uint8_t mode;

  // different effects
  void displayFill(DisplayParams p);
  void displayBlend(DisplayParams p);
};

#endif