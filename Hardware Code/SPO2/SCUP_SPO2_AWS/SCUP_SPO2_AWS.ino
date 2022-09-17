#include "certs.h"
#include <WiFiClientSecure.h>
#include <MQTT.h>
#include <WiFi.h>
#include <Wire.h>
#include <MAX30100_PulseOximeter.h>

#define REPORTING_PERIOD_MS        500
#define REPORTING_PERIOD_MS_CHART  60
#define DEVICE_NAME "spo2-1"
#define AWS_IOT_TOPIC "/cos/spo2-1"

#define ESP_getChipId() ((uint32_t)ESP.getEfuseMac())

float so2 = 0;
float bpm = 0;

int counter = 0;
int subcounter = 0;
int acCounter = 0;
int bpmc = 0;
int so2c = 0;
String jsonSends = "";
bool flagc = false;

PulseOximeter pox;
uint32_t tsLastReport = 0;
uint32_t tsLastReport2 = 0;
float state[10];
int count = 0;

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

void connect(void);
char* string2char(String command);
void connectToWiFi();
void connectToAWS();

void setup()
{
  Serial.begin(115200);
  if (!pox.begin(PULSEOXIMETER_DEBUGGINGMODE_AC_VALUES)) {
    for (;;);
  }
  WiFi.begin(ssid, password);
  connect();
}

void loop()
{
  client.loop();
  static float values;
  pox.update(&values);
  bool flag = false;
  Serial.println(values);

  //Chart Values
  
  if (millis() - tsLastReport > REPORTING_PERIOD_MS_CHART) {
    tsLastReport = millis();
    state[count] = values;
    count++;
    bool flags = false;
    if (count == 10) {
      count = 0;
      for (int i = 0; i < 9; i++) {
        if (state[i] == state[i + 1]) {
          count++;
          if (count == 9)
          {
            while (!pox.begin(PULSEOXIMETER_DEBUGGINGMODE_AC_VALUES)) {
              delay(10);
            }
          }
        }
      }
      count = 0;
    }
    if (int(values) < 500 && int(values) > -1000) {
      jsonSends += String(int(values));
      flagc=true;
    }
  }

  // BPM and So2 Values

  if (millis() - tsLastReport2 > REPORTING_PERIOD_MS) {
    so2 = pox.getSpO2();
    bpm = pox.getHeartRate();
    if (bpm > 60 && so2 > 50 && so2 <= 100)
    {
      bpmc += bpm;
      so2c += so2;
      counter++;
      flag = true;
    }
    else if (bpm == 0 && so2 == 0) {
      bpmc = 0;
      so2c = 0;
      jsonSends = "";
      counter = 0;
    }
    else {
      bpmc = 0;
      so2c = 0;
      jsonSends = "";
      counter = 0;
    }
    tsLastReport2 = millis();
  }
  if (flag) {
    if (counter > 5) {
      bpmc = bpmc / counter;
      so2c = so2c / counter;
      jsonSends += String(int(values));
      String jsonSend = String(so2c) + "," + String(int(bpmc));
      client.publish(AWS_IOT_TOPIC, string2char("{\"graph\":[" + jsonSends + "],\"data\":[" + jsonSend + "],\"pat\":\"1\"}"));
      counter = 0;
      jsonSends = "";
      bpmc = 0;
      so2c = 0;
      acCounter = 0;
    }
    }
  else {
    if (jsonSends != "") {
      if (flagc) {
        jsonSends += ",";
        flagc = false;
      }
    }
  }

  if (!client.connected()) {
    digitalWrite(5, HIGH);
    connect();
    digitalWrite(5, LOW);
  }
}

char* string2char(String command) {
  if (command.length() != 0) {
    char *p = const_cast<char*>(command.c_str());
    return p;
  }
}

void connectToWiFi() {
  int conCounter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    conCounter++;
    if (conCounter == 10) {
      ESP.restart();
    }
  }
}

void connectToAWS()
{
  // Configure WiFiClientSecure to use the AWS certificates we generated
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.begin(AWS_IOT_ENDPOINT, 8883, net);

  // Try to connect to AWS and count how many times we retried.

  while (!client.connect(DEVICE_NAME)) {
    delay(100);
  }

  // Make sure that we did indeed successfully connect to the MQTT broker
  // If not we just end the function and wait for the next loop.
  if (!client.connected()) {
    return;
  }

}

void connect(void) {
  connectToWiFi();
  connectToAWS();
}
