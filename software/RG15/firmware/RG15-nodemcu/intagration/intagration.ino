


#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include "Stream.h"
SoftwareSerial mySerial(7, 8); // RX, TX
int previous_accumulation = 0;
int i = 0;



  

class RG15 {
  private:
    Stream* stream;

    bool sendCommandLetter(char a);
  public:
    float acc, eventAcc, totalAcc, rInt;

    //    unsigned int xTBTips;
    //    float xTbEventAcc, xTbInt;
    bool metric;
    char unit[4];

    String getAvailable();
    void setStream(Stream* stream);

    //Reset the tracking retrieved by poll
    bool resetAccumulation();
    bool restartDevice();

    //Updates acc, eventAcc, totalAcc, rInt & metric
    bool poll();
    //Check if the device is still alive
    bool ping();
    bool setBaud(int baudCode);
    bool useExternalTippingBucket(bool isTippingBucketPresent);
    bool setContinuous();
    bool setPolling();
    bool setMetric();
    bool setImperial();
    bool setLowResolution();
    bool setHighResolution();
    bool resetSwitchToPhysical();
};
RG15 rg15;

bool RG15::poll() {
  stream->write('r');
  stream->write('\n');
  String response = stream->readStringUntil('\n');
  delay(1000);
  if (response.startsWith(F("Acc"))) {

    char accB[7], eventAccB[7], totalAccB[7], rIntB[7], unit[7];

    sscanf (response.c_str(), "%*s %s %[^,] , %*s %s %*s %*s %s %*s %*s %s", &accB, &unit, &eventAccB, &totalAccB, &rIntB);

    acc = atof(accB);
    eventAcc = atof(eventAccB);
    totalAcc = atof(totalAccB);
    rInt = atof(rIntB);

    metric = !(unit[0] == 'i' && unit[1] == 'n');

    return true;
  }
  return false;
}

bool RG15::ping() {
  return sendCommandLetter('p');
}

void RG15::setStream(Stream* stream) {
  this->stream = stream;
}

//Note that these will ignore the physical switch, undo this with useSwitchUnitAndResolution()
bool RG15::setMetric() {
  return sendCommandLetter('m');
}
bool RG15::setImperial() {
  return sendCommandLetter('i');
}

bool RG15::resetSwitchToPhysical() {
  return sendCommandLetter('s');
}

bool RG15::resetAccumulation() {
  return sendCommandLetter('o');
}

bool RG15::restartDevice() {
  return sendCommandLetter('k');
}

bool RG15::setLowResolution() {
  return sendCommandLetter('l');
}

bool RG15::setHighResolution() {
  return sendCommandLetter('h');
}

bool RG15::setContinuous() {
  return sendCommandLetter('c');
}

String RG15::getAvailable() {
  return stream->readStringUntil('\0');
}

bool RG15::useExternalTippingBucket(bool isTippingBucketPresent) {
 return sendCommandLetter( (isTippingBucketPresent) ? 'x' : 'y');
}

bool RG15::setBaud(int baudCode) {
  stream->write("b ");
  stream->write(baudCode);
  stream->write('\n');
  String response = stream->readStringUntil('\n');
  return response.charAt(2) == baudCode;
}


bool RG15::sendCommandLetter(char a) {
  stream->write(a);
  stream->write('\n');
  String response = stream->readStringUntil('\n');
  return response.charAt(0) == a;
}


void setup() {

  while (!Serial); // wait for Serial to be initialized
  Serial.begin(9600);
  Serial.println("setup");
  mySerial.begin(9600);
  rg15.setStream(&mySerial);
  Serial.println(F("Starting"));



}

void loop()
{
  if (rg15.poll())
  {
    int Accumulation = int(rg15.acc * 100);
    int Event_Accumulation = int(rg15.eventAcc * 100);
    int Total_Accumulation = int(rg15.totalAcc * 100);
    int Rain_Intensity = int(rg15.rInt * 100);
    delay(1000);
    Serial.print("Accumulation ");
    Serial.println(Accumulation);
    Serial.print("Event_Accumulation");
    Serial.println(Event_Accumulation);
    Serial.print("Total_Accumulation");
    Serial.println(Total_Accumulation);
    Serial.print("Rain_Intensity");
    Serial.println(Rain_Intensity);
    delay(10000);
 }
 }
