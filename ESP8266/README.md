# Requirements
- 8 channel relay 
- ESP8266 V1 12*
- Dupont wire
- Android Phone
- USB OTG

# Flashing 
1. [Download](https://github.com/xiv3r/ESP8266-ESP32-8ch-smart-switch/releases/download/flasher/ESPFlash.xapk) the apps from the release and install
2. Download the [firmware.bin](https://github.com/xiv3r/ESP8266-ESP32-8ch-smart-switch/raw/refs/heads/main/ESP8266/esp8266-8ch-firmware-0x0.bin) and import in the app. Firmware flash offset is `0x0`
3. Enable otg from android settings and connect the otg cable to the esp8266
4. Flash the firmware in the espflash app
5. Connect to the wifi ssid:`ESP8266_8CH_Smart_Switches` password:`ESP8266-admin`
6. Open the web browser and type:`192.168.4.1`

# Diagram
<img src="https://github.com/xiv3r/ESP8266-ESP32-8ch-smart-switch/blob/main/ESP8266/ESP8266_Pinout.jpg">
