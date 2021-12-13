#include <TFT_eSPI.h> // Graphics and font library for ILI9341 driver chip
#include <SPI.h>
#include "certs.h"
#include <WiFiClientSecure.h>
#include <MQTT.h>
#include "rpcWiFi.h"

#define TFT_GREY 0xDEDB // New colour

TFT_eSPI tft = TFT_eSPI(); // Invoke library
WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

int x[290], y[290], z[290];
int x2[290], y2[290], z2[290];

int in_data[1000];
int in_data_extra[2];
int in_data_len = 0;
int counter = 0;
int flag = 0;

void graphBackground(int w, int h);
void graphData(int w, int h);
void normalize(int w, int h);
void graphRandomTest(int w, int h);
char *string2char(String command);
void connectToWiFi();
void connectToAWS();
void connect();

void messageReceived(String &topic, String &payload)
{
    if (topic == AWS_IOT_SUB_TOPIC && flag == 1)
    {
        JSONtoArray(payload);
        Serial.println(payload);
        for (int i = 0; i < 21; i++)
        {
            if (counter == 290)
            {
                graphDataDel(tft.width(), tft.height());
                graphBackground(tft.width(), tft.height());
                normalize(tft.width(), tft.height());
                for (int j = 0; j < 290; j++)
                {
                    z[j] = y[j];
                }
                graphData(tft.width(), tft.height());
                counter = 0;
            }
            y[counter] = in_data[i];
            counter += 1;
        }
    }
    if (topic == AWS_IOT_SUB_TOPIC2 && flag == 2)
    {
        JSONtoArray2(payload);
        for (int i = 0; i < in_data_len; i++)
        {
            if (counter == in_data_len)
            {
                graphDataDel2(tft.width(), tft.height(), in_data_len);
                graphBackground(tft.width(), tft.height());
                normalizeY(tft.width(), tft.height(), in_data_len);
                normalizeX(tft.width(), tft.height(), in_data_len);
                for (int j = 0; j < in_data_len; j++)
                {
                    z2[j] = y2[j];
                }
                graphData2(tft.width(), tft.height(), in_data_len);
                counter = 0;
                tft.setCursor(40, 214, 2);
                tft.setTextColor(TFT_BLACK);
                tft.setTextFont(1);
                tft.setTextSize(2);
                tft.fillRect(32, 212, 288, 28, TFT_WHITE);
                tft.print(string2char("SPO2:"+String(in_data_extra[0])+"%:BPM:"+String(in_data_extra[1])));
            }
            y2[counter] = in_data[i];
            counter += 1;
        }
        Serial.println();
        Serial.println(in_data_extra[1]);
    }
}

void JSONtoArray(String input)
{
    String temp = input.substring(input.indexOf('[') + 1, input.indexOf(']')) + ",";
    int i = 0;
    while (temp.indexOf(',') != -1)
    {
        in_data[i] = temp.substring(0, temp.indexOf(',')).toInt();
        temp = temp.substring(temp.indexOf(',') + 1, temp.length());
        i++;
    }
    in_data_len = i;
}

void JSONtoArray2(String input)
{
    String temp = input.substring(input.indexOf('[') + 1, input.indexOf(']')) + ",";
    int i = 0;
    while (temp.indexOf(',') != -1)
    {
        in_data[i] = temp.substring(0, temp.indexOf(',')).toInt();
        temp = temp.substring(temp.indexOf(',') + 1, temp.length());
        i++;
    }
    in_data_len = i;
    i = 0;
    temp = input.substring(input.indexOf(']') + 1, input.length());
    temp = temp.substring(temp.indexOf('[') + 1, temp.indexOf(']')) + ",";
    while (temp.indexOf(',') != -1)
    {
        in_data_extra[i] = temp.substring(0, temp.indexOf(',')).toInt();
        temp = temp.substring(temp.indexOf(',') + 1, temp.length());
        i++;
    }
}

void setup(void)
{
    WiFi.begin(ssid, password);
    client.onMessage(messageReceived);
    connect();
    tft.init();
    tft.setRotation(3);
    Serial.begin(115200);
    tft.fillScreen(TFT_WHITE);
    tft.fillCircle(160, 0, 160, TFT_BLACK);
    tft.setCursor(40, 20, 2);
    tft.setTextColor(TFT_YELLOW);
    tft.setTextFont(1);
    tft.setTextSize(7);
    tft.print("AWS 5G");
    tft.setCursor(35, 180, 2);
    tft.setTextColor(TFT_RED);
    tft.setTextFont(1);
    tft.setTextSize(6);
    tft.print("VERIZON");
    for (int i = 0; i < 290; i++)
    {
        x[i] = i + 31;
        z[i] = 400;
    }
    for (int i = 0; i < 290; i++)
    {
        x2[i] = i + 31;
        z2[i] = 400;
    }
    client.subscribe(AWS_IOT_SUB_TOPIC);
    client.subscribe(AWS_IOT_SUB_TOPIC2);
    Serial.println("Connected to AWS");
    pinMode(WIO_KEY_A, INPUT_PULLUP);
    pinMode(WIO_KEY_B, INPUT_PULLUP);
    pinMode(WIO_KEY_C, INPUT_PULLUP);
}

void loop()
{
    client.loop();
    int key = readKey();
    switch (key)
    {
    case 3:
        flag = 1;
        tft.fillScreen(TFT_WHITE);
        tft.drawFastVLine(30, 0, tft.height(), TFT_BLACK);
        tft.drawFastHLine(0, tft.height() - 30, tft.width(), TFT_BLACK);
        graphBackground(tft.width(), tft.height());
        for (int i = 0; i < 290; i++)
        {
            x[i] = i + 31;
            y[i] = 400;
            z[i] = 400;
        }
        counter = 0;
        tft.setCursor(40, 214, 2);
        tft.setTextColor(TFT_BLACK);
        tft.setTextFont(1);
        tft.setTextSize(3);
        tft.print("ECG Data");
        break;
    case 1:
        flag = 2;
        tft.fillScreen(TFT_WHITE);
        tft.drawFastVLine(30, 0, tft.height(), TFT_BLACK);
        tft.drawFastHLine(0, tft.height() - 30, tft.width(), TFT_BLACK);
        graphBackground(tft.width(), tft.height());
        for (int i = 0; i < 290; i++)
        {
            x2[i] = i + 31;
            y2[i] = 400;
            z2[i] = 400;
        }
        counter = 0;
        tft.setCursor(40, 214, 2);
        tft.setTextColor(TFT_BLACK);
        tft.setTextFont(1);
        tft.setTextSize(2);
        tft.print("SPO2:");
        break;
    case 2:
        flag = 3;
        tft.fillScreen(TFT_WHITE);
        tft.fillCircle(160, 0, 160, TFT_BLACK);
        tft.setCursor(40, 20, 2);
        tft.setTextColor(TFT_YELLOW);
        tft.setTextFont(1);
        tft.setTextSize(7);
        tft.print("AWS 5G");
        tft.setCursor(35, 180, 2);
        tft.setTextColor(TFT_RED);
        tft.setTextFont(1);
        tft.setTextSize(6);
        tft.print("VERIZON");
        break;
    default:
        // nothing
        break;
    }
    if (!client.connected())
    {
        connect();
    }
}

void graphRandomTest(int w, int h)
{
    for (int i = 0; i < 290; i++)
    {
        x[i] = i + 31;
        y[i] = random(0, h * 50);
    }
}

void graphBackground(int w, int h)
{

    for (int i = 40; i < w; i += 10)
    {
        tft.drawFastVLine(i, 0, h - 30, TFT_GREY);
    }
    for (int i = 0; i < h - 30; i += 10)
    {
        tft.drawFastHLine(31, i, w, TFT_GREY);
    }
}

void graphData(int w, int h)
{
    for (int i = 0; i < w - 31; i++)
    {
        tft.drawLine(x[i], y[i], x[i + 1], y[i + 1], TFT_RED);
    }
}

void graphDataDel(int w, int h)
{
    for (int i = 0; i < w - 31; i++)
    {
        tft.drawLine(x[i], z[i], x[i + 1], z[i + 1], TFT_WHITE);
    }
}

void graphData2(int w, int h, int value)
{
    for (int i = 0; i < value - 1; i++)
    {
        tft.drawLine(x2[i], y2[i], x2[i + 1], y2[i + 1], TFT_BLUE);
    }
}

void graphDataDel2(int w, int h, int value)
{
    for (int i = 0; i < value - 1; i++)
    {
        tft.drawLine(x2[i], z2[i], x2[i + 1], z2[i + 1], TFT_WHITE);
    }
}

void normalize(int w, int h)
{
    int max = 0;
    int min = 100000;
    for (int i = 0; i < w - 30; i++)
    {
        if (y[i] > max)
        {
            max = y[i];
        }
        if (y[i] < min)
        {
            min = y[i];
        }
    }
    for (int i = 0; i < w - 30; i++)
    {
        y[i] = (y[i] - min) * (h - 31) / (max - min);
    }
    for (int i = 0; i < w - 30; i++)
    {
        y[i] = h - 31 - y[i];
    }
}

void normalizeY(int w, int h, int value)
{
    int max = 0;
    int min = 100000;
    for (int i = 0; i < value; i++)
    {
        if (y2[i] > max)
        {
            max = y2[i];
        }
        if (y2[i] < min)
        {
            min = y2[i];
        }
    }
    for (int i = 0; i < value; i++)
    {
        y2[i] = (y2[i] - min) * (h - 31) / (max - min);
    }
    for (int i = 0; i < value; i++)
    {
        y2[i] = h - 31 - y2[i];
    }
}

void normalizeX(int w, int h, int value)
{
    for (int i = 0; i < value; i++)
    {
        x2[i] = (w / value) * i + 31;
    }
}

char *string2char(String command)
{
    if (command.length() != 0)
    {
        char *p = const_cast<char *>(command.c_str());
        return p;
    }
}

void connectToWiFi()
{
    int conCounter = 0;
    Serial.println("Connecting to WiFi..");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        conCounter++;
    }
    Serial.println("Connected to the WiFi network");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
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

    while (!client.connect(DEVICE_NAME))
    {
        delay(100);
    }

    // Make sure that we did indeed successfully connect to the MQTT broker
    // If not we just end the function and wait for the next loop.
    if (!client.connected())
    {
        connect();
    }
}

void connect()
{
    connectToWiFi();
    connectToAWS();
}

int readKey()
{
    int value = 0;
    if (digitalRead(WIO_KEY_A) == LOW)
    {
        delay(10);
        if (digitalRead(WIO_KEY_A) == LOW)
        {
            while (1)
            {
                if (digitalRead(WIO_KEY_A) != LOW)
                {
                    delay(10);
                    if (digitalRead(WIO_KEY_A) != LOW)
                    {
                        value = 1;
                        break;
                    }
                }
            }
        }
    }
    else if (digitalRead(WIO_KEY_B) == LOW)
    {
        delay(10);
        if (digitalRead(WIO_KEY_B) == LOW)
        {
            while (1)
            {
                if (digitalRead(WIO_KEY_B) != LOW)
                {
                    delay(10);
                    if (digitalRead(WIO_KEY_B) != LOW)
                    {
                        value = 2;
                        break;
                    }
                }
            }
        }
    }
    else if (digitalRead(WIO_KEY_C) == LOW)
    {
        delay(10);
        if (digitalRead(WIO_KEY_C) == LOW)
        {
            while (1)
            {
                if (digitalRead(WIO_KEY_C) != LOW)
                {
                    delay(10);
                    if (digitalRead(WIO_KEY_C) != LOW)
                    {
                        value = 3;
                        break;
                    }
                }
            }
        }
    }
    return value;
}