#include <Arduino.h>
#include <SPICREATE.h> // 変更済みのものを使用
#include <S25FL512S.h>
#include <H3LIS331.h>
#include <ICM20948_beta.h>
#include <LPS25HB.h>
#include <LogBoard67.h>

// ピンの定義
#define flashCS 27
#define SCK1 33
#define MISO1 25
#define MOSI1 26
#define SCK2 19
#define MISO2 22
#define MOSI2 23
#define H3LIS331CS 32
#define LPSCS 14
#define ICMCS 13
#define LEDPIN 18

// コマンドの定義
#define COMMANDPREPARATION 'p'
#define COMMANDDELETE 'd'
#define COMMANDSTOP 's'
#define COMMANDLOG 'l'
#define COMMANDFINISHSETUP 'r'
#define COMMANDDELETEDONE 'f'

SPICREATE::SPICreate SPIC1;
SPICREATE::SPICreate SPIC2;

LogBoard67 logboard67;

#define SPIFREQ 5000000

// #define loggingPeriod 2
#define loggingPeriod2 1

TimerHandle_t thand_test;
xTaskHandle xlogHandle;

// チェッカー(logging関数でこれを動かす)
uint8_t checker = 0;

// Serial2で受け取るchar型の変数
char receive;

// Serial2で使う
bool exitLoop = false;

IRAM_ATTR void logging(void *parameters)
{
  portTickType xLastWakeTime = xTaskGetTickCount();
  for (;;)
  {
    checker++;
    vTaskDelayUntil(&xLastWakeTime, loggingPeriod2 / portTICK_PERIOD_MS); // 1ms = 1000Hz
  }
}

void setup()
{
  delay(1000);
  digitalWrite(flashCS, HIGH);
  digitalWrite(H3LIS331CS, HIGH);
  digitalWrite(ICMCS, HIGH);
  digitalWrite(LPSCS, HIGH);
  Serial.begin(115200);
  delay(1);
  Serial.println("start Serial");
  Serial2.begin(9600);
  while (!Serial2)
    ;
  delay(1);
  Serial.println("start Serial2");
  SPIC1.begin(VSPI, SCK1, MISO1, MOSI1);
  SPIC2.begin(HSPI, SCK2, MISO2, MOSI2);
  Serial.println("SPI1");
  flash1.begin(&SPIC1, flashCS, SPIFREQ);
  Serial.println("flash1");
  H3lis331.begin(&SPIC1, H3LIS331CS, SPIFREQ);
  Serial.println("H3lis331");
  icm20948.begin(&SPIC2, ICMCS, SPIFREQ);
  Serial.println("icm20948");
  Lps25.begin(&SPIC2, LPSCS, SPIFREQ);
  Serial.println("Lps25hb");

  micros();
  Serial.println("Timer Start!");
  Serial.write("Started At: ");
  Serial.write(timer.start_time);

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
  }

  a = Lps25.WhoAmI();
  Serial.print("WhoAmI:");
  Serial.println(a);
  if (a == 0b10111101)
  {
    Serial.println("LPS25HB is OK");
  }
  else
  {
    Serial.println("LPS25HB is NG");
  }

  a = icm20948.WhoAmI();
  Serial.print("WhoAmI:");
  Serial.println(a);
  if (a == 0xEA)
  {
    Serial.println("ICM20948 is OK");
  }
  else
  {
    Serial.println("ICM20948 is NG");
  }

  // logging関数を起動
  xTaskCreateUniversal(logging, "logging", 8192, NULL, 1, &xlogHandle, PRO_CPU_NUM);

  SPIFlashLatestAddress = flash1.setFlashAddress();
  Serial.printf("SPIFlashLatestAddress: %d\n", SPIFlashLatestAddress);
  Serial2.write(COMMANDFINISHSETUP); // 'r'
}

void loop()
{
  while (Serial2.available())
  {
    if (Serial2.read() == COMMANDPREPARATION) // 'p'
    {
      Serial2.write(COMMANDPREPARATION);  // 'p'
      Serial.println("Preparation mode"); // 1
      while (1)
      {
        receive = Serial2.read();
        switch (receive)
        {
        case COMMANDLOG:                  // 'l'
          Serial2.write(COMMANDLOG);      // 'l'
          Serial.println("Logging mode"); // 1
          while (1)
          {
            if (checker > 0)
            {
              checker = 0;
              logboard67.RoutineWork();
            }
            if (Serial2.read() == COMMANDSTOP) // 's'
            {
              Serial2.write(COMMANDSTOP); // 's'
              Serial.println("Stop logging");
              exitLoop = true;
              break;
            }
          }
          Serial.write("Done Recorded:");
          Serial.write(timer.Gettime_record());
          break;
        case COMMANDDELETE:             // 'd'
          Serial2.write(COMMANDDELETE); // 'd'
          Serial.println("Delete mode");
          flash1.erase();
          Serial2.write(COMMANDDELETEDONE); // 'f'
          SPIFlashLatestAddress = 0x000;
          exitLoop = true;
          break;
        case COMMANDPREPARATION: // 'p'
          break;
        default:
          if ('a' <= receive && receive <= 'z')
          {
            Serial2.println(receive);                // 1
            Serial.println("Exit Preparation mode"); // 1
            exitLoop = true;
          }
          break;
        }
        if (exitLoop)
        {
          exitLoop = false;
          break;
        }
      }
    }
  }
}