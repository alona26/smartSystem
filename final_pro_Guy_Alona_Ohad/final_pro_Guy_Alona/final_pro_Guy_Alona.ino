/***************************************************
  
  SHENKAR - SMART SYSTEMS
  By: Ohad Beer, Guy Bar-Dor & Alona Rozner
  DATE: May-2021
  
 ****************************************************/
#include <analogWrite.h>
#include <WiFi.h>
#include "Arduino.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#include <Wire.h>
#include "Adafruit_MCP9808.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID "Rozner1234"
#define WLAN_PASS "RoznerAir1234"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883 // use 8883 for SSL
#define AIO_USERNAME "1993gbd"
#define AIO_KEY "aio_tCwB56UZ2BV2ZjHLhulJLcvM8tDR"

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
Adafruit_MQTT_Publish temperture = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/tempertue");
Adafruit_MQTT_Publish light = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/light");
Adafruit_MQTT_Publish person = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/person");

// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe tempo = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/tempertue"); // recive data from the cloud
Adafruit_MQTT_Subscribe lighto = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/light");
Adafruit_MQTT_Subscribe persono = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/person");
/*************************** Sketch Code ************************************/

// Create the MCP9808 temperature sensor object
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();
const int led_pin = 10;
const double fan = 23.00;
const int light_outside = 2300;
const int far = 3200;

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial.println(F("Starting..."));
    delay(1000);

    pinMode(led_pin, OUTPUT);

    // Connect to WiFi access point.
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WLAN_SSID);

    WiFi.begin(WLAN_SSID, WLAN_PASS);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println();
    pinMode(4, OUTPUT); //green
    if (!tempsensor.begin(0x1C))
    { // Address For MANTA Temperature Sensor
        Serial.println("Couldn't find MCP9808!");
        while (1)
            ;
    }
    mqtt.subscribe(&tempo); // Setup MQTT subscription for temp_point_read feed,litsen to the
    mqtt.subscribe(&lighto);
    mqtt.subscribe(&persono);
}

void loop()
{

    MQTT_connect();
    char *counter("57");
    delay(2000);
    int valueP = analogRead(34);
    int personB;
    if (valueP > far)
    {
        personB = 1;
    }
    else
    {
        personB = 0;
    }
    delay(200);
    Serial.print(F("\nSending value "));
    Serial.print(personB);
    Serial.print("...");
    if (!person.publish(personB))
    {
        Serial.println(personB);
        Serial.println("Failed");
    }
    else
    {
        Serial.println(F("OK! person"));
    }
    int value = analogRead(33);
    Serial.print(F("\nSending value "));
    Serial.print(value);
    Serial.print("...");
    delay(200);
    if (!light.publish(value))
    {
        Serial.println(F("Failed"));
    }
    else
    {
        Serial.println(F("OK! light"));
    }

    float c = tempsensor.readTempC();
    float f = c * 9.0 / 5.0 + 32;

    Serial.print("Temp: ");
    Serial.print(c);
    Serial.print("*C\t");
    Serial.print(f);
    Serial.println("*F");
    Serial.print(F("\nSending value "));
    Serial.print(c);
    Serial.print("...");
    delay(200);
    if (!temperture.publish(c))
    {
        Serial.println(F("Failed"));
    }
    else
    {
        Serial.println(F("OK!"));
    }
    Adafruit_MQTT_Subscribe *subscription1;
    Adafruit_MQTT_Subscribe *subscription2;
    Adafruit_MQTT_Subscribe *subscription3;
    delay(200);

    while (subscription3 = mqtt.readSubscription(3000))
    {
        if (subscription3 == &persono)
        {

            int value_person;
            value_person = atof((char *)persono.lastread);

            delay(200);

            while (subscription1 = mqtt.readSubscription(3000))
            {
                if (subscription1 == &lighto)
                {

                    int value_light;
                    value_light = atof((char *)lighto.lastread);

                    delay(200);
                    while (subscription2 = mqtt.readSubscription(3000))
                    {

                        if (subscription2 == &tempo)
                        {
                            float value_tem;
                            value_tem = atof((char *)tempo.lastread);
                            if (value_person)
                            {
                                if (value_light > light_outside)
                                {
                                    if (value_tem > fan)
                                    {
                                        for (int i = 100; i < 255; i++)
                                        {
                                            analogWrite(led_pin, i);
                                            //valueP=0;
                                            delay(5);
                                        }
                                    }
                                    else
                                    {
                                        analogWrite(led_pin, 0);
                                        delay(5);
                                    }
                                }
                                else
                                {
                                    analogWrite(led_pin, 0);
                                    delay(5);
                                }
                            }
                            else
                            {
                                analogWrite(led_pin, 0);
                                delay(5);
                            }
                        }
                    }
                }
            }
        }
    }
}
// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect()
{
    int8_t ret;

    // Stop if already connected.
    if (mqtt.connected())
    {
        return;
    }

    Serial.print("Connecting to MQTT... ");

    uint8_t retries = 3;
    while ((ret = mqtt.connect()) != 0)
    {
        // connect will return 0 for connected
        Serial.println(mqtt.connectErrorString(ret));
        Serial.println("Retrying MQTT connection in 5 seconds...");
        mqtt.disconnect();
        delay(5000); // wait 5 seconds
        retries--;
        if (retries == 0)
        {
            // basically die and wait for WDT to reset me
            while (1)
                ;
        }
    }
    Serial.println("MQTT Connected!");
}
