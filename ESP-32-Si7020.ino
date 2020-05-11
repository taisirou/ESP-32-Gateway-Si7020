/*
   Ambientのサンプルを基に製作しています。
   https://ambidata.io/samples/m5stack/m5stack-arduino/
   M5StackとBME280をI2C接続し、温度、湿度、気圧を測定しプリントアプトする
*/
#include "Adafruit_Si7021.h"
#include <M5Stack.h>
#include <Wire.h>
#include "Ambient.h"

#define PERIOD 60
Adafruit_Si7021 sensor = Adafruit_Si7021();

WiFiClient client;
Ambient ambient;

const char* ssid = "SSID";
const char* password = "password";

unsigned int channelId = channelId; // AmbientのチャネルID
const char* writeKey = "writeKey"; // ライトキー

void setup() {
  M5.begin();//M5Stackの初期化ルーチン
  Wire.begin(); // I2Cの初期化
  Serial.begin(115200);
  delay(100);
  Serial.println("\r\n　ESP-32+Si7020->Ambient test");

  WiFi.begin(ssid, password);  //  Wi-Fi APに接続
  while (WiFi.status() != WL_CONNECTED) {  //  Wi-Fi AP接続待ち
    delay(100);
  }

  Serial.print("WiFi connected\r\nIP address: ");
  Serial.println(WiFi.localIP());

  pinMode(21, INPUT_PULLUP); //デファルトのSDAピン21　のプルアップの指定
  pinMode(22, INPUT_PULLUP); //デファルトのSDAピン22　のプルアップの指定

  // wait for serial port to open
  while (!Serial) {
    delay(10);
  }

  Serial.println("Si7021 test!");

  if (!sensor.begin()) {
    Serial.println("Did not find Si7021 sensor!");
    while (true)
      ;
  }
  Serial.print("Found model ");
  switch (sensor.getModel()) {
    case SI_Engineering_Samples:
      Serial.print("SI engineering samples"); break;
    case SI_7013:
      Serial.print("Si7013"); break;
    case SI_7020:
      Serial.print("Si7020"); break;
    case SI_7021:
      Serial.print("Si7021"); break;
    case SI_UNKNOWN:
    default:
      Serial.print("Unknown");
  }
  Serial.print(" Rev(");
  Serial.print(sensor.getRevision());
  Serial.print(")");
  Serial.print(" Serial #"); Serial.print(sensor.sernum_a, HEX); Serial.println(sensor.sernum_b, HEX);

  ambient.begin(channelId, writeKey, &client); // チャネルIDとライトキーを指定してAmbientの初期化
}

void loop() {
  int t = millis();
  float temp, humid;

  // BME280で温度、湿度、気圧を測定する
  temp = (float)sensor.readTemperature();
  humid = (float)sensor.readHumidity();

  // 温度、湿度、気圧、CO2、TVOCの値をAmbientに送信する
  ambient.set(1, String(temp).c_str());
  ambient.set(2, String(humid).c_str());
  ambient.send();
  Serial.print("Humidity:    ");
  Serial.print(sensor.readHumidity(), 2);
  Serial.print("\tTemperature: ");
  Serial.println(sensor.readTemperature(), 2);
  t = millis() - t;
  t = (t < PERIOD * 1000) ? (PERIOD * 1000 - t) : 1;
  delay(t);
}
