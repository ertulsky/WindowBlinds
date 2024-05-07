#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Stepper.h>

#define LED 4 // GPIO 5 (D1) for LED
bool ledState = false;

const int stepsPerRevolution = 300;  // change this to fit the number of steps per revolution
// for your motor

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 0, 1,2, 3);

//c:\Users\ertul\Downloads\ESP8266WiFi.h
//c:\Users\ertul\OneDrive\Desktop\Mechatronics\ESP_steppermotor_working\PubSubClient.h

// WiFi settings
const char *ssid = "507 Euclid";             // Replace with your WiFi name
const char *password = "Rent8-17";   // Replace with your WiFi password

// MQTT Broker settings
//const char *mqtt_broker = "broker.emqx.io";  // EMQX broker endpoint
//const char *mqtt_topic = "emqx/esp8266";     // MQTT topic
//const char *mqtt_username = "emqx";  // MQTT username for authentication
//const char *mqtt_password = "public";  // MQTT password for authentication
//const int mqtt_port = 1883;//8883;  // MQTT port (TCP)

const char *mqtt_broker = "broker.emqx.io";  // EMQX broker endpoint
const char *mqtt_topic = "emqx/esp8266";     // MQTT topic
const char *mqtt_username = "emqx";  // MQTT username for authentication
const char *mqtt_password = "public";  // MQTT password for authentication
const int mqtt_port = 1883;//8883;  // MQTT port (TCP)

WiFiClient espClient;
PubSubClient mqtt_client(espClient);

void connectToWiFi();

void connectToMQTTBroker();

void mqttCallback(char *topic, byte *payload, unsigned int length);

void setup() {
    Serial.begin(115200);
    connectToWiFi();
    mqtt_client.setServer(mqtt_broker, mqtt_port);
    mqtt_client.setCallback(mqttCallback);
    connectToMQTTBroker();
    myStepper.setSpeed(70);
}

void connectToWiFi() {
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to the WiFi network");
}

void connectToMQTTBroker() {
    while (!mqtt_client.connected()) {
        String client_id = "esp8266-client-" + String(WiFi.macAddress());
        Serial.printf("Connecting to MQTT Broker as %s.....\n", client_id.c_str());
        if (mqtt_client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Connected to MQTT broker");
            mqtt_client.subscribe(mqtt_topic);
            // Publish message upon successful connection
            mqtt_client.publish(mqtt_topic, "Hi EMQX I'm ESP8266 ^^");
        } else {
            Serial.print("Failed to connect to MQTT broker, rc=");
            Serial.print(mqtt_client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
      Serial.print((char) payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------");
}

void mqttCallback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message received on topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    String message;
    for (int i = 0; i < length; i++) {
        message += (char) payload[i];  // Convert *byte to string
    }
    Serial.println(message);
    // Control the LED based on the message received
    if (message == "on" && !ledState) {
        digitalWrite(LED, HIGH);  // Turn on the LED
        ledState = true;
        myStepper.step(stepsPerRevolution);
        Serial.println("Motor is turned on");
        mqtt_client.publish(mqtt_topic, "Motor is turned on");
    }
    if (message == "off" && ledState) {
        digitalWrite(LED, LOW); // Turn off the LED
        ledState = false;
        myStepper.step(0);
        Serial.println("Motor is turned off");
        mqtt_client.publish(mqtt_topic, "Motor is turned off");
    }
    Serial.println();
    Serial.println("-----------------------");
}

void loop() {
    if (!mqtt_client.connected()) {
        connectToMQTTBroker();
    }
    mqtt_client.loop();
    //mqtt_client.mqttCallback();
}