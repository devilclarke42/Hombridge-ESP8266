//ESP8266 HomeBridge MQTT Lamp with local power loss correction

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>

// Update these with values suitable for your network.
const char* ssid = "xxxxxxxx";
const char* password = "xxxxxxxxxx";
const char* mqtt_server = "xxxxxxxx";

//Update this with the name of device.
const char* device = "lincoln's_lamp"; 

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
char x;
int value = 0;
int correct_device = 0;
int Last_State = 0;
int addr = 0;
int inputPin  = D3; //Hardware button for local switching (If You Use another pin please add 10K pull up)
String content = "";


void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
      x =  ((char)payload[i]);
      content.concat(x);
  }
  Serial.println(content);

  // If content is the correct device set boolean
  if (content == device){
    correct_device = 1;
  }
  
  if (correct_device == 1 && content == "true") {
    Serial.println(device);
    Serial.println("ON");
    EEPROM.write(addr, '1');
    EEPROM.commit();
    correct_device = 0;
    // Anything added here before the } will be run when the right device and the on function is recieved
    digitalWrite(BUILTIN_LED, LOW);  // turn on LED and RELAY with voltage LOW
  }
    if (correct_device == 1 && content == "false") {
    Serial.println(device);
    Serial.println("OFF");
    EEPROM.write(addr, '0');
    EEPROM.commit();
    correct_device = 0;
    // Anything added here before the } will be run when the right device and the off function is recieved
    digitalWrite(BUILTIN_LED, HIGH);  // turn off LED and RELAY with voltage HIGH
  }
  content = "";
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("Heartbeat", "hello world");
      // ... and resubscribe
      client.subscribe("Lamps");
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
  EEPROM.begin(512);
  pinMode(inputPin, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);  // initialize onboard LED as output
  digitalWrite(BUILTIN_LED, HIGH); //eleminates flicker

  // Check the last state in eeprom and set lamp accordingly
  if (char(EEPROM.read(addr)) == '1') {
    digitalWrite(BUILTIN_LED, LOW);  // turn on LED and RELAY with voltage LOW
  }
    else {
    digitalWrite(BUILTIN_LED, HIGH);  // turn off LED and RELAY with voltage HIGH
  }
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

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
    correct_device = 0;
    client.publish(device, "On");
    digitalWrite(BUILTIN_LED, LOW);  // turn on LED and RELAY with voltage LOW
    delay(1000);
  }

  if (digitalRead(inputPin) == LOW && char(EEPROM.read(addr)) == '1') {
    Serial.println(device);
    Serial.println("OFF");
    EEPROM.write(addr, '0');
    EEPROM.commit();
    correct_device = 0;
    client.publish(device, "Off");
    digitalWrite(BUILTIN_LED, HIGH);  // turn off LED and RELAY with voltage HIGH
    delay(1000);
  }

// This is Debug (Heartbeat) for testing purposes 
  long now = millis();
  if (now - lastMsg > 1000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 75, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("Heartbeat", msg);
  }
}
