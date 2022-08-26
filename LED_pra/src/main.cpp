#include <Arduino.h>

#include <SPICREATE.h>
#include <H3LIS331.h>

//ココナニ
#define H3LIS331CS 26
#define SPIFREQ 12000000

H3LIS331 H3lis331;

SPICREATE::SPICreate SPI;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("launched");

  SPI.begin();
  //センサのbeginをかく。
  // void begin(SPICREATE::SPICreate *targetSPI, int cs, uint32_t freq = 8000000);
  H3lis331.begin(&SPI, H3LIS331CS, SPIFREQ);
  // WhoamIをかく
  int a;
  a = H3lis331.WhoImI();
  Serial.print("WhoAmI:");
  Serial.println(a);
  if (a == 0x32)
  {
    Serial.println("H3LIS331 is OK");
  }
  else
  {
    Serial.println("H3LIS331 is NG");
  }
}

void loop()
{
  // put your main code here, to run repeatedly:
}