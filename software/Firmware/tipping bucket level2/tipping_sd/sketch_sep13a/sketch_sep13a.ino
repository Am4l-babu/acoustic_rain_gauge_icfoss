const int reedSwitchPin = 2;  // Connect the reed switch to digital pin 2
volatile bool switchState = LOW;  // Initialize switch state
volatile unsigned long lastDebounceTime = 0;  // Last time the switch state changed
const unsigned long debounceDelay = 50;  // Debounce time in milliseconds

void setup() {
  pinMode(reedSwitchPin, INPUT_PULLUP);  // Enable the internal pull-up resistor
  attachInterrupt(digitalPinToInterrupt(reedSwitchPin), debounceInterrupt, CHANGE);  // Attach the interrupt to the reed switch pin
}

void loop() {
  // Your main code here (executed continuously)
}

void debounceInterrupt() {
  // Get the current time
  unsigned long currentTime = millis();
  
  // Check if the switch state has changed and debounce time has passed
  if (currentTime - lastDebounceTime >= debounceDelay) {
    // Read the state of the reed switch
    int newSwitchState = digitalRead(reedSwitchPin);
    
    // Update the last debounce time
    lastDebounceTime = currentTime;

    // If the new state is different from the previous state
    if (newSwitchState != switchState) {
      switchState = newSwitchState;  // Update the switch state

      // Perform your desired action here when the switch state changes
      if (switchState == HIGH) {
        Serial.println("high");// Reed switch is closed (activated)
        // Add your code for the switch closed state
      } else {
       Serial.println("low"); // Reed switch is open (deactivated)
        // Add your code for the switch open state
      }
    }
  }
}
