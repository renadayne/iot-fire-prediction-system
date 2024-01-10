#define BLYNK_TEMPLATE_ID "TMPL68p7Kt_62"
#define BLYNK_TEMPLATE_NAME "IOT"
#define BLYNK_AUTH_TOKEN "neW9o0OAFcfoGE86stgBMEvmz7fBO0W1"
#define BLYNK_PRINT Serial

#include <BlynkSimpleEsp8266.h>

#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "DHT.h"
const int DHTTYPE = DHT11;
DHT dht1(4, DHTTYPE);
DHT dht2(0, DHTTYPE);

const char *ssid = "khactinhgioitre";
const char *password = "12356789";
const char *mqtt_server = "192.168.0.18";
const int mqtt_port = 1883;
const char *mqtt_id = "esp8266";
const char *topic_subcribe = "to-esp8266";
const char *topic_publish = "from-esp8266";
unsigned long lastMsg = 0;
String myTempString1;
String myTempString2;
String temp_data1;
String temp_data2;
String gas_data1;
String gas_data2;

BlynkTimer timer;

WiFiClient client;
PubSubClient mqtt_client(client);

void myTimer() 
{
  
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Recived data from: ");
  Serial.println(topic);
  Serial.print("Message: ");
  for (size_t i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  Serial.println("------------------------------------------");
}

void setup()
{
  Serial.begin(9600);
  dht1.begin();
  dht2.begin();
  Serial.print("Connecting to Wifi... ");
  Serial.print(ssid);
  Serial.println();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.print("Connected to Wifi ");
  Serial.println(ssid);
  Serial.print("IP Address: ");
  Serial.print(WiFi.localIP());
  Serial.println();
  delay(1000);

  mqtt_client.setServer(mqtt_server, mqtt_port);
  mqtt_client.setCallback(callback);

  Serial.println("Connecting to mqtt... ");
  while (!mqtt_client.connect(mqtt_id))
  {
    delay(500);
  }
  Serial.println("Connected to mqtt ");
  mqtt_client.subscribe("to-esp8266");
  mqtt_client.publish("from-esp8266 temp_data1", temp_data1.c_str());
  mqtt_client.publish("from-esp8266 temp_data2", temp_data2.c_str());
  mqtt_client.publish("from-esp8266 gas_data1", gas_data1.c_str());
  mqtt_client.publish("from-esp8266 gas_data2", gas_data2.c_str());

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
  timer.setInterval(1000L, myTimer);
}

int previousValue1 = 0, previousValue2 = 0;
int currentValue1, currentValue2;
int check1 = 0, check2 = 0;
int sum1 = 0, sum2 = 0;

void check_fire1()
{
  Serial.println();
  Serial.println("Location 1");
  if(currentValue1 > previousValue1){
    if(previousValue1 != 0) {
    sum1 += (currentValue1 - previousValue1);}
  }
  else if(sum1 >= 1) {  
      sum1 -= (previousValue1 - currentValue1);
      if(sum1 < 0) sum1 = 0;
  }
  Serial.print("Warning level: ");
  Serial.println(sum1);
  if (sum1 >= 3){
    check1 = 1;
  }
  else {  check1 = 0;}
  previousValue1 = currentValue1;
  if(check1 == 1) {
    int a = random(390, 410);
    // String b = String(a);
    gas_data1 = "{\"smoke\":" + String(a) + "}";
    mqtt_client.publish("from-esp8266 gas_data1", gas_data1.c_str());
    Serial.print("Fire in the floor 1");
    Blynk.logEvent("fire_alarm_floor1");
  }
  else {
    int a = random(80, 120);
    String b = String(a);
    gas_data1 = "{\"smoke\":" + b + "}";
    mqtt_client.publish("from-esp8266 gas_data1", gas_data1.c_str());
  }
}

void check_fire2()
{
  Serial.println();
  Serial.println("Location 2");
  if(currentValue2 > previousValue2){
    if(previousValue2 != 0) {
    sum2 += (currentValue2 - previousValue2);}
  }
  else if(sum2 >= 1) {  
      sum2 -= (previousValue2 - currentValue2);
      if(sum2 < 0) sum2 = 0;
  }
  Serial.print("Warning level: ");
  Serial.println(sum2);
  if (sum2 >= 3){
    check2 = 1;
  }
  else {  check2 = 0;}
  previousValue2 = currentValue2;
  if(check2 == 1) {
    int a = random(390, 410);
    // String b = String(a);
    gas_data2 = "{\"smoke\":" + String(a) + "}";
    mqtt_client.publish("from-esp8266 gas_data2", gas_data2.c_str());
    Serial.print("Fire in the floor 2");
    Blynk.logEvent("fire_alarm_floor2");
  }
  else{
    int a = random(80, 120);
    String b = String(a);
    gas_data2 = "{\"smoke\":" + b + "}";
    mqtt_client.publish("from-esp8266 gas_data2", gas_data2.c_str());
  }
}

void loop()
{
  int temp1 = dht1.readTemperature();
  int temp2 = dht2.readTemperature();
  //check
  currentValue1 = temp1;
  currentValue2 = temp2;
  check_fire1();
  check_fire2();
  myTempString1 = String(temp1);
  myTempString2 = String(temp2);
  temp_data1 = "{\"temperature\":" + myTempString1 + "}";
  // Serial.println(temp_data1);
  temp_data2 = "{\"temperature\":" + myTempString2 + "}";
  // Serial.println(temp_data2);
  delay(2000);

  mqtt_client.loop();
  unsigned long now = millis();

  if (now - lastMsg > 1000) {
    lastMsg = now;
    //convert temp to char
    mqtt_client.publish("from-esp8266 temp_data1", temp_data1.c_str());
    mqtt_client.publish("from-esp8266 temp_data2", temp_data2.c_str());
  }

  Blynk.run();
  
  timer.run();
}
////////////////////////////////////////