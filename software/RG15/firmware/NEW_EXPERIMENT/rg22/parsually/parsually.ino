

#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>

bool call();
#include <SoftwareSerial.h>
unsigned long previous_time = 0;
unsigned long previous_day_time = 0;
unsigned long current_time = 0;
#include "Stream.h"
SoftwareSerial unoSerial(7, 8); // RX, TX
int previous_accumulation = 0;
int i = 0; int j = 0;
//enum Unit {
//  in, mm,
//  iph, mmph,
//  tips
//};

struct
{
  int Accumulation;
  int Event_Accumulation;
  int Total_Accumulation;
  int Rain_Intensity;
  int Device_status ;
} mydata;



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


static osjob_t sendjob;

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 60;

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
    case EV_SCAN_TIMEOUT:
      //            Serial.println(F("EV_SCAN_TIMEOUT"));
      break;
    case EV_BEACON_FOUND:
      //            Serial.println(F("EV_BEACON_FOUND"));
      break;
    case EV_BEACON_MISSED:
      //            Serial.println(F("EV_BEACON_MISSED"));
      break;
    case EV_BEACON_TRACKED:
      //            Serial.println(F("EV_BEACON_TRACKED"));
      break;
    case EV_JOINING:
      //            Serial.println(F("EV_JOINING"));
      break;
    case EV_JOINED:
      //            Serial.println(F("EV_JOINED"));
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
      //            Serial.println(F("EV_JOIN_FAILED"));
      break;
    case EV_REJOIN_FAILED:
      //            Serial.println(F("EV_REJOIN_FAILED"));
      break;
    case EV_TXCOMPLETE:
      Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
      if (LMIC.txrxFlags & TXRX_ACK)
        //              Serial.println(F("Received ack"));
        if (LMIC.dataLen) {
          //              Serial.println(F("Received "));
          //              Serial.println(LMIC.dataLen);
          //              Serial.println(F(" bytes of payload"));
        }
      // Schedule next transmission
      os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
      break;
    case EV_LOST_TSYNC:
      //            Serial.println(F("EV_LOST_TSYNC"));
      break;
    case EV_RESET:
      //            Serial.println(F("EV_RESET"));
      break;
    case EV_RXCOMPLETE:
      // data received in ping slot
      //            Serial.println(F("EV_RXCOMPLETE"));
      break;
    case EV_LINK_DEAD:
      //            Serial.println(F("EV_LINK_DEAD"));
      break;
    case EV_LINK_ALIVE:
      //            Serial.println(F("EV_LINK_ALIVE"));
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
      //            Serial.println(F("EV_TXSTART"));
      break;
    case EV_TXCANCELED:
      //            Serial.println(F("EV_TXCANCELED"));
      break;
    case EV_RXSTART:
      /* do not print anything -- it wrecks timing */
      break;
    case EV_JOIN_TXCOMPLETE:
      //            Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
      break;
    default:
      //            Serial.print(F("Unknown event: "));
      //            Serial.println((unsigned) ev);
      break;
  }
}

void do_send(osjob_t* j) {
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.println(F("OP_TXRXPEND, not sending"));
  } else {


    while (call() == false)
    {
      call();
      j = j++;
      if (j > 5)
        break;
    }
    j = 0;
    //    Serial.print("mydata.Accumulation = ");
     Serial.println( mydata.Accumulation );
  //    Serial.print(" mydata.Event_Accumulation = ");
      Serial.println( mydata.Event_Accumulation);
  //    Serial.print("mydata.Total_Accumulation = ");
     Serial.println(mydata.Total_Accumulation);
  
     Serial.println(mydata.Rain_Intensity);
     Serial.print(mydata.Device_status);
  //    previous_day_time = current_time;

    // Prepare upstream data transmission at the next possible time.
    LMIC_setTxData2(1, (unsigned char *)&mydata, sizeof(mydata) - 1, 0);
    Serial.println(F("Packet queued"));
    Serial.println("sizeof(mydata) : ");
    Serial.println(sizeof(mydata));
  }
  // Next TX is scheduled after TX_COMPLETE event.
}





void setup() {
  //    pinMode(13, OUTPUT);
  while (!Serial); // wait for Serial to be initialized
  Serial.begin(9600);
  Serial.println("setup");
  unoSerial.begin(9600);
  // rg15.setStream(&mySerial);
  Serial.println(F("Starting"));

#ifdef VCC_ENABLE
  // For Pinoccio Scout boards

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
  LMIC_setSession (0x13, DEVADDR, nwkskey, appskey);
#else
  // If not running an AVR with PROGMEM, just use the arrays directly
  LMIC_setSession (0x13, DEVADDR, NWKSKEY, APPSKEY);
#endif

#if defined(CFG_eu868)
  // Set up the channels used by the Things Network, which corresponds
  // to the defaults of most gateways. Without this, only three base
  // channels from the LoRaWAN specification are used, which certainly
  // works, so it is good for debugging, but can overload those
  // frequencies, so be sure to configure the full frequency range of
  // your network here (unless your network autoconfigures them).
  // Setting up channels should happen after LMIC_setSession, as that
  // configures the minimal channel set. The LMIC doesn't let you change
  // the three basic settings, but we show them here.
  //    LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  //    LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI);      // g-band
  //    LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  //    LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  //    LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  //    LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  //    LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  //    LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  //    LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK,  DR_FSK),  BAND_MILLI);      // g2-band
  // TTN defines an additional channel at 869.525Mhz using SF9 for class B
  // devices' ping slots. LMIC does not have an easy way to define set this
  // frequency and support for class B is spotty and untested, so this
  // frequency is not configured here.
#elif defined(CFG_us915) || defined(CFG_au915)
  // NA-US and AU channels 0-71 are configured automatically
  // but only one group of 8 should (a subband) should be active
  // TTN recommends the second sub band, 1 in a zero based count.
  // https://github.com/TheThingsNetwork/gateway-conf/blob/master/US-global_conf.json
  LMIC_selectSubBand(1);
#elif defined(CFG_as923)
  // Set up the channels used in your country. Only two are defined by default,
  // and they cannot be changed.  Use BAND_CENTI to indicate 1% duty cycle.
  // LMIC_setupChannel(0, 923200000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);
  // LMIC_setupChannel(1, 923400000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);

  // ... extra definitions for channels 2..n here
#elif defined(CFG_kr920)
  // Set up the channels used in your country. Three are defined by default,
  // and they cannot be changed. Duty cycle doesn't matter, but is conventionally
  // BAND_MILLI.
  // LMIC_setupChannel(0, 922100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
  // LMIC_setupChannel(1, 922300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
  // LMIC_setupChannel(2, 922500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);

  // ... extra definitions for channels 3..n here.
#elif defined(CFG_in866)
  // Set up the channels used in your country. Three are defined by default,
  // and they cannot be changed. Duty cycle doesn't matter, but is conventionally
  // BAND_MILLI.
  LMIC_setupChannel(0, 865062500, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
  LMIC_setupChannel(1, 865402500, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
  LMIC_setupChannel(2, 865985000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);

  // ... extra definitions for channels 3..n here.
#else
# error Region not supported
#endif

  // Disable link check validation
  LMIC_setLinkCheckMode(0);

  // TTN uses SF9 for its RX2 window.
  LMIC.dn2Dr = DR_SF9;

  // Set data rate and transmit power for uplink
  LMIC_setDrTxpow(DR_SF7, 14);

  // Start job
  do_send(&sendjob);
}

void loop() {

  //  current_time = millis();
  //  if (current_time - previous_day_time >= 60000)
  //  {
  //    call();
  //    Serial.print("mydata.Accumulation = ");
  //    Serial.println( mydata.Accumulation );
  //    Serial.print(" mydata.Event_Accumulation = ");
  //    Serial.println( mydata.Event_Accumulation);
  //    Serial.print("mydata.Total_Accumulation = ");
  //    Serial.println(mydata.Total_Accumulation);
  //    Serial.print("mydata.Rain_Intensity = ");
  //    Serial.println(mydata.Rain_Intensity);
  //    previous_day_time = current_time;
  //  }
  os_runloop_once();

}

bool call() {
  unoSerial.write('r');
  unoSerial.write('\n');
  String response =  unoSerial.readStringUntil('\n');
  delay(1000);
  if (response.startsWith(F("Acc"))) {

    char accB[7], eventAccB[7], totalAccB[7], rIntB[7], unit[7];

    sscanf (response.c_str(), "%*s %s %[^,] , %*s %s %*s %*s %s %*s %*s %s", &accB, &unit, &eventAccB, &totalAccB, &rIntB);

    float acc = atof(accB);
    float eventAcc = atof(eventAccB);
    float totalAcc = atof(totalAccB);
    float rInt = atof(rIntB);

    delay(1000);




    int Accumulation1 = int(acc * 100);
    int Event_Accumulation1 = int(eventAcc * 100);
    int Total_Accumulation1 = int(totalAcc * 100);
    int Rain_Intensity1 = int(rInt * 100);
    
    delay(1000);
    
    if (previous_accumulation != Event_Accumulation1)
    {
      i = 0;
      mydata.Accumulation = Accumulation1;
      mydata.Event_Accumulation = Event_Accumulation1;
      mydata.Total_Accumulation = Total_Accumulation1;
      mydata.Rain_Intensity = Rain_Intensity1;
      mydata.Device_status =1;
      delay(1000);
    }
    else
    {


      mydata.Accumulation = (Accumulation1 * 0);
      mydata.Event_Accumulation = (Event_Accumulation1 * 0);
      mydata.Total_Accumulation = (Total_Accumulation1 * 0);
      mydata.Rain_Intensity = (Rain_Intensity1 * 0);
      mydata.Device_status =1;
      delay(1000);
    }
    previous_accumulation = Event_Accumulation1;
    Serial.println("previous_accumulation");
    Serial.println(previous_accumulation);
    return true;
  }

  else {
    Serial.println("Timeout!");
    i++;
    mydata.Accumulation = 0;
    mydata.Event_Accumulation = 0;
    mydata.Total_Accumulation = 0;
    mydata.Rain_Intensity = 0;
    mydata.Device_status = 0;
    delay(1000);
//    if (i > 5)
//    {
//      unoSerial.write("/0");
//      delay(1000);
//    }
    return false;
  }



  delay(1000);
}
