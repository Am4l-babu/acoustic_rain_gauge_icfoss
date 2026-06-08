 #include <Seeed_FS.h>
#include <TFT_eSPI.h>

#define LED_PIN 27 // Pin connected to the built-in LED on Wio Terminal

TFT_eSPI tft;

void setup() {
    pinMode(LED_PIN, OUTPUT);
    tft.begin();
    tft.setRotation(3);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    tft.println("Wio Terminal 1-Minute Interval");

    // Set up a timer interrupt for 1-minute interval (60,000 milliseconds)
    Timer1.initialize(60000);
    Timer1.attachInterrupt(blinkLED); // Attach the blinkLED function to the timer interrupt
}

void loop() {
    // Your main loop code here (if any)
}

void blinkLED() {
    static bool ledState = LOW;

    digitalWrite(LED_PIN, ledState); // Toggle the LED state
    ledState = !ledState;

    tft.fillScreen(TFT_BLACK);
    tft.setCursor(10, 10);
    tft.print("LED is ");
    tft.setTextColor(ledState ? TFT_GREEN : TFT_RED);
    tft.println(ledState ? "ON" : "OFF");
}
