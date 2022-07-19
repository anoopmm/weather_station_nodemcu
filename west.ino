#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClient.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include "icons.h"

Adafruit_PCD8544 display = Adafruit_PCD8544(14, 13, 5, 15, 4); /*D5, D7, D1, D8, D2 */
int contrastValue = 60; /* Default Contrast Value */
const char* ssid = "xxxxxx";
const char* password = "xxxx";
WiFiClient wifiClient;
void setIcon(String iconId) {
  Serial.println(iconId);
  String id = iconId.substring(0, 2);
  String dayOrNight = iconId.substring(2, 1);
  Serial.println(id);
  int idInt = atoi(id.c_str());
  Serial.println(idInt);
  switch (idInt) {

    case 1:
      if (dayOrNight == "n")
        display.drawBitmap(60, 10, night_clear, 24, 24, BLACK);
      else
        display.drawBitmap(60, 10, day_clear, 24, 24, BLACK);
      break;
    case 2:
      if (dayOrNight == "n")
        display.drawBitmap(60, 10, night_cloudy, 24, 24, BLACK);
      else
        display.drawBitmap(60, 10, day_cloudy, 24, 24, BLACK);
      break;
    case 3:
      display.drawBitmap(60, 10, cloudy, 24, 24, BLACK);
      break;
    case 4:
      display.drawBitmap(60, 10, cloudy, 24, 24, BLACK);
      break;
    case 9:
      display.drawBitmap(60, 10, shower, 24, 24, BLACK);
      break;
    case 10:
      display.drawBitmap(60, 10, rain, 24, 24, BLACK);
      break;
    case 11:
      display.drawBitmap(60, 10, thunder, 24, 24, BLACK);
      break;
    case 13:
      display.drawBitmap(60, 10, snow, 24, 24, BLACK);
      break;
    case 50:
      display.drawBitmap(60, 10, mist, 24, 24, BLACK);
      break;
    default :
      display.drawBitmap(60, 10, cloudy, 24, 24, BLACK);
  }

}
//to parse json data recieved from OWM
void parseJson(const String & jsonString) {
  //StaticJsonBuffer<4000> jsonBuffer;
  const size_t bufferSize = 2 * JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(2) + 4 * JSON_OBJECT_SIZE(1) + 3 * JSON_OBJECT_SIZE(2) + 3 * JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + 2 * JSON_OBJECT_SIZE(7) + 2 * JSON_OBJECT_SIZE(8) + 720;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  // FIND FIELDS IN JSON TREE
  JsonObject& root = jsonBuffer.parseObject(jsonString);
  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }

  JsonArray& list = root["list"];
  JsonObject& nowT = list[0];
  JsonObject& later = list[1];


  String city = root["city"]["name"];
  Serial.print("City:-");
  Serial.println(city);
  float tempNow = nowT["main"]["temp"];
  Serial.print("Temp Now:-");
  Serial.println(tempNow);
  float humidityNow = nowT["main"]["humidity"];
  Serial.print("humidityNow:-");
  Serial.println(humidityNow);
  int pressure = nowT["main"]["pressure"];
  Serial.print("pressure:-");
  Serial.println(pressure);
  String weatherNow = nowT["weather"][0]["description"];
  String iconId = nowT["weather"][0]["icon"];
  Serial.print("weatherNow:-");
  Serial.println(weatherNow);

  //display data on screen

  display.clearDisplay();
  setIcon(iconId);
  display.drawBitmap(76, 1, wifi, 7, 7, BLACK);
  display.drawRoundRect(0, 8, 56, 32, 2, BLACK);
  display.setTextColor(BLACK);
  display.setTextSize(1);
  display.setCursor(2, 0);
  display.println(city);
  display.setTextColor(BLACK);
  display.setTextSize(1);
  display.setCursor(2, 10);
  display.println("T: ");
  display.setCursor(12, 10);
  display.println(tempNow);
  display.setCursor(42, 10);
  display.print((char)247);
  display.setCursor(48, 10);
  display.println("C");
  display.setCursor(2, 20);
  display.println("H: ");
  display.setCursor(12, 20);
  display.println(humidityNow);
  display.setCursor(46, 20);
  display.println("%");
  display.setCursor(2, 30);
  display.println("P: ");
  display.setCursor(12, 30);
  display.println(pressure);
  display.setCursor(37, 30);
  display.println("hpa");
  display.setCursor(0, 41);
  display.println(weatherNow);
  display.display();
}

void setup()
{
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  /* Initialize the Display*/
  display.begin();

  /* Change the contrast using the following API*/
  display.setContrast(contrastValue);

  /* Clear the buffer */
  display.clearDisplay();
  display.display();
  delay(100);
  display.clearDisplay();
  display.setTextColor(BLACK);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Connecting.....");
    display.print(".");
    display.display();
    delay(1000);
  }
}

void loop()
{

  if (WiFi.status() == WL_CONNECTED)
  {
    display.clearDisplay();
    display.setCursor(0, 20);
    display.println("Fetching...");
    display.drawBitmap(76, 1, wifi, 7, 7, BLACK);
    display.display();
    delay(1000);
    HTTPClient http; //Object of class HTTPClient
    http.begin(wifiClient, "http://api.openweathermap.org/data/2.5/forecast?q=Kochi&APPID=xxxxxxxxxxxxxx&mode=json&units=metric&cnt=1");
    int httpCode = http.GET();
    Serial.println(httpCode);
    ///Serial.println(http.getString());
    if (httpCode == 200) {
      parseJson(http.getString());
    }
  }
  delay(30000);
}
