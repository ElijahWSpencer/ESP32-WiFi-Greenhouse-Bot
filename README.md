# ESP32 WiFi Greenhouse Bot

## A WiFi enabled ESP32 project utilizing telegram to access greenhouse temperature and humidity information from anywhere

This project was made in order to allow easy viewing of temperature and humidity information of a greenhouse aquaponics system. Through use of Telegram bot API, air temperature, humidity, and water temperature information can be received instantly from anywhere in the world.

Sending 'status' to the bot will trigger it to send the information.
<a href="https://ibb.co/rsWzBjx"><img src="https://i.ibb.co/rsWzBjx/Screenshot-20240910-100429-Telegram.jpg" alt="Screenshot-20240910-100429-Telegram" border="0"></a>

Alerts are also set to be sent when levels reach a particular high or low level. An alert is then sent again whenever levels normalize.
<a href="https://ibb.co/mqd6r6p"><img src="https://i.ibb.co/mqd6r6p/Screenshot-20240901-173901-Telegram.jpg" alt="Screenshot-20240901-173901-Telegram" border="0"></a>

## REQUIRED LIBRARIES:

* Arduino Core Library
* WiFi Library for ESP32/ESP8266
* ArduinoJson by Benoit Blanchon (ver 7.1.0)
* DHT sensor library by Adafruit (ver 1.4.6)
* OneWire by Jim Studt (ver 2.3.8)
* DallasTemperature by Miles Burton
* UniversalTelegramBot by Brian Lough (ver 1.3.0)


Here is an example of what the hardware for this project looks like
<a href="https://ibb.co/Bt0zbCR"><img src="https://i.ibb.co/Bt0zbCR/20240807-193910.jpg" alt="20240807-193910" border="0"></a>
<a href="https://ibb.co/5TBwV3q"><img src="https://i.ibb.co/5TBwV3q/20240807-193915.jpg" alt="20240807-193915" border="0"></a>

## HARDWARE USED

* ESP-WROOM-32 ESP32 Development Board
* HiLetgo DHT11 Temperature Humidity Sensor
* WWZMDiB DS18B20 Waterproof Temperature Sensor
* Smraza Double Sided PCB Prototype Board
* QILIPSU IP67 Waterproof Outdoor Junction Box
