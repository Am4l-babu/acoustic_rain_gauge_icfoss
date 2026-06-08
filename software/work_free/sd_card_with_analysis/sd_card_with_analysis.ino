#include"seeed_line_chart.h" //include the library
#include <math.h>
#include <SPI.h>
#include <Seeed_FS.h>
#include "SD/Seeed_SD.h"
File myFile;
int val; //value from mic

#define max_size 50 //maximum size of data
doubles data; //Initilising a doubles type to store data

void setup() {
  Serial.begin(115200);
  Serial.print("Initializing SD card...");
  if (!SD.begin(SDCARD_SS_PIN, SDCARD_SPI)) {
    Serial.println("initialization failed!");
    while (1);
}
  pinMode(WIO_MIC, INPUT);
  //tft.begin();
 // tft.setRotation(3);
 // spr.createSprite(TFT_HEIGHT,TFT_WIDTH);
  
Serial.println("initialization done.");
}
void loop()
{
  val = analogRead(WIO_MIC);

    if (data.size() == max_size) {
        data.pop();//this is used to remove the first read variable
    }
    data.push(val); //read variables and store in data
    saveData();
    
  
}
void saveData()
{
    myFile = SD.open("Dataset1.txt", FILE_APPEND);
    val = analogRead(WIO_MIC);
    Serial.println(val);
    myFile.println(val);
    myFile.close();
}
