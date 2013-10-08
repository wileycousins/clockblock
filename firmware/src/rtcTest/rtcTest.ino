// Testing sketch for clockblock
// using SFE Arduino Pro Micro 3.3V

// TLC5940 pin definitions
// communication pins - define in makefile or set appropriately
// greyscale clock

// greyscale clock - D2
#define TLC5940_GS_PORT PORTD
#define TLC5940_GS_PIN 2
// serial clock - D4
#define TLC5940_SCK_PORT PORTD
#define TLC5940_SCK_PIN 4
// latch - D3
#define TLC5940_XLAT_PORT PORTD
#define TLC5940_XLAT_PIN 3
// programming select - D1
#define TLC5940_VPRG_PORT PORTD
#define TLC5940_VPRG_PIN  1
// blank outputs - C6
#define TLC5940_BLANK_PORT PORTC
#define TLC5940_BLANK_PIN 6
// serial data master out slave in - D7
#define TLC5940_MOSI_PORT PORTD
#define TLC5940_MOSI_PIN 7
// number of drivers
#define TLC5940_N 3

#include "StuPId.h"
#include "DS3234.h"
#include "TLC5940.h"

// pin defines
#define RTC_INT_PIN 3
#define RTC_INT 0

// led defines
// TLC5940 can only handle up to 16 leds
#define NUM_LEDS LED_N
// clock stuff
#define NUM_DOTS (3*12)
#define LVL 100

// flag that's set every second by the RTC square wave interrupt
volatile bool secFlag;

// create an SPI object
//   mosi - PB2
//   sck  - PB1  
StuPId spi(&DDRB, 2, &DDRB, 1);

// create an RTC object
//   cs  - PB5
//   rst - PB4
//   int - PD0
DS3234 rtc(&spi, &PORTB, 5, &PORTB, 4, &PORTD, 0);
uint8_t tm[3];

// create an TLC5940 LED driver object
TLC5940 tlc;
uint16_t dots[NUM_DOTS];

void setup() {
  secFlag = false;

  // debugging
  Serial.begin(9600);
  
  // wait for input
  while(!Serial.available());
  Serial.read();
  Serial.println("Beginning program");
  
  // initialize the RTC
  rtc.init();
  Serial.println("RTC enabled");

  // check if it's got a good time
  if(rtc.hasLostTime()) {
    uint8_t tm[3] = {0, 0, 0};
    // if it's lost the time, tell it that it's 21:42:42
    Serial.println("RTC has lost the time");
    Serial.print("Please enter hours: ");
    while(!Serial.available());
    tm[2] = Serial.parseInt();
    Serial.println(tm[2]);
    Serial.print("Please enter minutes: ");
    while(!Serial.available());
    tm[1] = Serial.parseInt();
    Serial.println(tm[1]);
    Serial.print("Setting time to "); Serial.print(tm[2]); Serial.print(":"); Serial.println(tm[1]);
    // { s, m, h }
    rtc.setTime(0, tm);
  }

  // enable a 1 Hz squarewave output on interrupt pin
  rtc.enableSquareWave(0);
  Serial.println("1 Hz square wave enabled");

  // initialize the TLC
  tlc.init();
  Serial.println("LED driver enabled");
  
  // setup interrupt
  pinMode(RTC_INT_PIN, INPUT);
  attachInterrupt(0, intFlag, FALLING);
}

void loop() {  
  // check the second flag
  if (secFlag) {
    // clear the flag and get the time
    secFlag = false;
    rtc.getTime(tm);
    
    // update the clock arms
    updateArms();

    // print the time for debugging
    //Serial.print("Time: ");
    //Serial.print(tm[2]); Serial.print(":");
    //Serial.print(tm[1]); Serial.print(":");
    //Serial.println(tm[0]);
  }
}

// update the clock arms
// dots array structure: { hr0, mn0, sc0, hr1, mn1, sc1, ... , hr11, mn11, sc11 }
void updateArms() {
  //Serial.println("Updating arms");

  // time variables for convenience
  uint8_t hour = tm[2];
  uint8_t min = tm[1];
  uint8_t sec = tm[0];
  Serial.print(hour); Serial.print(":"); Serial.print(min); Serial.print(":"); Serial.println(sec);
  // hands
  uint8_t minHand = min/5;
  uint8_t secHand = sec/5;
  //Serial.print("Minute hand: "); Serial.println(minHand);
  //Serial.print("Second hand: "); Serial.println(secHand);
  // fractions
  uint8_t minFrac = min/12;
  uint8_t secFrac = sec/12;
  //Serial.print("Minute fraction: "); Serial.println(minFrac);
  //Serial.print("Second fraction: "); Serial.println(secFrac);
  // mod
  uint8_t minMod = min%5;
  uint8_t secMod = sec%5;
  //Serial.print("Minute modulus: "); Serial.println(minMod);
  //Serial.print("Second modulus: "); Serial.println(secMod);
  
  // fill the hour dots
  // all hours previous are full
  for (uint8_t i=0; i<hour; i++) {
    dots[i*3] = 5*LVL;
  }
  // current hour to fraction
  dots[hour*3] = LVL*(minFrac+1);
  // all other hours off
  for (uint8_t i=hour+1; i<12; i++) {
    dots[i*3] = 0;
  }

  // do the same with the minute dots
  // all minute dots previous get set to full
  for (uint8_t i=0; i<minHand; i++) {
    dots[(i*3)+1] = 5*LVL;
  }
  // current minute dot to fraction
  dots[(minHand*3)+1] = LVL*(minMod+1);
  // all other minute dots off
  for (uint8_t i=minHand+1; i<12; i++) {
    dots[(i*3)+1] = 0;
  }

  // finally, seconds
  // all second dots previous get set to full
  for (uint8_t i=0; i<secHand; i++) {
    dots[(i*3)+2] = 5*LVL;
  }
  // current second dot to fraction
  dots[(secHand*3)+2] = LVL*(secMod+1);
  // all other second dots off
  for (uint8_t i=secHand+1; i<12; i++) {
    dots[(i*3)+2] = 0;
  }

  // send the data to the drivers
  //Serial.println("gs = { hr0\tmn0\tsc0\thr1\tmn1\tsc1\thr2\tmn2\tsc2\thr3\tmn3\tsc3\thr4\tmn4\tsc4\t}");
  //Serial.print("gs = { ");
  for (uint8_t i=0; i<NUM_DOTS; i++) {
  //  Serial.print(dots[i]); Serial.print("\t");
    tlc.setLed(i, dots[i]);
  }
  tlc.update();
  //Serial.println("}");
  //Serial.println();
}

// ISRs
void intFlag() {
  secFlag = true;
}

// ISR for serial data input into TLC5940
// run in non-blocking mode so that the greyscale cycle continues regardless of serial data being clocked in
ISR(TIMER0_COMPA_vect) {
  tlc.serialCycle();
}

// ISR for greyscale clock on TLC5940
ISR(TIMER1_COMPA_vect) {
  tlc.gsCycle();
}
