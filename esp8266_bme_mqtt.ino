#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include "config.h"

#define SEALEVELPRESSURE_HPA (1030)  // sea level pressure in Seattle - for calculating altitude

#define BME_SCK 1
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10


// MQTT Broker
const char *mqtt_broker = MQTT_IP; // Enter mqtt server WiFi or Ethernet IP
//const char *topic = "test";
const int mqtt_port = 1883;
const char* mqtt_username = MQTT_USERNAME;
const char* mqtt_password = MQTT_PASSWORD;

// unique ID for this sensor
const char* sensor_name = SENSOR_NAME;

WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_BME280 bme; // I2C
unsigned long delayTime = 1000; // delay between BME queries

void setup() 
{
    // Set software serial baud to 115200;
    Serial.begin(115200);

    // Initialize random seed for random values generated below
    randomSeed(analogRead(0));

    // start up wifi manager - this is a tool that lets the user enter their wifi info through a web page
    WiFiManager wm;
    // wm.resetSettings(); // clears saved wifi settings. for testing
    bool res;
    res = wm.autoConnect();
 
    if(!res) 
    {
        Serial.println("Failed to connect");
        // ESP.restart();
    } 
    else 
    {
        //if you get here you have connected to the WiFi    
        Serial.println("connected...yeey :)");
    }


    Serial.println("Connecting to mqtt broker");
    //connecting to a mqtt broker
    client.setServer(mqtt_broker, mqtt_port);
    connectToMQTTServer(mqtt_username, mqtt_password);
       

    Serial.println("Connecting to BME sensor");
    unsigned status; // BME status
    // default settings
    status = bme.begin();  
    // You can also pass in a Wire library object like &Wire2
    // status = bme.begin(0x76, &Wire2)
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        while (1) delay(10);
      }

 
}

void connectToMQTTServer(const char* mqtt_username, const char* mqtt_password) {
    while (!client.connected()) 
    {
        String client_id = "esp8266-client-";
        client_id += String(WiFi.macAddress());
        
        Serial.printf("The client %s connects to mosquitto mqtt broker\n", client_id.c_str());
        
        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) 
        {
            Serial.println("Public emqx mqtt broker connected");
        } else 
        {
            Serial.print("failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }
}



void loop() {
    Serial.println("Starting loop");
    Serial.println("Reconnecting to mqtt server if needed.");
    connectToMQTTServer(mqtt_username, mqtt_password);
    client.loop();

 
    // read and publish temperature
    float temperature_degC = bme.readTemperature();
    char temperature_buffer[10];
    dtostrf(temperature_degC, 6, 2, temperature_buffer);
    Serial.print("Temperature = ");
    Serial.println(temperature_degC);
    char temperature_degC_topic_buffer[50];
    const char temperature_degC_topic_prefix[] = "temperature_degC/";
    strcpy(temperature_degC_topic_buffer,temperature_degC_topic_prefix);
    strcat(temperature_degC_topic_buffer, sensor_name);
    Serial.print("publishing to ");
    Serial.println(temperature_degC_topic_buffer);
    client.publish(temperature_degC_topic_buffer,temperature_buffer);

    // read and publish humidity
    float humidity_pct = bme.readHumidity();
    char humidity_buffer[10];
    dtostrf(humidity_pct, 6, 2, humidity_buffer);
    Serial.print("Humidity = ");
    Serial.println(humidity_pct);
    char humidity_pct_topic_buffer[50];
    const char humidity_pct_topic_prefix[] = "humidity_pct/";
    strcpy(humidity_pct_topic_buffer,humidity_pct_topic_prefix);
    strcat(humidity_pct_topic_buffer, sensor_name);
    Serial.print("publishing to ");
    Serial.println(humidity_pct_topic_buffer);
    client.publish(humidity_pct_topic_buffer, humidity_buffer);

    // read and publish pressure
    float pressure_hpa = bme.readPressure() / 100.0F;
    char pressure_buffer[10];
    dtostrf(pressure_hpa, 6, 2, pressure_buffer);
    Serial.print("Pressure = ");
    Serial.println(pressure_hpa);
    char pressure_hpa_topic_buffer[50];
    const char pressure_hpa_topic_prefix[] = "pressure_hpa/";
    strcpy(pressure_hpa_topic_buffer,pressure_hpa_topic_prefix);
    strcat(pressure_hpa_topic_buffer, sensor_name);
    Serial.print("publishing to ");
    Serial.println(pressure_hpa_topic_buffer);
    client.publish(pressure_hpa_topic_buffer, pressure_buffer);

    delay(delayTime);
}
