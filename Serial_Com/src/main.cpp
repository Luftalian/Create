#include <Arduino.h>
#include <LogBoard67.h>
#include <Log67Serial.h>

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
#define COMMANDRETURN 'j'

SPICREATE::SPICreate SPIC1;
SPICREATE::SPICreate SPIC2;

LogBoard67 logboard67;

#define SPIFREQ 5000000

#define loggingPeriod2 1

TimerHandle_t thand_test;
xTaskHandle xlogHandle;

TaskHandle_t taskHandle;

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

Log67Serial Log67Serial1;

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

  Log67Serial1.setup();

  // logging関数を起動
  xTaskCreateUniversal(logging, "logging", 8192, NULL, 1, &xlogHandle, PRO_CPU_NUM);

  SPIFlashLatestAddress = flash1.setFlashAddress();
  Serial.printf("SPIFlashLatestAddress: %d\n", SPIFlashLatestAddress);

  Log67Serial1.setCommand(COMMANDFINISHSETUP);
}

void loop()
{
  Log67Serial1.sendSerial2();
  while (Serial2.available())
  {
    char pre = Serial2.read();
    switch (pre)
    {
    case COMMANDRETURN: // 'j'
      Serial.println("return text");
      Log67Serial1.stopCommand();
      break;
    case COMMANDPREPARATION: // 'p'
      Log67Serial1.setCommand(COMMANDPREPARATION);

      Serial.println("Preparation mode");
      while (1)
      {
        Log67Serial1.sendSerial2();
        receive = Serial2.read();
        switch (receive)
        {
        case COMMANDLOG: // 'l'
          Log67Serial1.setCommand(COMMANDLOG);

          Serial.println("Logging mode");
          while (1)
          {
            if (checker > 0)
            {
              checker = 0;
              logboard67.RoutineWork();
            }
            Log67Serial1.sendSerial2();
            char pre2 = Serial2.read();
            if (pre2 == COMMANDRETURN) // 'j'
            {
              Serial.println("return text");
              Log67Serial1.stopCommand();
            }
            if (pre2 == COMMANDSTOP) // 's'
            {
              Log67Serial1.setCommand(COMMANDSTOP);

              Serial.println("Stop logging");
              exitLoop = true;
              break;
            }
          }
          Serial.write("Done Recorded");
          break;
        case COMMANDDELETE: // 'd'
          Serial.println("Delete mode");
          Serial2.write(COMMANDDELETE);

          xTaskCreatePinnedToCore(Log67Serial1.sendTask, "sendTask1", 8192, NULL, 2, &taskHandle, 0);

          flash1.erase();

          vTaskDelete(taskHandle);

          Log67Serial1.setCommand(COMMANDDELETEDONE);

          SPIFlashLatestAddress = 0x000;
          exitLoop = true;
          break;
        case COMMANDPREPARATION: // 'p'
          break;
        case COMMANDRETURN: // 'j'
          Serial.println("return text");
          Log67Serial1.stopCommand();
          break;
        default:
          if ('a' <= receive && receive <= 'z')
          {
            Log67Serial1.setCommand(receive);

            Serial.println("Exit Preparation mode");
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
      break;
    default:
      break;
    }
  }
}