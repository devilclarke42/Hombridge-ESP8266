/*

ESP8266 HomeBridge MQTT Lamp with local power loss correction
and Over The Air update functionality (require pyton 2.7) 
Also avoids hard coded network values using WiFiMangager Library

Written and edited by Rhys Clarke (DevilClarke)

This software is licensed under the GPL,
which means you cannot use it for commercial purposes this includes but is not limmited to
use of this software in a commercial product for sale without express written permision from
the author (Rhys Clarke) contactable through email at {RhysClarke2009@gmail.com}

*/

#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <WiFiUdp.h>
#include <EEPROM.h>

// Update these with values suitable for your network and device
const char* mqtt_server = "192.168.0.32";
const char* device = "Lincoln's_lamp"; 

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
char x;
int value = 0;
int Last_State = 0;
int addr = 0;
int inputPin  = D3; //Hardware button for local switching (If You Use another pin please add 10K pull up)
int RelayPin = D5; //Relay pin
String content = "";
String prefix = "home/";
String On ="/on";
String Brightness = "/brightness";
String Hue = "/hue";
String Saturation = "/sat";
String On_Topic = "";
String Brightness_Topic = "";
String Hue_Topic = "";
String Saturation_Topic ="";

void callback(char* topic, byte* payload, unsigned int length) {
 //Debug
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    x =  ((char)payload[i]);
    content.concat(x);
  }
  Serial.println(content);
  //Debug

  if (strcmp(topic,On_Topic.c_str())==0 && content == "true") {
    Serial.println(device);
    Serial.println("ON");
    EEPROM.write(addr, '1');
    EEPROM.commit();
    // Anything added here before the } will be run when the right device and the on function is recieved
    digitalWrite(BUILTIN_LED, LOW);  // turn on LED with voltage LOW
    digitalWrite(RelayPin, HIGH); // turn on Relay with voltage HIGH 
  }
  if (strcmp(topic,On_Topic.c_str())==0 && content == "false") {
    Serial.println(device);
    Serial.println("OFF");
    EEPROM.write(addr, '0');
    EEPROM.commit();
    // Anything added here before the } will be run when the right device and the off function is recieved
    digitalWrite(BUILTIN_LED, HIGH);  // turn off LED with voltage HIGH
    digitalWrite(RelayPin, LOW); // turn off Relay with voltage LOW
  }
  if (strcmp(topic,Brightness_Topic.c_str())==0) {
    
  }
  if (strcmp(topic,Hue_Topic.c_str())==0) {
    
  }
  if (strcmp(topic,Saturation_Topic.c_str())==0) {
    
  }
  content = "";
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe(On_Topic.c_str());
      client.subscribe(Brightness_Topic.c_str());
      client.subscribe(Hue_Topic.c_str());
      client.subscribe(Saturation_Topic.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {

  //Serial Setup
  Serial.begin(115200);
  Serial.println("Booting");

  //WiFi setup
  WiFiManager wifiManager;
  wifiManager.autoConnect(device);

  //MQTT Subscibing Channels construction
  On_Topic = prefix + device + On;
  Brightness_Topic = prefix + device + Brightness;
  Hue_Topic = prefix + device + Hue;
  Saturation_Topic = prefix + device + Saturation;

  //Setup EEPROM and pins
  EEPROM.begin(512);
  pinMode(inputPin, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);  // initialize onboard LED as output
  pinMode(RelayPin, OUTPUT);  // initialize Relay pin as output
  digitalWrite(BUILTIN_LED, HIGH); //eleminates flicker
  digitalWrite(RelayPin, LOW); // turn on Relay with voltage HIGH

  // Check the last state in eeprom and set lamp accordingly
  if (char(EEPROM.read(addr)) == '1') {
    digitalWrite(BUILTIN_LED, LOW);  // turn on LED with voltage LOW
    digitalWrite(RelayPin, HIGH); // turn on Relay with voltage HIGH
  }
    else {
    digitalWrite(BUILTIN_LED, HIGH);  // turn off LED and RELAY with voltage HIGH
    digitalWrite(RelayPin, LOW); // turn off Relay with voltage LOW
  }

  //OTA Configuartion
  ArduinoOTA.setHostname(device);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


void loop() {

  ArduinoOTA.handle();

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  //This allows the lamp to be turned on and off localy with a button
  if (digitalRead(inputPin) == LOW && char(EEPROM.read(addr)) == '0') {
    Serial.println(device);
    Serial.println("ON");
    EEPROM.write(addr, '1');
    EEPROM.commit();
    client.publish(On_Topic.c_str(), "true");
    digitalWrite(BUILTIN_LED, LOW);  // turn on LED and RELAY with voltage LOW
    digitalWrite(RelayPin, HIGH); // turn on Relay with voltage HIGH
    delay(1000);
  }

  if (digitalRead(inputPin) == LOW && char(EEPROM.read(addr)) == '1') {
    Serial.println(device);
    Serial.println("OFF");
    EEPROM.write(addr, '0');
    EEPROM.commit();
    client.publish(On_Topic.c_str(), "false");
    digitalWrite(BUILTIN_LED, HIGH);  // turn off LED with voltage HIGH
    digitalWrite(RelayPin, LOW); // turn off Relay with voltage LOW
    delay(1000);
  }

}
