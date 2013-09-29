// Testing sketch for DS3234 class
// using SFE Arduino Pro Micro 3.3V

#include "StuPId.h"
#include "DS3234.h"

// pin defines
//rtc
#define RTC_CS  9
#define RTC_INT 0
// led
#define LED_PIN 2
int ledState = 0;

// create our SPI object
StuPId spi(&DDRB, 2, &DDRB, 1);
StuPId *s = &spi;

// create the RTC object
// cs  - PB5
// rst - PB4
// int - PD0
DS3234 rtc(&spi, &PORTB, 5, &PORTB, 4, &PORTD, 0);

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
  rtc.init();
  Serial.println("RTC enabled");
  
  // byte buffer
  uint8_t c[0x14];
  rtc.readReg(DS3234_SEC, 0x14, c);

/*
  rtc.spiStart();
  //PORTB &= ~(1<<5);
  s->transfer(DS3234_SEC);
  //spi.transfer(DS3234_SEC);
  for (uint8_t i=0; i<0x14; i++) {
    //delay(100);
    c[i] = s->transfer(0);
    //c[i] = spi.transfer(0);
  }
  rtc.spiEnd();
  //PORTB |= (1<<5);
*/


  // finally pull the chip select line low to begin the transfer
  //PORTB &= ~(1 << 5);
  // transfer start register
  //spi.transfer(DS3234_SEC);
  for (uint8_t i=0; i<0x14; i++) {
    //uint8_t c;
    
    // DS3234 is MSB first
    //s->setDataOrder(SPI_MSB_FIRST);
    // runs at up to 4MHz. dividing clock by 4 will ensure this isn't exceeded
    //s->setDataRate(SPI_DIV_4, SPI_SPEED_NORMAL);
    // supports SPI modes 1 and 3 (autodetects). let's use 1 (CPOL = 0, CPHA = 1)
    //s->setDataMode(1);
    
    // get data
    //c = spi.transfer(0);
    Serial.print("reg "); Serial.print(i, HEX); Serial.print(": ");
    Serial.println(c[i], BIN);
  }
  // pull cs high
  //PORTB |= (1<<5);
  
  // tell the RTC that it is 4 o'clock
  //unsigned char tm[3] = {0, 0, 16};
  //rtc.setTime(tm);
  
  // setup interrupt
  //pinMode(RTC_INT, INPUT);
  //attachInterrupt(0, toggle, FALLING);
}

void loop() {
  uint8_t tm[3];

  // wait for input
  while(!Serial.available());
  Serial.read();

  toggle();
  rtc.readReg(DS3234_SEC, 3, tm);
  /*
  // finally pull the chip select line low to begin the transfer
  PORTB &= ~(1 << 5);
  // transfer start register
  spi.transfer(DS3234_SEC);
  tm[0] = spi.transfer(0);
  tm[1] = spi.transfer(0);
  tm[2] = spi.transfer(0);
  // pull cs high
  PORTB |= (1<<5);
  */
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

void toggle() {
  ledState = !ledState;
  digitalWrite(LED_PIN, ledState);
}
