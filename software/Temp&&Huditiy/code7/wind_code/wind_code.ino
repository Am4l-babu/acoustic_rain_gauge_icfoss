
#include <GyverBME280.h>
GyverBME280 bme;
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <EnableInterrupt.h>

#define reed_pin 10
#define reed_pin1 8
#define pot_pin A0
#define pot_pin1 A1


unsigned long previousTime = 0;
volatile uint16_t count = 0;
volatile uint16_t count1 = 0;

float wind_speed = 0;
float wind_speed1 = 0;

float dir_Value = 0;
float dir_Value1 = 0;

float volt;

//struct {
//int Temperature;
//int Humidity;
//int Pressure;
//  
//} mydata;

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
static const PROGMEM u1_t NWKSKEY[16] = { 0x0C, 0x05, 0x45, 0xFD, 0x1C, 0x09, 0x5C, 0x62, 0x86, 0xED, 0x53, 0xF0, 0x2F, 0x1B, 0x86, 0x99  };

// LoRaWAN AppSKey, application session key
// This should also be in big-endian (aka msb).
static const u1_t PROGMEM APPSKEY[16] = { 0x50, 0xFC, 0xD9, 0xA4, 0xF1, 0x2C, 0xFD, 0x90, 0x11, 0x54, 0x62, 0xD4, 0xEF, 0x9F, 0x4F, 0xCE };

// LoRaWAN end-device address (DevAddr)
// See http://thethingsnetwork.org/wiki/AddressSpace
// The library converts the address to network byte order as needed, so this should be in big-endian (aka msb) too.
static const u4_t DEVADDR = 0xfc00976b ; // <-- Change this address for every node!

// These callbacks are only used in over-the-air activation, so they are
// left empty here (we cannot leave them out completely unless
// DISABLE_JOIN is set in arduino-lmic/project_config/lmic_project_config.h,
// otherwise the linker will complain).
void os_getArtEui (u1_t* buf) { }
void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }

//static uint8_t mydata[] = "Hello, world!";
static uint8_t mydata[10];

static osjob_t sendjob;

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 300;

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
      os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
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
    wind_data();
    voltage();
    // Prepare upstream data transmission at the next possible time.
    LMIC_setTxData2(1, mydata, sizeof(mydata), 0);
    Serial.println(F("Packet queued"));


    count = 0;
    count1 = 0;
    wind_speed = 0;
    wind_speed1 = 0;


  }
  // Next TX is scheduled after TX_COMPLETE event.
}




void setup() {
  pinMode(reed_pin, INPUT_PULLUP);
  pinMode(reed_pin1, INPUT_PULLUP);

  enableInterrupt(reed_pin, wind_count, FALLING);
  enableInterrupt(reed_pin1, wind_count1, FALLING);


  while (!Serial); // wait for Serial to be initialized
  Serial.begin(115200);
  delay(100);     // per sample code on RF_95 test
  Serial.println(F("Starting"));

#ifdef VCC_ENABLE
  // For Pinoccio Scout boards
  pinMode(VCC_ENABLE, OUTPUT);
  digitalWrite(VCC_ENABLE, HIGH);
  delay(1000);
#endif
  bme.setFilter(FILTER_COEF_8);
  bme.setTempOversampling(OVERSAMPLING_8);
  bme.setPressOversampling(OVERSAMPLING_16);
  bme.setStandbyTime(STANDBY_500MS);
  bme.begin();
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

  // ... extra definitions for channels 3..n here.
#if defined(CFG_in866)
  // Set up the channels used in your country. Three are defined by default,
  // and they cannot be changed. Duty cycle doesn't matter, but is conventionally
  // BAND_MILLI.
  LMIC_setupChannel(0, 865062500, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
  LMIC_setupChannel(1, 865232500, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
  LMIC_setupChannel(2, 865402500, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
  LMIC_setupChannel(3, 865985000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
  LMIC_setupChannel(4, 866185000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
  LMIC_setupChannel(5, 866385000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
  LMIC_setupChannel(6, 866585000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
  LMIC_setupChannel(7, 866785000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);


  // ... extra definitions for channels 3..n here.
#else
# error Region not supported
#endif
  LMIC_setLinkCheckMode(0);
  LMIC.dn2Dr = DR_SF9;

  // Set data rate and transmit power for uplink
  LMIC_setDrTxpow(DR_SF12, 14);

  // Start job
  do_send(&sendjob);
}

void loop() {
  os_runloop_once();
}


// When is this function called ?

void wind_data()
{
  enableInterrupt(reed_pin, wind_count, FALLING);
  enableInterrupt(reed_pin1, wind_count1, FALLING);
  int dir_Value = analogRead(A0);
  int dir_Value1 = analogRead(A1);
  mydata[0] = highByte(dir_Value);
  mydata[1] = lowByte(dir_Value);
  mydata[2] = highByte((int)(wind_speed * 100));
  mydata[3] = lowByte((int)(wind_speed * 100));
  mydata[4] = highByte(dir_Value1);
  mydata[5] = lowByte(dir_Value1);
  mydata[6] = highByte((int)(wind_speed1 * 100));
  mydata[7] = lowByte((int)(wind_speed1 * 100));
  bme.readTemperature();
  bme.readHumidity();
  pressureToMmHg(bme.readPressure());
//  Serial.print(dir_Value);
//  Serial.print(",");
//  Serial.print(dir_Value1);
//  Serial.print(" | ");
//  Serial.print(wind_speed);
//  Serial.print(",");
//  Serial.print(wind_speed1);
//  Serial.print(".........");
//  Serial.println(count);
  count = 0;
  count1 = 0;
  wind_speed = 0;
  wind_speed1 = 0;
}




// function which takes the wind count from sparkfun mechanical anemometer.
void wind_count()
{
  count++;
  wind_speed = count * 1.609 * (2.4 / 300);  // for mechanical anemometer (2.4), *tion with 1.609 gives mph to kmph.
}



// function which takes the wind count from Davis non-mechanical anemometer.
void wind_count1()
{
  count1++;
  wind_speed1 = count1 * 1.609 * (2.25 / 300);  // for non-mechanical anemometer (2.25)*tion with 1.609 gives mph to kmph.
}





void voltage()
{
  double analogvalue = analogRead(A3);
  double bat_temp = ((analogvalue * 3.3) / 1024); //ADC voltage*Ref. Voltage/1024
  double sum = 0;
  double avg = 0;
  short int volt = 0;
  for (byte  i = 0; i < 4; i++)
  {
    sum += bat_temp * 2;
  }
  avg = (sum / 4);
  volt = avg * 1000;
  Serial.print(avg);
  Serial.println(F(" V avg"));
  Serial.println(F("-------------"));
  Serial.print(F(" Battery Voltage: "));
  Serial.print(volt);
  Serial.println(F(" V"));
  Serial.println(F("-------------"));
  mydata[8] = highByte(volt);
  mydata[9] = lowByte(volt);
}
