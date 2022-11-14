#include <Arduino.h>
#include <ICM20948.h>
// #include <ICM20948_beta.h>
#include <SPICREATE.h>

#define ICMSCK 33  // 18
#define ICMMISO 25 // 19
#define ICMMOSI 26 // 23
#define ICMCS 13   // 27//33
#define SPIFREQ 1200000

ICM icm20948;

SPICREATE::SPICreate SPIC;

int16_t ICM_data[6];
// uint8_t rx_buf[12];

void setup()
{
  delay(100);
  Serial.begin(115200);
  delay(100);
  SPIC.begin(VSPI, ICMSCK, ICMMISO, ICMMOSI);
  delay(100);
  icm20948.begin(&SPIC, ICMCS, SPIFREQ);
  delay(100);
  uint8_t a;
  a = icm20948.WhoAmI();
  Serial.println(a);
  // while (a != 0xEA)
  // {
  Serial.print("error: ");
  a = icm20948.WhoAmI();
  Serial.println(a);
  delay(1000);
  // }
}

void loop()
{
  icm20948.Get(ICM_data); // , rx_buf);
  Serial.print(ICM_data[0]);
  Serial.print(",");
  Serial.print(ICM_data[1]);
  Serial.print(",");
  Serial.print(ICM_data[2]);
  Serial.print(",");
  Serial.print(ICM_data[3]);
  Serial.print(",");
  Serial.print(ICM_data[4]);
  Serial.print(",");
  Serial.print(ICM_data[5]);
  Serial.print("\n");
  // for (int i = 0; i < 12; i++)
  // {
  //   Serial.print(rx_buf[i]);
  // }
  // Serial.print("\n");
}