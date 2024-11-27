#include <DHT.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

DHT dht(19, DHT11);
#define ONE_WIRE_BUS 32
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Gets air temperature, humidity, and water temperature
float getTempF() {
  return ((dht.readTemperature() * 1.8) + 32);
}

int getHumidity() {
  return dht.readHumidity();
}

float getWaterTempF() {
  return ((sensors.getTempCByIndex(0) * 1.8) + 32);
}

// Alerts for high/low temps/humidity
bool tempHighAlert = false;
bool tempLowAlert = false;
bool humHighAlert = false;
bool humLowAlert = false;
bool waterHighAlert = false;
bool waterLowAlert = false;
float prevTempF = 0;
int prevHumidity = 0;
float prevWaterTempF = 0;
unsigned long lastStatusCheck;

const char* ssid = "XXXXX";
const char* password = "XXXXX";
String BOTtoken = "XXXXX";
String chat_id = "XXXXX";

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOTtoken, secured_client);

// Checks for new messages every second
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

String giveElapsedTime(unsigned long elapsedSecs) {
  unsigned long years = elapsedSecs / 31536000;
  elapsedSecs %= 31536000;
  unsigned long months = elapsedSecs / 2592000;
  elapsedSecs %= 2592000;
  unsigned long days = elapsedSecs / 86400;
  elapsedSecs %= 86400;
  unsigned long hours = elapsedSecs / 3600;
  elapsedSecs %= 3600;
  unsigned long mins = elapsedSecs / 60;
  elapsedSecs %= 60;
  unsigned long secs = elapsedSecs;

  String elapsedTime = "";
  if (years > 0) elapsedTime += String(years) + " year" + (years != 1 ? "s" : "") + ", ";
  if (months > 0) elapsedTime += String(months) + " month" + (months != 1 ? "s" : "") + ", ";
  if (days > 0) elapsedTime += String(days) + " day" + (days != 1 ? "s" : "") + ", ";
  if (hours > 0) elapsedTime += String(hours) + " hour" + (hours != 1 ? "s" : "") + ", ";
  if (mins > 0) elapsedTime += String(mins) + " minute" + (mins != 1 ? "s" : "") + ", ";
  elapsedTime += String(secs) + " second" + (secs != 1 ? "s" : "");
  if (years > 0) elapsedTime += ". Whew! It's been a while, hasn't it?";
  return elapsedTime;
}

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i < numNewMessages; i++) {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    if (text == "/start") {
      bot.sendMessage(chat_id, "Send 'status' to check temperature and humidity");
    }

    if (text == "status") {
      unsigned long currTime = millis();
      unsigned long lastTime = (lastStatusCheck == 0) ? 0 : (currTime - lastStatusCheck) / 1000;
      String lastTimeString = giveElapsedTime(lastTime);

      String prevStatus = "Last status check: " + lastTimeString;
      prevStatus += "\nPrevious Temperature (F): ";
      prevStatus += prevTempF;
      prevStatus += "\nPrevious Humidity: ";
      prevStatus += prevHumidity;
      prevStatus += "%";
      prevStatus += "\nPrevious Water Temp (F): ";
      prevStatus += prevWaterTempF;
      prevTempF = getTempF();
      prevHumidity = getHumidity();
      prevWaterTempF = getWaterTempF();
      String STATUS = "Current Temperature (F): ";
      STATUS += getTempF();
      STATUS += "\nCurrent Humidity: ";
      STATUS += getHumidity();
      STATUS += "%";
      STATUS += "\nCurrent Water Temp (F): ";
      STATUS += getWaterTempF();
      bot.sendMessage(chat_id, prevStatus);
      bot.sendMessage(chat_id, STATUS);
      lastStatusCheck = currTime;
      delay(500);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // attempt to connect to Wifi network:
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
  bot.sendMessage(chat_id, "ESP32 connected to WiFi successfully");

  Serial.print("Retrieving time: ");
  configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);

  sensors.begin();
  dht.begin();
  delay(2000);
}

void loop() {
  sensors.requestTemperatures();

  if (millis() - lastTimeBotRan > botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    lastTimeBotRan = millis();
    getTempF();
    getHumidity();
    getWaterTempF();
    // Alerts for high air temp
    if (getTempF() >= 100 && tempHighAlert == false) {
      bot.sendMessage(chat_id, "ALERT: Air temperature is dangerously high!");
      tempHighAlert = true;
    }
    if (tempHighAlert == true && getTempF() < 100) {
      bot.sendMessage(chat_id, "Air temperature has normalized");
      tempHighAlert = false;
    }
    // Alerts for low air temp
    if (getTempF() <= 32 && tempLowAlert == false) {
      bot.sendMessage(chat_id, "ALERT: Air temperature is dangerously low!");
      tempLowAlert = true;
    }
    if (tempLowAlert == true && getTempF() > 32) {
      bot.sendMessage(chat_id, "Air temperature has normalized");
      tempLowAlert = false;
    }
    // Alerts for high humidity
    if (getHumidity() >= 95 && humHighAlert == false) {
      bot.sendMessage(chat_id, "ALERT: Air humidity is dangerously high!");
      humHighAlert = true;
    }
    if (humHighAlert == true && getHumidity() < 95) {
      bot.sendMessage(chat_id, "Air humidity has normalized");
      humHighAlert = false;
    }
    // Alerts for low humidity
    if (getHumidity() <= 40 && humLowAlert == false) {
      bot.sendMessage(chat_id, "ALERT: Air humidity is dangerously low!");
      humLowAlert = true;
    }
    if (humLowAlert == true && getHumidity() > 40) {
      bot.sendMessage(chat_id, "Air humidity has normalized");
      humLowAlert = false;
    }
    // Alerts for high water temperature
    if (getWaterTempF() >= 94 && waterHighAlert == false) {
      bot.sendMessage(chat_id, "ALERT: Water temperature is dangerously high!");
      waterHighAlert = true;
    }
    if (waterHighAlert == true && getWaterTempF() < 94) {
      bot.sendMessage(chat_id, "Water temperature has normalized");
      waterHighAlert = false;
    }
    // Alerts for low temperature water
    if (getWaterTempF() <= 40 && waterLowAlert == false) {
      bot.sendMessage(chat_id, "ALERT: Water temperature is dangerously low!");
      waterLowAlert = true;
    }
    if (waterLowAlert == true && getWaterTempF() > 40) {
      bot.sendMessage(chat_id, "Water temperature has normalized");
      waterLowAlert = false;
    }
    delay(1000);
  }
}