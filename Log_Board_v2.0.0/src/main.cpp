#include <Arduino.h>
#include <SPICREATE.h> // 変更済みのものを使用
#include <S25FL512S_beta.h>
#include <H3LIS331_beta.h>
#include <ICM20948_beta.h>
#include <LPS25HB.h>

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

SPICREATE::SPICreate SPIC1;
SPICREATE::SPICreate SPIC2;

// センサのクラス
H3LIS331 H3lis331;
ICM icm20948;
LPS Lps25;
Flash flash1;

// 気圧の回数の測定(5回に1回)
uint8_t count_lps = 0;

// CountSPIFlashDataSetExistInBuffは列
int CountSPIFlashDataSetExistInBuff = 0;

// SPI_FlashBuffは送る配列
uint8_t SPI_FlashBuff[256] = {};

// SPIFlashLatestAddressは書き込むアドレス。初期値は0x000
// 0x000はreboot対策のどこまでSPI Flashに書き込んだかを記録するページ
// setup()で初期値でも0x100にしている
uint32_t SPIFlashLatestAddress = 0x000;

// SPI Flashの最大のアドレス (1回で1/2ページ書き込んでいる点に注意)
// (512 * 1024 * 1024 / 8 / 256 ページ * 256) * 2 = 524288 * 256
uint32_t SPI_FLASH_MAX_ADDRESS = 0x8000000;

#define SPIFREQ 5000000

// #define loggingPeriod 2
#define loggingPeriod2 1

TimerHandle_t thand_test;
xTaskHandle xlogHandle;

// 時間
unsigned long Record_time;

// チェッカー(logging関数でこれを動かす)
uint8_t checker = 0;

// Serial2で受け取るchar型の変数
char receive;

// Serial2で使う
bool exitLoop = false;

class Timer
{
public:
  unsigned long Gettime_record()
  {
    time = micros();
    time -= start_time;
    return time;
  }
  unsigned long start_time;
  unsigned long time;
  bool start_flag = true;
};

// Timerクラスのインスタンス化
Timer timer;

uint32_t FlashAddress = 0x00;
uint8_t flashRead[256];
uint8_t flashRead1[256];
uint8_t flashRead2[256];
uint8_t count = 1;

// uint32_t checkAddress(uint32_t FlashAddress)
// {
//   count++;
//   // if (count >= 20) // これ以上行くとページの途中を読むことになる。
//   // {
//   //   Serial.printf("FlashAddress: %u\n", FlashAddress);
//   //   return FlashAddress;
//   // }
//   if ((FlashAddress + 0x100) >= SPI_FLASH_MAX_ADDRESS)
//   {
//     Serial.printf("FlashAddress: %u\n", FlashAddress);
//     Serial.printf("count: %u\n", count);
//     return SPI_FLASH_MAX_ADDRESS;
//   }
//   flash1.read(FlashAddress, flashRead1);
//   delay(1);
//   flash1.read(FlashAddress + 0x100, flashRead2);
//   delay(1);
//   if (flashRead1[0] != 0xFF)
//   {
//     if (flashRead2[0] != 0xFF)
//     {
//       // ++
//       Serial.println("---");
//       uint32_t a = FlashAddress + SPI_FLASH_MAX_ADDRESS / pow(2, count);
//       Serial.printf("FlashAddress: %u\n", a);
//       Serial.printf("count: %u\n", count);
//       return checkAddress(FlashAddress + SPI_FLASH_MAX_ADDRESS / pow(2, count));
//     }
//     else
//     {
//       Serial.printf("FlashAddress: %u\n", FlashAddress);
//       return FlashAddress;
//     }
//   }
//   else
//   {
//     if (flashRead2[0] != 0xFF)
//     {
//       Serial.printf("FlashAddress: %u\n", FlashAddress);
//       Serial.println("error");
//       return 0;
//     }
//     else
//     {
//       // --
//       Serial.printf("FlashAddress: %u\n", FlashAddress);
//       return checkAddress(FlashAddress - SPI_FLASH_MAX_ADDRESS / pow(2, count));
//     }
//   }
// }

void RoutineWork()
{
  if (SPIFlashLatestAddress >= SPI_FLASH_MAX_ADDRESS)
  {
    Serial.printf("SPIFlashLatestAddress: %u\n", SPIFlashLatestAddress);
    // Serial2.write("SPI Flash is full");
    // Serial2.write("Started At: ");
    // Serial2.write(timer.start_time);
    // Serial2.write("Now: ");
    // Serial2.write(timer.Gettime_record());
    return;
  }
  // Serial.println("Running");
  if (timer.start_flag)
  {
    timer.start_time = micros();
    timer.start_flag = false;
  }
  Record_time = timer.Gettime_record();
  // From SPI, Get data is tx
  int16_t H3lisReceiveData[3] = {};
  uint8_t H3lis_rx_buf[6] = {};
  int16_t Icm20948ReceiveData[6] = {};
  uint8_t Icm20948_rx_buf[12] = {};
  uint8_t lps_rx[3] = {};
  // CountSPIFlashDataSetExistInBuffは列。indexは行。
  // 時間をとる
  for (int index = 0; index < 4; index++)
  {
    SPI_FlashBuff[32 * CountSPIFlashDataSetExistInBuff + index] = 0xFF & (Record_time >> (8 * index));
  }

  // 加速度をとる
  H3lis331.Get(H3lisReceiveData, H3lis_rx_buf);
  icm20948.Get(Icm20948ReceiveData, Icm20948_rx_buf);
  for (int index = 4; index < 10; index++)
  {
    SPI_FlashBuff[32 * CountSPIFlashDataSetExistInBuff + index] = H3lis_rx_buf[index - 4];
  }

  // ICM20948の加速度をとる
  for (int index = 10; index < 16; index++)
  {
    SPI_FlashBuff[32 * CountSPIFlashDataSetExistInBuff + index] = Icm20948_rx_buf[index - 10];
  }

  // ICM20948の角速度をとる
  for (int index = 16; index < 22; index++)
  {
    SPI_FlashBuff[32 * CountSPIFlashDataSetExistInBuff + index] = Icm20948_rx_buf[index - 10];
  }

  // ICM20948の地磁気をとる
  // for (int index = 22; index < 28; index++)
  // {
  //   SPI_FlashBuff[32 * CountSPIFlashDataSetExistInBuff + index] = Icm20948_rx_buf[index - 10];
  // }

  // LPSの気圧をとる
  if (count_lps % 20 == 0)
  {
    Lps25.Get(lps_rx);
    for (int index = 28; index < 31; index++)
    {
      SPI_FlashBuff[32 * CountSPIFlashDataSetExistInBuff + index] = lps_rx[index - 28];
      count_lps = 0;
    }
  }

  count_lps++;
  CountSPIFlashDataSetExistInBuff++;

  // 8個のデータが溜まったらSPIFlashに書き込む
  if (CountSPIFlashDataSetExistInBuff >= 8)
  {
    // データの書き込み
    flash1.write(SPIFlashLatestAddress, SPI_FlashBuff);
    // アドレスの更新
    SPIFlashLatestAddress += 0x100;
    // 列の番号の初期化
    CountSPIFlashDataSetExistInBuff = 0;
  }
}

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
  // Serial2.write("Started At: ");
  // Serial2.write(timer.start_time);

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

  // SPI Flashがどこまで書き込まれているか確認→二分岐探索作る
  // uint8_t flashRead[256]; // 256でないとflash.read()でRebootした。
  // uint8_t SPIFlashLatestAddress = 0x000;

  // Serial2.write(COMMANDFINISHSETUP); // 'r'
  while (1)
  {
    if (SPIFlashLatestAddress <= 0x1000)
    {
      flash1.read(SPIFlashLatestAddress, flashRead);
      SPIFlashLatestAddress += 0x100;
      Serial.printf("SPIFlashLatestAddress: %u\n", SPIFlashLatestAddress);
      Serial.print(flashRead[0]);
      delay(1);
      Serial.print("\n");
      if (flashRead[0] == 0xFF)
      {
        Serial.println("255");
        break;
      }
      if (SPIFlashLatestAddress >= SPI_FLASH_MAX_ADDRESS)
      {
        Serial.printf("SPIFlashLatestAddress: %u\n", SPIFlashLatestAddress);
        break;
      }
    }
    else
    {
      SPIFlashLatestAddress = checkAddress(SPI_FLASH_MAX_ADDRESS / 2);
      break;
    }
  }
  Serial.println(SPIFlashLatestAddress);
  Serial2.write(COMMANDFINISHSETUP); // 'r'
}

void loop()
{
  while (Serial2.available())
  {
    // err = 301;
    if (Serial2.read() == COMMANDPREPARATION) // 'p'
    {
      Serial2.write(COMMANDPREPARATION); // 'p'
      // Serial2.println("Preparation mode"); // 1
      while (1)
      {
        receive = Serial2.read();
        switch (receive)
        {
        case COMMANDLOG:             // 'l'
          Serial2.write(COMMANDLOG); // 'l'
          // Serial2.println("Logging mode"); // 1
          while (1)
          {
            if (checker > 0)
            {
              checker = 0;
              RoutineWork();
            }
            if (Serial2.read() == COMMANDSTOP) // 's'
            {
              Serial2.write(COMMANDSTOP); // 's'
              // Serial.println("Stop logging");
              exitLoop = true;
              break;
            }
          }
          // Serial2.write("Done Recorded:");
          // Serial2.write(timer.Gettime_record());
          break;
        case COMMANDDELETE:             // 'd'
          Serial2.write(COMMANDDELETE); // 'd'
          Serial.println("Delete mode");
          flash1.erase();
          SPIFlashLatestAddress = 0x100;
          exitLoop = true;
          break;
        default:
          if ('a' < receive && receive < 'z')
          {
            Serial2.println(receive); // 1
            // Serial2.println("Exit Preparation mode"); // 1
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