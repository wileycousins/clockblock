// clockblock
// Copyright 2013 by Wiley Cousins, LLC.
// shared under the terms of the MIT License
//
// file: display.cpp
// description: display class to handle computing the time display

#include "display.h"

Display::Display(void) {
  // set default mode to fill
  mode = DISPLAY_MODE_FILL;
}

void Display::getDisplay(uint8_t hour, uint8_t min, uint8_t sec, uint8_t frame, uint16_t *dots) {
  DisplayParams p = {
    hour,
    min,
    sec,
    frame,
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

    case DISPLAY_MODE_PIE:
      displayPie(p);
      break;

    case DISPLAY_MODE_ARMS:
      displayArms(p);
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

uint8_t Display::getMode(void) {
  return mode;
}

// different effects
void Display::displayFill(DisplayParams p) {
  // hour correction
  p.hour %= 12;
  // hands
  uint8_t minHand = p.min/5;
  uint8_t secHand = p.sec/5;

  // percentage of the second hand passed (offset by one frame to fill properly)
  float secFrac = ((p.sec%5) + ((p.frame+1)/DISPLAY_FRAMERATE_FLOAT))/5;
  // percentage of minute hand passed
  float minFrac = ((p.min%5) + ((p.sec+((p.frame+1)/DISPLAY_FRAMERATE_FLOAT))/60))/5;
  // percentage of hour passed
  float hourFrac = (((p.frame+1)/DISPLAY_FRAMERATE_FLOAT) + p.sec + (60*p.min))/3600.0;
  
  // fill the hour dots
  // all hours previous are full
  for (uint8_t i=0; i<p.hour; i++) {
    p.dots[i*3] = DISPLAY_LVL_MAX;
  }
  // current hour to fraction
  p.dots[p.hour*3] = (uint16_t)(DISPLAY_LVL_MAX * hourFrac);
  // all other hours off
  for (uint8_t i=p.hour+1; i<12; i++) {
    p.dots[i*3] = 0;
  }

  // do the same with the minute dots
  // all minute dots previous get set to full
  for (uint8_t i=0; i<minHand; i++) {
    p.dots[(i*3)+1] = DISPLAY_LVL_MAX;
  }
  // current minute dot to fraction
  p.dots[(minHand*3)+1] = (uint16_t)(DISPLAY_LVL_MAX * minFrac);
  // all other minute dots off
  for (uint8_t i=minHand+1; i<12; i++) {
    p.dots[(i*3)+1] = 0;
  }

  // finally, seconds
  // all second dots previous get set to full
  for (uint8_t i=0; i<secHand; i++) {
    p.dots[(i*3)+2] = DISPLAY_LVL_MAX;
  }
  // current second dot to fraction
  p.dots[(secHand*3)+2] = (uint16_t)(DISPLAY_LVL_MAX * secFrac);
  // all other second dots off
  for (uint8_t i=secHand+1; i<12; i++) {
    p.dots[(i*3)+2] = 0;
  }

  // wrap-arounds!
  if (p.sec == 59 && p.frame >= 21) {
    // turn off 1 led every frame in the last 11 frames of the second
    for (uint8_t i=1; i<=p.frame-20; i++) {
      p.dots[(i*3)+2] = 0;
    }
    // do the same if minutes are wrapping around
    if (p.min == 59) {
      // turn off 1 led every frame in the last 11 frames of the second
      for (uint8_t i=1; i<=p.frame-20; i++) {
        p.dots[(i*3)+1] = 0;
      }
      // do the same if hours are wrapping around
      if (p.hour == 11) {
        // turn off 1 led every frame in the last 11 frames of the second
        for (uint8_t i=1; i<=p.frame-20; i++) {
          p.dots[i*3] = 0;
        }
      }
    }
  }
}

void Display::displayBlend(DisplayParams p) {
  // hour correction
  p.hour %= 12;

  // hands (take care of the wrap around)
  //uint8_t minHand = p.min/5;
  //uint8_t secHand = p.sec/5;
  uint8_t minHand = 0;
  uint8_t minMod = p.min;
  while (minMod > 4) {
    minHand++;
    minMod -= 4;
  }
  uint8_t secHand = 0;
  uint8_t secMod = p.sec;
  while (minMod > 4) {
    secHand++;
    secMod -= 4;
  }

  uint8_t nextMinHand = (minHand == 11) ? 0 : minHand+1;
  uint8_t nextSecHand = (secHand == 11) ? 0 : secHand+1;
  uint8_t nextHour     = (p.hour == 11) ? 0 : p.hour+1;

  // percentage of the second hand passed
  //float secFrac = ((p.sec%5) + (p.frame/DISPLAY_FRAMERATE_FLOAT)) * 0.2;
  // percentage of minute hand passed
  //float minFrac = ((p.min%5) + ((p.sec+(p.frame/DISPLAY_FRAMERATE_FLOAT))/60))/5;
  // percentage of hour passed
  //float hourFrac = ((p.frame/DISPLAY_FRAMERATE_FLOAT) + p.sec + (60*p.min))/3600.0;
  uint32_t hourFrac = (((p.frame + (p.sec<<5) + ((p.min*60)<<5))  << DISPLAY_LEFT_SHIFT) * DISPLAY_HOUR_SCALE) >> DISPLAY_RIGHT_SHIFT;
  uint32_t minFrac =  (((p.frame + (p.sec<<5) + ((minMod*60)<<5)) << DISPLAY_LEFT_SHIFT) * DISPLAY_MIN_SCALE)  >> DISPLAY_RIGHT_SHIFT;
  uint32_t secFrac =  (((p.frame + (secMod<<5))                   << DISPLAY_LEFT_SHIFT) * DISPLAY_SEC_SCALE)  >> DISPLAY_RIGHT_SHIFT;

  // fill the hour dots
  // all hours previous are off
  for (uint8_t i=0; i<p.hour; i++) {
    p.dots[i*3] = 0;
  }
  // current hour and next hours to percentages of the hour
  p.dots[p.hour*3]   = (uint16_t)(DISPLAY_LVL_MAX - hourFrac);
  p.dots[nextHour*3] = (uint16_t)(hourFrac);
  // all other hours off
  for (uint8_t i=p.hour+2; i<12; i++) {
    p.dots[i*3] = 0;
  }

  // do the same with the minute dots
  // all minute dots previous get set to off
  for (uint8_t i=0; i<minHand; i++) {
    p.dots[(i*3)+1] = 0;
  }
  // current and next minute dot to fractions
  p.dots[(minHand*3)+1]     = (uint16_t)(DISPLAY_LVL_MAX - minFrac);
  p.dots[(nextMinHand*3)+1] = (uint16_t)(minFrac);
  // all other minute dots off
  for (uint8_t i=minHand+2; i<12; i++) {
    p.dots[(i*3)+1] = 0;
  }

  // finally, seconds
  // all second dots previous get set to off
  for (uint8_t i=0; i<secHand; i++) {
    p.dots[(i*3)+2] = 0;
  }
  // current and next second dot to fraction (don't have milliseconds yet, so use modulus)
  p.dots[(secHand*3)+2]     = (uint16_t)(DISPLAY_LVL_MAX - secFrac);
  p.dots[(nextSecHand*3)+2] = (uint16_t)(secFrac);
  // all other second dots off
  for (uint8_t i=secHand+2; i<12; i++) {
    p.dots[(i*3)+2] = 0;
  }
}

void Display::displayPie(DisplayParams p) {
  // hour correction
  p.hour %= 12;
  // percentage of hour passed
  float hourFrac = (p.sec + (60*p.min))/3600.0;

  // set all dots up to hour to full around the clock
  for (uint8_t i=0; i<3*p.hour; i+=3) {
    p.dots[i]   = DISPLAY_LVL_MAX;
    p.dots[i+1] = DISPLAY_LVL_MAX;
    p.dots[i+2] = DISPLAY_LVL_MAX;
  }

  // dots on fractional arm get set according to percentage
  p.dots[3*p.hour]   = (uint16_t)(DISPLAY_LVL_MAX * hourFrac);
  p.dots[3*p.hour+1] = (uint16_t)(DISPLAY_LVL_MAX * hourFrac);
  p.dots[3*p.hour+2] = (uint16_t)(DISPLAY_LVL_MAX * hourFrac);

  // all others off
  for (uint8_t i=3*(p.hour+1); i<DISPLAY_NUM_DOTS; i+=3) {
    p.dots[i]   = 0;
    p.dots[i+1] = 0;
    p.dots[i+2] = 0;
  }
}

void Display::displayArms(DisplayParams p) {
  // hands
  uint8_t hourHand = p.hour % 12;
  uint8_t minHand  = p.min / 5;
  uint8_t secHand  = p.sec / 5;

  // empty out the array
  for (uint8_t i=0; i<DISPLAY_NUM_DOTS; i++) {
    p.dots[i] = 0;
  }

  // set the hands
  p.dots[3*hourHand]    = DISPLAY_LVL_MAX;
  p.dots[(3*minHand)+1] = DISPLAY_LVL_MAX;
  p.dots[(3*secHand)+2] = DISPLAY_LVL_MAX;

}
