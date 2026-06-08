

#include <GyverBME280.h>
GyverBME280 bme;

void setup() {
  Serial.begin(9600);
  bme.setFilter(FILTER_COEF_8);
  bme.setTempOversampling(OVERSAMPLING_8);
  bme.setPressOversampling(OVERSAMPLING_16);
  bme.setStandbyTime(STANDBY_500MS);
  bme.begin();
}
void loop() {
  Serial.print("Temperature: ");
  Serial.println(bme.readTemperature());
  int Temperature = int(100 * (bme.readTemperature()));
  Serial.print(Temperature);
  Serial.println(" *C");
  Serial.print("Humidity: ");
  int Humidity = int((100 * (bme.readHumidity())));
  Serial.println(bme.readHumidity());
  Serial.print(Humidity);
  Serial.println(" %");

  Serial.print("Pressure: ");
  //Serial.println(pressureToMmHg(bme.readPressure()));


  int Pressure = pressureToMmHg(bme.readPressure());
  Serial.println("Pressure2");
  Serial.println(Pressure );
  int pre = Pressure;
  Serial.print("pre = ");
  Serial.print(pre);
  Serial.println(" mm Hg");

  delay(10000);



}
