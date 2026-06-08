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
//#include <Arduino.h>
#include <Wire.h>
#include <EnableInterrupt.h>

#define reed_pin 10
#define pot_pin A0

unsigned long previousTime = 0;
volatile uint16_t count = 0;
float wind_speed = 0;
float dir_Value = 0;




#define NORMAL_MODE  0x03
#define FORCED_MODE 0x02
#define STANDBY_500US  0x00
#define STANDBY_10MS   0x06
#define STANDBY_20MS   0x07
#define STANDBY_6250US 0x01
#define STANDBY_125MS  0x02
#define STANDBY_250MS  0x03
#define STANDBY_500MS  0x04
#define STANDBY_1000MS 0x05


#define OVERSAMPLING_1        0x01
#define OVERSAMPLING_2        0x02
#define OVERSAMPLING_4        0x03
#define OVERSAMPLING_8        0x04
#define OVERSAMPLING_16       0x05


#define FILTER_COEF_2   0x01
#define FILTER_COEF_4   0x02
#define FILTER_COEF_8   0x03
#define FILTER_COEF_16  0x04


// ================================= CLASS ===================================

class GyverBME280 {

public:
    GyverBME280();                // Create an object of class BME280
    bool begin(void);             // Initialize sensor with standart 0x76 address 
    bool begin(uint8_t address);        // Initialize sensor with not standart 0x76 address 
    bool isMeasuring(void);           // Returns 'true' while the measurement is in progress          
    float readPressure(void);         // Read and calculate atmospheric pressure [float , Pa]
    float readHumidity(void);         // Read and calculate air humidity [float , %]
    void oneMeasurement(void);          // Make one measurement and go back to sleep [FORCED_MODE only]
    void setMode(uint8_t mode);
    float readTemperature(void);        // Read and calculate air temperature [float , *C]
    void setFilter(uint8_t mode);       // Adjust the filter ratio other than the standard one [before begin()]
    void setStandbyTime(uint8_t mode);      // Adjust the sleep time between measurements [NORMAL_MODE only][before begin()]
   // void setHumOversampling(uint8_t mode);    // Set oversampling or disable humidity module [before begin()]
   // void setTempOversampling(uint8_t mode);   // Set oversampling or disable temperature module [before begin()]
    //void setPressOversampling(uint8_t mode);  // Set oversampling or disable pressure module [before begin()]

private:

    //============================== DEFAULT SETTINGS ========================================|
    int _i2c_address = 0x76;          // BME280 address on I2C bus                  |
    uint8_t _operating_mode = NORMAL_MODE;    // Sensor operation mode                      |
    uint8_t _standby_time = STANDBY_250MS;    // Time between measurements in NORMAL_MODE   |
    uint8_t _filter_coef = FILTER_COEF_16;    // Filter ratio IIR                           |
    uint8_t _temp_oversampl = OVERSAMPLING_4; // Temperature module oversampling parameter  |
    uint8_t _hum_oversampl = OVERSAMPLING_1;  // Humidity module oversampling parameter     |
    uint8_t _press_oversampl = OVERSAMPLING_2;  // Pressure module oversampling parameter     |
    //========================================================================================|
    
    bool reset(void);                                     // BME280 software reset 
    int32_t readTempInt();                                // Temperature reading in integers for the function of reading
    void readCalibrationData(void);                       // Read all cells containing calibration values
    uint8_t readRegister(uint8_t address);          // Read one 8-bit BME280 register
    uint32_t readRegister24(uint8_t address);           // Read and combine three BME280 registers
    bool writeRegister(uint8_t address , uint8_t data);   // Write one 8-bit BME280 register

    struct {                        // Structure to store all calibration values
        uint16_t _T1;
        int16_t _T2;
        int16_t _T3;
        uint16_t _P1;
        int16_t _P2;
        int16_t _P3;
        int16_t _P4;
        int16_t _P5;
        int16_t _P6;
        int16_t _P7;
        int16_t _P8;
        int16_t _P9;
        uint8_t _H1;
        int16_t _H2;
        uint8_t _H3;
        int16_t _H4;
        int16_t _H5;
        int8_t _H6;
    } CalibrationData;
};

float pressureToMmHg(float pressure);   // Convert [Pa] to [mm Hg]  
float pressureToAltitude(float pressure); // Convert pressure to altitude



float pressureToAltitude(float pressure) {
    if (!pressure) return 0;                  // If the pressure module has been disabled return '0'
    pressure /= 100.0F;                     // Convert [Pa] to [hPa]
    return 44330.0 * (1.0 - pow(pressure / 1013.25, 0.1903)); // Сalculate altitude
}

float pressureToMmHg(float pressure) {              
    return (float)(pressure * 0.00750061683f);          // Convert [Pa] to [mm Hg]
}

/* ============ Setup & begin ============ */

bool GyverBME280::begin(void) {
    return begin(0x76);
}

bool GyverBME280::begin(uint8_t address) {  
    _i2c_address = address;
    /* === Start I2C bus & check BME280 === */
    Wire.begin();                                 // Start I2C bus 
    if (!reset()) return false;                     // BME280 software reset & ack check
    uint8_t ID = readRegister(0xD0);
    if (ID != 0x60 && ID != 0x58) return false;     // Check chip ID (bme/bmp280)
    readCalibrationData();                // Read all calibration values

    /* === Load settings to BME280 === */
    writeRegister(0xF2, _hum_oversampl);                                                    // write hum oversampling value
    writeRegister(0xF2, readRegister(0xF2));                                  // rewrite hum oversampling register
    writeRegister(0xF4, ((_temp_oversampl << 5) | (_press_oversampl << 2) | _operating_mode));    // write temp & press oversampling value , normal mode
    writeRegister(0xF5, ((_standby_time << 5) | (_filter_coef << 2)));                      // write standby time & filter coef
    return true;
}

void GyverBME280::setMode(uint8_t mode) {
    _operating_mode = mode;
}


void GyverBME280::setFilter(uint8_t mode) {
    _filter_coef = mode;
}

void GyverBME280::setStandbyTime(uint8_t mode) {
    _standby_time = mode;
}

/*void GyverBME280::setHumOversampling(uint8_t mode) {
    _hum_oversampl = mode;
}

void GyverBME280::setTempOversampling(uint8_t mode) {
    _temp_oversampl = mode;
}

void GyverBME280::setPressOversampling(uint8_t mode) {
    _press_oversampl = mode;
} */

/* ============ Reading ============ */

int32_t GyverBME280::readTempInt(void) {
    int32_t temp_raw = readRegister24(0xFA);                  // Read 24-bit value
    if (temp_raw == 0x800000) return 0;               // If the temperature module has been disabled return '0'

    temp_raw >>= 4;                         // Start temperature reading in integers
    int32_t value_1 = ((((temp_raw >> 3) - ((int32_t)CalibrationData._T1 << 1))) *
    ((int32_t)CalibrationData._T2)) >> 11;
    int32_t value_2 = (((((temp_raw >> 4) - ((int32_t)CalibrationData._T1)) * 
    ((temp_raw >> 4) - ((int32_t)CalibrationData._T1))) >> 12) * ((int32_t)CalibrationData._T3)) >> 14;

    return ((int32_t)value_1 + value_2);              // Return temperature in integers
}


float GyverBME280::readTemperature(void) {
    int32_t temp_raw = readTempInt();
    float T = (temp_raw * 5 + 128) >> 8;
    return T / 100.0;                       // Return temperature in float
}


float GyverBME280::readPressure(void) {
    uint32_t press_raw = readRegister24(0xF7);                  // Read 24-bit value
    if (press_raw == 0x800000) return 0;              // If the pressure module has been disabled return '0'

    press_raw >>= 4;                        // Start pressure converting
    int64_t value_1 = ((int64_t)readTempInt()) - 128000;
    int64_t value_2 = value_1 * value_1 * (int64_t)CalibrationData._P6;
    value_2 = value_2 + ((value_1 * (int64_t)CalibrationData._P5) << 17);
    value_2 = value_2 + (((int64_t)CalibrationData._P4) << 35);
    value_1 = ((value_1 * value_1 * (int64_t)CalibrationData._P3) >> 8) + ((value_1 * (int64_t)CalibrationData._P2) << 12);
    value_1 = (((((int64_t)1) << 47) + value_1)) * ((int64_t)CalibrationData._P1) >> 33;

    if (!value_1) return 0;                     // Avoid division by zero

    int64_t p = 1048576 - press_raw;
    p = (((p << 31) - value_2) * 3125) / value_1;
    value_1 = (((int64_t)CalibrationData._P9) * (p >> 13) * (p >> 13)) >> 25;
    value_2 = (((int64_t)CalibrationData._P8) * p) >> 19;
    p = ((p + value_1 + value_2) >> 8) + (((int64_t)CalibrationData._P7) << 4);

    return (float)p / 256;                      // Return pressure in float       
}


float GyverBME280::readHumidity(void) {
    Wire.beginTransmission(_i2c_address);                 // Start I2C transmission
    Wire.write(0xFD);                             // Request humidity data register 
    if (Wire.endTransmission() != 0) return 0;                    
    Wire.requestFrom(_i2c_address, 2);                    // Request humidity data 
    int32_t hum_raw = ((uint16_t)Wire.read() << 8) | (uint16_t)Wire.read(); // Read humidity data 
    if (hum_raw == 0x8000) return 0;                    // If the humidity module has been disabled return '0'

    int32_t value  = (readTempInt() - ((int32_t)76800));                // Start humidity converting
    value = (((((hum_raw << 14) - (((int32_t)CalibrationData._H4) << 20) -
    (((int32_t)CalibrationData._H5) * value)) +((int32_t)16384)) >> 15) * 
    (((((((value * ((int32_t)CalibrationData._H6)) >> 10) *(((value * 
    ((int32_t)CalibrationData._H3)) >> 11) + ((int32_t)32768))) >> 10) +
    ((int32_t)2097152)) * ((int32_t)CalibrationData._H2) + 8192) >> 14));
    value = (value - (((((value >> 15) * (value >> 15)) >> 7) * ((int32_t)CalibrationData._H1)) >> 4));
    value = (value < 0) ? 0 : value;
    value = (value > 419430400) ? 419430400 : value;
    float h = (value >> 12);

    return h / 1024.0;                      // Return humidity in float
}

/* ============ Misc ============ */

bool GyverBME280::isMeasuring(void) {               
    return (bool)((readRegister(0xF3) & 0x08) >> 3);            // Read status register & mask bit "measuring"
}

void GyverBME280::oneMeasurement(void) {
    writeRegister(0xF4 , ((readRegister(0xF4) & 0xFC) | 0x02)); // Set the operating mode to FORCED_MODE
}

GyverBME280::GyverBME280() {}

/* ============ Private ============ */

/* = BME280 software reset = */
bool GyverBME280::reset(void) {
    if (!writeRegister(0x0E , 0xB6)) return false; 
    delay(10);
    return true;
}


/* = Read and combine three BME280 registers = */
uint32_t GyverBME280::readRegister24(uint8_t address) {  
    Wire.beginTransmission(_i2c_address);
    Wire.write(address);
    if (Wire.endTransmission() != 0) return 0x800000;
    Wire.requestFrom(_i2c_address, 3);
    return (((uint32_t)Wire.read() << 16) | ((uint32_t)Wire.read() << 8) | (uint32_t)Wire.read());
}


/* = Write one 8-bit BME280 register = */
bool GyverBME280::writeRegister(uint8_t address , uint8_t data) {
    Wire.beginTransmission(_i2c_address);
    Wire.write(address);
    Wire.write(data);
    if (Wire.endTransmission() != 0) return false;
    return true;
}


/* = Read one 8-bit BME280 register = */
uint8_t GyverBME280::readRegister(uint8_t address) {
    Wire.beginTransmission(_i2c_address);
    Wire.write(address);
    if (Wire.endTransmission() != 0) return 0;
    Wire.requestFrom(_i2c_address , 1);
    return Wire.read();
}

/* = Structure to store all calibration values = */
void GyverBME280::readCalibrationData(void) {
    /* first part request*/
    Wire.beginTransmission(_i2c_address);
    Wire.write(0x88);
    if (Wire.endTransmission() != 0) return;
    Wire.requestFrom(_i2c_address , 25);
    /* reading */
    CalibrationData._T1 = (Wire.read() | (Wire.read() << 8));
    CalibrationData._T2 = (Wire.read() | (Wire.read() << 8));
    CalibrationData._T3 = (Wire.read() | (Wire.read() << 8));
    CalibrationData._P1 = (Wire.read() | (Wire.read() << 8));
    CalibrationData._P2 = (Wire.read() | (Wire.read() << 8));
    CalibrationData._P3 = (Wire.read() | (Wire.read() << 8));
    CalibrationData._P4 = (Wire.read() | (Wire.read() << 8));
    CalibrationData._P5 = (Wire.read() | (Wire.read() << 8));
    CalibrationData._P6 = (Wire.read() | (Wire.read() << 8));
    CalibrationData._P7 = (Wire.read() | (Wire.read() << 8));
    CalibrationData._P8 = (Wire.read() | (Wire.read() << 8));
    CalibrationData._P9 = (Wire.read() | (Wire.read() << 8));
    CalibrationData._H1 = Wire.read();

    /* second part request*/
    Wire.beginTransmission(_i2c_address);
    Wire.write(0xE1);
    Wire.endTransmission();
    Wire.requestFrom(_i2c_address , 8);
    /* reading */
    CalibrationData._H2 = (Wire.read() | (Wire.read() << 8));
    CalibrationData._H3 = Wire.read();
    CalibrationData._H4 = (Wire.read() << 4);
    uint8_t interVal = Wire.read();
    CalibrationData._H4 |= (interVal & 0xF);
    CalibrationData._H5 = (((interVal & 0xF0) >> 4) | (Wire.read() << 4));
    CalibrationData._H6 = Wire.read();
}
GyverBME280 bme;
//
// For normal use, we require that you edit the sketch to replace FILLMEIN
// with values assigned by the TTN console. However, for regression tests,
// we want to be able to compile these scripts. The regression tests define
// COMPILE_REGRESSION_TEST, and in that case we define FILLMEIN to a non-
// working but innocuous value.
//
#ifdef COMPILE_REGRESSION_TEST
# define CFG_in866
#else
# warning "You must replace the values marked FILLMEIN with real values from the TTN control panel!"
# define FILLMEIN (#dont edit this, edit the lines that use FILLMEIN)
#endif

// LoRaWAN NwkSKey, network session key
// This should be in big-endian (aka msb).
static const PROGMEM u1_t NWKSKEY[16] = { 0x05, 0x88, 0x28, 0x5B, 0xF7, 0xD9, 0x14, 0x3A, 0x55, 0xA7, 0xE9, 0x66, 0xFD, 0x2F, 0x25, 0x14  };

// LoRaWAN AppSKey, application session key
// This should also be in big-endian (aka msb).
static const u1_t PROGMEM APPSKEY[16] = { 0x8F, 0x88, 0x1A, 0x73, 0xA9, 0xB6, 0xF2, 0x3E, 0x69, 0xAC, 0xD1, 0x05, 0xA9, 0x51, 0x1C, 0xAD};

// LoRaWAN end-device address (DevAddr)
// See http://thethingsnetwork.org/wiki/AddressSpace
// The library converts the address to network byte order as needed, so this should be in big-endian (aka msb) too.
static const u4_t DEVADDR = 0Xfc0096a7; // <-- Change this address for every node!

// These callbacks are only used in over-the-air activation, so they are
// left empty here (we cannot leave them out completely unless
// DISABLE_JOIN is set in arduino-lmic/project_config/lmic_project_config.h,
// otherwise the linker will complain).
void os_getArtEui (u1_t* buf) { }
void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }

static uint8_t mydata[6];
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
  .dio = {2, 3, 4}, // assumes external jumpers [feather_lora_jumper]
  // DIO1 is on JP1-1: is io1 - we connect to GPO6
  // DIO1 is on JP5-3: is D2 - we connect to GPO5
};




void onEvent (ev_t ev) {
  Serial.print(os_getTime());
  Serial.print(": ");
  switch (ev) {
    case EV_SCAN_TIMEOUT:
      //Serial.println(F("EV_SCAN_TIMEOUT"));
      break;
    case EV_BEACON_FOUND:
      //Serial.println(F("EV_BEACON_FOUND"));
      break;
    case EV_BEACON_MISSED:
      //Serial.println(F("EV_BEACON_MISSED"));
      break;
    case EV_BEACON_TRACKED:
     // Serial.println(F("EV_BEACON_TRACKED"));
      break;
    case EV_JOINING:
     // Serial.println(F("EV_JOINING"));
      break;
    case EV_JOINED:
     // Serial.println(F("EV_JOINED"));
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
     // Serial.println(F("EV_JOIN_FAILED"));
      break;
    case EV_REJOIN_FAILED:
      //Serial.println(F("EV_REJOIN_FAILED"));
      break;
    case EV_TXCOMPLETE:
      //Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
      if (LMIC.txrxFlags & TXRX_ACK)
      //  Serial.println(F("Received ack"));
      if (LMIC.dataLen) {
        //Serial.println(F("Received "));
       // Serial.println(LMIC.dataLen);
       // Serial.println(F(" bytes of payload"));
      }
      // Schedule next transmission
      os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
      break;
    case EV_LOST_TSYNC:
      //Serial.println(F("EV_LOST_TSYNC"));
      break;
    case EV_RESET:
     // Serial.println(F("EV_RESET"));
      break;
    case EV_RXCOMPLETE:
      // data received in ping slot
     // Serial.println(F("EV_RXCOMPLETE"));
      break;
    case EV_LINK_DEAD:
    //  Serial.println(F("EV_LINK_DEAD"));
      break;
    case EV_LINK_ALIVE:
     // Serial.println(F("EV_LINK_ALIVE"));
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
      //Serial.println(F("EV_TXSTART"));
      break;
    case EV_TXCANCELED:
      //Serial.println(F("EV_TXCANCELED"));
      break;
    case EV_RXSTART:
      /* do not print anything -- it wrecks timing */
      break;
    case EV_JOIN_TXCOMPLETE:
     // Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
      break;
    default:
      //Serial.print(F("Unknown event: "));
     // Serial.println((unsigned) ev);
      break;
  }
}

void do_send(osjob_t* j) {
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND) {
   // Serial.println(F("OP_TXRXPEND, not sending"));
  } else {
    measure();
    // Prepare upstream data transmission at the next possible time.
    LMIC_setTxData2(1, mydata, sizeof(mydata), 0);
   // Serial.println(F("Packet queued"));

  }
  // Next TX is scheduled after TX_COMPLETE event.
}





void measure()
{ 
  int Temperature = int(100*(bme.readTemperature()));
  int Pressure = int((100*(bme.readHumidity())));
  int Humidity = int((100*(pressureToMmHg(bme.readPressure()))));
  mydata[0] = highByte(Temperature);;
  mydata[1] = lowByte(Temperature);
  mydata[2] = highByte(Pressure);;
  mydata[3] = lowByte(Pressure);
  mydata[4] = highByte(Humidity);
  mydata[5] = lowByte(Humidity);
  delay(1000);
}


void wind_data()
{
  if (previousTime > millis())
    previousTime = 0;

  else if ((millis() - previousTime) > 500)
  {
    previousTime = millis();
    int dir_Value = analogRead(A0);
    
    mydata[6] = highByte(dir_Value);
    mydata[7] = lowByte(dir_Value);
    
    count = 0;
    wind_speed = 0;

  }
}

void wind_count()
{
  count++;
  wind_speed = count * (2.25 / 1);
 //mydata[8] = highByte( wind_speed );
 //mydata[9] = lowByte( wind_speed );
}

void setup() {
  //    pinMode(13, OUTPUT);
  while (!Serial); // wait for Serial to be initialized
  Serial.begin(9600);
  delay(100);     // per sample code on RF_95 test
  Serial.println(F("Starting"));
  // bme.setStandbyTime(STANDBY_1000MS);
  bme.setFilter(FILTER_COEF_8);
  bme.setStandbyTime(STANDBY_500MS);
  bme.begin();
#ifdef VCC_ENABLE

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
  LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI);      // g-band
  LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK,  DR_FSK),  BAND_MILLI);      // g2-band
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
  // LMIC_setupChannel(0, 865062500, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
  // LMIC_setupChannel(1, 865402500, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
  // LMIC_setupChannel(2, 865985000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);

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


  os_runloop_once();

}
