#include "certs.h"
#include <WiFiClientSecure.h>
#include <MQTT.h>
#include <WiFi.h>

#define packageSize 20
#define mydelay 20000
#define ESP_getChipId()   ((uint32_t)ESP.getEfuseMac())

// The name of the device. This MUST match up with the name defined in the AWS console
#define DEVICE_NAME "ecg-1"
#define AWS_IOT_TOPIC "/cos/ecg-1"

// Wifi credentials
int hrArray[packageSize];
int counter = 0;

volatile int interruptCounter;
int totalInterruptCounter;

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  int sensorValue = analogRead(A2);
  hrArray[counter] = sensorValue;
  counter++;
  portEXIT_CRITICAL_ISR(&timerMux);
}

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  connect();
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 4000, true);
  timerAlarmEnable(timer);
}

void loop() {
  client.loop();
  if (counter > packageSize) {
    String jsonSend = "";
    for (int i = 0; i < counter; i++) {
      jsonSend += String(hrArray[i]);
      if (i != (counter - 1)) {
        jsonSend += ",";
      }
    }
    counter = 0;
    client.publish(AWS_IOT_TOPIC, string2char("{\"data\":[" + jsonSend + "],\"pat\":\"1\"}"));
  }
  if (!client.connected()) {
    connect();
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
    Serial.print(".");
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
    connect();
  }

}

void connect() {
  connectToWiFi();
  connectToAWS();
}
