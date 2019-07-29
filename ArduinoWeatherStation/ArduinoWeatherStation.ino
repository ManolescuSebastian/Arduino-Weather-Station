#include <SPI.h>
#include <Wire.h>

//TFT display
#include "Adafruit_GFX.h"
#include <Adafruit_ST7735.h>

//wifi module
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include "WeatherData.h"

// Color definitions
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0
#define WHITE    0xFFFF
#define GREY     0xC618

// Define pins that will be used as parameters in the TFT constructor
#define cs   D8
#define dc   D3
#define rst  D4

WeatherData weatherData = WeatherData();

const String API_KEY = ""; //add your own accuweather API KEY
String city = "Frankfurt "; // add your prefered city
String units = "metric"; // (options: metric/imperial )

const char* ssid = ""; //TODO REMOVE before push to github
const char* password = ""; //TODO REMOVE before push to github

// Instantiate the TFT constructor with the pin values defined above
Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, rst);

long weatherDataTimer = 0;

void setup () {

  Serial.begin(115200);

  Wire.begin();

  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.fillScreen(BLACK);
  tft.setCursor(30, 80);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.print("Connecting...");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print("Connecting..");
  }

  tft.fillScreen(BLACK);
  tft.setCursor(30, 80);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.print("Connected");

  initWeather();
}

void loop() {
  if (millis() - weatherDataTimer > 60000) {
    initWeather();
    weatherDataTimer = millis();
  }
}

void initWeather() {
  if (WiFi.status() != WL_CONNECTED) {
    tft.fillScreen(BLACK);
    tft.setCursor(30, 80);
    tft.setTextColor(WHITE);
    tft.setTextSize(1);
    tft.print("Disconnected");
  } else {
    getWeatherData();
  }
}


void getWeatherData() {
  String url = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "&units=" + units + "&APPID=" + API_KEY;

  HTTPClient http;  //Declare an object of class HTTPClient
  http.begin(url);  //Specify request destination
  int httpCode = http.GET();//Send the request
  if (httpCode > 0) { //Check t he returning code
    String payload = http.getString();   //Get the request response payload
    //parse data
    parseWeatherData(payload);
  }
  http.end();//Close connection
}

/**
 * Comverted json data using 
 * https://arduinojson.org/v5/assistant/
 */
void parseWeatherData(String payload) {
  const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + 2 * JSON_OBJECT_SIZE(2) + 2 * JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(7) + JSON_OBJECT_SIZE(11) + 500;
  DynamicJsonBuffer jsonBuffer(capacity);

  JsonObject& root = jsonBuffer.parseObject(payload);

  float coord_lon = root["coord"]["lon"]; // 25.61
  float coord_lat = root["coord"]["lat"]; // 45.65

  JsonObject& weather_0 = root["weather"][0];
  int weather_0_id = weather_0["id"]; // 803
  const char* weather_0_main = weather_0["main"]; // "Clouds"
  const char* weather_0_description = weather_0["description"]; // "broken clouds"
  const char* weather_0_icon = weather_0["icon"]; // "04d"

  const char* base = root["base"]; // "stations"

  JsonObject& main = root["main"];
  float main_temp = main["temp"]; // -6.04
  float main_pressure = main["pressure"]; // 1036.21
  int main_humidity = main["humidity"]; // 65
  float main_temp_min = main["temp_min"]; // -6.04
  float main_temp_max = main["temp_max"]; // -6.04
  float main_sea_level = main["sea_level"]; // 1036.21
  float main_grnd_level = main["grnd_level"]; // 922.42

  float wind_speed = root["wind"]["speed"]; // 1.21
  float wind_deg = root["wind"]["deg"]; // 344.501

  int clouds_all = root["clouds"]["all"]; // 68

  long dt = root["dt"]; // 1551023165

  JsonObject& sys = root["sys"];
  float sys_message = sys["message"]; // 0.0077
  const char* sys_country = sys["country"]; // COUNTRY
  long sys_sunrise = sys["sunrise"]; // 1550984672
  long sys_sunset = sys["sunset"]; // 1551023855

  long id = root["id"]; // 683844
  const char* cityName = root["name"]; // CITY
  int cod = root["cod"]; // 200

  tft.fillScreen(BLACK);

  getCurrentTimeRequest(coord_lat, coord_lon);

  displayTemperature(main_temp);
  displayIcon(weather_0_icon);
  displayDescription(weather_0_description);
  displaySunriseTime(sys_sunrise);
  displaySunsetTime(sys_sunset);
  displayLocation(cityName);

  delay(5000);
}

void getCurrentTimeRequest(float latitude, float longitude) {
  String url = "http://api.geonames.org/timezoneJSON?lat=" + String(latitude) + "&lng=" + String(longitude) + "&username=kode";

  HTTPClient http;  //Declare an object of class HTTPClient
  http.begin(url);  //Specify request destination
  int httpCode = http.GET();//Send the request
  if (httpCode > 0) { //Check the returning code
    String payload = http.getString();   //Get the request response payload
    //parse data
    parseTimeData(payload);
  }
  http.end();   //Close connection
}

void parseTimeData(String payload) {
  const size_t capacity = JSON_OBJECT_SIZE(11) + 220;
  DynamicJsonBuffer jsonBuffer(capacity);

  JsonObject& root = jsonBuffer.parseObject(payload);

  const char* sunrise = root["sunrise"]; // "2019-03-08 06:44"
  float lng = root["lng"]; // 25.61
  const char* countryCode = root["countryCode"]; // "RO"
  int gmtOffset = root["gmtOffset"]; // 2
  int rawOffset = root["rawOffset"]; // 2
  const char* sunset = root["sunset"]; // "2019-03-08 18:13"
  const char* timezoneId = root["timezoneId"]; // "Europe/Bucharest"
  int dstOffset = root["dstOffset"]; // 3
  const char* countryName = root["countryName"]; // "Romania"
  const char* currentTime = root["time"]; // "2019-03-08 01:11"
  float lat = root["lat"]; // 45.65

  Serial.print(currentTime);

  displayCurrentTime(currentTime);
}

// TIME
void displayCurrentTime(String currentTime) {
  tft.setTextSize(3);
  tft.setCursor(1, 70);

  String timeOnly = currentTime.substring(10);
  tft.print(timeOnly);
}

// TEMPERATURE
void displayTemperature(float main_temp) {
  tft.setTextColor(WHITE);
  tft.setTextSize(2.5);

  tft.setCursor(5, 20);
  String temperatureValue = String((int)main_temp) + (char)247 + "C";
  tft.print(temperatureValue);
}

// ICON
void displayIcon(String weatherIcon) {
  tft.setTextSize(1);
  tft.drawBitmap(75, 5, weatherData.GetIcon(weatherIcon) , 50, 50, WHITE);
}

// DESCRIPTION
void displayDescription(String weatherDescription) {
  tft.setTextSize(1);
  tft.setCursor(5, 50);
  if(weatherDescription.length() > 18){
   weatherDescription = weatherDescription.substring(0, 15) + "...";
  }
  String description = String(weatherDescription);
  tft.print(description);
}

// SUNRISE
void displaySunriseTime(long sys_sunrise) {

  sys_sunrise = sys_sunrise + 7200 + 3600;//ADD 2 Hours (For GMT+2)

  int hr = (sys_sunrise  % 86400L) / 3600;
  int minute = (sys_sunrise % 3600) / 60;
  int sec = (sys_sunrise % 60);

  String sunriseHour;
  String sunriseMinute;

  if (hr < 10) {
    sunriseHour = "0" + String(hr);
  } else {
    sunriseHour = String(hr);
  }

  if (minute < 10) {
    sunriseMinute = "0" + String(minute);
  } else {
    sunriseMinute = String(minute);
  }

  tft.setTextSize(1);
  tft.setCursor(5, 105);

  String sunrise = "Sunrise: " + sunriseHour + " : " + sunriseMinute;
  tft.print(sunrise);
}


// SUNSET
void displaySunsetTime(long sys_sunset) {

  sys_sunset = sys_sunset + 7200 + 3600;//ADD 2 Hours (For GMT+2)

  int sunset_hr = (sys_sunset  % 86400L) / 3600;
  int sunset_minute = (sys_sunset % 3600) / 60;
  int sunset_sec = (sys_sunset % 60);

  String sunsetHour;
  String sunsetMinute;

  if (sunset_hr < 10) {
    sunsetHour = "0" + String(sunset_hr);
  } else {
    sunsetHour = String(sunset_hr);
  }

  if (sunset_minute < 10) {
    sunsetMinute = "0" + String(sunset_minute);
  } else {
    sunsetMinute = String(sunset_minute);
  }

  tft.setTextSize(1);
  tft.setCursor(5, 115);

  String sunset = "Sunset: " + sunsetHour + " : " + sunsetMinute;
  tft.print(sunset);
}

// LOCATION
void displayLocation(String cityName) {
  tft.setTextSize(1);
  tft.setCursor(5, 140);
  String loc = "City: " + String(cityName);
  tft.print(loc);
}
