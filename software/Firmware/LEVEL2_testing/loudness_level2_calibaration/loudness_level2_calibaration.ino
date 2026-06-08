



unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
unsigned long  loudness;
unsigned long sum=0;
unsigned long loud;
const unsigned long period = 60000;  //the value is a number of milliseconds
const byte ledPin = 13;    //using the built in LED

void setup()
{
  Serial.begin(9600);  //start Serial in case we need to print debugging info
  Serial.print("Hello come to loudness");
  Serial.println();
  startMillis = millis();  //initial start time
}

void loop()
{
  
  //delay(1000);
  
  while(analogRead>0){
  currentMillis = millis();
  loud=analogRead(A0);
  delay(1000);
 // Serial.println(loud);
  sum=loud+sum;
  delay(200);
 //Serial.println(sum);
  //delay(100);
    //get the current "time" (actually the number of milliseconds since the program started)
  if (currentMillis - startMillis >= period)  //test whether the period has elapsed
{
    loudness=sum/60;
    Serial.print("loudness == ");
    Serial.print(loudness);
    Serial.println();
    
    //digitalWrite(ledPin, !digitalRead(ledPin));  //if so, change the state of the LED.  Uses a neat trick to change the state
    startMillis = currentMillis; 
        loud=0;
         sum=0;
    delay(100);//IMPORTANT to save the start time of the current LED state.
    break;
  }

}
//Serial.println("again start loop");



}
