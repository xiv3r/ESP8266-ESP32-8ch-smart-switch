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
        .time-display{
            background:#0f3460;
            color:#e94560;
            text-align:center;
            padding:12px;
            border-radius:10px;
            font-size:1.3rem;
            font-weight:bold;
            margin-bottom:16px;
            letter-spacing:0.5px;
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
        @media (max-width:400px){
            .container{padding:12px}
            .btn{font-size:1rem;padding:12px 6px}
            .time-display{font-size:1.1rem}
        }
    </style>
</head>
<body>
<div class="container">
    <div class="time-display" id="datetime"></div>
    <div class="grid">
        <button class="btn" onclick="toggleRelay(1)">
            Relay 1 <span class="status" id="statled1">---</span>
        </button>
        <button class="btn" onclick="toggleRelay(2)">
            Relay 2 <span class="status" id="statled2">---</span>
        </button>
        <button class="btn" onclick="toggleRelay(3)">
            Relay 3 <span class="status" id="statled3">---</span>
        </button>
        <button class="btn" onclick="toggleRelay(4)">
            Relay 4 <span class="status" id="statled4">---</span>
        </button>
        <button class="btn" onclick="toggleRelay(5)">
            Relay 5 <span class="status" id="statled5">---</span>
        </button>
        <button class="btn" onclick="toggleRelay(6)">
            Relay 6 <span class="status" id="statled6">---</span>
        </button>
        <button class="btn" onclick="toggleRelay(7)">
            Relay 7 <span class="status" id="statled7">---</span>
        </button>
        <button class="btn" onclick="toggleRelay(8)">
            Relay 8 <span class="status" id="statled8">---</span>
        </button>
        <button class="btn action-btn full-width" onclick="allOn()">
            ⚡ ALL ON
        </button>
        <button class="btn action-btn full-width" onclick="allOff()">
            🔴 ALL OFF
        </button>
    </div>
</div>

<script>
(function(){
    var states=[0,0,0,0,0,0,0,0];
    
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
    
    setInterval(updateTime,1000);
    setInterval(fetchStates,4000);
    updateTime();
    fetchStates();
})();
</script>
</body>
</html>
)=====";
