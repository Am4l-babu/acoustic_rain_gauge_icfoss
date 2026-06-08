#include <Arduino.h>
#include <SoftwareSerial.h>
unsigned long previous_time = 0;
unsigned long previous_day_time = 0;
unsigned long current_time = 0;
#include "Stream.h"
SoftwareSerial mySerial(7, 8); // RX, TX
int previous_accumulation=0;
int i = 0;
enum Unit {
  in, mm,
  iph, mmph,
  tips
};

struct
{
  int Accumulation;
  int Event_Accumulation;
  int Total_Accumulation;
  int Rain_Intensity;
  // int status_device;
} mydata;

void rain();
void call();
class RG15Arduino {
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
RG15Arduino rg15;

bool RG15Arduino::poll() {
  stream->write('r');
  stream->write('\n');
  String response = stream->readStringUntil('\n');
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

bool RG15Arduino::ping() {
  return sendCommandLetter('p');
}

void RG15Arduino::setStream(Stream* stream) {
  this->stream = stream;
}

//Note that these will ignore the physical switch, undo this with useSwitchUnitAndResolution()
bool RG15Arduino::setMetric() {
  return sendCommandLetter('m');
}
bool RG15Arduino::setImperial() {
  return sendCommandLetter('i');
}

bool RG15Arduino::resetSwitchToPhysical() {
  return sendCommandLetter('s');
}

bool RG15Arduino::resetAccumulation() {
  return sendCommandLetter('o');
}

bool RG15Arduino::restartDevice() {
  return sendCommandLetter('k');
}

bool RG15Arduino::setLowResolution() {
  return sendCommandLetter('l');
}

bool RG15Arduino::setHighResolution() {
  return sendCommandLetter('h');
}

bool RG15Arduino::setContinuous() {
  return sendCommandLetter('c');
}

String RG15Arduino::getAvailable() {
  return stream->readStringUntil('\0');
}

bool RG15Arduino::useExternalTippingBucket(bool isTippingBucketPresent) {
  return sendCommandLetter( (isTippingBucketPresent) ? 'x' : 'y');
}

bool RG15Arduino::setBaud(int baudCode) {
  stream->write("b ");
  stream->write(baudCode);
  stream->write('\n');
  String response = stream->readStringUntil('\n');
  return response.charAt(2) == baudCode;
}


bool RG15Arduino::sendCommandLetter(char a) {
  stream->write(a);
  stream->write('\n');
  String response = stream->readStringUntil('\n');
  return response.charAt(0) == a;
}





void setup() {
  Serial.begin(9600);
  Serial.println("setup");
  mySerial.begin(9600);
  rg15.setStream(&mySerial);

}

void loop() {

  current_time = millis();
  while (current_time - previous_day_time >= 30000)
  {
    call();
    Serial.print("mydata.Accumulation = ");
    Serial.println( mydata.Accumulation );
    Serial.print(" mydata.Event_Accumulation = ");
    Serial.println( mydata.Event_Accumulation);
    Serial.print("mydata.Total_Accumulation = ");
    Serial.println(mydata.Total_Accumulation);
    Serial.print("mydata.Rain_Intensity = ");
    Serial.println(mydata.Rain_Intensity);
    previous_day_time = current_time;
  }

}




void call() {
  if (rg15.poll()) {
    i = 0;
    
    int Accumulation = int(rg15.acc * 100);
    int Event_Accumulation = int(rg15.eventAcc * 100);
    int Total_Accumulation = int(rg15.totalAcc * 100);
    int Rain_Intensity = int(rg15.rInt * 100);
    delay(1000);
    if (previous_accumulation != Event_Accumulation)
    {
      
      mydata.Accumulation = Accumulation;
      mydata.Event_Accumulation = Event_Accumulation;
      mydata.Total_Accumulation = Total_Accumulation;
      mydata.Rain_Intensity = Rain_Intensity;
      delay(1000);
    }
    else
    {
     

      mydata.Accumulation = (Accumulation * 0);
      mydata.Event_Accumulation = (Event_Accumulation * 0);
      mydata.Total_Accumulation = (Total_Accumulation * 0);
      mydata.Rain_Intensity = (Rain_Intensity* 0);
      delay(1000);
    }
    previous_accumulation = Event_Accumulation;
    Serial.println("previous_accumulation");
    Serial.println(previous_accumulation);
  }

  else {
    Serial.println("Timeout!");
    i++;
    if (i > 5)
    {
      rg15.restartDevice();
    }
  }



  delay(1000);
}
