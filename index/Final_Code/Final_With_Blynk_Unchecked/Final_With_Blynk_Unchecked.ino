#include <Wire.h>
#define samp_siz 4
#define rise_threshold 4
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#define BLYNK_TEMPLATE_ID "TMPLMidodInr"
#define BLYNK_DEVICE_NAME "ECSProject"
#define BLYNK_AUTH_TOKEN "-dh0LkLXE7D5k5XSXhaWU6jD7Xndt6XB"


char auth[] = "-dh0LkLXE7D5k5XSXhaWU6jD7Xndt6XB"; 
const char *ssid = ""; 
const char *pass = ""; 

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


int sensorPin = 0;


void setup() {
 Wire.begin();
 Wire.beginTransmission(MPU_addr);
 Blynk.begin(auth, ssid, pass);
 Wire.write(0x6B);  
 Wire.write(0);     
 Wire.endTransmission(true);
 Serial.begin(115200);
 pinMode(11, OUTPUT);

   Serial.println("Wrote to IMU");
   Serial.println("Connecting to ");
   Serial.println(ssid);
   WiFi.begin(ssid, pass);
   while (WiFi.status() != WL_CONNECTED)
   {
     delay(500);
     Serial.print(".");              
   }
   Serial.println("");
   Serial.println("WiFi connected");
}

float HeartRate(){
    float reads[samp_siz], sum;
    long int now, ptr;
    float last, reader, start;
    float first, second, third, before, print_value;
    bool rising;
    int rise_count;
    int n;
    long int last_beat;
    float values[5];
    int flag  = 0;

   for (int i = 0; i < samp_siz; i++)
      reads[i] = 0;
    sum = 0;
    ptr = 0;
    
    int j = 1;
    float avg = 0;
    int sum_values = 0;
    while(j <= 5){
       n = 0;
      start = millis();
      reader = 0.;
      do
      {
        reader += analogRead (sensorPin);
        n++;
        now = millis();
      }
      while (now < start + 20);  
      reader /= n; 
      
    
      sum -= reads[ptr];
      sum += reader;
      reads[ptr] = reader;
      last = sum / samp_siz;
    
    if (last > before)
      {
        rise_count++;
        if (!rising && rise_count > rise_threshold)
        {
         
          rising = true;
          first = millis() - last_beat;
          last_beat = millis();

         
          print_value = 60000. / (0.4 * first + 0.3 * second + 0.3 * third);
          
           values[j - 1] = print_value;
           Serial.println("Measured Value ;");
           Serial.println(values[j-1]);
            sum_values = sum_values + print_value;
            j++;
           
           third = second;
           second = first;

            if(j == 5){
              j = 1;
              avg = (sum_values / 4);
              break;
            }
       }
      }
      else
      {
       
        rising = false;
        rise_count = 0;
      }
      before = last;
      ptr++;
      ptr %= samp_siz;

    }
    
    if(avg * 1 != 0){
      flag = 1;
    }
    
    if(flag == 1){
        float h = avg;
   
   if (isnan(h)) {
    Serial.println("Failed to read from Heart sensor!");
  }
  Blynk.virtualWrite(V5, h);
    return 0.0;
    }else{
    Serial.println("Not Getting Correct Value");
    }
    
    return 0.0;
    }

void Falldetection(){
   mpu_read();
   ax = (AcX - 2050) / 16384.00;
   ay = (AcY - 77) / 16384.00;
   az = (AcZ - 1947) / 16384.00;
   gx = (GyX + 270) / 131.07;
   gy = (GyY - 351) / 131.07;
   gz = (GyZ + 136) / 131.07;
 
   float Raw_Amp = pow(pow(ax, 2) + pow(ay, 2) + pow(az, 2), 0.5);
   int Amp = Raw_Amp * 10;  

if (Amp <= 2 && trigger2 == false) { //if AM breaks lower threshold (0.4g)     
trigger1 = true;     
Serial.println("TRIGGER 1 ACTIVATED");   
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
   if (trigger1count >= 6) { //allow 0.5s for AM to break upper threshold
     trigger1 = false; trigger1count = 0;
     Serial.println("TRIGGER 1 DECACTIVATED");
   }
   delay(100);
}


    

void loop() {
   Blynk.run();
   Falldetection();
   HeartRate(); 

   }
