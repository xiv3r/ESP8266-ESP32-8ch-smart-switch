# Requirements
- 8-Channel Relay 
- ESP32 30/38 Pins Devkit
- Dupont Wire

# Installation 
1. Download the [firmware.bin](https://github.com/xiv3r/ESP8266-ESP32-8ch-smart-switch/blob/main/ESP32/ESP32-8CH-firmware-0x0.bin) and import. flash offset is `0x0`
- Linux/Windows esptool flash command
```
esptool.py --port COM3 --baud 115200 write_flash 0x0 ESP32-8CH-firmware-0x0.bin
```
2. Connect to the wifi ssid:`ESP32_8CH_Smart_Switches` password:`ESP32-admin`
3. Open the web browser and type:`192.168.4.1`

# Schematic
```
RELAY        ESP32

VCC __________ VIN ==>  + 5vDC
IN8 __________ D23
IN7 __________ D32
IN6 __________ D33
IN5 __________ D25
IN4 __________ D26
IN3 __________ D27
IN2 __________ D11
IN1 __________ D13
GND __________ GND ==>  - 5vDC
```

<img src="https://github.com/xiv3r/ESP8266-ESP32-8ch-smart-switch/blob/main/ESP32/ESP32_Pinout.jpg">
