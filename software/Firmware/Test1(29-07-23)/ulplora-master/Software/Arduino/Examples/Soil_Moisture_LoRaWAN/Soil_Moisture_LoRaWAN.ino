/*******************************************************************************
   Copyright (c) 2015 Thomas Telkamp and Matthijs Kooijman
   Modified by Akshai M, ICFOSS

   Permission is hereby granted, free of charge, to anyone
   obtaining a copy of this document and accompanying files,
   to do whatever they want with them without any restriction,
   including, but not limited to, copying, modification and redistribution.
   NO WARRANTY OF ANY KIND IS PROVIDED.

   This example sends a valid LoRaWAN packet with static payload,
   using frequency and encryption settings matching those of
   the (early prototype version of) The Things Network.

   Note: LoRaWAN per sub-band duty-cycle limitation is enforced (1% in g1,
    0.1% in g2).

   ToDo:
   - set NWKSKEY (value from staging.thethingsnetwork.com)
   - set APPKSKEY (value from staging.thethingsnetwork.com)
   - set DEVADDR (value from staging.thethingsnetwork.com)
   - optionally comment #define DEBUG
   - optionally comment #define SLEEP
   - set TX_INTERVAL in seconds
   - change mydata to another (small) static text

 *******************************************************************************/
#include <lmic.h> // Use https://gitlab.com/icfoss/OpenIoT/ulplora/tree/master/V2.1/Software/Arduino/Libraries/arduino-lmic-master
#include <hal/hal.h>
#include <SPI.h>

// LoRaWAN NwkSKey, network session key
// This is the default Semtech key, which is used by the early prototype TTN
// network.
static const PROGMEM u1_t NWKSKEY[16] = { 0x9A, 0x65, 0x19, 0xEC, 0x3C, 0x33, 0x83, 0xAC, 0x31, 0x07, 0x11, 0x97, 0x76, 0x78, 0xF8, 0xDC };

// LoRaWAN AppSKey, application session key
// This is the default Semtech key, which is used by the early prototype TTN
// network.
static const u1_t PROGMEM APPSKEY[16] = { 0x1E, 0x11, 0xF4, 0x4E, 0x3F, 0x42, 0xFF, 0x6E, 0xBA, 0xD2, 0x4B, 0x0E, 0xBB, 0xBA, 0xAA, 0x32 };

// LoRaWAN end-device address (DevAddr)
static const u4_t DEVADDR = 0x26000E00 ; // <-- Change this address for every node!


// show debug statements; comment next line to disable debug statements
#define DEBUG
// use low power sleep; comment next line to not use low power sleep
#define SLEEP

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 960;

struct {
  int16_t a;
  int16_t b;
} mydata;


#ifdef SLEEP
#include "LowPower.h" //https://github.com/rocketscream/Low-Power
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
  }
#ifdef DEBUG
  Serial.println(F("Leave onEvent"));
#endif

}

void do_send(osjob_t* j) {

  mydata.a = batterycalc();
  mydata.b = soil_moisture();
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

int val = 0, batv = 0, bat_status = 0, temp = 0, diff = 0; // variable to store the value read

//Soil Moisture
int sensor_pin = A0, moisture;
const int sensorEn = 10;


void setup() {
  Serial.begin(9600);
  Serial.print("Starting..");
  delay(5000);
  pinMode(sensorEn, OUTPUT);
  Serial.println(F("Enter setup"));

#ifdef VCC_ENABLE
  // For Pinoccio Scout boards
  pinMode(VCC_ENABLE, OUTPUT);
  digitalWrite(VCC_ENABLE, HIGH);
  delay(1000);
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

  // Disable link check validation
  LMIC_setLinkCheckMode(0);

  // Set data rate and transmit power (note: txpow seems to be ignored by the library)
  LMIC_setDrTxpow(DR_SF7, 14);

  // Start job
  do_send(&sendjob);

#ifdef DEBUG
  Serial.println(F("Leave setup"));
#endif
}

void loop() {

#ifndef SLEEP

  os_runloop_once();

#else
  extern volatile unsigned long timer0_overflow_count;

  if (next == false) {

    os_runloop_once();

  } else {

    int sleepcycles = TX_INTERVAL / 8;  // calculate the number of sleepcycles (8s) given the TX_INTERVAL
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
int batteryv()
{

  val = analogRead(A3) >> 2;     // read the input pin
  batv = (val * 2.0 * 3.3 / 255) * 1000; //1.85 is the voltage multiplier
  delay(0);
  return batv;
}

int batterycalc()
{
  if (bat_status == 0)
  {
    for (int j = 0; j < 5; j++)
    {
      bat_status = bat_status + batteryv();
    }
    bat_status = bat_status / 5;
  }
#ifdef DEBUG
  Serial.println("Battery_status = ");
  Serial.print(bat_status);
#endif
  for (int i = 0; i < 5; i++)
  {
    batv = batv + batteryv();
  }
  batv = batv / 5;
  diff = abs(bat_status - batv);
#ifdef DEBUG
  Serial.println("Now Avg = ");
  Serial.println(batv);
  Serial.println("Value diff");
  Serial.println(diff);
#endif
  if (diff > 25 && diff < 70)
  {
    bat_status = batv;
  }
  batv = 0;
#ifdef DEBUG
  Serial.println("Battery Voltage");
  Serial.println(bat_status);
#endif
return bat_status;
}


int soil_moisture()
{
  digitalWrite(sensorEn, HIGH); //Sensor power driven directly by GPIO
  delay(200);
  moisture = analogRead(sensor_pin);
  moisture = map(moisture, 850, 350, 0, 100);
  digitalWrite(sensorEn, LOW);
#ifdef DEBUG
  Serial.println("Moisture = ");
  Serial.print(moisture);
  Serial.print("%");
#endif
  return moisture;
}
