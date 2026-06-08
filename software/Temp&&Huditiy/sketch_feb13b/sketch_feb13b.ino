#include <lmic.h>
#include <Wire.h>

#define reed_pin 10
#define pot_pin A0

unsigned long previousTime = 0;
volatile uint16_t count = 0;
float wind_speed = 0;
float dir_Value = 0;

#define FILTER_COEF_8   0x03
#define STANDBY_500MS   0x04

class GyverBME280 {
public:
    GyverBME280();
    bool begin(uint8_t address);
    float readPressure();
    float readHumidity();
    float readTemperature();
    void setFilter(uint8_t mode);
    void setStandbyTime(uint8_t mode);

private:
    int _i2c_address;
    uint8_t _filter_coef;
    uint8_t _standby_time;
    // ... (other members)

    bool reset();
    int32_t readTempInt();
    void readCalibrationData();
    uint8_t readRegister(uint8_t address);
    uint32_t readRegister24(uint8_t address);
    bool writeRegister(uint8_t address, uint8_t data);
};

GyverBME280::GyverBME280() {}
// ... (rest of the GyverBME280 implementation)

GyverBME280 bme;

static uint8_t mydata[6];
static osjob_t sendjob;

// ... (rest of the code)

void setup() {
    while (!Serial); // wait for Serial to be initialized
    Serial.begin(9600);
    delay(100);
    Serial.println(F("Starting"));

    bme.setFilter(FILTER_COEF_8);
    bme.setStandbyTime(STANDBY_500MS);
    bme.begin(0x76);

    // ... (rest of the setup)
}

void measure() {
    int Temperature = int(100 * (bme.readTemperature()));
    int Pressure = int((100 * (bme.readHumidity())));
    int Humidity = int((100 * (pressureToMmHg(bme.readPressure()))));

    mydata[0] = highByte(Temperature);
    mydata[1] = lowByte(Temperature);
    mydata[2] = highByte(Pressure);
    mydata[3] = lowByte(Pressure);
    mydata[4] = highByte(Humidity);
    mydata[5] = lowByte(Humidity);
    delay(1000);
}

void wind_data() {
    if (previousTime > millis())
        previousTime = 0;
    else if ((millis() - previousTime) > 500) {
        previousTime = millis();
        int dir_Value = analogRead(A0);

        mydata[6] = highByte(dir_Value);
        mydata[7] = lowByte(dir_Value);

        count = 0;
        wind_speed = 0;
    }
}

void wind_count() {
    count++;
    wind_speed = count * (2.25 / 1);
}

void do_send(osjob_t* j) {
    if (LMIC.opmode & OP_TXRXPEND) {
        // Serial.println(F("OP_TXRXPEND, not sending"));
    } else {
        measure();
        LMIC_setTxData2(1, mydata, sizeof(mydata), 0);
    }
    os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
}

void loop() {
    os_runloop_once();
    wind_data();
    wind_count();
}
