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
// cs  - PB5
// rst - PB4
// int - PD0
DS3234 rtc(PORTB, 5, PORTB, 4, PORTD, 0);

void setup() {
  // LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, ledState);
  
  // debugging
  Serial.begin(9600);
  
  // initialize RTC
  rtc.init();
  SPI.begin();
  
  // tell the RTC that it is 4 o'clock
  unsigned char tm[3] = {0, 0, 16};
  rtc.setTime(tm);
  
  // setup interrupt
  //pinMode(RTC_INT, INPUT);
  //attachInterrupt(0, toggle, FALLING);
}

void loop() {
  toggle();
  unsigned char theTime[3];
  rtc.getTime(theTime);
  Serial.print("it is ");
  Serial.print(theTime[2], DEC);
  Serial.print(":");
  Serial.print(theTime[1], DEC);
  Serial.print(":");
  Serial.println(theTime[0], DEC);
  delay(2000);
}

void toggle() {
  // debounce
  delay(50);
  if (digitalRead(3) == LOW) {
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
  }
}
