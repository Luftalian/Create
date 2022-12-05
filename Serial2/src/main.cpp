#include <Arduino.h>

#define LED1 19

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial2.begin(9600);
}

void loop()
{
  // put your main code here, to run repeatedly:
  while (Serial2.available())
  {
    char receive;
    receive = Serial2.read();
    Serial.write(receive);
    if (receive == 'e')
    {
      Serial2.println("OK");
      digitalWrite(LED1, HIGH);
      delay(100);
      digitalWrite(LED1, LOW);
      delay(100);
    }
  }
}