#include <Arduino.h>

#include <SPICREATE.h>
#include <H3LIS331.h>

#define H3LIS331SCK 14
#define H3LIS331MISO 12
#define H3LIS331MOSI 13
#define H3LIS331CS 33

int count = 0;

// #define LIS331HH_SCK 13
// #define LIS331HH_MISO 12
// #define LIS331HH_MOSI 11
// #define LIS331HH_CS 10

#define SPIFREQ 120000

H3LIS331 H3lis331;

SPICREATE::SPICreate SPIC;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("launched");

  SPIC.begin(HSPI, H3LIS331SCK, H3LIS331MISO, H3LIS331MOSI);
  //センサのbeginをかく。
  // void begin(SPICREATE::SPICreate *targetSPI, int cs, uint32_t freq = 8000000);
  H3lis331.begin(&SPIC, H3LIS331CS, SPIFREQ);
}

void loop()
{
  // put your main code here, to run repeatedly:
  // WhoamIをかく
  uint8_t a;
  a = H3lis331.WhoImI();
  Serial.print("WhoAmI:");
  Serial.println(a);
  if (a == 0x32)
  {
    Serial.println("H3LIS331 is OK");
    count++;
    Serial.println(count);
  }
  else
  {
    Serial.println("H3LIS331 is NG");
    Serial.println(count);
  }
  delay(100);
}