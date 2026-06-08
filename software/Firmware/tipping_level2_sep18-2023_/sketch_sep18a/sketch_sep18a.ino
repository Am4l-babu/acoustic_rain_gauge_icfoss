const byte ledPin = 13;
const byte interruptPin = 2;
volatile byte state = LOW;
int flag;
int control(int num);
int raincount;
int count;

int control(int num)
{
  Serial.println(num);
//  int occur=timer()
  
 int count =count+num;
  raincount++;
  return 0;
  }



void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, RISING);
  Serial.begin(9600);
  count=0;
  }

void loop() {
  digitalWrite(ledPin, state);
  Serial.println(count);
}

void blink() {
  state = !state;
  int n=1;
  control(n);
}
