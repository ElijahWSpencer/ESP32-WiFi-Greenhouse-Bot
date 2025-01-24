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
bool sendTempAlerts = false;
bool sendHumAlerts = false;
bool sendWaterAlerts = false;
float airTempMin = 32;
float airTempMax = 100;
int airHumMin = 40;
int airHumMax = 95;
float waterTempMin = 40;
float waterTempMax = 94;
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
      bot.sendMessage(chat_id, "Send 'status' to check temperature and humidity or /help for additional functions");
    }

    // Basic status command
    if (text == "status") {
      unsigned long currTime = millis();
      unsigned long lastTime = (lastStatusCheck == 0) ? 0 : (currTime - lastStatusCheck) / 1000;
      String lastTimeString = giveElapsedTime(lastTime);

      // Values from previous status check
      String prevStatus = "Last status check: " + lastTimeString;
      prevStatus += "\nPrevious Temperature (F): ";
      prevStatus += prevTempF;
      prevStatus += "\nPrevious Humidity: ";
      prevStatus += prevHumidity;
      prevStatus += "%";
      prevStatus += "\nPrevious Water Temp (F): ";
      prevStatus += prevWaterTempF;
      // Set prev values to current values for the next time status is called
      prevTempF = getTempF();
      prevHumidity = getHumidity();
      prevWaterTempF = getWaterTempF();
      // Values at the time of status check
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

    // Default help command
    if (text == "/help") {
      String helpCommand = "status: Gives you the current air temperature, humidity, and water temperature being detected as well as the temperatues and humidities from the last time the command was used\n";
      helpCommand += "/alerts help: Gives information on how to view the status of alerts, enable and disable alerts, as well as modify the trigger values of alerts";
      bot.sendMessage(chat_id, helpCommand);
    }

    // Command to view if alerts are enabled or disabled as well as high/low values
    if (text == "/alerts status") {
      String alertsStatus = "Air Temp Alerts: " + String(sendTempAlerts ? ("Enabled (" + String(airTempMin) + "F min, " + String(airTempMax) + "F max)") : "Disabled") + "\n";
      alertsStatus += "Humidity Alerts: " + String(sendHumAlerts ? ("Enabled (" + String(airHumMin) + "% min, " + String(airHumMax) + "% max)") : "Disabled") + "\n";
      alertsStatus += "Water Temp Alerts: " + String(sendWaterAlerts ? ("Enabled (" + String(waterTempMin) + "F min, " + String(waterTempMax) + "F max)") : "Disabled") + "\n";
      bot.sendMessage(chat_id, alertsStatus);
      delay(500);
    }

    // Command to view alert commands and formatting
    if (text == "/alerts help") {
      String alertsHelp = "/alerts status: Lists out whether each alert is enabled or disabled\n";
      alertsHelp += "/alerts enable {alert}: Enable specified alert\n";
      alertsHelp += "/alerts disable {alert}: Disable specified alert\n";
      alertsHelp += "/alerts change {alert} {min/max} {value}: Change the minimum or maximum trigger temperature of an alert\n";
      alertsHelp += "\nairtemp, airhum, and watertemp are the alerts that can be enabled, disabled or modified (e.g. /alerts enable airnum, /alerts change airhum min 50)";
      bot.sendMessage(chat_id, alertsHelp);
    }

    // Command for user to enable high/low alerts
    if (text.startsWith("/alerts enable ")) {
      String alertType = text.substring(15);
      if (alertType == "airtemp") { 
        sendTempAlerts = true;
        bot.sendMessage(chat_id, "Air temperature alerts have been enabled");
      }
      else if (alertType == "airhum") {
        sendHumAlerts = true;
        bot.sendMessage(chat_id, "Air humidity alerts have been enabled");
      }
      else if (alertType == "watertemp") {
        sendWaterAlerts = true;
        bot.sendMessage(chat_id, "Water temperature alerts have been enabled");
      }
      else {
        bot.sendMessage(chat_id, "ERROR: Unrecognized alert, send '/alerts help' to see the list of alerts");
      }
    }

    // Command for user to disable high/low alerts
    if (text.startsWith("/alerts disable ")) {
      String alertType = text.substring(16);
      if (alertType == "airtemp") {
        sendTempAlerts = false;
        bot.sendMessage(chat_id, "Air temperature alerts have been disabled");
      }
      else if (alertType == "airhum") {
        sendHumAlerts = false;
        bot.sendMessage(chat_id, "Air humidity alerts have been disabled");
      }
      else if (alertType == "watertemp") {
        sendWaterAlerts = false;
        bot.sendMessage(chat_id, "Water temperature alerts have been disabled");
      }
      else {
        bot.sendMessage(chat_id, "ERROR: Unrecognized alert, send '/alerts help' to see the list of alerts");
      }
    }

    // Command to change high/low alert values
    if (text.startsWith("/alerts change ")) {
      String command = text.substring(15);
      int firstSpace = command.indexOf(' ');
      int secondSpace = command.indexOf(' ', firstSpace + 1);
      int thirdSpace = command.indexOf(' ', secondSpace + 1);

      if (firstSpace != -1 && secondSpace != -1) {
        String alertType = command.substring(0, firstSpace);
        String minOrMax = command.substring(firstSpace + 1, secondSpace);
        String valueStr = command.substring(secondSpace + 1);
        valueStr.trim();  // Remove whitespace that may interfere with functionality
        float value = valueStr.toFloat();

        if (alertType == "airtemp") {
          if (minOrMax == "min") {
            airTempMin = value;
            bot.sendMessage(chat_id, "Air temperature minimum changed to " + String(value) + "F");
          } 
          else if (minOrMax == "max") {
            airTempMax = value;
            bot.sendMessage(chat_id, "Air temperature maximum changed to " + String(value) + "F");
          } 
          else {
            bot.sendMessage(chat_id, "ERROR: Unrecognized paramenters. Please specify either 'min' or 'max' as the value to be modified");
          }
        } 
        else if (alertType == "airhum") {
          if (minOrMax == "min") {
            airHumMin = value;
            bot.sendMessage(chat_id, "Air humidity minimum changed to " + String(airHumMin) + "%");
          } 
          else if (minOrMax == "max") {
            airHumMax = value;
            bot.sendMessage(chat_id, "Air humidity maximum changed to " + String(airHumMax) + "%");
          } 
          else {
            bot.sendMessage(chat_id, "ERROR: Unrecognized paramenters. Please specify either 'min' or 'max' as the value to be modified");
          }
        } 
        else if (alertType == "watertemp") {
          if (minOrMax == "min") {
            waterTempMin = value;
            bot.sendMessage(chat_id, "Water temperature minimum changed to " + String(value) + "F");
          } 
          else if (minOrMax == "max") {
            waterTempMax = value;
            bot.sendMessage(chat_id, "Water temperature maximum changed to " + String(value) + "F");
          } 
          else {
            bot.sendMessage(chat_id, "ERROR: Unrecognized paramenters. Please specify either 'min' or 'max' as the value to be modified");
          }
        } 
        else {
          bot.sendMessage(chat_id, "ERROR: Unrecognized alert type. Send '/alerts help' for more information");
        }
      } 
      else {
        bot.sendMessage(chat_id, "ERROR: Unrecognized command format. Use '/alerts help' for more information.");
      }
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
    if (getTempF() >= airTempMax && tempHighAlert == false) {
      if (sendTempAlerts == true) bot.sendMessage(chat_id, "ALERT: Air temperature is dangerously high!");
      tempHighAlert = true;
    }
    if (tempHighAlert == true && getTempF() < airTempMax) {
      if (sendTempAlerts == true) bot.sendMessage(chat_id, "Air temperature has normalized");
      tempHighAlert = false;
    }
    // Alerts for low air temp
    if (getTempF() <= airTempMin && tempLowAlert == false) {
      if (sendTempAlerts == true) bot.sendMessage(chat_id, "ALERT: Air temperature is dangerously low!");
      tempLowAlert = true;
    }
    if (tempLowAlert == true && getTempF() > airTempMin) {
      if (sendTempAlerts == true) bot.sendMessage(chat_id, "Air temperature has normalized");
      tempLowAlert = false;
    }
    // Alerts for high humidity
    if (getHumidity() >= airHumMax && humHighAlert == false) {
      if (sendHumAlerts == true) bot.sendMessage(chat_id, "ALERT: Air humidity is dangerously high!");
      humHighAlert = true;
    }
    if (humHighAlert == true && getHumidity() < airHumMax) {
      if (sendHumAlerts == true) bot.sendMessage(chat_id, "Air humidity has normalized");
      humHighAlert = false;
    }
    // Alerts for low humidity
    if (getHumidity() <= airHumMin && humLowAlert == false) {
      if (sendHumAlerts == true) bot.sendMessage(chat_id, "ALERT: Air humidity is dangerously low!");
      humLowAlert = true;
    }
    if (humLowAlert == true && getHumidity() > airHumMin) {
      if (sendHumAlerts == true) bot.sendMessage(chat_id, "Air humidity has normalized");
      humLowAlert = false;
    }
    // Alerts for high water temperature
    if (getWaterTempF() >= waterTempMax && waterHighAlert == false) {
      if (sendWaterAlerts == true) bot.sendMessage(chat_id, "ALERT: Water temperature is dangerously high!");
      waterHighAlert = true;
    }
    if (waterHighAlert == true && getWaterTempF() < waterTempMax) {
      if (sendWaterAlerts == true) bot.sendMessage(chat_id, "Water temperature has normalized");
      waterHighAlert = false;
    }
    // Alerts for low temperature water
    if (getWaterTempF() <= waterTempMin && waterLowAlert == false) {
      if (sendWaterAlerts == true) bot.sendMessage(chat_id, "ALERT: Water temperature is dangerously low!");
      waterLowAlert = true;
    }
    if (waterLowAlert == true && getWaterTempF() > waterTempMin) {
      if (sendWaterAlerts == true) bot.sendMessage(chat_id, "Water temperature has normalized");
      waterLowAlert = false;
    }
    delay(1000);
  }
}
