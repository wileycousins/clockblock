// clockblock
// Copyright 2013 by Wiley Cousins, LLC.
// shared under the terms of the MIT License
//
// file: display.h
// description: header for display class that handles computing the time display

#ifndef CLOCKBLOCK_DISPLAY_H
#define CLOCKBLOCK_DISPLAY_H

// avr includes
#include <stdint.h>
#include <util/atomic.h>

// display parameters
// LED brightness levels
#define DISPLAY_LVL_MAX  65535
#define DISPLAY_LVL_BG   700
#define DISPLAY_LVL_OFF  0
// clock stuff
#define DISPLAY_NUM_DOTS    36
// framerate
#define DISPLAY_FRAMERATE        32
#define DISPLAY_FRAMERATE_FLOAT  32.0

// fixed point scaling factors
// hours
//   - fixed point factor: 2^8
#define DISPLAY_HOUR_FACTOR   256
#define DISPLAY_HOUR_L_SHIFT  8
#define DISPLAY_HOUR_R_SHIFT  16
// minutes
//   - fixed point factor: 2^4
#define DISPLAY_MIN_FACTOR   16
#define DISPLAY_MIN_L_SHIFT  4
#define DISPLAY_MIN_R_SHIFT  8
// seconds
//   - fixed point factor: 2^0
#define DISPLAY_SEC_FACTOR   1
#define DISPLAY_SEC_L_SHIFT  0
#define DISPLAY_SEC_R_SHIFT  0

// effect modes
#define DISPLAY_NUM_MODES      3
#define DISPLAY_MODE_BLEND     0
#define DISPLAY_MODE_BLEND_BG  1
#define DISPLAY_MODE_FILL      2
#define DISPLAY_MODE_PIE       3
#define DISPLAY_MODE_ARMS      4

class Display {
public:
  // constructor initializes the mode
  Display(void);

  // get the dot display
  // parameters:
  //   tm - {sec, min, hr}
  //   frame - animation frame
  //   dots - dot array output
  void getDisplay(uint8_t *tm, uint8_t frame, uint16_t *dots);

  // set / get the display mode
  // parameters:
  //   m - mode as defined above
  void setMode(uint8_t m);
  uint8_t getMode(void);

private:
  // display parameters (gets passed from getDisplay to the specific private mode method)
  // contains the current time to display as well as a pointer to the dots array the app will give the LED drivers
  typedef struct {
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint8_t frame;
  } DisplayParams;

  // display mode
  uint8_t mode;

  // ratio of brightness to frame count for fixed point calculation of LED settings
  uint32_t secLevelScale;
  uint32_t minLevelScale;
  uint32_t hourLevelScale;

  // different effects
  void displayFill(DisplayParams p, uint16_t* dots);
  void displayBlend(DisplayParams p, uint16_t bgLvl, uint16_t* dots);
  void displayPie(DisplayParams p, uint16_t* dots);
  void displayArms(DisplayParams p, uint16_t* dots);
};

#endif
