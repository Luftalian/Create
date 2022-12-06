#include <Arduino.h>

#define LED1 19

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);
  Serial2.begin(9600);
  delay(100);
  Serial.println("start Serial");
}

void loop()
{
  // put your main code here, to run repeatedly:
  // Serial2.write('e');
  // while (Serial2.available())
  // {
  char receive;
  receive = Serial2.read();
  Serial2.write(receive);
  Serial.println(receive);
  delay(1000);
  Serial.println("a");
  if (receive == 'e')
  {
    Serial2.println("OK");
    Serial.println("OK");
    digitalWrite(LED1, HIGH);
    delay(100);
    digitalWrite(LED1, LOW);
    delay(100);
  }
  // }
}