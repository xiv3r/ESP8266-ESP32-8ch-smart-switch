// REQUIRES the following Arduino libraries:
// github.com/xiv3r

#include <EEPROM.h>
#include <WiFi.h>
#include <WebServer.h>
#include "index.h" 

#define EEPROM_SIZE 9
WebServer server(80);
int relay1 = 13, relay2 = 14, relay3 = 27, relay4 = 26;
int relay5 = 25, relay6 = 33, relay7 = 32, relay8 = 23;
int state1 = HIGH, state2 = HIGH, state3 = HIGH, state4 = HIGH;
int state5 = HIGH, state6 = HIGH, state7 = HIGH, state8 = HIGH;

const char* ApSsid = "ESP32_8CH_Smart_Switches";
const char* ApPassword = "ESP32-admin";

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
void setup() {
  EEPROM.begin(EEPROM_SIZE);
  WiFi.softAP(ApSsid, ApPassword);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
  pinMode(relay5, OUTPUT);
  pinMode(relay6, OUTPUT);
  pinMode(relay7, OUTPUT);
  pinMode(relay8, OUTPUT);
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
  server.begin();
  getstate();
}
void loop() {
  server.handleClient();
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
