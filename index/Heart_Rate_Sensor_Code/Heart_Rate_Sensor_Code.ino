#define samp_siz 4
#define rise_threshold 4


int sensorPin = 0;

void setup() {
    Serial.begin(9600);
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
    Serial.println("Average Value :");  
    Serial.println(avg);
    }else{
    Serial.println("Not Getting Correct Value");
    }
    
    return 0.0;
    }
    
void loop ()
{
HeartRate();
}
