unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
const unsigned long period = 60000;  //the value is a number of milliseconds
const byte ledPin = 13;    //using the built in LED
const byte interruptPin = 2;
volatile byte state = LOW;
int count=0;
void counting();
int totalcount;
int flag;

void blink() {
  state = !state;
  flag++;
}





void setup()
{
  Serial.begin(9600);  //start Serial in case we need to print debugging info
  pinMode(ledPin, OUTPUT);
  startMillis = millis();
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, RISING);
}

void loop()
{
digitalWrite(ledPin, state);

  int rain_count=+flag;
  Serial.println(rain_count);
  currentMillis = millis();  //get the current "time" (actually the number of milliseconds since the program started)
  if (currentMillis - startMillis >= period)  //test whether the period has elapsed
  {
    int total_rain=flag;
    int avg_rain_count=total_rain/60;
    int rain=total_rain*0.28;
    int avg_rain =avg_rain_count*0.28;
    
   flag=0;//digitalWrite(ledPin, !digitalRead(ledPin));  //if so, change the state of the LED.  Uses a neat trick to change the state
    startMillis = currentMillis;  //IMPORTANT to save the start time of the current LED state.
  }
}
