
#include <stdio.h>

#include <SoftwareSerial.h>

SoftwareSerial sim(10, 11);
String number = "+917658904361"; // +880 is the country code
void setup() {
  Serial.begin(9600);
  Serial.println("Wait for Some time for the System to get booted...");
  delay(5000);
  Serial.println("System Started...");
  sim.begin(9600);
  delay(1000);
  Serial.println("Type c to make a call for the encoded number and s to send an SMS");
}

void loop() {
  if (Serial.available() > 0)
    switch (Serial.read())
    {
      case 'c':
        callNumber();
        break;
      case 's':
        SendMessage();
        break;
    }
  if (sim.available() > 0)
    Serial.write(sim.read());
}

void SendMessage()
{
  sim.println("AT+CMGF=1");
  delay(1000);
  sim.println("AT+CMGS=\"" + number + "\"\r");
  delay(1000);
  String SMS; 
  printf("enter the message that you wanna send : "); 
  scanf("%d", &age);
  sim.println(SMS);
  delay(100);
  sim.println((char)26);
  delay(1000);
}

void callNumber() {
  sim.print (F("ATD"));
  sim.print (number);
  sim.print (F(";\r\n"));
}
