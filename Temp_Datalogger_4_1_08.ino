/* Arduino Temp Datalogger
 *  
 * Tested wth:
 *  Arduino Uno R3
 *  Adafruit BME280 Sensor
 *  Adafruit Datalogger Shield R3 Compatible
 * 
 * Based on code written by: 
 *  https://github.com/srebroa
 *  https://github.com/CharlesVercauteren
 *  https://learn.adafruit.com/adafruit-data-logger-shield?view=all
 * 
*/


#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "RTClib.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Narcoleptic.h>


RTC_PCF8523 rtc;
Adafruit_BME280 bme;


const int chipSelect = 10;
String dataString = "";
char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thur", "Fri", "Sat"};
#define LOG_INTERVAL  1000


void setup(){
  SDcardInit();
  Serial.begin(9600);
  Serial.print("Sensor BME280");    
  delay(10);
  Serial.println(bme.begin(), HEX);

  
  #ifndef ESP8266
  while (!Serial);
  #endif
  
  
  rtc.start();
  if (! rtc.begin()) {
   Serial.println("Couldn't find RTC");
   Serial.flush();
   abort();
  }

  if (! rtc.initialized() || rtc.lostPower()) {
   Serial.println("RTC is NOT initialized, let's set the time!");
   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

}



void loop(){
  DateTime now = rtc.now();
  delay((LOG_INTERVAL -1) - (millis() % LOG_INTERVAL));
  processSensorInput();
  writeToSDcard(dataString);   
  Narcoleptic.delay( 899990 );
}



void processSensorInput(){
  String temp = String(1.8 * bme.readTemperature() + 32);
  dataString = temp;
}


void SDcardInit(){
  Serial.print("Initializing SD card...");
  pinMode(chipSelect, OUTPUT);
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("card initialized.");
}


void writeToSDcard(String currentDataString){
  File dataFile = SD.open("templog.csv", FILE_WRITE);
  if (dataFile) {
    DateTime now = rtc.now();
    dataFile.print(' ');
    dataFile.print(now.month(), DEC);
    dataFile.print(' ');
    dataFile.print(now.day(), DEC);
    dataFile.print(',');
    dataFile.print(now.year(), DEC);
    dataFile.print(' ');
    dataFile.print(now.hour(), DEC);
    dataFile.print(':');
    dataFile.print(now.minute(), DEC);
    dataFile.print(':');
    dataFile.print(now.second(), DEC);
    dataFile.print(' ');
    dataFile.println(currentDataString+" *F");
    dataFile.close();  
  }
  else {
    Serial.println("error opening templog.csv");
  }  
}
