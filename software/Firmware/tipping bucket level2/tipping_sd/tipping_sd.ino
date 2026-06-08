const byte ledPin = 13;
const byte interruptPin = 2;
volatile byte state = LOW;
int interrupt_state;
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

int count=0;
void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(interruptPin), blink, CHANGE);
}

void loop() {
  digitalWrite(ledPin, state);
  int reading = digitalRead(interruptPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != state) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != interrupt_state) {
     // buttonState = reading;
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, CHANGE);

      // only toggle the LED if the new button state is HIGH
      if (interrupt_state == HIGH) {
        state = !state ;
      }
}
  }
}

void blink() {
  state = !state;
  
  Serial.print("rain count=");
  Serial.print("\t");
  Serial.print(count++);
  Serial.println();
}
