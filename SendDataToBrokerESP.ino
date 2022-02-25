#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

#define DHTPIN 13
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);



const char* ssid = "YOUR WIFI SSID";
const char* password = "YOUR WIFI PASSWD";
const char* mqttServer = "RASPBERRY PI IP";
const int mqttPort = 1883;
const char* mqttUser = "Jeff";
const char* mqttPassword = "123";
const char* clientID = "client_esp32";


const char* temp_topic = "home/esp32/temperatuur";
const char* hum_topic = "home/esp32/humidity";


WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  dht.begin();
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  client.setServer(mqttServer, mqttPort);


  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP32Client", mqttUser, mqttPassword ))
    {
      Serial.println("connected");
    } else
    {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void loop() {
  client.loop();

  float hum = dht.readHumidity();
  float temp = dht.readTemperature();
 
  // check if returns are valid and print the sensor data
  if (isnan(temp) || isnan(hum)) {
    Serial.println("Failed to read from DHT");
  } else {
    Serial.print("Humidity: "); 
    Serial.print(hum);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(temp);
    Serial.println(" *C");
  }
  
  String tempMQTT = String((float)temp);
  String humMQTT = String((float)hum);


  if (tempMQTT != "nan" && humMQTT != "nan") {
    client.publish(temp_topic, tempMQTT.c_str());
    delay(1000);
    client.publish(hum_topic, humMQTT.c_str());
    delay(1000);
  }

  delay(1000); // Wait for 55 seconds
}
