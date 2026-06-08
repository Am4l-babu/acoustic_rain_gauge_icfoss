
/*******************************************************************************
   Copyright (c) 2021 Arun C ,Thomas Telkamp and Matthijs Kooijman

   Permission is hereby granted, free of charge, to anyone
   obtaining a copy of this document and accompanying files,
   to do whatever they want with them without any restriction,
   including, but not limited to, copying, modification and redistribution.
   NO WARRANTY OF ANY KIND IS PROVIDED.

   ToDo:
   - set NWKSKEY
   - set APPKSKEY
   - set DEVADDR
   - optionally comment #define DEBUG
   - optionally comment #define SLEEP
   - set TX_INTERVAL in seconds
   - change mydata to another (small) static text

 *******************************************************************************/
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#define pinAdc A0
int sound();
int loud();

static const PROGMEM u1_t NWKSKEY[16] = { 0x23, 0xB0, 0x86, 0x08, 0x1E, 0xB7, 0x7D, 0x1F, 0xDE, 0xA4, 0x24, 0x2B, 0xEA, 0xC9, 0xFB, 0xDA };

static const u1_t PROGMEM APPSKEY[16] = { 0xDE, 0x29, 0xE3, 0x85, 0x42, 0xA6, 0x88, 0x3C, 0xC6, 0xBC, 0xC1, 0xBC, 0xFA, 0x8B, 0x45, 0x90};

static const u4_t DEVADDR = 0xfc009552 ; // <-- Change this address for every node!



// show debug statements; comment next line to disable debug statemen
#define DEBUG

// use low power sleep; comment next line to not use low power sleep
#define SLEEP

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 10;

struct {
  
int loudness;
int16_t sound;

} mydata;

#ifdef SLEEP
#include "LowPower.h"
bool next = false;
#endif

// These callbacks are only used in over-the-air activation, so they are
// left empty here (we cannot leave them out completely unless
// DISABLE_JOIN is set in config.h, otherwise the linker will complain).
void os_getArtEui (u1_t* buf) { }
void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }

static osjob_t sendjob;

// Pin mapping
const lmic_pinmap lmic_pins = {
.nss = 6,
.rxtx = LMIC_UNUSED_PIN,
.rst = 5,
.dio = {2, 3, 4},
};

void onEvent (ev_t ev) {
#ifdef DEBUG
Serial.println(F("Enter onEvent"));
#endif

switch (ev) {
#ifdef DEBUG
case EV_SCAN_TIMEOUT:
Serial.println(F("EV_SCAN_TIMEOUT"));
break;
case EV_BEACON_FOUND:
Serial.println(F("EV_BEACON_FOUND"));
break;
case EV_BEACON_MISSED:
Serial.println(F("EV_BEACON_MISSED"));
break;
case EV_BEACON_TRACKED:
Serial.println(F("EV_BEACON_TRACKED"));
break;
case EV_JOINING:
Serial.println(F("EV_JOINING"));
break;
case EV_JOINED:
Serial.println(F("EV_JOINED"));
break;
case EV_RFU1:
Serial.println(F("EV_RFU1"));
break;
case EV_JOIN_FAILED:
Serial.println(F("EV_JOIN_FAILED"));
break;
case EV_REJOIN_FAILED:
Serial.println(F("EV_REJOIN_FAILED"));
break;
#endif
case EV_TXCOMPLETE:
Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));

if (LMIC.dataLen) {
// data received in rx slot after tx
Serial.print(F("Data Received: "));
Serial.write(LMIC.frame + LMIC.dataBeg, LMIC.dataLen);
Serial.println();
}

// Schedule next transmission
#ifndef SLEEP
os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
#else
next = true;
#endif

break;

#ifdef DEBUG
case EV_LOST_TSYNC:
Serial.println(F("EV_LOST_TSYNC"));
break;
case EV_RESET:
Serial.println(F("EV_RESET"));
break;
case EV_RXCOMPLETE:
// data received in ping slot
Serial.println(F("EV_RXCOMPLETE"));
break;
case EV_LINK_DEAD:
Serial.println(F("EV_LINK_DEAD"));
break;
case EV_LINK_ALIVE:
Serial.println(F("EV_LINK_ALIVE"));
break;
default:
Serial.println(F("Unknown event"));
break;
#endif
}
#ifdef DEBUG
Serial.println(F("Leave onEvent"));
#endif

}

void do_send(osjob_t* j)

{

mydata.loudness = loud();
Serial.println("mydata.loudness=");
Serial.print(mydata.loudness);
mydata.sound = sound();
Serial.println("mydata.sound=");
Serial.print(mydata.sound);
#ifdef DEBUG
Serial.println(F("Enter do_send"));
#endif

// Check if there is not a current TX/RX job running
if (LMIC.opmode & OP_TXRXPEND) {
Serial.println(F("OP_TXRXPEND, not sending"));
} else {
// Prepare upstream data transmission at the next possible time.
LMIC_setTxData2(1, (unsigned char *)&mydata, sizeof(mydata) - 1, 0);
Serial.println(F("Packet queued"));
}
// Next TX is scheduled after TX_COMPLETE event.
#ifdef DEBUG
Serial.println(F("Leave do_send"));
#endif

}

//Soil Moisture
//int sensor_pin = A5, moisture;
//const int sensorEn = 8;
int sound()
{
  long sum = 0;
  for (int i = 0; i < 32; i++)
  {
    sum += analogRead(pinAdc);
  }

  sum >>= 5;
  //Serial.print("sound= ");
  Serial.println(sum);
  delay(200);
  return sum;
}


int loud()
{

  int loudn = analogRead(A3);
  // Serial.print("loudness= ");
  //do_send(&sendjob);
  Serial.print(loudn);
  //Serial.print("\t");
  Serial.print("\t");
  delay(200);
  return loudn;
}



//int soil_moisture()
//{
//  
//digitalWrite(sensorEn, HIGH);
//delay(200);
//moisture = analogRead(sensor_pin);
//moisture = map(moisture, 850, 350, 0, 100);
//digitalWrite(sensorEn, LOW);
//#ifdef DEBUG
//Serial.println("Moisture = ");
//Serial.print(moisture);
//Serial.print("%");
//#endif
//return moisture;
//
//}

//Battery Monitor
//int batv = 0, bat_status = 0;
//
//int batteryv()
//{
//
// float analogvalue = 0, battVolt = 0;
// 
//  for (byte  i = 0; i < 10; i++) {
//    analogvalue += analogRead(A3);
//    delay(5);
//  }
//  analogvalue = analogvalue / 10;
//#ifdef DEBUG
//  Serial.print("analogvalue= ");
//  Serial.println(analogRead(A3));
//#endif
//  battVolt = ((analogvalue * 3.3) / 1024) * 2; //ADC voltage*Ref. Voltage/1024
//
//  int batt_millivolt = battVolt * 100;
//
//#ifdef DEBUG
//  Serial.print("Voltage= ");
//  Serial.print(battVolt);
//  Serial.println("V");
//#endif
//
//  batv = batt_millivolt;
// return batv;
//}

void setup() {
  
Serial.begin(9600);
Serial.print("Starting..");
//pinMode(sensorEn, OUTPUT);
Serial.println(F("Enter setup"));

#ifdef VCC_ENABLE
// For Pinoccio Scout boards
pinMode(VCC_ENABLE, OUTPUT);
digitalWrite(VCC_ENABLE, HIGH);
#endif
// LMIC init
os_init();
// Reset the MAC state. Session and pending data transfers will be discarded.
LMIC_reset();

// Set static session parameters. Instead of dynamically establishing a session
// by joining the network, precomputed session parameters are be provided.
#ifdef PROGMEM
// On AVR, these values are stored in flash and only copied to RAM
// once. Copy them to a temporary buffer here, LMIC_setSession will
// copy them into a buffer of its own again.
uint8_t appskey[sizeof(APPSKEY)];
uint8_t nwkskey[sizeof(NWKSKEY)];
memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
memcpy_P(nwkskey, NWKSKEY, sizeof(NWKSKEY));
LMIC_setSession (0x1, DEVADDR, nwkskey, appskey);
#else
// If not running an AVR with PROGMEM, just use the arrays directly
LMIC_setSession (0x1, DEVADDR, NWKSKEY, APPSKEY);
#endif

#if defined(CFG_eu868)
LMIC_setupChannel(0, 865062500, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI); // g-band
LMIC_setupChannel(1, 865402500, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI); // g-band
LMIC_setupChannel(2, 865985000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI); // g-band
#elif defined(CFG_us915)
LMIC_selectSubBand(1);
#endif

// Disable link check validation
LMIC_setLinkCheckMode(0);

// TTN uses SF9 for its RX2 window.
LMIC.dn2Dr = DR_SF9;

// Set data rate and transmit power for uplink (note: txpow seems to be ignored by the library)
LMIC_setDrTxpow(DR_SF7, 14);

// Start job
do_send(&sendjob);
// Wait a maximum of 10s for Serial Monitor
// while (!debugSerial && millis() < 10000);

}

void loop() {

#ifndef SLEEP

os_runloop_once();

#else
extern volatile unsigned long timer0_overflow_count;

if (next == false) {

os_runloop_once();

} 
else 
{

int sleepcycles = TX_INTERVAL / 8; // calculate the number of sleepcycles (8s) given the TX_INTERVAL

#ifdef DEBUG
Serial.print(F("Enter sleeping for "));
Serial.print(sleepcycles);
Serial.println(F(" cycles of 8 seconds"));
#endif

Serial.flush(); // give the serial print chance to complete

for (int i = 0; i < sleepcycles; i++) {
// Enter power down state for 8 s with ADC and BOD module disabled
LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
//LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);

// LMIC uses micros() to keep track of the duty cycle, so
// hack timer0_overflow for a rude adjustment:
cli();
timer0_overflow_count += 8 * 64 * clockCyclesPerMicrosecond();
sei();
}

#ifdef DEBUG
Serial.println(F("Sleep complete"));
#endif
next = false;
// Start job
do_send(&sendjob);
}

#endif

}
