#include <Arduino.h>
#include <fstream>

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (Serial.available())
  {
    Serial.println("start");
    char cmd = Serial.read();
    if (cmd == 'r')
    {
      std::fstream fileStream;
      fileStream.open("example.txt");
      if (fileStream.fail())
      {
        // file could not be opened
        Serial.println("file could not be opened");
      }
      else
      {
        Serial.println("file could be opened");
      }
      std::ofstream myfile;
      myfile.open("example.txt");
      myfile << "Writing this to a file.\n";
      myfile.close();
    }
  }
}