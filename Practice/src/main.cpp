#include <Arduino.h>

void setup()
{
  // put your setup code here, to run once:
  pinMode(5, OUTPUT);
  Serial.begin(115200);
  Serial.println("Hello World!");
}

void loop()
{
  // put your main code here, to run repeatedly:
  digitalWrite(5, HIGH);
  Serial.println("LED ON");
  delay(1000);
  digitalWrite(5, LOW);
  Serial.println("LED OFF");
  delay(1000);
}