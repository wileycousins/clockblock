// Testing sketch for DS3234 class
// using SFE Arduino Pro Micro 3.3V

#include "StuPId.h"
//#include "DS3234.h"

// pin defines
//rtc
#define RTC_CS  9
#define RTC_INT 0
// led
#define LED_PIN 2
int ledState = 0;

// create our SPI object
StuPId spi(&DDRB, 2, &DDRB, 1);

// create the RTC object
// cs  - PB5
// rst - PB4
// int - PD0
//DS3234 rtc(&PORTB, 5, &PORTB, 4, &PORTD, 0);

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
  
  // chip select
  DDRB |= (1<<5);
  PORTB |= (1<<5);
  
  // initialize SPI
  // DS3234 is MSB first
  spi.setDataOrder(SPI_MSB_FIRST);
  // runs at up to 4MHz. dividing clock by 4 will ensure this isn't exceeded
  spi.setDataRate(SPI_DIV_4, SPI_SPEED_NORMAL);
  // supports SPI modes 1 and 3 (autodetects). let's use 1 (CPOL = 0, CPHA = 1)
  spi.setDataMode(1);
  spi.enable();
  Serial.println("SPI initialized");
  
  //rtc.init();
  //Serial.println("RTC initialized");
  
  // let's try to read the registers
  PORTB &= ~(1<<5);
  unsigned char c = 0x00;
  spi.transfer(c);
  for (int i=0; i<0x14; i++) {
    c = spi.transfer(0);
    Serial.print("reg "); Serial.print(i, HEX); Serial.print(": ");
    Serial.println(c, BIN);
    delay(10);
  }
  PORTB |= (1<<5);
  
  // tell the RTC that it is 4 o'clock
  //unsigned char tm[3] = {0, 0, 16};
  //rtc.setTime(tm);
  
  // setup interrupt
  //pinMode(RTC_INT, INPUT);
  //attachInterrupt(0, toggle, FALLING);
}

void loop() {
  toggle();
  //unsigned char theTime[3] = {0, 0, 0};
  //Serial.print("it is ");
  //Serial.print(theTime[2], DEC);
  //Serial.print(":");
  //Serial.print(theTime[1], DEC);
  //Serial.print(":");
  //Serial.println(theTime[0], DEC);
  delay(2000);
}

void toggle() {
  ledState = !ledState;
  digitalWrite(LED_PIN, ledState);
}