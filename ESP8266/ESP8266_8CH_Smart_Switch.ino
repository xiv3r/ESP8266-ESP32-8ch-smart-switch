#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include "index.h"

#define EEPROM_SIZE 512
ESP8266WebServer server(80);
DNSServer dnsServer;

int relay1 = 5, relay2 = 4, relay3 = 0, relay4 = 2;
int relay5 = 14, relay6 = 12, relay7 = 13, relay8 = 3;
int state1 = HIGH, state2 = HIGH, state3 = HIGH, state4 = HIGH;
int state5 = HIGH, state6 = HIGH, state7 = HIGH, state8 = HIGH;

// Default relay names
String relayNames[8] = {
  "Relay 1", "Relay 2", "Relay 3", "Relay 4",
  "Relay 5", "Relay 6", "Relay 7", "Relay 8"
};

// WiFi credentials
String wifiSsid = "ESP8266_8CH_Smart_Switches";
String wifiPassword = "ESP8266-admin";

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

// EEPROM addresses
// 0: reserved
// 1-8: relay states
// 9-188: relay names (20 chars each * 8 relays = 160 bytes)
// 189-220: WiFi SSID (32 bytes max)
// 221-284: WiFi Password (64 bytes max)
#define EEPROM_NAMES_START 9
#define EEPROM_WIFI_SSID_START 189
#define EEPROM_WIFI_PASS_START 221
#define MAX_NAME_LENGTH 20
#define MAX_SSID_LENGTH 32
#define MAX_PASS_LENGTH 64

// Captive portal handler
bool captivePortal() {
  if (!isIp(server.hostHeader()) && server.hostHeader() != (String("192.168.4.1"))) {
    server.sendHeader("Location", String("http://192.168.4.1"), true);
    server.send(302, "text/plain", "");
    server.client().stop();
    return true;
  }
  return false;
}

bool isIp(String str) {
  for (size_t i = 0; i < str.length(); i++) {
    int c = str.charAt(i);
    if (c != '.' && (c < '0' || c > '9')) {
      return false;
    }
  }
  return true;
}

void handleRoot() {
    String s = MAIN_page;
    server.send(200, "text/html", s);
}

void handlestate1() {
    handleRoot();
    state1 = !state1;
    digitalWrite(relay1, state1);
    EEPROM.write(1, state1);
    EEPROM.commit();
}

void handlestate2() {
    handleRoot();
    state2 = !state2;
    digitalWrite(relay2, state2);
    EEPROM.write(2, state2);
    EEPROM.commit();
}

void handlestate3() {
    handleRoot();
    state3 = !state3;
    digitalWrite(relay3, state3);
    EEPROM.write(3, state3);
    EEPROM.commit();
}

void handlestate4() {
    handleRoot();
    state4 = !state4;
    digitalWrite(relay4, state4);
    EEPROM.write(4, state4);
    EEPROM.commit();
}

void handlestate5() {
    handleRoot();
    state5 = !state5;
    digitalWrite(relay5, state5);
    EEPROM.write(5, state5);
    EEPROM.commit();
}

void handlestate6() {
    handleRoot();
    state6 = !state6;
    digitalWrite(relay6, state6);
    EEPROM.write(6, state6);
    EEPROM.commit();
}

void handlestate7() {
    handleRoot();
    state7 = !state7;
    digitalWrite(relay7, state7);
    EEPROM.write(7, state7);
    EEPROM.commit();
}

void handlestate8() {
    handleRoot();
    state8 = !state8;
    digitalWrite(relay8, state8);
    EEPROM.write(8, state8);
    EEPROM.commit();
}

void handleallon() {
    handleRoot();
    state1 = LOW, state2 = LOW, state3 = LOW, state4 = LOW;
    state5 = LOW, state6 = LOW, state7 = LOW, state8 = LOW;
    setrelaystate();
    EEPROM.write(1, state1);
    EEPROM.write(2, state2);
    EEPROM.write(3, state3);
    EEPROM.write(4, state4);
    EEPROM.write(5, state5);
    EEPROM.write(6, state6);
    EEPROM.write(7, state7);
    EEPROM.write(8, state8);
    EEPROM.commit();
}

void handlealloff() {
    handleRoot();
    state1 = HIGH, state2 = HIGH, state3 = HIGH, state4 = HIGH;
    state5 = HIGH, state6 = HIGH, state7 = HIGH, state8 = HIGH;
    setrelaystate();
    EEPROM.write(1, state1);
    EEPROM.write(2, state2);
    EEPROM.write(3, state3);
    EEPROM.write(4, state4);
    EEPROM.write(5, state5);
    EEPROM.write(6, state6);
    EEPROM.write(7, state7);
    EEPROM.write(8, state8);
    EEPROM.commit();
}

void handlestate() {
  String content = "<?xml version = \"1.0\" ?>";
  content += "<inputs><analog>";
  content += state1;
  content += "</analog><analog>";
  content += state2;
  content += "</analog><analog>";
  content += state3;
  content += "</analog><analog>";
  content += state4;
  content += "</analog><analog>";
  content += state5;
  content += "</analog><analog>";
  content += state6;
  content += "</analog><analog>";
  content += state7;
  content += "</analog><analog>";
  content += state8;
  content += "</analog></inputs>";
  server.sendHeader("Cache-Control", "no-cache");
  server.send(200, "text/xml", content);
}

void handleGetNames() {
  String json = "{";
  for(int i = 0; i < 8; i++) {
    json += "\"relay" + String(i+1) + "\":\"" + relayNames[i] + "\"";
    if(i < 7) json += ",";
  }
  json += "}";
  server.send(200, "application/json", json);
}

void handleUpdateName() {
  if(server.hasArg("relay") && server.hasArg("name")) {
    int relayNum = server.arg("relay").toInt();
    String newName = server.arg("name");
    
    if(relayNum >= 1 && relayNum <= 8 && newName.length() > 0 && newName.length() <= MAX_NAME_LENGTH) {
      relayNames[relayNum-1] = newName;
      saveRelayName(relayNum-1, newName);
      server.send(200, "text/plain", "OK");
      return;
    }
  }
  server.send(400, "text/plain", "Invalid request");
}

// WiFi configuration handlers
void handleGetWifiConfig() {
  String json = "{";
  json += "\"ssid\":\"" + wifiSsid + "\",";
  json += "\"password\":\"" + wifiPassword + "\"";
  json += "}";
  server.send(200, "application/json", json);
}

void handleUpdateWifi() {
  if(server.hasArg("ssid") && server.hasArg("password")) {
    String newSsid = server.arg("ssid");
    String newPass = server.arg("password");
    
    if(newSsid.length() > 0 && newSsid.length() <= MAX_SSID_LENGTH && 
       newPass.length() <= MAX_PASS_LENGTH) {
      wifiSsid = newSsid;
      wifiPassword = newPass;
      saveWifiConfig();
      
      server.send(200, "text/plain", "OK");
      
      // Restart AP with new credentials after a short delay
      delay(1000);
      WiFi.softAP(wifiSsid.c_str(), wifiPassword.length() > 0 ? wifiPassword.c_str() : NULL);
      return;
    }
  }
  server.send(400, "text/plain", "Invalid request");
}

void saveWifiConfig() {
  // Save SSID
  int addr = EEPROM_WIFI_SSID_START;
  for(int i = 0; i < MAX_SSID_LENGTH; i++) {
    if(i < wifiSsid.length()) {
      EEPROM.write(addr + i, wifiSsid[i]);
    } else {
      EEPROM.write(addr + i, 0);
    }
  }
  
  // Save Password
  addr = EEPROM_WIFI_PASS_START;
  for(int i = 0; i < MAX_PASS_LENGTH; i++) {
    if(i < wifiPassword.length()) {
      EEPROM.write(addr + i, wifiPassword[i]);
    } else {
      EEPROM.write(addr + i, 0);
    }
  }
  EEPROM.commit();
}

void loadWifiConfig() {
  // Load SSID
  int addr = EEPROM_WIFI_SSID_START;
  String ssid = "";
  for(int i = 0; i < MAX_SSID_LENGTH; i++) {
    char c = EEPROM.read(addr + i);
    if(c == 0 || c == 255) break;
    ssid += c;
  }
  if(ssid.length() > 0) {
    wifiSsid = ssid;
  }
  
  // Load Password
  addr = EEPROM_WIFI_PASS_START;
  String pass = "";
  for(int i = 0; i < MAX_PASS_LENGTH; i++) {
    char c = EEPROM.read(addr + i);
    if(c == 0 || c == 255) break;
    pass += c;
  }
  if(pass.length() > 0) {
    wifiPassword = pass;
  }
}

void saveRelayName(int index, String name) {
  int addr = EEPROM_NAMES_START + (index * MAX_NAME_LENGTH);
  for(int i = 0; i < MAX_NAME_LENGTH; i++) {
    if(i < name.length()) {
      EEPROM.write(addr + i, name[i]);
    } else {
      EEPROM.write(addr + i, 0);
    }
  }
  EEPROM.commit();
}

void loadRelayNames() {
  for(int i = 0; i < 8; i++) {
    int addr = EEPROM_NAMES_START + (i * MAX_NAME_LENGTH);
    String name = "";
    for(int j = 0; j < MAX_NAME_LENGTH; j++) {
      char c = EEPROM.read(addr + j);
      if(c == 0 || c == 255) break;
      name += c;
    }
    if(name.length() > 0) {
      relayNames[i] = name;
    }
  }
}

void setup() {
  EEPROM.begin(EEPROM_SIZE);
  
  // Load saved configurations
  loadRelayNames();
  loadWifiConfig();
  
  // Set up Access Point with static IP
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(wifiSsid.c_str(), wifiPassword.length() > 0 ? wifiPassword.c_str() : NULL);
  
  // Start DNS server for captive portal
  dnsServer.start(DNS_PORT, "*", apIP);
  
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
  pinMode(relay5, OUTPUT);
  pinMode(relay6, OUTPUT);
  pinMode(relay7, OUTPUT);
  pinMode(relay8, OUTPUT);
  
  // Route handlers
  server.on("/", handleRoot);
  server.on("/LED1", handlestate1);
  server.on("/LED2", handlestate2);
  server.on("/LED3", handlestate3);
  server.on("/LED4", handlestate4);
  server.on("/LED5", handlestate5);
  server.on("/LED6", handlestate6);
  server.on("/LED7", handlestate7);
  server.on("/LED8", handlestate8);
  server.on("/allon", handleallon);
  server.on("/alloff", handlealloff);
  server.on("/redstate", handlestate);
  server.on("/getnames", handleGetNames);
  server.on("/updatename", handleUpdateName);
  server.on("/getwifi", handleGetWifiConfig);
  server.on("/updatewifi", handleUpdateWifi);
  
  // Captive portal handlers
  server.onNotFound([]() {
    if (captivePortal()) {
      return;
    }
    handleRoot();
  });
  
  server.begin();
  getstate();
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
  delay(50);
}

void setrelaystate() {
    digitalWrite(relay1, state1);
    digitalWrite(relay2, state2);
    digitalWrite(relay3, state3);
    digitalWrite(relay4, state4);
    digitalWrite(relay5, state5);
    digitalWrite(relay6, state6);
    digitalWrite(relay7, state7);
    digitalWrite(relay8, state8);
}

void getstate() {
    state1 = EEPROM.read(1);
    state2 = EEPROM.read(2);
    state3 = EEPROM.read(3);
    state4 = EEPROM.read(4);
    state5 = EEPROM.read(5);
    state6 = EEPROM.read(6);
    state7 = EEPROM.read(7);
    state8 = EEPROM.read(8);
    setrelaystate();
}
