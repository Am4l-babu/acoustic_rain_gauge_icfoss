#include <HardwareSerial.h>
#define RX_PIN 16  // Pin for receiving data
#define TX_PIN 17  // Pin for transmitting data
HardwareSerial sensorSerial(1); 


void setup() {
Serial.begin(9600);
Serial1.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);// put your setup code here, to run once:

}

void loop() {
Serial1.write("hello world");// put your main code here, to run repeatedly:
delay(1000);
}
