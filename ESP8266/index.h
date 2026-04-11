const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP8266 Relay Control</title>
    <style>
        *{box-sizing:border-box;margin:0;padding:0}
        body{
            font-family:system-ui,-apple-system,sans-serif;
            background:#1a1a2e;
            min-height:100vh;
            display:flex;
            flex-direction:column;
            align-items:center;
            justify-content:center;
            padding:12px;
        }
        .container{
            max-width:500px;
            width:100%;
            background:#16213e;
            border-radius:16px;
            padding:16px;
            box-shadow:0 4px 20px rgba(0,0,0,0.3);
        }
        .header{
            display:flex;
            justify-content:space-between;
            align-items:center;
            margin-bottom:16px;
        }
        .time-display{
            background:#0f3460;
            color:#e94560;
            text-align:center;
            padding:12px;
            border-radius:10px;
            font-size:1.3rem;
            font-weight:bold;
            letter-spacing:0.5px;
            flex:1;
            margin-right:10px;
        }
        .settings-btn{
            background:#533483;
            color:#fff;
            border:none;
            border-radius:10px;
            padding:12px 16px;
            font-size:1.3rem;
            cursor:pointer;
            transition:all 0.2s;
        }
        .settings-btn:hover{
            background:#e94560;
            transform:scale(0.95);
        }
        .grid{
            display:grid;
            grid-template-columns:1fr 1fr;
            gap:10px;
        }
        .btn{
            background:#0f3460;
            color:#fff;
            border:none;
            border-radius:10px;
            padding:14px 8px;
            font-size:1.2rem;
            font-weight:600;
            cursor:pointer;
            transition:all 0.2s;
            width:100%;
            display:flex;
            align-items:center;
            justify-content:space-between;
            box-shadow:0 2px 4px rgba(0,0,0,0.2);
        }
        .btn:hover{
            background:#e94560;
            transform:scale(0.98);
        }
        .btn:active{
            background:#c73e54;
        }
        .status{
            padding:4px 12px;
            border-radius:20px;
            font-size:1rem;
            min-width:55px;
            text-align:center;
        }
        .status.on{
            background:#00b894;
            color:#fff;
        }
        .status.off{
            background:#d63031;
            color:#fff;
        }
        .full-width{
            grid-column:span 2;
        }
        .action-btn{
            background:#533483;
        }
        .action-btn:hover{
            background:#e94560;
        }
        .modal {
            display: none;
            position: fixed;
            z-index: 1000;
            left: 0;
            top: 0;
            width: 100%;
            height: 100%;
            background-color: rgba(0,0,0,0.5);
        }
        .modal-content {
            background-color: #16213e;
            margin: 15% auto;
            padding: 20px;
            border-radius: 12px;
            width: 90%;
            max-width: 350px;
            box-shadow: 0 4px 20px rgba(0,0,0,0.5);
        }
        .modal-header {
            color: #fff;
            font-size: 1.3rem;
            margin-bottom: 15px;
            text-align: center;
        }
        .modal-input {
            width: 100%;
            padding: 12px;
            border-radius: 8px;
            border: none;
            background: #0f3460;
            color: #fff;
            font-size: 1rem;
            margin-bottom: 15px;
        }
        .modal-input:focus {
            outline: 2px solid #e94560;
        }
        .modal-buttons {
            display: flex;
            gap: 10px;
            justify-content: flex-end;
        }
        .modal-btn {
            padding: 10px 20px;
            border: none;
            border-radius: 6px;
            cursor: pointer;
            font-size: 1rem;
            font-weight: 600;
            transition: all 0.2s;
        }
        .modal-btn.save {
            background: #00b894;
            color: #fff;
        }
        .modal-btn.cancel {
            background: #636e72;
            color: #fff;
        }
        .modal-btn:hover {
            opacity: 0.9;
            transform: scale(0.95);
        }
        .edit-icon {
            margin-left: 8px;
            font-size: 0.9rem;
            opacity: 0.7;
            cursor: pointer;
            padding: 2px 5px;
        }
        .edit-icon:hover {
            opacity: 1;
        }
        .relay-text {
            display: flex;
            align-items: center;
        }
        .wifi-status{
            background:#0f3460;
            color:#fff;
            padding:8px 12px;
            border-radius:8px;
            margin-bottom:16px;
            font-size:0.9rem;
            text-align:center;
        }
        @media (max-width:400px){
            .container{padding:12px}
            .btn{font-size:1rem;padding:12px 6px}
            .time-display{font-size:1.1rem}
        }
    </style>
</head>
<body>
<div class="container">
    <div class="header">
        <div class="time-display" id="datetime"></div>
        <button class="settings-btn" onclick="openWifiModal()">⚙️</button>
    </div>
    <div class="wifi-status" id="wifiStatus"></div>
    <div class="grid">
        <button class="btn" onclick="toggleRelay(1)">
            <span class="relay-text">
                <span id="relayName1">Relay 1</span>
                <span class="edit-icon" onclick="event.stopPropagation(); openEditModal(1)"></span>
            </span>
            <span class="status" id="statled1">---</span>
        </button>
        <button class="btn" onclick="toggleRelay(2)">
            <span class="relay-text">
                <span id="relayName2">Relay 2</span>
                <span class="edit-icon" onclick="event.stopPropagation(); openEditModal(2)"></span>
            </span>
            <span class="status" id="statled2">---</span>
        </button>
        <button class="btn" onclick="toggleRelay(3)">
            <span class="relay-text">
                <span id="relayName3">Relay 3</span>
                <span class="edit-icon" onclick="event.stopPropagation(); openEditModal(3)"></span>
            </span>
            <span class="status" id="statled3">---</span>
        </button>
        <button class="btn" onclick="toggleRelay(4)">
            <span class="relay-text">
                <span id="relayName4">Relay 4</span>
                <span class="edit-icon" onclick="event.stopPropagation(); openEditModal(4)"></span>
            </span>
            <span class="status" id="statled4">---</span>
        </button>
        <button class="btn" onclick="toggleRelay(5)">
            <span class="relay-text">
                <span id="relayName5">Relay 5</span>
                <span class="edit-icon" onclick="event.stopPropagation(); openEditModal(5)"></span>
            </span>
            <span class="status" id="statled5">---</span>
        </button>
        <button class="btn" onclick="toggleRelay(6)">
            <span class="relay-text">
                <span id="relayName6">Relay 6</span>
                <span class="edit-icon" onclick="event.stopPropagation(); openEditModal(6)"></span>
            </span>
            <span class="status" id="statled6">---</span>
        </button>
        <button class="btn" onclick="toggleRelay(7)">
            <span class="relay-text">
                <span id="relayName7">Relay 7</span>
                <span class="edit-icon" onclick="event.stopPropagation(); openEditModal(7)"></span>
            </span>
            <span class="status" id="statled7">---</span>
        </button>
        <button class="btn" onclick="toggleRelay(8)">
            <span class="relay-text">
                <span id="relayName8">Relay 8</span>
                <span class="edit-icon" onclick="event.stopPropagation(); openEditModal(8)"></span>
            </span>
            <span class="status" id="statled8">---</span>
        </button>
        <button class="btn action-btn full-width" onclick="allOn()">
            🟡 ALL ON
        </button>
        <button class="btn action-btn full-width" onclick="allOff()">
            🔴 ALL OFF
        </button>
    </div>
</div>

<!-- Relay Edit Modal -->
<div id="editModal" class="modal">
    <div class="modal-content">
        <div class="modal-header">Edit Relay Name</div>
        <input type="text" id="relayNameInput" class="modal-input" maxlength="20" placeholder="Enter new name">
        <div class="modal-buttons">
            <button class="modal-btn cancel" onclick="closeModal()">Cancel</button>
            <button class="modal-btn save" onclick="saveRelayName()">Save</button>
        </div>
    </div>
</div>

<!-- WiFi Settings Modal -->
<div id="wifiModal" class="modal">
    <div class="modal-content">
        <div class="modal-header">WiFi Settings</div>
        <input type="text" id="wifiSsidInput" class="modal-input" maxlength="32" placeholder="Network Name (SSID)">
        <input type="text" id="wifiPassInput" class="modal-input" maxlength="64" placeholder="Password (leave empty for open)">
        <div class="modal-buttons">
            <button class="modal-btn cancel" onclick="closeWifiModal()">Cancel</button>
            <button class="modal-btn save" onclick="saveWifiConfig()">Save & Restart</button>
        </div>
    </div>
</div>

<script>
(function(){
    var states=[0,0,0,0,0,0,0,0];
    var currentEditRelay = 0;
    
    function updateTime(){
        var d=new Date();
        var days=['Sunday','Monday','Tuesday','Wednesday','Thursday','Friday','Saturday'];
        document.getElementById('datetime').textContent=
            days[d.getDay()]+' '+d.toLocaleString();
    }
    
    function updateUI(){
        for(var i=1;i<=8;i++){
            var el=document.getElementById('statled'+i);
            var isOn=(states[i-1]==0);
            el.textContent=isOn?'ON':'OFF';
            el.className='status '+(isOn?'on':'off');
        }
    }
    
    function sendRequest(url){
        var x=new XMLHttpRequest();
        x.open('POST',url,true);
        x.send();
    }
    
    function fetchStates(){
        var x=new XMLHttpRequest();
        x.onreadystatechange=function(){
            if(this.readyState==4 && this.status==200){
                var xml=this.responseXML;
                if(xml){
                    for(var i=0;i<8;i++){
                        var val=xml.getElementsByTagName('analog')[i].childNodes[0].nodeValue;
                        states[i]=parseInt(val);
                    }
                    updateUI();
                }
            }
        };
        x.open('GET','redstate',true);
        x.send();
    }
    
    function fetchRelayNames(){
        var x=new XMLHttpRequest();
        x.onreadystatechange=function(){
            if(this.readyState==4 && this.status==200){
                try{
                    var names=JSON.parse(this.responseText);
                    for(var i=1;i<=8;i++){
                        document.getElementById('relayName'+i).textContent=names['relay'+i];
                    }
                }catch(e){}
            }
        };
        x.open('GET','getnames',true);
        x.send();
    }
    
    function fetchWifiConfig(){
        var x=new XMLHttpRequest();
        x.onreadystatechange=function(){
            if(this.readyState==4 && this.status==200){
                try{
                    var config=JSON.parse(this.responseText);
                    document.getElementById('wifiStatus').textContent='📶 SSID: '+config.ssid;
                }catch(e){}
            }
        };
        x.open('GET','getwifi',true);
        x.send();
    }
    
    window.toggleRelay=function(num){
        sendRequest('/LED'+num);
        setTimeout(fetchStates,300);
    };
    
    window.allOn=function(){
        sendRequest('/allon');
        setTimeout(fetchStates,300);
    };
    
    window.allOff=function(){
        sendRequest('/alloff');
        setTimeout(fetchStates,300);
    };
    
    window.openEditModal=function(relayNum){
        currentEditRelay = relayNum;
        var currentName = document.getElementById('relayName'+relayNum).textContent;
        document.getElementById('relayNameInput').value = currentName;
        document.getElementById('editModal').style.display = 'block';
        document.getElementById('relayNameInput').focus();
    };
    
    window.closeModal=function(){
        document.getElementById('editModal').style.display = 'none';
        currentEditRelay = 0;
    };
    
    window.saveRelayName=function(){
        var newName = document.getElementById('relayNameInput').value.trim();
        if(newName && currentEditRelay > 0){
            var x=new XMLHttpRequest();
            x.open('POST','updatename?relay='+currentEditRelay+'&name='+encodeURIComponent(newName),true);
            x.onreadystatechange=function(){
                if(this.readyState==4 && this.status==200){
                    document.getElementById('relayName'+currentEditRelay).textContent=newName;
                    closeModal();
                }
            };
            x.send();
        }
    };
    
    window.openWifiModal=function(){
        var x=new XMLHttpRequest();
        x.onreadystatechange=function(){
            if(this.readyState==4 && this.status==200){
                try{
                    var config=JSON.parse(this.responseText);
                    document.getElementById('wifiSsidInput').value = config.ssid;
                    document.getElementById('wifiPassInput').value = config.password;
                }catch(e){}
            }
        };
        x.open('GET','getwifi',true);
        x.send();
        document.getElementById('wifiModal').style.display = 'block';
    };
    
    window.closeWifiModal=function(){
        document.getElementById('wifiModal').style.display = 'none';
    };
    
    window.saveWifiConfig=function(){
        var newSsid = document.getElementById('wifiSsidInput').value.trim();
        var newPass = document.getElementById('wifiPassInput').value;
        
        if(newSsid){
            var x=new XMLHttpRequest();
            x.open('POST','updatewifi?ssid='+encodeURIComponent(newSsid)+'&password='+encodeURIComponent(newPass),true);
            x.onreadystatechange=function(){
                if(this.readyState==4 && this.status==200){
                    alert('WiFi settings saved. The device will restart with new settings.');
                    closeWifiModal();
                    setTimeout(function(){
                        location.reload();
                    }, 3000);
                }
            };
            x.send();
        }else{
            alert('SSID cannot be empty!');
        }
    };
    
    // Close modals when clicking outside
    window.onclick = function(event) {
        var editModal = document.getElementById('editModal');
        var wifiModal = document.getElementById('wifiModal');
        if (event.target == editModal) {
            closeModal();
        }
        if (event.target == wifiModal) {
            closeWifiModal();
        }
    };
    
    // Handle Enter key in inputs
    document.getElementById('relayNameInput').addEventListener('keypress', function(e) {
        if(e.key === 'Enter') {
            saveRelayName();
        }
    });
    
    document.getElementById('wifiSsidInput').addEventListener('keypress', function(e) {
        if(e.key === 'Enter') {
            saveWifiConfig();
        }
    });
    
    document.getElementById('wifiPassInput').addEventListener('keypress', function(e) {
        if(e.key === 'Enter') {
            saveWifiConfig();
        }
    });
    
    setInterval(updateTime,1000);
    setInterval(fetchStates,4000);
    updateTime();
    fetchStates();
    fetchRelayNames();
    fetchWifiConfig();
})();
</script>
</body>
</html>
)=====";
