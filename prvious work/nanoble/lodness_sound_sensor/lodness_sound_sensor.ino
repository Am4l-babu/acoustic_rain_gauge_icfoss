unsigned long startMillis; 
unsigned long currentMillis;
unsigned long loudness;
unsigned long loud;
int sound = 0;
//const int pinAdc = A0;

int ReadSound();
const unsigned long period = 60000;  

void setup()
{
  Serial.begin(9600); 
  Serial.print("Hello come to loudness");
  Serial.println();
  startMillis = millis();  
}

int ReadSound()
{
      Serial.print("ReadSound fn executed..");

    long sound = 0;
    for(int i=0; i<32; i++)
    {
        sound += analogRead(A0);
    }

    sound >>= 5;
    Serial.print("Sound= ");
    Serial.println(sound);
    //delay(1000);
    //return sound;                          
    
}
void loop()
{
  
  //delay(1000);
  unsigned long sum=0;

  while(analogRead>0){
  currentMillis = millis();
  loud=analogRead(A3);

  delay(1000);
 
  sum=loud+sum;
  delay(200);
  
  if (currentMillis - startMillis >= period)  
{

    loudness=sum/60;
    Serial.print("loudness == ");
    Serial.println(loudness);
    

    startMillis = currentMillis; 
        loud=0;
        sum=0;
        
    delay(100);
    break;
  }

  }
  
   ReadSound();

}
