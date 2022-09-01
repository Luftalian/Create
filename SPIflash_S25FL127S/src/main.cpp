#include <Arduino.h>
#include <SPICREATE.h>
#include <SPIflash.h>

#define flashCS 27
#define SCK1 33
#define MISO1 25
#define MOSI1 26

SPICREATE::SPICreate SPIC1;
Flash flash1;

// SPIflash S25FL127S 用サンプルコード

void setup()
{
  Serial.begin(115200);
  Serial.println("ssttaarrtt");
  SPIC1.begin(VSPI, SCK1, MISO1, MOSI1);
  flash1.begin(&SPIC1, flashCS, 60000);
  // put your setup code here, to run once:
}

void loop()
{
  if (Serial.available())
  {
    char cmd = Serial.read();
    if (cmd == 'e')
    {
      flash1.erase();
      delay(1000);
    }

    if (cmd == 't')
    {
      uint8_t rx[256];
      flash1.read(0, rx);
      delay(1000);
      for (int i = 0; i < 256; i++)
      {
        Serial.print(rx[i]);
      }
      Serial.println();

      Serial.println("asd1");
      delay(1000);

      uint8_t tx[256];
      for (int i = 0; i < 256; i++)
      {
        tx[i] = 0xFF - i;
      }
      Serial.println("asd2");
      flash1.write(0, tx);
      Serial.println("asd3");

      delay(1000);

      flash1.read(0, rx);
      delay(1000);
      Serial.println("asd4");
      for (int i = 0; i < 256; i++)
      {
        Serial.print(rx[i]);
      }
      Serial.println();
    }
  }
  delay(100);
}