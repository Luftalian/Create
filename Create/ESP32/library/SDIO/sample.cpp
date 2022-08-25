#include <Arduino.h>
#include <SPICREATE.h>
#include <SDIOLogWrapper.h>

#define loggingPeriod 1

SDIOLogWrapper SDIO;

xTaskHandle xlogHandle;

IRAM_ATTR void logging(void *parameters)
{
  portTickType xLastWakeTime = xTaskGetTickCount();
  for (;;)
  {
    char bfChar[128] = "very tekitou na data\n";

    if (SDIO.appendQueue(bfChar) == 1)
    {
      vTaskDelete(&xlogHandle);
      Serial.println("queue filled!");
      ESP.restart();
    }

    vTaskDelayUntil(&xLastWakeTime, loggingPeriod / portTICK_PERIOD_MS);
  }
}

void setup()
{
  Serial.begin(115200);

  SDIO.makeQueue(64, 128);
}

void loop()
{
  // put your main code here, to run repeatedly:
  while (Serial.available())
  {
    char cmd = Serial.read();

    if (cmd == 'r')
    {
      ESP.restart();
    }

    if (cmd == 'a')
    {
      SDIO.initSD();
      SDIO.openFile();
      SDIO.writeTaskCreate(APP_CPU_NUM);
      xTaskCreateUniversal(logging, "logging", 8192, NULL, 1, &xlogHandle, PRO_CPU_NUM);
    }

    if (cmd == 'g')
    {
      vTaskDelete(xlogHandle);
      delay(1000);
      SDIO.writeTaskDelete();
      SDIO.closeFile();
      SDIO.deinitSD();
    }
  }
  delay(100);
}