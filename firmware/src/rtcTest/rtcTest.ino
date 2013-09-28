// Testing sketch for DS3234 class
// using SFE Arduino Pro Micro 3.3V

#include "SPI.h"
#include "DS3234.h"

// pin defines
#define RTC_CS  9
#define RTC_RST 8
#define RTC_INT 0

// led
#define LED_PIN 2
int ledState = 0;

// create the RTC object
//DS3234 rtc;

void setup() {
  // setup pins
  pinMode(RTC_CS, OUTPUT);
  digitalWrite(RTC_CS, HIGH);
  pinMode(RTC_RST, OUTPUT);
  digitalWrite(RTC_RST, HIGH);
  
  pinMode(RTC_INT, INPUT);
  attachInterrupt(0, toggle, FALLING);
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, ledState);
  
  // initialize SPI:
  //SPI.begin(); 
}

void loop() {
  //toggle();
  //delay(100);
}

void toggle() {
  // debounce
  delay(50);
  if (digitalRead(3) == LOW) {
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
  }
}
