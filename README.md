# ESP32 WiFi Greenhouse Bot

## A WiFi enabled ESP32 project utilizing telegram to access greenhouse temperature and humidity information from anywhere

This project was made in order to allow easy viewing of temperature and humidity information of a greenhouse aquaponics system. Through use of Telegram bot API, air temperature, humidity, and water temperature information can be received instantly from anywhere in the world.

Sending **'status'** to the bot will trigger it to send the information.

![Screenshot_20241126_160417_Telegram](https://github.com/user-attachments/assets/ae58207b-ae52-4448-9a92-04d56df3c3e9)


Alerts are also set to be sent when levels reach a particular high or low level. An alert is then sent again whenever levels normalize.

![Screenshot_20240901_173901_Telegram](https://github.com/user-attachments/assets/0fe25a16-532b-47dc-902c-21e7a8b3d357)

Users have the capability to choose which alerts they wish to have enabled and disabled, as well as what they want the minimum and maximum trigger values to be for these alerts. Further information on setting these values is given with the in-app command **/alerts help**

![Screenshot_20241130_152539_Telegram](https://github.com/user-attachments/assets/06fc0f9a-2655-4df2-abc5-5c62caca6825)


## REQUIRED LIBRARIES:

* Arduino Core Library
* WiFi Library for ESP32/ESP8266
* ArduinoJson by Benoit Blanchon (ver 7.1.0)
* DHT sensor library by Adafruit (ver 1.4.6)
* OneWire by Jim Studt (ver 2.3.8)
* DallasTemperature by Miles Burton
* UniversalTelegramBot by Brian Lough (ver 1.3.0)


Here is an example of what the hardware for this project looks like

![20240807_193910](https://github.com/user-attachments/assets/a13f7632-ebe1-4441-b7a9-5b3d2256016f)

![20240807_193915](https://github.com/user-attachments/assets/388c4117-0072-43f4-988c-48b49d355001)

## HARDWARE USED

* ESP-WROOM-32 ESP32 Development Board
* HiLetgo DHT11 Temperature Humidity Sensor
* WWZMDiB DS18B20 Waterproof Temperature Sensor
* Smraza Double Sided PCB Prototype Board
* QILIPSU IP67 Waterproof Outdoor Junction Box
