#include <Wire.h>
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPLMidodInr"
#define BLYNK_DEVICE_NAME "ECSProject"


#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>


char auth[] = "-dh0LkLXE7D5k5XSXhaWU6jD7Xndt6XB"; 
char ssid[] = "Sri Ram"; 
char pass[] = "Reddy2211."; 

#include <SoftwareSerial.h>
BlynkTimer timer;
SoftwareSerial EspSerial(4, 5); // RX, TX

#define ESP8266_BAUD 9600

ESP8266 wifi(&EspSerial);

const int MPU_addr=0x68;  
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
float ax=0, ay=0, az=0, gx=0, gy=0, gz=0;

 
boolean fall = false; 
boolean trigger1=false; 
boolean trigger2=false; 
boolean trigger3=false; 

byte trigger1count=0; 
byte trigger2count=0; 
byte trigger3count=0; 
int angleChange=0;

void mpu_read(){
  
 Wire.beginTransmission(MPU_addr);
 Wire.write(0x3B);  
 Wire.endTransmission(false);
 Wire.requestFrom(MPU_addr,14,true);  
 AcX=Wire.read()<<8|Wire.read();    
 AcY=Wire.read()<<8|Wire.read(); 
 AcZ=Wire.read()<<8|Wire.read();  
 Tmp=Wire.read()<<8|Wire.read();  
 GyX=Wire.read()<<8|Wire.read();  
 GyY=Wire.read()<<8|Wire.read();  
 GyZ=Wire.read()<<8|Wire.read();  
 
 }

void setup(){
 Wire.begin();
 Wire.beginTransmission(MPU_addr);
 Wire.write(0x6B);  
 Wire.write(0);     
 Wire.endTransmission(true);
 
 Serial.begin(9600);
   delay(10);

  EspSerial.begin(ESP8266_BAUD);
  delay(10);

  Blynk.begin(auth, wifi, ssid, pass);
 }


void loop()
{
   Blynk.run();

   
  Serial.println("Started");
   mpu_read();
   ax = (AcX - 2050) / 16384.00;
   ay = (AcY - 77) / 16384.00;
   az = (AcZ - 1947) / 16384.00;
   gx = (GyX + 270) / 131.07;
   gy = (GyY - 351) / 131.07;
   gz = (GyZ + 136) / 131.07;

   float Raw_Amp = pow(pow(ax, 2) + pow(ay, 2) + pow(az, 2), 0.5);
   int Amp = Raw_Amp * 10;  
   Serial.println(Amp);
if (Amp <= 2 && trigger2 == false) { 
trigger1 = true;     
Serial.println("TRIGGER 1 ACTIVATED");
Blynk.logEvent("Fall Detected ,Take Necessary actions");   
}   
if (trigger1 == true) {     
trigger1count++;     
if (Amp >= 12) { 
       trigger2 = true;
       Serial.println("TRIGGER 2 ACTIVATED");
       trigger1 = false; trigger1count = 0;
     }
   }
   if (trigger2 == true) {
     trigger2count++;
     angleChange = pow(pow(gx, 2) + pow(gy, 2) + pow(gz, 2), 0.5); 
     Serial.println(angleChange);
     if (angleChange >= 30 && angleChange <= 400) {    
trigger3 = true; trigger2 = false; trigger2count = 0;       
Serial.println(angleChange);       
Serial.println("TRIGGER 3 ACTIVATED");     
}   
}   
if (trigger3 == true) {     
trigger3count++;     
if (trigger3count >= 10) {
       angleChange = pow(pow(gx, 2) + pow(gy, 2) + pow(gz, 2), 0.5);
       //delay(10);
       Serial.println(angleChange);
       if ((angleChange >= 0) && (angleChange <= 10)) {    
fall = true; trigger3 = false; trigger3count = 0;         
Serial.println(angleChange);       }       
else {          
trigger3 = false; trigger3count = 0;         
Serial.println("TRIGGER 3 DEACTIVATED");       
}     
}   
}   
if (fall == true) {     
Serial.println("FALL DETECTED");     
Blynk.logEvent("Fall Detected ,Take Necessary actions");
fall = false;   
}   
if (trigger2count >= 6) { 
     trigger2 = false; trigger2count = 0;
     Serial.println("TRIGGER 2 DECACTIVATED");
   }
   if (trigger1count >= 6) { 
     trigger1 = false; trigger1count = 0;
     Serial.println("TRIGGER 1 DECACTIVATED");
   }
   delay(100);
  
 }
