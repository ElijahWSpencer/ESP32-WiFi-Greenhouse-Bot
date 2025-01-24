# ESP32 WiFi Greenhouse Bot

## A WiFi enabled ESP32 project utilizing telegram to access greenhouse temperature and humidity information from anywhere

This project was made in order to allow easy viewing of temperature and humidity information of a greenhouse aquaponics system. Through use of Telegram bot API, air temperature, humidity, and water temperature information can be received instantly from anywhere in the world.

Sending **'status'** to the bot will trigger it to send the information.

![390155201-ae58207b-ae52-4448-9a92-04d56df3c3e9](https://github.com/user-attachments/assets/33dc756d-efa7-4bec-9bc6-eb748a86f9ed)

Alerts are also set to be sent when levels reach a particular high or low level. An alert is then sent again whenever levels normalize.

![366109144-0fe25a16-532b-47dc-902c-21e7a8b3d357](https://github.com/user-attachments/assets/2553dccc-97b8-4d7d-b230-16b79c5fb51b)

Users have the capability to choose which alerts they wish to have enabled and disabled, as well as what they want the minimum and maximum trigger values to be for these alerts. Further information on setting these values is given with the in-app command **/alerts help**

![391294071-06fc0f9a-2655-4df2-abc5-5c62caca6825](https://github.com/user-attachments/assets/e87b3427-0faa-432d-b8bb-ecf08816ae19)

## REQUIRED LIBRARIES:

* Arduino Core Library
* WiFi Library for ESP32/ESP8266
* ArduinoJson by Benoit Blanchon (ver 7.1.0)
* DHT sensor library by Adafruit (ver 1.4.6)
* OneWire by Jim Studt (ver 2.3.8)
* DallasTemperature by Miles Burton
* UniversalTelegramBot by Brian Lough (ver 1.3.0)


Here is an example of what the hardware for this project looks like

![366109970-a13f7632-ebe1-4441-b7a9-5b3d2256016f](https://github.com/user-attachments/assets/ed9a4ff8-2755-4de9-8188-53c10201fb9e)

![366110122-388c4117-0072-43f4-988c-48b49d355001](https://github.com/user-attachments/assets/e57160f8-d6ff-490b-bb23-ab9e89670352)


## HARDWARE USED

* ESP-WROOM-32 ESP32 Development Board
* HiLetgo DHT11 Temperature Humidity Sensor
* WWZMDiB DS18B20 Waterproof Temperature Sensor
* Smraza Double Sided PCB Prototype Board
* QILIPSU IP67 Waterproof Outdoor Junction Box
