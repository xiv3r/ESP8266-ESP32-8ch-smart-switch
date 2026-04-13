#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <EEPROM.h>
#include <ArduinoJson.h>

// EEPROM Configuration
#define EEPROM_SIZE 4096
#define EEPROM_MAGIC 0x1234
#define EEPROM_VERSION 1

// Customizable WiFi AP Configuration - MODIFY THESE AS NEEDED
const char* ap_ssid = "ESP32_8CH_Smart_Switch";  // Change this to your desired AP name
const char* ap_password = "ESP32-admin";         // Change this to your desired AP password (min 8 chars)

// DNS and Web Server
DNSServer dnsServer;
WebServer server(80);
const byte DNS_PORT = 53;

// NTP Configuration
String ntpServer = "ph.pool.ntp.org";
long Offset_sec = 28800;  // Default: Philippines GMT+8:00
int daylightOffset_sec = 0;

// WiFi Station Configuration
String sta_ssid = "";
String sta_password = "";

// NTP Client
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer.c_str(), gmtOffset_sec, daylightOffset_sec);

// Relay Configuration
#define NUM_RELAYS 8
const int relayPins[NUM_RELAYS] = {32, 33, 25, 26, 27, 14, 12, 13}; // Adjust pins as needed
const bool relayActiveLow = true; // Set true for active LOW relays

// Timer Schedule Structure
struct ScheduleData {
  uint8_t startHour;
  uint8_t startMinute;
  uint8_t startSecond;
  uint8_t stopHour;
  uint8_t stopMinute;
  uint8_t stopSecond;
  bool enabled;
};

struct RelayConfig {
  ScheduleData schedules[4];
  bool manualOverride;
  bool manualState;
  bool relayState;
};

RelayConfig relayConfigs[NUM_RELAYS];

// System Configuration
struct SystemConfig {
  uint16_t magic;
  uint8_t version;
  char sta_ssid[32];
  char sta_password[64];
  char ntp_server[48];
  long gmt_offset;
  int daylight_offset;
  char ap_ssid[32];      // Added: Custom AP SSID storage
  char ap_password[32];   // Added: Custom AP Password storage
};

SystemConfig sysConfig;
bool wifiConnected = false;
unsigned long lastNTPSync = 0;
const unsigned long NTP_SYNC_INTERVAL = 1000; // 1 hour
unsigned long lastUITime = 0;

// HTML Pages
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 8-Channel Relay Timer</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        * { box-sizing: border-box; }
        body { 
            font-family: Arial, sans-serif; 
            margin: 10px; 
            background: #f0f0f0; 
        }
        .container { 
            max-width: 1400px; 
            margin: 0 auto; 
        }
        .header { 
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white; 
            padding: 20px; 
            border-radius: 8px; 
            margin-bottom: 20px;
            box-shadow: 0 4px 6px rgba(0,0,0,0.1);
        }
        .header h1 { margin: 0 0 10px 0; }
        .relay-grid { 
            display: grid; 
            grid-template-columns: repeat(auto-fit, minmax(320px, 1fr)); 
            gap: 20px; 
        }
        .relay-card { 
            background: white; 
            padding: 20px; 
            border-radius: 8px; 
            box-shadow: 0 2px 8px rgba(0,0,0,0.1);
            transition: transform 0.2s;
        }
        .relay-card:hover {
            transform: translateY(-2px);
            box-shadow: 0 4px 12px rgba(0,0,0,0.15);
        }
        .relay-title { 
            font-size: 20px; 
            font-weight: bold; 
            margin-bottom: 15px; 
            color: #333;
            display: flex;
            justify-content: space-between;
            align-items: center;
        }
        .schedule { 
            margin-bottom: 15px; 
            padding: 12px; 
            background: #f8f9fa; 
            border-radius: 6px;
            border-left: 3px solid #667eea;
        }
        .schedule h4 { 
            margin: 0 0 12px 0; 
            color: #495057;
            display: flex;
            align-items: center;
            gap: 10px;
        }
        .time-group {
            display: flex;
            gap: 10px;
            margin-bottom: 8px;
        }
        .time-input { 
            flex: 1;
        }
        .time-input label { 
            display: block; 
            font-size: 11px; 
            color: #6c757d; 
            margin-bottom: 3px;
            font-weight: bold;
        }
        .time-input input { 
            width: 100%; 
            padding: 6px; 
            border: 1px solid #dee2e6; 
            border-radius: 4px;
            font-size: 13px;
            transition: border-color 0.2s;
        }
        .time-input input:focus {
            outline: none;
            border-color: #667eea;
        }
        .time-separator {
            font-size: 18px;
            font-weight: bold;
            color: #6c757d;
            align-self: flex-end;
            padding-bottom: 6px;
        }
        .button-group {
            display: flex;
            gap: 8px;
            margin-top: 12px;
            flex-wrap: wrap;
        }
        .button { 
            padding: 8px 16px; 
            border: none; 
            border-radius: 4px; 
            cursor: pointer; 
            font-size: 13px;
            font-weight: bold;
            transition: all 0.2s;
            flex: 1;
            min-width: 70px;
        }
        .button:hover {
            transform: translateY(-1px);
            box-shadow: 0 2px 4px rgba(0,0,0,0.2);
        }
        .button.save { 
            background: #28a745; 
            color: white; 
        }
        .button.save:hover {
            background: #218838;
        }
        .button.on { 
            background: #17a2b8; 
            color: white; 
        }
        .button.on:hover {
            background: #138496;
        }
        .button.off { 
            background: #dc3545; 
            color: white; 
        }
        .button.off:hover {
            background: #c82333;
        }
        .button.auto { 
            background: #ffc107; 
            color: #212529; 
        }
        .button.auto:hover {
            background: #e0a800;
        }
        .button.saveall {
            background: #007bff;
            color: white;
            padding: 12px 24px;
            font-size: 16px;
            margin-top: 20px;
        }
        .status { 
            padding: 5px 12px; 
            border-radius: 20px; 
            font-weight: bold; 
            font-size: 13px;
        }
        .status.on { 
            background: #28a745; 
            color: white; 
        }
        .status.off { 
            background: #dc3545; 
            color: white; 
        }
        .nav { 
            margin-bottom: 20px;
            display: flex;
            gap: 15px;
        }
        .nav a { 
            color: white; 
            text-decoration: none; 
            padding: 8px 16px;
            background: rgba(255,255,255,0.2);
            border-radius: 4px;
            transition: background 0.2s;
        }
        .nav a:hover {
            background: rgba(255,255,255,0.3);
        }
        .nav span {
            margin-left: auto;
            font-size: 18px;
            font-weight: bold;
        }
        .save-indicator {
            position: fixed;
            top: 20px;
            right: 20px;
            background: #28a745;
            color: white;
            padding: 10px 20px;
            border-radius: 4px;
            display: none;
            animation: slideIn 0.3s;
            box-shadow: 0 4px 6px rgba(0,0,0,0.1);
        }
        @keyframes slideIn {
            from { transform: translateX(100%); opacity: 0; }
            to { transform: translateX(0); opacity: 1; }
        }
        input[type="checkbox"] {
            width: 18px;
            height: 18px;
            cursor: pointer;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>ESP32 8-Channel Relay Timer Controller</h1>
            <div class="nav">
                <a href="/">📊 Relay Control</a>
                <a href="/wifi">📶 WiFi Settings</a>
                <a href="/ntp">🕐 NTP/RTC Settings</a>
                <a href="/apconfig">📱 AP Config</a>
                <span id="currentTime">--:--:--</span>
            </div>
        </div>
        
        <div style="text-align: center; margin-bottom: 20px;">
            <button class="button saveall" onclick="saveAllRelays()">💾 Save All Relays Configuration</button>
        </div>
        
        <div class="relay-grid" id="relayGrid"></div>
    </div>
    
    <div id="saveIndicator" class="save-indicator">✓ Settings Saved Successfully!</div>
    
    <script>
        let relays = [];
        let timeUpdateInterval = null;
        let relayUpdateInterval = null;
        
        function showSaveIndicator() {
            const indicator = document.getElementById('saveIndicator');
            indicator.style.display = 'block';
            setTimeout(() => {
                indicator.style.display = 'none';
            }, 3000);
        }
        
        function loadRelays() {
            fetch('/api/relays')
                .then(response => response.json())
                .then(data => {
                    relays = data;
                    renderRelays();
                })
                .catch(error => console.error('Error loading relays:', error));
        }
        
        function renderRelays() {
            const grid = document.getElementById('relayGrid');
            grid.innerHTML = '';
            
            relays.forEach((relay, index) => {
                const card = document.createElement('div');
                card.className = 'relay-card';
                
                let html = `<div class="relay-title">
                    <span>🔌 Relay ${index + 1}</span>
                    <span class="status ${relay.state ? 'on' : 'off'}">${relay.state ? 'ON' : 'OFF'}</span>
                </div>`;
                
                // Manual control buttons
                html += `<div class="button-group">
                    <button class="button on" onclick="manualControl(${index}, true)">🔛 Turn ON</button>
                    <button class="button off" onclick="manualControl(${index}, false)">🔴 Turn OFF</button>
                    <button class="button auto" onclick="resetManual(${index})">🔄 Auto Mode</button>
                </div>`;
                
                // Schedules
                for (let s = 0; s < 4; s++) {
                    const schedule = relay.schedules[s];
                    html += `<div class="schedule">
                        <h4>
                            ⏰ Schedule ${s + 1}
                            <label style="margin-left: auto;">
                                <input type="checkbox" 
                                    id="relay${index}_sch${s}_enabled"
                                    ${schedule.enabled ? 'checked' : ''}
                                    onchange="updateCheckbox(${index}, ${s}, 'enabled', this.checked)">
                                Enabled
                            </label>
                        </h4>
                        <div class="time-group">
                            <div class="time-input">
                                <label>Start Hour</label>
                                <input type="number" min="0" max="23" 
                                    id="relay${index}_sch${s}_startHour"
                                    value="${schedule.startHour}"
                                    onchange="updateField(${index}, ${s}, 'startHour', this.value)">
                            </div>
                            <span class="time-separator">:</span>
                            <div class="time-input">
                                <label>Minute</label>
                                <input type="number" min="0" max="59" 
                                    id="relay${index}_sch${s}_startMinute"
                                    value="${schedule.startMinute}"
                                    onchange="updateField(${index}, ${s}, 'startMinute', this.value)">
                            </div>
                            <span class="time-separator">:</span>
                            <div class="time-input">
                                <label>Second</label>
                                <input type="number" min="0" max="59" 
                                    id="relay${index}_sch${s}_startSecond"
                                    value="${schedule.startSecond}"
                                    onchange="updateField(${index}, ${s}, 'startSecond', this.value)">
                            </div>
                        </div>
                        <div class="time-group">
                            <div class="time-input">
                                <label>Stop Hour</label>
                                <input type="number" min="0" max="23" 
                                    id="relay${index}_sch${s}_stopHour"
                                    value="${schedule.stopHour}"
                                    onchange="updateField(${index}, ${s}, 'stopHour', this.value)">
                            </div>
                            <span class="time-separator">:</span>
                            <div class="time-input">
                                <label>Minute</label>
                                <input type="number" min="0" max="59" 
                                    id="relay${index}_sch${s}_stopMinute"
                                    value="${schedule.stopMinute}"
                                    onchange="updateField(${index}, ${s}, 'stopMinute', this.value)">
                            </div>
                            <span class="time-separator">:</span>
                            <div class="time-input">
                                <label>Second</label>
                                <input type="number" min="0" max="59" 
                                    id="relay${index}_sch${s}_stopSecond"
                                    value="${schedule.stopSecond}"
                                    onchange="updateField(${index}, ${s}, 'stopSecond', this.value)">
                            </div>
                        </div>
                    </div>`;
                }
                
                html += `<div class="button-group">
                    <button class="button save" onclick="saveRelay(${index})">💾 Save Relay ${index + 1}</button>
                </div>`;
                
                card.innerHTML = html;
                grid.appendChild(card);
            });
        }
        
        function updateCheckbox(relayIndex, scheduleIndex, field, value) {
            relays[relayIndex].schedules[scheduleIndex][field] = value;
        }
        
        function updateField(relayIndex, scheduleIndex, field, value) {
            let numValue = parseInt(value);
            if (field.includes('Hour')) {
                numValue = Math.min(23, Math.max(0, numValue));
            } else {
                numValue = Math.min(59, Math.max(0, numValue));
            }
            relays[relayIndex].schedules[scheduleIndex][field] = numValue;
            document.getElementById(`relay${relayIndex}_sch${scheduleIndex}_${field}`).value = numValue;
        }
        
        function manualControl(relayIndex, state) {
            fetch('/api/relay/manual', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({relay: relayIndex, state: state})
            }).then(response => response.json())
              .then(data => {
                  if (data.success) {
                      loadRelays();
                  }
              });
        }
        
        function resetManual(relayIndex) {
            fetch('/api/relay/reset', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({relay: relayIndex})
            }).then(response => response.json())
              .then(data => {
                  if (data.success) {
                      loadRelays();
                  }
              });
        }
        
        function saveRelay(relayIndex) {
            const relayData = {
                relay: relayIndex,
                schedules: relays[relayIndex].schedules
            };
            
            fetch('/api/relay/save', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify(relayData)
            }).then(response => response.json())
              .then(data => {
                  if (data.success) {
                      showSaveIndicator();
                  } else {
                      alert('Failed to save relay ' + (relayIndex + 1));
                  }
              })
              .catch(error => {
                  console.error('Error:', error);
                  alert('Error saving relay configuration');
              });
        }
        
        function saveAllRelays() {
            const allRelaysData = [];
            for (let i = 0; i < relays.length; i++) {
                allRelaysData.push({
                    relay: i,
                    schedules: relays[i].schedules
                });
            }
            
            fetch('/api/relays/saveall', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({relays: allRelaysData})
            }).then(response => response.json())
              .then(data => {
                  if (data.success) {
                      showSaveIndicator();
                  } else {
                      alert('Failed to save all relays');
                  }
              })
              .catch(error => {
                  console.error('Error:', error);
                  alert('Error saving all relay configurations');
              });
        }
        
        function updateTime() {
            fetch('/api/time')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('currentTime').textContent = data.time;
                })
                .catch(error => console.error('Error updating time:', error));
        }
        
        // Initialize
        loadRelays();
        
        // Set up intervals
        if (relayUpdateInterval) clearInterval(relayUpdateInterval);
        if (timeUpdateInterval) clearInterval(timeUpdateInterval);
        
        relayUpdateInterval = setInterval(() => {
            // Only update relay states, don't reload entire config
            fetch('/api/relays/state')
                .then(response => response.json())
                .then(data => {
                    data.forEach((state, index) => {
                        if (relays[index]) {
                            relays[index].state = state;
                        }
                    });
                    renderRelays();
                })
                .catch(error => console.error('Error updating relay states:', error));
        }, 3000);
        
        timeUpdateInterval = setInterval(updateTime, 1000);
        updateTime();
    </script>
</body>
</html>
)rawliteral";

const char wifi_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>WiFi Settings</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial; margin: 10px; background: #f0f0f0; }
        .container { max-width: 600px; margin: 0 auto; }
        .header { background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; padding: 20px; border-radius: 8px; margin-bottom: 20px; }
        .card { background: white; padding: 25px; border-radius: 8px; box-shadow: 0 2px 8px rgba(0,0,0,0.1); }
        .form-group { margin-bottom: 20px; }
        label { display: block; margin-bottom: 8px; color: #495057; font-weight: bold; }
        input[type="text"], input[type="password"] { width: 100%; padding: 10px; border: 1px solid #dee2e6; border-radius: 4px; box-sizing: border-box; font-size: 14px; }
        .button { background: #28a745; color: white; border: none; padding: 12px 24px; border-radius: 4px; cursor: pointer; font-size: 16px; font-weight: bold; width: 100%; }
        .button:hover { background: #218838; }
        .nav { margin-bottom: 20px; }
        .nav a { color: white; text-decoration: none; margin-right: 20px; }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>WiFi Station Settings</h1>
            <div class="nav">
                <a href="/">📊 Relay Control</a>
                <a href="/wifi">📶 WiFi Settings</a>
                <a href="/ntp">🕐 NTP/RTC Settings</a>
                <a href="/apconfig">📱 AP Config</a>
            </div>
        </div>
        
        <div class="card">
            <form onsubmit="saveWiFi(event)">
                <div class="form-group">
                    <label>WiFi SSID:</label>
                    <input type="text" id="ssid" required>
                </div>
                <div class="form-group">
                    <label>WiFi Password:</label>
                    <input type="password" id="password">
                </div>
                <button type="submit" class="button">Save WiFi Settings</button>
            </form>
        </div>
    </div>
    
    <script>
        function loadWiFi() {
            fetch('/api/wifi')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('ssid').value = data.ssid || '';
                });
        }
        
        function saveWiFi(event) {
            event.preventDefault();
            const data = {
                ssid: document.getElementById('ssid').value,
                password: document.getElementById('password').value
            };
            
            fetch('/api/wifi', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify(data)
            }).then(response => response.json())
              .then(data => {
                  if (data.success) {
                      alert('WiFi settings saved! Device will restart in AP+STA mode.');
                      setTimeout(() => location.reload(), 2000);
                  }
              });
        }
        
        loadWiFi();
    </script>
</body>
</html>
)rawliteral";

const char ntp_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>NTP & RTC Settings</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial; margin: 10px; background: #f0f0f0; }
        .container { max-width: 600px; margin: 0 auto; }
        .header { background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; padding: 20px; border-radius: 8px; margin-bottom: 20px; }
        .card { background: white; padding: 25px; border-radius: 8px; box-shadow: 0 2px 8px rgba(0,0,0,0.1); }
        .form-group { margin-bottom: 20px; }
        label { display: block; margin-bottom: 8px; color: #495057; font-weight: bold; }
        input[type="text"], input[type="number"] { width: 100%; padding: 10px; border: 1px solid #dee2e6; border-radius: 4px; box-sizing: border-box; font-size: 14px; }
        .button { background: #28a745; color: white; border: none; padding: 12px 24px; border-radius: 4px; cursor: pointer; font-size: 16px; font-weight: bold; margin-right: 10px; }
        .button.sync { background: #007bff; }
        .button:hover { opacity: 0.9; }
        .nav { margin-bottom: 20px; }
        .nav a { color: white; text-decoration: none; margin-right: 20px; }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>NTP & RTC Settings</h1>
            <div class="nav">
                <a href="/">📊 Relay Control</a>
                <a href="/wifi">📶 WiFi Settings</a>
                <a href="/ntp">🕐 NTP/RTC Settings</a>
                <a href="/apconfig">📱 AP Config</a>
            </div>
        </div>
        
        <div class="card">
            <form onsubmit="saveNTP(event)">
                <div class="form-group">
                    <label>NTP Server:</label>
                    <input type="text" id="ntpServer" required>
                </div>
                <div class="form-group">
                    <label>GMT Offset (seconds):</label>
                    <input type="number" id="gmtOffset" required>
                </div>
                <div class="form-group">
                    <label>Daylight Offset (seconds):</label>
                    <input type="number" id="daylightOffset" value="0">
                </div>
                <button type="submit" class="button">Save NTP Settings</button>
                <button type="button" class="button sync" onclick="syncNTP()">Sync Time Now</button>
            </form>
        </div>
    </div>
    
    <script>
        function loadNTP() {
            fetch('/api/ntp')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('ntpServer').value = data.ntpServer || 'ph.pool.ntp.org';
                    document.getElementById('gmtOffset').value = data.gmtOffset || 28800;
                    document.getElementById('daylightOffset').value = data.daylightOffset || 0;
                });
        }
        
        function saveNTP(event) {
            event.preventDefault();
            const data = {
                ntpServer: document.getElementById('ntpServer').value,
                gmtOffset: parseInt(document.getElementById('gmtOffset').value),
                daylightOffset: parseInt(document.getElementById('daylightOffset').value)
            };
            
            fetch('/api/ntp', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify(data)
            }).then(response => response.json())
              .then(data => {
                  if (data.success) {
                      alert('NTP settings saved successfully!');
                  }
              });
        }
        
        function syncNTP() {
            fetch('/api/ntp/sync', {method: 'POST'})
                .then(response => response.json())
                .then(data => {
                    if (data.success) {
                        alert('Time synchronized successfully!');
                    } else {
                        alert('Failed to sync time. Check WiFi connection.');
                    }
                });
        }
        
        loadNTP();
    </script>
</body>
</html>
)rawliteral";

// New AP Configuration HTML Page
const char apconfig_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Access Point Configuration</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial; margin: 10px; background: #f0f0f0; }
        .container { max-width: 600px; margin: 0 auto; }
        .header { background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; padding: 20px; border-radius: 8px; margin-bottom: 20px; }
        .card { background: white; padding: 25px; border-radius: 8px; box-shadow: 0 2px 8px rgba(0,0,0,0.1); }
        .form-group { margin-bottom: 20px; }
        label { display: block; margin-bottom: 8px; color: #495057; font-weight: bold; }
        input[type="text"], input[type="password"] { width: 100%; padding: 10px; border: 1px solid #dee2e6; border-radius: 4px; box-sizing: border-box; font-size: 14px; }
        .button { background: #28a745; color: white; border: none; padding: 12px 24px; border-radius: 4px; cursor: pointer; font-size: 16px; font-weight: bold; width: 100%; }
        .button.reset { background: #dc3545; margin-top: 10px; }
        .button:hover { opacity: 0.9; }
        .info-box { background: #e7f3ff; border-left: 4px solid #2196F3; padding: 12px; margin-bottom: 20px; border-radius: 4px; }
        .nav { margin-bottom: 20px; }
        .nav a { color: white; text-decoration: none; margin-right: 20px; }
        .warning { color: #856404; background: #fff3cd; border: 1px solid #ffeeba; padding: 12px; border-radius: 4px; margin-bottom: 20px; }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>Access Point Configuration</h1>
            <div class="nav">
                <a href="/">📊 Relay Control</a>
                <a href="/wifi">📶 WiFi Settings</a>
                <a href="/ntp">🕐 NTP/RTC Settings</a>
                <a href="/apconfig">📱 AP Config</a>
            </div>
        </div>
        
        <div class="card">
            <div class="info-box">
                <strong>Current AP Settings:</strong><br>
                SSID: <span id="currentSSID"></span><br>
                IP Address: 192.168.4.1
            </div>
            
            <div class="warning">
                <strong>⚠️ Important:</strong> Changing AP settings will restart the device. 
                You'll need to reconnect to the new AP SSID after changes.
            </div>
            
            <form onsubmit="saveAPConfig(event)">
                <div class="form-group">
                    <label>AP SSID (Network Name):</label>
                    <input type="text" id="apSsid" required minlength="1" maxlength="31">
                </div>
                <div class="form-group">
                    <label>AP Password:</label>
                    <input type="password" id="apPassword" minlength="8" maxlength="31">
                    <small style="color: #6c757d;">Minimum 8 characters. Leave empty for open network.</small>
                </div>
                <button type="submit" class="button">Save AP Configuration</button>
            </form>
            
            <button onclick="resetToDefault()" class="button reset">Reset to Default Settings</button>
        </div>
    </div>
    
    <script>
        function loadAPConfig() {
            fetch('/api/apconfig')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('apSsid').value = data.ap_ssid || 'ESP32_Relay_Config';
                    document.getElementById('currentSSID').textContent = data.ap_ssid || 'ESP32_Relay_Config';
                });
        }
        
        function saveAPConfig(event) {
            event.preventDefault();
            
            const apSsid = document.getElementById('apSsid').value.trim();
            const apPassword = document.getElementById('apPassword').value;
            
            if (!apSsid) {
                alert('AP SSID cannot be empty!');
                return;
            }
            
            if (apPassword && apPassword.length < 8) {
                alert('AP Password must be at least 8 characters!');
                return;
            }
            
            const data = {
                ap_ssid: apSsid,
                ap_password: apPassword
            };
            
            fetch('/api/apconfig', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify(data)
            }).then(response => response.json())
              .then(data => {
                  if (data.success) {
                      alert('AP Configuration saved! Device will restart. Reconnect to: ' + apSsid);
                      setTimeout(() => {
                          window.location.href = 'http://192.168.4.1';
                      }, 3000);
                  } else {
                      alert('Failed to save AP configuration');
                  }
              });
        }
        
        function resetToDefault() {
            if (confirm('Reset AP settings to default? Device will restart.')) {
                fetch('/api/apconfig/reset', {method: 'POST'})
                    .then(response => response.json())
                    .then(data => {
                        if (data.success) {
                            alert('Reset to default settings. Reconnect to: ESP32_Relay_Config');
                            setTimeout(() => {
                                window.location.href = 'http://192.168.4.1';
                            }, 3000);
                        }
                    });
            }
        }
        
        loadAPConfig();
    </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  Serial.println("\n\nESP32 8-Channel Relay Timer Starting...");
  
  // Initialize EEPROM
  EEPROM.begin(EEPROM_SIZE);
  
  // Initialize relay pins
  for (int i = 0; i < NUM_RELAYS; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], relayActiveLow ? HIGH : LOW);
    relayConfigs[i].relayState = false;
  }
  
  // Load configuration
  loadConfiguration();
  
  // Use custom AP settings from EEPROM if available
  const char* current_ap_ssid = (strlen(sysConfig.ap_ssid) > 0) ? sysConfig.ap_ssid : ap_ssid;
  const char* current_ap_password = (strlen(sysConfig.ap_password) > 0) ? sysConfig.ap_password : ap_password;
  
  // Setup WiFi
  if (strlen(sysConfig.sta_ssid) > 0) {
    Serial.printf("Connecting to WiFi: %s\n", sysConfig.sta_ssid);
    WiFi.begin(sysConfig.sta_ssid, sysConfig.sta_password);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(500);
      Serial.print(".");
      attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      wifiConnected = true;
      Serial.println("\nConnected to WiFi");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      
      // Initialize NTP
      ntpServer = String(sysConfig.ntp_server);
      gmtOffset_sec = sysConfig.gmt_offset;
      daylightOffset_sec = sysConfig.daylight_offset;
      timeClient.setPoolServerName(ntpServer.c_str());
      timeClient.setTimeOffset(gmtOffset_sec);
      timeClient.begin();
      timeClient.update();
      lastNTPSync = millis();
    }
  }
  
  // Always start AP mode for configuration with custom settings
  WiFi.mode(WIFI_AP_STA);
  
  if (strlen(current_ap_password) > 0) {
    WiFi.softAP(current_ap_ssid, current_ap_password);
  } else {
    WiFi.softAP(current_ap_ssid);
  }
  
  // Setup DNS server for captive portal
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
  
  // Setup web server routes
  setupWebServer();
  
  Serial.println("Access Point IP: " + WiFi.softAPIP().toString());
  Serial.printf("AP SSID: %s\n", current_ap_ssid);
  Serial.println("Web server started!");
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
  
  // Update NTP time
  if (wifiConnected && millis() - lastNTPSync > NTP_SYNC_INTERVAL) {
    timeClient.update();
    lastNTPSync = millis();
  }
  
  // Process relay schedules
  processRelaySchedules();
  
  delay(10);
}

void setupWebServer() {
  // Main pages
  server.on("/", HTTP_GET, []() {
    server.send_P(200, "text/html", index_html);
  });
  
  server.on("/wifi", HTTP_GET, []() {
    server.send_P(200, "text/html", wifi_html);
  });
  
  server.on("/ntp", HTTP_GET, []() {
    server.send_P(200, "text/html", ntp_html);
  });
  
  server.on("/apconfig", HTTP_GET, []() {
    server.send_P(200, "text/html", apconfig_html);
  });
  
  // API endpoints
  server.on("/api/relays", HTTP_GET, handleGetRelays);
  server.on("/api/relays/state", HTTP_GET, handleGetRelayStates);
  server.on("/api/relay/manual", HTTP_POST, handleManualControl);
  server.on("/api/relay/reset", HTTP_POST, handleResetManual);
  server.on("/api/relay/save", HTTP_POST, handleSaveRelay);
  server.on("/api/relays/saveall", HTTP_POST, handleSaveAllRelays);
  server.on("/api/time", HTTP_GET, handleGetTime);
  server.on("/api/wifi", HTTP_GET, handleGetWiFi);
  server.on("/api/wifi", HTTP_POST, handleSaveWiFi);
  server.on("/api/ntp", HTTP_GET, handleGetNTP);
  server.on("/api/ntp", HTTP_POST, handleSaveNTP);
  server.on("/api/ntp/sync", HTTP_POST, handleSyncNTP);
  server.on("/api/apconfig", HTTP_GET, handleGetAPConfig);
  server.on("/api/apconfig", HTTP_POST, handleSaveAPConfig);
  server.on("/api/apconfig/reset", HTTP_POST, handleResetAPConfig);
  
  // Captive portal redirect
  server.onNotFound([]() {
    if (server.hostHeader() != WiFi.softAPIP().toString()) {
      server.sendHeader("Location", "http://192.168.4.1/", true);
      server.send(302, "text/plain", "");
    } else {
      server.send(404, "text/plain", "Not Found");
    }
  });
  
  server.begin();
}

void processRelaySchedules() {
  time_t currentTime;
  struct tm *timeinfo;
  
  if (wifiConnected) {
    currentTime = timeClient.getEpochTime();
    timeinfo = localtime(&currentTime);
  } else {
    return; // No time sync, skip schedule processing
  }
  
  for (int i = 0; i < NUM_RELAYS; i++) {
    if (relayConfigs[i].manualOverride) {
      bool targetState = relayActiveLow ? !relayConfigs[i].manualState : relayConfigs[i].manualState;
      digitalWrite(relayPins[i], targetState);
      relayConfigs[i].relayState = relayConfigs[i].manualState;
      continue;
    }
    
    bool shouldBeOn = false;
    
    for (int s = 0; s < 4; s++) {
      if (!relayConfigs[i].schedules[s].enabled) continue;
      
      int startSeconds = relayConfigs[i].schedules[s].startHour * 3600 + 
                         relayConfigs[i].schedules[s].startMinute * 60 + 
                         relayConfigs[i].schedules[s].startSecond;
      int stopSeconds = relayConfigs[i].schedules[s].stopHour * 3600 + 
                        relayConfigs[i].schedules[s].stopMinute * 60 + 
                        relayConfigs[i].schedules[s].stopSecond;
      int currentSeconds = timeinfo->tm_hour * 3600 + 
                           timeinfo->tm_min * 60 + 
                           timeinfo->tm_sec;
      
      if (startSeconds < stopSeconds) {
        if (currentSeconds >= startSeconds && currentSeconds < stopSeconds) {
          shouldBeOn = true;
          break;
        }
      } else if (startSeconds > stopSeconds) {
        // Overnight schedule
        if (currentSeconds >= startSeconds || currentSeconds < stopSeconds) {
          shouldBeOn = true;
          break;
        }
      }
    }
    
    bool targetState = relayActiveLow ? !shouldBeOn : shouldBeOn;
    digitalWrite(relayPins[i], targetState);
    relayConfigs[i].relayState = shouldBeOn;
  }
}

void loadConfiguration() {
  EEPROM.get(0, sysConfig);
  
  if (sysConfig.magic != EEPROM_MAGIC || sysConfig.version != EEPROM_VERSION) {
    // Initialize with defaults
    Serial.println("Initializing default configuration...");
    sysConfig.magic = EEPROM_MAGIC;
    sysConfig.version = EEPROM_VERSION;
    strcpy(sysConfig.sta_ssid, "");
    strcpy(sysConfig.sta_password, "");
    strcpy(sysConfig.ntp_server, "ph.pool.ntp.org");
    sysConfig.gmt_offset = 28800;
    sysConfig.daylight_offset = 0;
    strcpy(sysConfig.ap_ssid, "");     // Will use default from code
    strcpy(sysConfig.ap_password, ""); // Will use default from code
    
    // Initialize relay configs with defaults
    for (int i = 0; i < NUM_RELAYS; i++) {
      relayConfigs[i].manualOverride = false;
      relayConfigs[i].manualState = false;
      for (int s = 0; s < 4; s++) {
        relayConfigs[i].schedules[s].startHour = 8;
        relayConfigs[i].schedules[s].startMinute = 0;
        relayConfigs[i].schedules[s].startSecond = 0;
        relayConfigs[i].schedules[s].stopHour = 17;
        relayConfigs[i].schedules[s].stopMinute = 0;
        relayConfigs[i].schedules[s].stopSecond = 0;
        relayConfigs[i].schedules[s].enabled = false;
      }
    }
    
    saveConfiguration();
  } else {
    // Load relay configurations
    int addr = sizeof(SystemConfig);
    for (int i = 0; i < NUM_RELAYS; i++) {
      EEPROM.get(addr, relayConfigs[i]);
      addr += sizeof(RelayConfig);
    }
  }
}

void saveConfiguration() {
  EEPROM.put(0, sysConfig);
  
  int addr = sizeof(SystemConfig);
  for (int i = 0; i < NUM_RELAYS; i++) {
    EEPROM.put(addr, relayConfigs[i]);
    addr += sizeof(RelayConfig);
  }
  
  EEPROM.commit();
  Serial.println("Configuration saved to EEPROM");
}

// New AP Config Handlers
void handleGetAPConfig() {
  DynamicJsonDocument doc(256);
  
  const char* current_ap_ssid = (strlen(sysConfig.ap_ssid) > 0) ? sysConfig.ap_ssid : ap_ssid;
  const char* current_ap_password = (strlen(sysConfig.ap_password) > 0) ? sysConfig.ap_password : ap_password;
  
  doc["ap_ssid"] = current_ap_ssid;
  doc["ap_password"] = current_ap_password;
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleSaveAPConfig() {
  DynamicJsonDocument doc(256);
  deserializeJson(doc, server.arg("plain"));
  
  const char* new_ap_ssid = doc["ap_ssid"];
  const char* new_ap_password = doc["ap_password"];
  
  if (new_ap_ssid && strlen(new_ap_ssid) > 0) {
    strcpy(sysConfig.ap_ssid, new_ap_ssid);
  }
  
  if (new_ap_password) {
    strcpy(sysConfig.ap_password, new_ap_password);
  } else {
    strcpy(sysConfig.ap_password, "");
  }
  
  saveConfiguration();
  
  server.send(200, "application/json", "{\"success\":true}");
  delay(1000);
  ESP.restart();
}

void handleResetAPConfig() {
  // Clear custom AP settings
  strcpy(sysConfig.ap_ssid, "");
  strcpy(sysConfig.ap_password, "");
  saveConfiguration();
  
  server.send(200, "application/json", "{\"success\":true}");
  delay(1000);
  ESP.restart();
}

// API Handlers
void handleGetRelays() {
  DynamicJsonDocument doc(8192);
  JsonArray array = doc.to<JsonArray>();
  
  for (int i = 0; i < NUM_RELAYS; i++) {
    JsonObject relay = array.createNestedObject();
    relay["state"] = relayConfigs[i].relayState;
    
    JsonArray schedules = relay.createNestedArray("schedules");
    for (int s = 0; s < 4; s++) {
      JsonObject schedule = schedules.createNestedObject();
      schedule["startHour"] = relayConfigs[i].schedules[s].startHour;
      schedule["startMinute"] = relayConfigs[i].schedules[s].startMinute;
      schedule["startSecond"] = relayConfigs[i].schedules[s].startSecond;
      schedule["stopHour"] = relayConfigs[i].schedules[s].stopHour;
      schedule["stopMinute"] = relayConfigs[i].schedules[s].stopMinute;
      schedule["stopSecond"] = relayConfigs[i].schedules[s].stopSecond;
      schedule["enabled"] = relayConfigs[i].schedules[s].enabled;
    }
  }
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleGetRelayStates() {
  DynamicJsonDocument doc(512);
  JsonArray array = doc.to<JsonArray>();
  
  for (int i = 0; i < NUM_RELAYS; i++) {
    array.add(relayConfigs[i].relayState);
  }
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleManualControl() {
  DynamicJsonDocument doc(256);
  deserializeJson(doc, server.arg("plain"));
  
  int relay = doc["relay"];
  bool state = doc["state"];
  
  if (relay >= 0 && relay < NUM_RELAYS) {
    relayConfigs[relay].manualOverride = true;
    relayConfigs[relay].manualState = state;
    saveConfiguration();
    server.send(200, "application/json", "{\"success\":true}");
  } else {
    server.send(400, "application/json", "{\"success\":false}");
  }
}

void handleResetManual() {
  DynamicJsonDocument doc(256);
  deserializeJson(doc, server.arg("plain"));
  
  int relay = doc["relay"];
  
  if (relay >= 0 && relay < NUM_RELAYS) {
    relayConfigs[relay].manualOverride = false;
    saveConfiguration();
    server.send(200, "application/json", "{\"success\":true}");
  } else {
    server.send(400, "application/json", "{\"success\":false}");
  }
}

void handleSaveRelay() {
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, server.arg("plain"));
  
  int relay = doc["relay"];
  JsonArray schedules = doc["schedules"].as<JsonArray>();
  
  if (relay >= 0 && relay < NUM_RELAYS) {
    for (int s = 0; s < 4 && s < schedules.size(); s++) {
      relayConfigs[relay].schedules[s].startHour = schedules[s]["startHour"];
      relayConfigs[relay].schedules[s].startMinute = schedules[s]["startMinute"];
      relayConfigs[relay].schedules[s].startSecond = schedules[s]["startSecond"];
      relayConfigs[relay].schedules[s].stopHour = schedules[s]["stopHour"];
      relayConfigs[relay].schedules[s].stopMinute = schedules[s]["stopMinute"];
      relayConfigs[relay].schedules[s].stopSecond = schedules[s]["stopSecond"];
      relayConfigs[relay].schedules[s].enabled = schedules[s]["enabled"];
    }
    saveConfiguration();
    server.send(200, "application/json", "{\"success\":true}");
  } else {
    server.send(400, "application/json", "{\"success\":false}");
  }
}

void handleSaveAllRelays() {
  DynamicJsonDocument doc(8192);
  deserializeJson(doc, server.arg("plain"));
  
  JsonArray relays = doc["relays"].as<JsonArray>();
  
  for (JsonObject relayData : relays) {
    int relay = relayData["relay"];
    JsonArray schedules = relayData["schedules"].as<JsonArray>();
    
    if (relay >= 0 && relay < NUM_RELAYS) {
      for (int s = 0; s < 4 && s < schedules.size(); s++) {
        relayConfigs[relay].schedules[s].startHour = schedules[s]["startHour"];
        relayConfigs[relay].schedules[s].startMinute = schedules[s]["startMinute"];
        relayConfigs[relay].schedules[s].startSecond = schedules[s]["startSecond"];
        relayConfigs[relay].schedules[s].stopHour = schedules[s]["stopHour"];
        relayConfigs[relay].schedules[s].stopMinute = schedules[s]["stopMinute"];
        relayConfigs[relay].schedules[s].stopSecond = schedules[s]["stopSecond"];
        relayConfigs[relay].schedules[s].enabled = schedules[s]["enabled"];
      }
    }
  }
  
  saveConfiguration();
  server.send(200, "application/json", "{\"success\":true}");
}

void handleGetTime() {
  DynamicJsonDocument doc(128);
  char timeStr[9];
  
  if (wifiConnected) {
    sprintf(timeStr, "%02d:%02d:%02d", 
            timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds());
  } else {
    strcpy(timeStr, "--:--:--");
  }
  
  doc["time"] = timeStr;
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleGetWiFi() {
  DynamicJsonDocument doc(256);
  doc["ssid"] = sysConfig.sta_ssid;
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleSaveWiFi() {
  DynamicJsonDocument doc(256);
  deserializeJson(doc, server.arg("plain"));
  
  strcpy(sysConfig.sta_ssid, doc["ssid"]);
  strcpy(sysConfig.sta_password, doc["password"]);
  saveConfiguration();
  
  server.send(200, "application/json", "{\"success\":true}");
  delay(1000);
  ESP.restart();
}

void handleGetNTP() {
  DynamicJsonDocument doc(256);
  doc["ntpServer"] = sysConfig.ntp_server;
  doc["gmtOffset"] = sysConfig.gmt_offset;
  doc["daylightOffset"] = sysConfig.daylight_offset;
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleSaveNTP() {
  DynamicJsonDocument doc(256);
  deserializeJson(doc, server.arg("plain"));
  
  strcpy(sysConfig.ntp_server, doc["ntpServer"]);
  sysConfig.gmt_offset = doc["gmtOffset"];
  sysConfig.daylight_offset = doc["daylightOffset"];
  saveConfiguration();
  
  if (wifiConnected) {
    ntpServer = String(sysConfig.ntp_server);
    gmtOffset_sec = sysConfig.gmt_offset;
    daylightOffset_sec = sysConfig.daylight_offset;
    timeClient.setPoolServerName(ntpServer.c_str());
    timeClient.setTimeOffset(gmtOffset_sec);
    timeClient.update();
  }
  
  server.send(200, "application/json", "{\"success\":true}");
}

void handleSyncNTP() {
  if (wifiConnected) {
    timeClient.update();
    server.send(200, "application/json", "{\"success\":true}");
  } else {
    server.send(400, "application/json", "{\"success\":false}");
  }
}
