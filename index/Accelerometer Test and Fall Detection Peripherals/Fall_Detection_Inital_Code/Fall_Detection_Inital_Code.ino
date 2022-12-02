#include <Wire.h>
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPLMidodInr"
#define BLYNK_DEVICE_NAME "ECSProject"
#define BLYNK_AUTH_TOKEN "-dh0LkLXE7D5k5XSXhaWU6jD7Xndt6XB"

#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>


char auth[] = "-dh0LkLXE7D5k5XSXhaWU6jD7Xndt6XB"; 
char ssid[] = "Sri Ram"; 
char   pass[] = "Reddy2211."; 

#include <SoftwareSerial.h>
SoftwareSerial EspSerial(4, 5); // RX, TX

#define ESP8266_BAUD 9600

ESP8266 wifi(&EspSerial);

const int MPU_addr=0x68;  // I2C address of the MPU-6050
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
  Falldetection();

 

//   Serial.println("Wrote to IMU");
//   Serial.println("Connecting to ");
//   Serial.println(ssid);
//   WiFi.begin(ssid, pass);
//   while (WiFi.status() != WL_CONNECTED)
//   {
//     delay(500);
//     Serial.print(".");              
//   }
//   Serial.println("");
//   Serial.println("WiFi connected");

 pinMode(11, OUTPUT);
 
}


void Falldetection(){
   mpu_read();
   ax = (AcX - 2050) / 16384.00;
   ay = (AcY - 77) / 16384.00;
   az = (AcZ - 1947) / 16384.00;
   gx = (GyX + 270) / 131.07;
   gy = (GyY - 351) / 131.07;
   gz = (GyZ + 136) / 131.07;
   // calculating Amplitute vactor for 3 axis
   float Raw_Amp = pow(pow(ax, 2) + pow(ay, 2) + pow(az, 2), 0.5);
   int Amp = Raw_Amp * 10;  // Mulitiplied by 10 bcz values are between 0 to 1
   Serial.println(Amp);
if (Amp <= 2 && trigger2 == false) { //if AM breaks lower threshold (0.4g)     
trigger1 = true;     
Serial.println("TRIGGER 1 ACTIVATED");   
}   
if (trigger1 == true) {     
trigger1count++;     
if (Amp >= 12) { //if AM breaks upper threshold (3g)
       trigger2 = true;
       Serial.println("TRIGGER 2 ACTIVATED");
       trigger1 = false; trigger1count = 0;
     }
   }
   if (trigger2 == true) {
     trigger2count++;
     angleChange = pow(pow(gx, 2) + pow(gy, 2) + pow(gz, 2), 0.5); Serial.println(angleChange);
     if (angleChange >= 30 && angleChange <= 400) { //if orientation changes by between 80-100 degrees       
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
       if ((angleChange >= 0) && (angleChange <= 10)) { //if orientation changes remains between 0-10 degrees         
fall = true; trigger3 = false; trigger3count = 0;         
Serial.println(angleChange);       }       
else { //user regained normal orientation         
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


void loop(){
   Blynk.run();
 
 }

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
