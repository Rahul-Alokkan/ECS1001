#define BLYNK_PRINT Serial
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#define samp_siz 4
#define rise_threshold 4

char auth[] = "";
char ssid[] = "";
char pass[] = "";
 
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

   for (int i = 0; i < samp_siz; i++)
      reads[i] = 0;
    sum = 0;
    ptr = 0;
    
    int j = 1;
    float avg = 0;
    int sum_values = 0;
    while(j < 5){
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
            sum = sum + print_value;
            j++;
           
           third = second;
           second = first;

            if(j == 5){
              j = 1;
              int avg = sum / 5;
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
    return avg;
    }
    

void sendSensor()
{
  float h = HeartRate();
   
   if (isnan(h)) {
    Serial.println("Failed to read from Heart sensor!");
    return;
  }
  
  Blynk.virtualWrite(V5, h);
 
}
 
void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(5000L, sendSensor);
}
 
void loop()
{
  Blynk.run();
  timer.run();
}
