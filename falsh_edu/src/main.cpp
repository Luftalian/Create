#include <Arduino.h>
#include <S25FL512S.h>

#define SPIFREQ 5000000

uint8_t tx[256] = {};
uint8_t rx[256] = {};

#define flashCS 27
#define SCK1 33
#define MISO1 25
#define MOSI1 26

SPICREATE::SPICreate SPIC1;
Flash flash1;

bool flag = false;

void setup()
{
  // FlashのCSピンを非選択に設定
  digitalWrite(flashCS, HIGH);
  // SPIの初期化
  Serial.begin(115200);
  while (!Serial)
    ;
  Serial.println("Start");
  // SPIの初期化
  SPIC1.begin(VSPI, SCK1, MISO1, MOSI1);
  Serial.println("Start2");
  // Flashの初期化
  flash1.begin(&SPIC1, flashCS, SPIFREQ);
  Serial.println("Start3");
  // シリアルの初期化
  Serial.println("Start4");
}

void loop()
{
  if (flag == false)
  {
    // Flashの初期化
    flash1.erase();
    delay(10);
    // 書き込むデータを作る
    for (int i = 0; i < 256; i++)
    {
      tx[i] = i;
    }
    // 書き込むデータをシリアルで表示
    Serial.print("tx: \n");
    for (int i = 0; i < 256; i++)
    {
      Serial.print(tx[i]);
      Serial.print(" ");
    }
    Serial.print("\n");
    // 書き込み
    flash1.write(0, tx);
    delay(100);
    // 読み込み
    flash1.read(0, rx);
    // 読み込んだデータをシリアルで表示
    Serial.print("rx: \n");
    for (int i = 0; i < 256; i++)
    {
      Serial.print(rx[i]);
      Serial.print(" ");
    }
    Serial.print("\n");
    flag = true;
    Serial.print("End");
    Serial.print("\n");
  }
}