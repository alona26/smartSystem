/***************************************************
  
  SHENKAR - SMART SYSTEMS
  By: Nitzan Ben Yehuda, Guy Bar-Dor & Alona Rozner
  DATE: May-2021
  
 ****************************************************/
//#include <ESP8266WiFi.h>
#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#include <Wire.h>
#include "Adafruit_MCP9808.h"



/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "BRGT"
#define WLAN_PASS       "0505508820"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "1993gbd"
#define AIO_KEY          "aio_Zmrc68MgU1XRXoIXapix3x69NhCf"


/************ Global State (you don't need to change this!) ******************/


// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish light_level = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/tempertue");

// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/tempertue");// recive data from the cloud

/*************************** Sketch Code ************************************/




// Create the MCP9808 temperature sensor object
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();


// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();



void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println(F("Starting..."));
  delay(1000);
  
  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
  Serial.println();
  pinMode(4,OUTPUT);//green
  if (!tempsensor.begin(0x1C)) {             // Address For MANTA Temperature Sensor
    Serial.println("Couldn't find MCP9808!");
    while (1);
  }
  mqtt.subscribe(&onoffbutton); // Setup MQTT subscription for temp_point_read feed,litsen to the 

}

void loop() {
   MQTT_connect();
  float c = tempsensor.readTempC();
  float f = c * 9.0 / 5.0 + 32;
  Serial.print("Temp: "); Serial.print(c); Serial.print("*C\t"); 
  Serial.print(f); Serial.println("*F");
//  if(26.6<c<27.9)
//  {
//    digitalWrite(4, HIGH);
//  }
//  else{
//  digitalWrite(4, LOW);
//  }
  //result=c;
  delay(2000);
  char *counter("57");
  delay(2000);
    Serial.print(F("\nSending value "));
    Serial.print(c);
    Serial.print("...");
  if (! light_level.publish(c)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
  Adafruit_MQTT_Subscribe *subscription;//recive data from the cloud 
  while ((subscription = mqtt.readSubscription(5000))) 
  {
      if (subscription == &onoffbutton) {
        float value_from_cloud_float;
        value_from_cloud_float = atof((char *)onoffbutton.lastread);
        if(value_from_cloud_float >= 27.20)
        {
          digitalWrite(4, HIGH);    
        }
        else
        {
           digitalWrite(4, LOW);
        }
        Serial.print(F("Got temp from cloud in float: "));
        Serial.println(value_from_cloud_float);
      }
  }
}
// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
