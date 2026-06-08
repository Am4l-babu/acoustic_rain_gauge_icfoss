/*******************************************************************************
   Copyright (c) 2015 Thomas Telkamp and Matthijs Kooijman
   Copyright (c) 2018 Terry Moore, MCCI

   Permission is hereby granted, free of charge, to anyone
   obtaining a copy of this document and accompanying files,
   to do whatever they want with them without any restriction,
   including, but not limited to, copying, modification and redistribution.
   NO WARRANTY OF ANY KIND IS PROVIDED.

   This example sends a valid LoRaWAN packet with payload "Hello,
   world!", using frequency and encryption settings matching those of
   the The Things Network.

   This uses ABP (Activation-by-personalisation), where a DevAddr and
   Session keys are preconfigured (unlike OTAA, where a DevEUI and
   application key is configured, while the DevAddr and session keys are
   assigned/generated in the over-the-air-activation procedure).

   Note: LoRaWAN per sub-band duty-cycle limitation is enforced (1% in
   g1, 0.1% in g2), but not the TTN fair usage policy (which is probably
   violated by this sketch when left running for longer)!

   To use this sketch, first register your application and device with
   the things network, to set or generate a DevAddr, NwkSKey and
   AppSKey. Each device should have their own unique values for these
   fields.

   Do not forget to define the radio type correctly in
   arduino-lmic/project_config/lmic_project_config.h or from your BOARDS.txt.

 *******************************************************************************/

// References:
// [feather] adafruit-feather-m0-radio-with-lora-module.pdf

#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <Arduino.h>
#include <ctype.h>

#include <SoftwareSerial.h>
SoftwareSerial unoSerial (7, 8);  // RX, TX

unsigned long previous_time = 0;
unsigned long previous_day_time = 0;
unsigned long current_time = 0;
uint8_t Accumulation = 0;
uint16_t Event_accumulation = 0;
uint16_t Total_accumulation = 0;
uint16_t IPH = 0;

#define DEBUG
bool next = true;
//
// For normal use, we require that you edit the sketch to replace FILLMEIN
// with values assigned by the TTN console. However, for regression tests,
// we want to be able to compile these scripts. The regression tests define
// COMPILE_REGRESSION_TEST, and in that case we define FILLMEIN to a non-
// working but innocuous value.
//
#ifdef COMPILE_REGRESSION_TEST
# define CFG_in866 1
#else
# warning "You must replace the values marked FILLMEIN with real values from the TTN control panel!"
# define FILLMEIN (#dont edit this, edit the lines that use FILLMEIN)
#endif

// LoRaWAN NwkSKey, network session key
// This should be in big-endian (aka msb).
static const PROGMEM u1_t NWKSKEY[16] = { 0x5E, 0xEF, 0x66, 0x8B, 0x20, 0x10, 0x2C, 0x2D, 0xCA, 0xF3, 0x4B, 0x49, 0x02, 0xE8, 0x23, 0xD8 };

// LoRaWAN AppSKey, application session key
// This should also be in big-endian (aka msb).
static const u1_t PROGMEM APPSKEY[16] = { 0xF8, 0xA6, 0x68, 0x10, 0x1B, 0x60, 0x36, 0x1D, 0x63, 0xFC, 0x08, 0x9C, 0x65, 0x11, 0xCA, 0x2C };

// LoRaWAN end-device address (DevAddr)
// See http://thethingsnetwork.org/wiki/AddressSpace
// The library converts the address to network byte order as needed, so this should be in big-endian (aka msb) too.
static const u4_t DEVADDR = 0xFC00978C ; // <-- Change this address for every node!

// These callbacks are only used in over-the-air activation, so they are
// left empty here (we cannot leave them out completely unless
// DISABLE_JOIN is set in arduino-lmic/project_config/lmic_project_config.h,
// otherwise the linker will complain).
void os_getArtEui (u1_t* buf) { }
void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }

static uint8_t mydata[7];
static osjob_t sendjob;

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned long TX_INTERVAL = 60;

// Pin mapping
// Adapted for Feather M0 per p.10 of [feather]
const lmic_pinmap lmic_pins = {
  .nss = 6,                       // chip select on feather (rf95module) CS
  .rxtx = LMIC_UNUSED_PIN,
  .rst = 5,                       // reset pin
  .dio = {2, 3, LMIC_UNUSED_PIN}, // assumes external jumpers [feather_lora_jumper]
  // DIO1 is on JP1-1: is io1 - we connect to GPO6
  // DIO1 is on JP5-3: is D2 - we connect to GPO5
};

void onEvent (ev_t ev) {
  Serial.print(os_getTime());
  Serial.print(": ");
  switch (ev) {
    case EV_JOINING:
      Serial.println(F("EV_JOINING"));
      break;
    case EV_JOINED:
      Serial.println(F("EV_JOINED"));
      break;
    /*
      || This event is defined but not used in the code. No
      || point in wasting codespace on it.
      ||
      || case EV_RFU1:
      ||     Serial.println(F("EV_RFU1"));
      ||     break;
    */
    case EV_JOIN_FAILED:
      Serial.println(F("EV_JOIN_FAILED"));
      break;
    case EV_REJOIN_FAILED:
      Serial.println(F("EV_REJOIN_FAILED"));
      break;
    case EV_TXCOMPLETE:
      Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
      if (LMIC.txrxFlags & TXRX_ACK)
        Serial.println(F("Received ack"));
      if (LMIC.dataLen) {
        Serial.println(F("Received "));
        Serial.println(LMIC.dataLen);
        Serial.println(F(" bytes of payload"));
      }
      // Schedule next transmission
      //os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
      next = false;
      break;
    case EV_RESET:
      Serial.println(F("EV_RESET"));
      break;
    case EV_RXCOMPLETE:
      // data received in ping slot
      Serial.println(F("EV_RXCOMPLETE"));
      break;

    /*
      || This event is defined but not used in the code. No
      || point in wasting codespace on it.
      ||
      || case EV_SCAN_FOUND:
      ||    Serial.println(F("EV_SCAN_FOUND"));
      ||    break;
    */
    case EV_TXSTART:
      Serial.println(F("EV_TXSTART"));
      break;
    case EV_TXCANCELED:
      Serial.println(F("EV_TXCANCELED"));
      break;
    case EV_RXSTART:
      /* do not print anything -- it wrecks timing */
      break;
    case EV_JOIN_TXCOMPLETE:
      Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
      break;
    default:
      Serial.print(F("Unknown event: "));
      Serial.println((unsigned) ev);
      break;
  }
}

void do_send(osjob_t* j) {
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.println(F("OP_TXRXPEND, not sending"));
  } else {
    RG_15_data();
    RG_15_Transmit();

    // Prepare upstream data transmission at the next possible time.
    LMIC_setTxData2(2, mydata, sizeof(mydata), 0);
    Serial.println(F("Packet queued"));
  }
  // Next TX is scheduled after TX_COMPLETE event.
}

void setup() {

  Serial.begin(115200);
  delay(100);     // per sample code on RF_95 test

  unoSerial.begin(9600);
  delay(1000);
  /*Device_Reset();
    Poll();
    RG_15_data();*/
  Set_BaudRate(9600);
  Reset_Accummulation();
  Poll();
  RG_15_data();
#ifdef DEBUG
  Serial.println(F("Starting"));
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
  LMIC_setSession (0x13, DEVADDR, nwkskey, appskey);
#else
  // If not running an AVR with PROGMEM, just use the arrays directly
  LMIC_setSession (0x13, DEVADDR, NWKSKEY, APPSKEY);
#endif

#if defined(CFG_in866)
  // Set up the channels used in your country. Three are defined by default,
  // and they cannot be changed. Duty cycle doesn't matter, but is conventionally
  // BAND_MILLI.
  LMIC_setupChannel(0, 865062500, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
  LMIC_setupChannel(1, 865402500, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
  LMIC_setupChannel(2, 865985000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
  //  LMIC_setupChannel(3, 865232500, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
  //  LMIC_setupChannel(4, 866185000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
  //  LMIC_setupChannel(5, 866385000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
  //  LMIC_setupChannel(6, 866585000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
  //  LMIC_setupChannel(7, 866785000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
  // ... extra definitions for channels 3..n here.
#else
# error Region not supported
#endif

  // Disable link check validation
  LMIC_setLinkCheckMode(0);

  // TTN uses SF9 for its RX2 window.
  LMIC.dn2Dr = DR_SF12;

  // Set data rate and transmit power for uplink
  LMIC_setDrTxpow(DR_SF12, 14);

  // Start job
  do_send(&sendjob);
}

void loop() {

  current_time = millis();
  if (next == true)
  {
    os_runloop_once();
    previous_time = current_time;
  }
  else
  {

    if (previous_time > current_time)
      previous_time = current_time;

    if (previous_day_time > current_time)
      previous_day_time = current_time;

    if (current_time - previous_day_time >= 86400000)
    {
      Reset_Accummulation();
      Poll();
      RG_15_data();
      previous_day_time = current_time;
    }

    if ((current_time - previous_time) >= (TX_INTERVAL * 1000))
    {

      next = true;
      do_send(&sendjob);

    }
    else
    {
      RG_15_data();
    }
  }
}


void RG_15_Transmit()
{


  mydata[0] = Accumulation;
  mydata[1] = highByte(Event_accumulation);
  mydata[2] = lowByte(Event_accumulation);
  mydata[3] = highByte(Total_accumulation);
  mydata[4] = lowByte(Total_accumulation);
  mydata[5] = highByte(IPH);
  mydata[6] = lowByte(IPH);
#ifdef DEBUG
  Serial.print("Accumulation= ");
  Serial.println(Accumulation);
  Serial.print("Event_accumulation= ");
  Serial.println(Event_accumulation);
  Serial.print("Total_accumulation= ");
  Serial.println(Total_accumulation);
  Serial.print("IPH= ");
  Serial.println(IPH);
#endif

  Accumulation = 0;
}

void RG_15_data()
{
  unoSerial.begin(9600);
  delay(1000);

  if (unoSerial.available() > 0)
  {
    String response = unoSerial.readStringUntil('\n');
  Serial.println(response);
  if (response.startsWith("Acc"))
  {
    //slicing string
    char Acc[7], EventAcc[7], TotalAcc[7], RInt[7], unit[4];
    sscanf (response.c_str(), "%*s %s %[^,] , %*s %s %*s %*s %s %*s %*s %s", &Acc, &unit, &EventAcc, &TotalAcc, &RInt);

    //converting char array to float
    float float_Acc = atof(Acc);
    float float_EventAcc = atof(EventAcc);
    float float_TotalAcc = atof(TotalAcc);
    float float_RInt = atof(RInt);

    }
    unoSerial.flush();
    delay(100);
    unoSerial.end();
    delay(100);
    return;
  }
  else
    unoSerial.flush();
    delay(100);
    unoSerial.end();
    delay(100);
    return;

}

void Reset_Accummulation()
{
  unoSerial.begin(9600);
  delay(1000);
  unoSerial.write('o');
  unoSerial.write('\n');
  unoSerial.flush();
}

void Set_BaudRate(int baudRate)
{
  unoSerial.begin(9600);
  delay(1000);
  unoSerial.write('b');
  unoSerial.write(baudRate);
  unoSerial.write('\n');
  unoSerial.flush();
}

void Device_Reset()
{
  unoSerial.begin(9600);
  delay(1000);
  unoSerial.write('k');
  unoSerial.write('\n');
  unoSerial.flush();
}

void Poll()
{
  unoSerial.begin(9600);
  delay(1000);
  unoSerial.write('r');
  unoSerial.write('\n');
  unoSerial.flush();
}
