#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>
#include <BH1750.h>

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = "Goyal's";    // your network SSID (name)
char pass[] = "00000001";    // your network password (use for WPA, or use as key for WEP)

BH1750 GY30; // instantiate a sensor object

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "mqtt://broker.hivemq.com";
int        port     = 1883;
const char topic[]  = "SensorData";

float people = 0; 
int ir1SensorState = LOW;
int ir2SensorState = LOW;

void setup() {
  Serial.begin(9600);
  GY30.begin(); // initialize the sensor object
  pinMode(8, OUTPUT);
  pinMode(9, INPUT);
  pinMode(10, INPUT);

  
  // attempt to connect to WiFi network:
  Serial.print("Attempting to connect to WPA SSID: Agam ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  // You can provide a unique client ID, if not set the library uses Arduino-millis()
  // Each client must have a unique client ID
   mqttClient.setId("clientId-nAS4dVvDAo");

  // You can provide a username and password for authentication
  mqttClient.setUsernamePassword("username", "password");

  Serial.print("Attempting to connect to the MQTT broker:broker.mqttdashboard.com ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  Serial.print("Subscribing to topic: SIT210/wave5 ");
  Serial.println(topic);
  Serial.println();

  // subscribe to a topic
  mqttClient.subscribe("SensorData");

  // topics can be unsubscribed using:
  // mqttClient.unsubscribe(topic);

  Serial.print("Waiting for messages on topic: SIT210/wave5 ");
  Serial.println(topic);
  Serial.println();
}

void loop() {
  ir1SensorState = digitalRead(9);
  ir2SensorState = digitalRead(10);
  float lux = GY30.readLightLevel(); // read the light level from the sensor

    
    // If motion is detected, blink the corresponding LED
    if (ir1SensorState == HIGH) {
      people = people + 1;
      Serial.println(people);
    }
    else if (people >= 1 && ir2SensorState == HIGH){
      people = people - 1;
      Serial.println(people);
    }

    if(people >= 1 && lux <= 500) {
      digitalWrite(8, HIGH);
    }
    if (lux > 450) {
      digitalWrite(8, LOW);
    }

    if (people == 0) {
        digitalWrite(8, LOW);
    }

    delay(500);
      mqttClient.beginMessage(topic);
    mqttClient.print("hello ");
    mqttClient.print(people);
    mqttClient.endMessage();
  }
