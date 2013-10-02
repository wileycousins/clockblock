// Testing sketch for DS3234 class
// using SFE Arduino Pro Micro 3.3V

#include "StuPId.h"
#include "DS3234.h"
#include "TLC5940.h"

// pin defines
//rtc
#define RTC_CS  9
#define RTC_INT 0
// led
#define LED_PIN 2
int ledState = 1;

// create our SPI object
//StuPId spi(&DDRB, 2, &DDRB, 1);
//StuPId *s = &spi;

// create the RTC object
// cs  - PB5
// rst - PB4
// int - PD0
//DS3234 rtc(&spi, &PORTB, 5, &PORTB, 4, &PORTD, 0);

// create an TLC5940 LED driver object
TLC5940 tlc;

void setup() {
  // LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, ledState);
  
  // debugging
  Serial.begin(9600);
  
  // wait for input
  while(!Serial.available());
  Serial.read();
  Serial.println("Beginning program");
  
  // initialize the RTC
//  rtc.init();
//  Serial.println("RTC enabled");

  // initialize the TLC
  tlc.init();
  // set the DC to to full to start off
  for (uint8_t i=0; i<15; i++) {
    tlc.setDC(i, 63);
  }
  // set GS to full to start off, too
  for (uint8_t i=0; i<15; i++) {
    tlc.setGS(i, 65536);
  }

  // check if it's got a good time
  // if(rtc.hasLostTime()) {
  //   // if it's lost the time, tell it that it's 21:42:42
  //   Serial.println("RTC has lost the time");
  //   Serial.println("Setting time to 21:42:42");
  //   // { s, m, h }
  //   uint8_t tm[3] = {42, 42, 21};
  //   rtc.setTime(tm);
  // }


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

  // tell the RTC that it is 4 o'clock
  //unsigned char tm[3] = {0, 0, 16};
  //rtc.setTime(tm);
  
  // setup interrupt
  //pinMode(RTC_INT, INPUT);
  //attachInterrupt(0, toggle, FALLING);
}

void loop() {
  uint8_t tm[3];

  tlc.refreshGS();

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

void toggle() {
  ledState = !ledState;
  digitalWrite(LED_PIN, ledState);
}
