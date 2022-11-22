#include <Arduino.h>
#include <SPICREATE.h>
#include <S25FL512S.h>
#include <H3LIS331_beta.h>
#include <ICM20948_beta.h>
#include <LPS25HB.h>

#define flashCS 27
#define SCK1 33
#define MISO1 25
#define MOSI1 26
#define SCK2 33
#define MISO2 25
#define MOSI2 26
#define H3LIS331CS 32
#define LPSCS 14
#define ICMCS 13
#define SPIFREQ 100000

H3LIS331 H3lis331;
LPS Lps25;
ICM icm20948;

SPICREATE::SPICreate SPIC1;
SPICREATE::SPICreate SPIC2;
Flash flash1;

void setup()
{
  delay(5000);
  digitalWrite(flashCS, HIGH);
  delay(100);
  digitalWrite(H3LIS331CS, HIGH);
  delay(100);
  digitalWrite(ICMCS, HIGH);
  delay(100);
  digitalWrite(LPSCS, HIGH);
  delay(100);
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(2000);
  Serial.println("start");
  delay(100);
  SPIC1.begin(VSPI, SCK1, MISO1, MOSI1);
  delay(100);
  // SPIC2.begin(HSPI, SCK2, MISO2, MOSI2);
  delay(100);
  Serial.println("SPI1");
  flash1.begin(&SPIC1, flashCS, SPIFREQ);
  delay(100);
  Serial.println("flash1");
  H3lis331.begin(&SPIC1, H3LIS331CS, SPIFREQ);
  delay(100);
  Serial.println("H3lis331");
  icm20948.begin(&SPIC1, ICMCS, SPIFREQ);
  delay(100);
  Serial.println("icm20948");
  // Lps25.begin(&SPIC2, LPSCS, SPIFREQ);
  // delay(100);
  // Serial.println("Lps25hb");

  Serial.println("Timer Start!");

  // WhoAmI
  uint8_t a;
  a = H3lis331.WhoAmI();
  Serial.print("WhoAmI:");
  Serial.println(a);
  if (a == 0x32)
  {
    Serial.println("H3LIS331 is OK");
  }
  else
  {
    Serial.println("H3LIS331 is NG");
    // while (1)
    // {
    //   delay(1000);
    // }
  }

  // a = Lps25.WhoAmI();
  // Serial.print("WhoAmI:");
  // Serial.println(a);
  // if (a == 0b10111101)
  // {
  //   Serial.println("LPS25HB is OK");
  //   // WhoAmI_Ok_LPS25HB = true;
  // }
  // else
  // {
  //   Serial.println("LPS25HB is NG");
  // }

  Serial.println();

  a = icm20948.WhoAmI();
  Serial.print("WhoAmI:");
  Serial.println(a);
  if (a == 0xEA)
  {
    Serial.println("ICM20948 is OK");
    // WhoAmI_Ok_ICM20948 = true;
  }
  else
  {
    Serial.println("ICM20948 is NG");
  }
}

void loop()
{
  int16_t H3lisReceiveData[3];
  uint8_t H3lis_rx_buf[6];

  int16_t Icm20948ReceiveData[6];
  uint8_t Icm20948_rx_buf[12];
  H3lis331.Get(H3lisReceiveData, H3lis_rx_buf);
  icm20948.Get(Icm20948ReceiveData, Icm20948_rx_buf);
  Serial.print("H3lis331:");
  for (int i = 0; i < 6; i++)
  {
    Serial.print(H3lisReceiveData[i]);
    Serial.print(",");
  }
  Serial.print("\n");
  Serial.print("icm20948:");
  for (int i = 0; i < 14; i++)
  {
    Serial.print(Icm20948ReceiveData[i]);
    Serial.print(",");
  }
  Serial.print("\n");
  delay(10000);
}