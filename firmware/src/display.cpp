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

  // calculate the LED brightness ratios
  secLevelScale = (uint32_t)((65536/(5*DISPLAY_FRAMERATE_FLOAT)) * DISPLAY_SEC_FACTOR);
  minLevelScale = (uint32_t)((65536/(300*DISPLAY_FRAMERATE_FLOAT)) * DISPLAY_MIN_FACTOR);
  hourLevelScale = (uint32_t)((65536/(3600*DISPLAY_FRAMERATE_FLOAT)) * DISPLAY_HOUR_FACTOR);
}

void Display::getDisplay(uint8_t *tm, uint8_t frame, uint16_t *dots) {
  // hour correction
  uint8_t hour = tm[2];
  if (hour == 12) {
    hour = 0;
  }

  DisplayParams p = {
    hour,
    tm[1],
    tm[0],
    frame,
  };

  // display mode switch case
  switch (mode) {
    case DISPLAY_MODE_FILL:
      displayFill(p, dots);
      break;

    case DISPLAY_MODE_BLEND:
      displayBlend(p, dots);
      break;

    case DISPLAY_MODE_PIE:
      displayPie(p, dots);
      break;

    case DISPLAY_MODE_ARMS:
      displayArms(p, dots);
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
void Display::displayFill(DisplayParams p, uint16_t* dots) {
  // hands and moduli
  uint8_t minHand = 0;
  uint8_t minMod = p.min;
  while (minMod > 4) {
    minHand++;
    minMod -= 5;
  }
  uint8_t secHand = 0;
  uint8_t secMod = p.sec;
  while (secMod > 4) {
    secHand++;
    secMod -= 5;
  }

  // offset by 1 frame to get fill to align properly
  p.frame++;

  // fixed point multiplication to find the (within 0.5% of "correct") LED setting
  // get the frame counts
  uint32_t secFrac  = p.frame + (DISPLAY_FRAMERATE * secMod);
  uint32_t minFrac  = p.frame + (DISPLAY_FRAMERATE * (p.sec + minMod*60));
  // cast DISPLAY_FRAMERATE as a 32bit int to make sure the multiply doesn't overflow (because it will otherwise)
  uint32_t hourFrac = p.frame + ((uint32_t)(DISPLAY_FRAMERATE) * (p.sec + p.min*60));
  // scale, multiply, and shift back
  // seconds
  secFrac  = (secFrac << DISPLAY_SEC_L_SHIFT) * secLevelScale;
  secFrac  >>= DISPLAY_SEC_R_SHIFT;
  // minutes
  minFrac  = (minFrac << DISPLAY_MIN_L_SHIFT) * minLevelScale;  
  minFrac  >>= DISPLAY_MIN_R_SHIFT;
  // hours
  hourFrac = (hourFrac << DISPLAY_HOUR_L_SHIFT) * hourLevelScale;
  hourFrac >>= DISPLAY_HOUR_R_SHIFT;

  
  // fill the hour dots
  // all hours previous are full
  for (uint8_t i=0; i<p.hour; i++) {
    dots[i*3] = DISPLAY_LVL_MAX;
  }
  // current hour to fraction
  //dots[p.hour*3] = (uint16_t)(DISPLAY_LVL_MAX * hourFrac);
  dots[p.hour*3] = (uint16_t)(hourFrac);
  // all other hours off
  for (uint8_t i=p.hour+1; i<12; i++) {
    dots[i*3] = 0;
  }

  // do the same with the minute dots
  // all minute dots previous get set to full
  for (uint8_t i=0; i<minHand; i++) {
    dots[(i*3)+1] = DISPLAY_LVL_MAX;
  }
  // current minute dot to fraction
  //dots[(minHand*3)+1] = (uint16_t)(DISPLAY_LVL_MAX * minFrac);
  dots[(minHand*3)+1] = (uint16_t)(minFrac);
  // all other minute dots off
  for (uint8_t i=minHand+1; i<12; i++) {
    dots[(i*3)+1] = 0;
  }

  // finally, seconds
  // all second dots previous get set to full
  for (uint8_t i=0; i<secHand; i++) {
    dots[(i*3)+2] = DISPLAY_LVL_MAX;
  }
  // current second dot to fraction
  //dots[(secHand*3)+2] = (uint16_t)(DISPLAY_LVL_MAX * secFrac);
  dots[(secHand*3)+2] = (uint16_t)(secFrac);
  // all other second dots off
  for (uint8_t i=secHand+1; i<12; i++) {
    dots[(i*3)+2] = 0;
  }

  // wrap-arounds!
  // should probably make this more responsive to framerate changes
  if (p.sec == 59 && p.frame >= 21) {
    // turn off 1 led every frame in the last 11 frames of the second
    for (uint8_t i=1; i<=p.frame-20; i++) {
      dots[(i*3)+2] = 0;
    }
    // do the same if minutes are wrapping around
    if (p.min == 59) {
      // turn off 1 led every frame in the last 11 frames of the second
      for (uint8_t i=1; i<=p.frame-20; i++) {
        dots[(i*3)+1] = 0;
      }
      // do the same if hours are wrapping around
      if (p.hour == 11) {
        // turn off 1 led every frame in the last 11 frames of the second
        for (uint8_t i=1; i<=p.frame-20; i++) {
          dots[i*3] = 0;
        }
      }
    }
  }
}

void Display::displayBlend(DisplayParams p, uint16_t* dots) {
  // hands, moduli, and wrap around)
  uint8_t minHand = 0;
  uint8_t minMod = p.min;
  while (minMod > 4) {
    minHand++;
    minMod -= 5;
  }
  uint8_t secHand = 0;
  uint8_t secMod = p.sec;
  while (secMod > 4) {
    secHand++;
    secMod -= 5;
  }
  uint8_t nextMinHand = (minHand < 11) ? minHand+1 : 0;
  uint8_t nextSecHand = (secHand < 11) ? secHand+1 : 0;
  uint8_t nextHour     = (p.hour < 11) ? p.hour+1 : 0;

  // fixed point multiplication to find the (within 0.5% of "correct") LED setting
  // get the frame counts (frames since last hand)
  uint32_t secFrac  = p.frame + (DISPLAY_FRAMERATE * secMod);
  uint32_t minFrac  = p.frame + (DISPLAY_FRAMERATE * (p.sec + minMod*60));
  // cast DISPLAY_FRAMERATE as a 32bit int to make sure the multiply doesn't overflow (because it will otherwise)
  uint32_t hourFrac = p.frame + ((uint32_t)(DISPLAY_FRAMERATE) * (p.sec + p.min*60));
  // scale, multiply, and shift back
  // seconds
  secFrac  = (secFrac << DISPLAY_SEC_L_SHIFT) * secLevelScale;
  secFrac  >>= DISPLAY_SEC_R_SHIFT;
  // minutes
  minFrac  = (minFrac << DISPLAY_MIN_L_SHIFT) * minLevelScale;  
  minFrac  >>= DISPLAY_MIN_R_SHIFT;
  // hours
  hourFrac = (hourFrac << DISPLAY_HOUR_L_SHIFT) * hourLevelScale;
  hourFrac >>= DISPLAY_HOUR_R_SHIFT;


  // fill the hour dots
  // all hours previous are off
  for (uint8_t i=0; i<p.hour; i++) {
    dots[i*3] = 0;
  }
  // current hour and next hours to percentages of the hour
  dots[p.hour*3]   = (uint16_t)(DISPLAY_LVL_MAX - hourFrac);
  dots[nextHour*3] = (uint16_t)(hourFrac);
  // all other hours off
  for (uint8_t i=p.hour+2; i<12; i++) {
    dots[i*3] = 0;
  }

  // do the same with the minute dots
  // all minute dots previous get set to off
  for (uint8_t i=0; i<minHand; i++) {
    dots[(i*3)+1] = 0;
  }
  // current and next minute dot to fractions
  dots[(minHand*3)+1]     = (uint16_t)(DISPLAY_LVL_MAX - minFrac);
  dots[(nextMinHand*3)+1] = (uint16_t)(minFrac);
  // all other minute dots off
  for (uint8_t i=minHand+2; i<12; i++) {
    dots[(i*3)+1] = 0;
  }

  // finally, seconds
  // all second dots previous get set to off
  for (uint8_t i=0; i<secHand; i++) {
    dots[(i*3)+2] = 0;
  }
  // current and next second dot to fraction (don't have milliseconds yet, so use modulus)
  dots[(secHand*3)+2]     = (uint16_t)(DISPLAY_LVL_MAX - secFrac);
  dots[(nextSecHand*3)+2] = (uint16_t)(secFrac);
  // all other second dots off
  for (uint8_t i=secHand+2; i<12; i++) {
    dots[(i*3)+2] = 0;
  }
}

void Display::displayPie(DisplayParams p, uint16_t* dots) {
  // percentage of hour passed
  float hourFrac = (p.sec + (60*p.min))/3600.0;

  // set all dots up to hour to full around the clock
  for (uint8_t i=0; i<3*p.hour; i+=3) {
    dots[i]   = DISPLAY_LVL_MAX;
    dots[i+1] = DISPLAY_LVL_MAX;
    dots[i+2] = DISPLAY_LVL_MAX;
  }

  // dots on fractional arm get set according to percentage
  dots[3*p.hour]   = (uint16_t)(DISPLAY_LVL_MAX * hourFrac);
  dots[3*p.hour+1] = (uint16_t)(DISPLAY_LVL_MAX * hourFrac);
  dots[3*p.hour+2] = (uint16_t)(DISPLAY_LVL_MAX * hourFrac);

  // all others off
  for (uint8_t i=3*(p.hour+1); i<DISPLAY_NUM_DOTS; i+=3) {
    dots[i]   = 0;
    dots[i+1] = 0;
    dots[i+2] = 0;
  }
}

void Display::displayArms(DisplayParams p, uint16_t* dots) {
  // hands
  uint8_t hourHand = p.hour;
  uint8_t minHand  = p.min / 5;
  uint8_t secHand  = p.sec / 5;

  // empty out the array
  for (uint8_t i=0; i<DISPLAY_NUM_DOTS; i++) {
    dots[i] = 0;
  }

  // set the hands
  dots[3*hourHand]    = DISPLAY_LVL_MAX;
  dots[(3*minHand)+1] = DISPLAY_LVL_MAX;
  dots[(3*secHand)+2] = DISPLAY_LVL_MAX;
}
