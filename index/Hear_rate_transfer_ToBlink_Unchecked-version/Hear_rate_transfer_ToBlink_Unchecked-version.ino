#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPLMidodInr"
#define BLYNK_DEVICE_NAME "ECSProject"
#define samp_siz 4
#define rise_threshold 4
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>

char auth[] = "-dh0LkLXE7D5k5XSXhaWU6jD7Xndt6XB";
char ssid[] = "Sri Ram";
char pass[] = "Reddy2211.";




#include <SoftwareSerial.h>
SoftwareSerial EspSerial(4, 5); // RX, TX

#define ESP8266_BAUD 9600

ESP8266 wifi(&EspSerial);

 
BlynkTimer timer;
 
int sensorPin = 0;

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
    
//    if(flag == 1){
//    Serial.println("Average Value :");  
//    Serial.println(avg);
//    }else{
//    Serial.println("Not Getting Correct Value");
//    }
    
    return avg;
    }

void sendSensor()
{
  float h = HeartRate();
   
   if (isnan(h)) {
    Serial.println("Failed to read from Heart sensor!");
    return;
  }
  
  Blynk.virtualWrite(V1, h);
 
}
 
void setup()
{
 
//  Serial.begin(9600);

//  Serial.println("Connecting to ");
//   Serial.println(ssid);
//   WiFi.begin(ssid, pass);
//   while (WiFi.status() != WL_CONNECTED)
//   {
//     delay(500);
//     Serial.print(".");              
//   }
//   Serial.println("");
//   Serial.println("WiFi connected");

  Serial.begin(9600);
  delay(10);

  EspSerial.begin(ESP8266_BAUD);
  delay(10);

   
  Blynk.begin(auth, wifi, ssid, pass);
  timer.setInterval(6000L, sendSensor);
}
 
void loop()
{
  Blynk.run();
  timer.run();
}
