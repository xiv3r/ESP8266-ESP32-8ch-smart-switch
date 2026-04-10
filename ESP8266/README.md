# Requirements
- 8 channel relay 
- ESP8266 V1 12*
- Dupont wire
- Android Phone
- USB OTG

# Installation
1. [Download](https://github.com/xiv3r/ESP8266-ESP32-8ch-smart-switch/releases/download/flasher/ESPFlash.xapk) the apps from the release and install
2. Download the [firmware.bin](https://github.com/xiv3r/ESP8266-ESP32-8ch-smart-switch/raw/refs/heads/main/ESP8266/ESP8266-8CH-firmware-0x0.bin) and import in the app. Firmware flash offset is `0x0`
- Linux/Windows Esptool flash command
```
esptool.py --port COM3 --baud 115200 write_flash 0x0 ESP8266-8CH-firmware-0x0.bin
```
3. Enable otg from android settings and connect the otg cable to the esp8266
4. Flash the firmware in the espflash app
5. Connect to the wifi ssid:`ESP8266_8CH_Smart_Switches` password:`ESP8266-admin`
6. Open the web browser and type:`192.168.4.1`

# Diagram
```
RELAY        ESP8266

VCC __________ VIN ==>  + 5vDC
IN8 __________ RX
IN7 __________ D7
IN6 __________ D6
IN5 __________ D5
IN4 __________ D4
IN3 __________ D3
IN2 __________ D2
IN1 __________ D1
GND __________ GND ==>  - 5vDC
```
<img src="https://github.com/xiv3r/ESP8266-ESP32-8ch-smart-switch/blob/main/ESP8266/ESP8266_Pinout.jpg">
