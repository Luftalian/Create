#include <Arduino.h>
#include <SPICREATE.h>

#define loggingPeriod 2

TimerHandle_t thand_test;
xTaskHandle xlogHandle;

IRAM_ATTR void logging(void *parameters)
{
  static int count = 100;
  portTickType xLastWakeTime = xTaskGetTickCount();
  for (;;)
  {
    char bfChar[128] = "very tekitou na data\n";

    // if (SDIO.appendQueue(bfChar) == 1)
    // {
    //   vTaskDelete(&xlogHandle);
    //   Serial.println("queue filled!");
    //   ESP.restart();
    // }
    count++;
    if (count % 500 == 0) // per 1s
    {
      Serial.println("zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz");
    }
    vTaskDelayUntil(&xLastWakeTime, loggingPeriod / portTICK_PERIOD_MS);
  }
}

void setup()
{
  Serial.begin(115200);

  xTaskCreateUniversal(logging, "logging", 8192, NULL, 1, &xlogHandle, PRO_CPU_NUM);
}

void loop()
{
}