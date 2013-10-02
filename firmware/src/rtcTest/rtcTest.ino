// Testing sketch for DS3234 class
// using SFE Arduino Pro Micro 3.3V

#include "StuPId.h"
#include "DS3234.h"
#include "TLC5940.h"

// pin defines
//rtc
//#define RTC_CS  9
//#define RTC_INT 0
// led
//#define LED_PIN 2
//int ledState = 1;

// create our SPI object
//   
StuPId spi(&DDRB, 2, &DDRB, 1);
StuPId *s = &spi;

// create the RTC object
//   cs  - PB5
//   rst - PB4
//   int - PD0
DS3234 rtc(&spi, &PORTB, 5, &PORTB, 4, &PORTD, 0);

// create an TLC5940 LED driver object
TLC5940 tlc;
uint16_t brite;
int8_t dir;
uint16_t count;

void setup() {
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
    rtc.setTime(tm);
  }

  // set the DC to to half to start off
  for (uint8_t i=0; i<16; i++) {
    tlc.setDC(i, 32);
  }
  // set GS to off
  for (uint8_t i=0; i<16; i++) {
    tlc.setGS(i, 0);
  }
  // initialize the TLC
  tlc.init();

/*
  // read all the registers for testing
  // byte buffer
  uint8_t c[0x14];
  
  rtc.readReg(DS3234_SEC, 0x14, c);

  for (uint8_t i=0; i<0x14; i++) {
    Serial.print("reg 0x"); Serial.print(i, HEX); Serial.print(": ");
    Serial.println(c[i], BIN);
  }
*/
  
  // setup interrupt
  //pinMode(RTC_INT, INPUT);
  //attachInterrupt(0, toggle, FALLING);
}

void loop() {
  uint8_t tm[3];
  count++;


  brite += 500 * dir;
  if ( ((dir == 1) && (brite >= 15000)) || ((dir == -1) && (brite <= 500)) ) {
    dir *= -1;
  }
  for (uint8_t i=0; i<16; i++) {
    tlc.setGS(i, brite);
  }
  
  //Serial.print("Refresh at: "); Serial.println(brite);
  tlc.refreshGS();
  //delay(1);
  if (++count >= 1000) {
    count = 0;
    rtc.getTime(tm);
    Serial.print("Time: ");
    Serial.print(tm[2]); Serial.print(":");
    Serial.print(tm[1]); Serial.print(":");
    Serial.println(tm[0]);
  }

/*
  // wait for input
  if (Serial.available()) {
    Serial.read();

    toggle();
    rtc.readReg(DS3234_SEC, 3, tm);

    // decode the time
    tm[2] = ((tm[2] >> 4) * 10) + (tm[2] & 0x0F);
    // minutes
    tm[1] = ((tm[1] >> 4) * 10) + (tm[1] & 0x0F);
    // seconds
    tm[0] = ((tm[0] >> 4) * 10) + (tm[0] & 0x0F);

    Serial.print("it is ");
    Serial.print(tm[2], DEC);
    Serial.print(":");
    Serial.print(tm[1], DEC);
    Serial.print(":");
    Serial.println(tm[0], DEC);
  }
  */
}
/*
void toggle() {
  ledState = !ledState;
  digitalWrite(LED_PIN, ledState);
}
*/