/***
 * name: Guy bar-dor ,Alona Rozner,Nitzahn Ben-yehoda
 */

/**************************************************************************/
/*!
This is a demo for the Adafruit MCP9808 breakout
----> http://www.adafruit.com/products/1782
Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!
*/
/**************************************************************************/

#include <Wire.h>
#include "Adafruit_MCP9808.h"


// Create the MCP9808 temperature sensor object
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();
//wifi parameters
#define Wifi_SSID "brgt2"
#define wifi_pass "0505508820"

//Adafruit Io
#define AIO_SERVER     "io.adafruit.com"
#define AIO_SERVERPORT "1883"
#define AIO_USERNAME   "1993gbd"
#define IO_KEY         "aio_vgcT59T5a9P1dO5xKamgqOi9DDro"
void setup() {
  Serial.begin(9600);
  Serial.println("MCP9808 demo");
  Wire.begin();
  pinMode(4,OUTPUT);//green
//    Wire.pins(D1, D2); //D4=SDA, D5=SCL on ESP8266
    
  // Make sure the sensor is found, you can also pass in a different i2c
  // address with tempsensor.begin(0x19) for example
  if (!tempsensor.begin(0x1C)) {             // Address For MANTA Temperature Sensor
    Serial.println("Couldn't find MCP9808!");
    while (1);
  }
}

void loop() {
  //Serial.println("wake up MCP9808.... "); // wake up MSP9808 - power consumption ~200 mikro Ampere
  //tempsensor.wake();   // wake up, ready to read!

  // Read and print out the temperature, then convert to *F
  float c = tempsensor.readTempC();
  float f = c * 9.0 / 5.0 + 32;
  Serial.print("Temp: "); Serial.print(c); Serial.print("*C\t"); 
  Serial.print(f); Serial.println("*F");
  
  //Serial.println("Shutdown MCP9808.... ");
  //tempsensor.shutdown(); // shutdown MSP9808 - power consumption ~0.1 mikro Ampere
  if(c<27.9)
  {
    digitalWrite(4, HIGH);
  }
  else{
  digitalWrite(4, LOW);
  }
  delay(2000);
}
