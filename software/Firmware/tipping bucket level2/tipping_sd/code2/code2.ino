const byte ledPin = 13;
const byte interruptPin = 2;
volatile byte state = LOW;
int count=0;
void counting();
int  currentMillis = millis();
int startMillis;
int period=60000;
int totalcount;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, RISING);
}

void loop() {
 
  digitalWrite(ledPin, state);
  
}

void blink() {
  state = !state;
  counting();
}

void counting()
{
  
  Serial.println(count++); 
  delay(500);

//  if(currentMillis - startMillis >= period)
//  {
//    count=0;
//    startMillis = currentMillis;
//  }
   
   
  }


  
