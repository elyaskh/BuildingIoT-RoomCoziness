
/*
  Basic MQTT example

  This sketch demonstrates the basic capabilities of the library.
  It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic"
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary

  It will reconnect to the server if the connection is lost using a blocking
  reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
  achieve the same result without blocking the main loop.

*/

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <math.h>


const int B = 4275;               // B value of the thermistor
const int R0 = 100000;            // R0 = 100k
const int pinTempSensor = A0;     // Grove - Temperature Sensor connected to A0
const int pinSoundSensor = A1;    // Grove - Sound Sensor connected to A1
const int pinLightSensor = A3;    // Grove - Light Sensor connected to A2


// Update these with values suitable for your network.
byte mac[]    = {  0x90, 0xA2, 0xBA, 0x10, 0xA8, 0x2D };
IPAddress ip(192, 168, 1, 30);
IPAddress server(192, 168, 1, 31);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

EthernetClient ethClient;
PubSubClient client(ethClient);

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("room/connection", "Connected");
      // ... and resubscribe
      //client.subscribe("room/temp");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup()
{
  Serial.begin(9600);

  client.setServer(server, 1883);
  client.setCallback(callback);

  Ethernet.begin(mac, ip);
  // Allow the hardware to sort itself out
  delay(1500);
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }

  //---------TheTempSensor----------------------------
  int a = analogRead(pinTempSensor);
  float R = 1023.0 / a - 1.0;
  char temp[5];
  R = R0 * R;
  float temperature = 1.0 / (log(R / R0) / B + 1 / 298.15) - 273.15; // convert to temperature via datasheet
  String temp0 = String(temperature, 2);
  temp0.toCharArray(temp, 5);
  delay(500);
  //---------------------------------------------------

  //---------TheLightSensor----------------------------
  int light0 = analogRead(pinLightSensor);
  float resistance = (1023 - light0) * 10 / light0;
  char light[4];
  String light1 = String(resistance, 2);
  light1.toCharArray(light, 5);
  delay(500);
  //---------------------------------------------------


  //if (client.subscribe("subs") == "1") {
  //client.publish("sound", sound);
  client.publish("temp", temp);
  client.publish("light", light);
  delay(58000);
  //}

  delay(1000);

  client.loop();
}
