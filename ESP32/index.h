const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
    <head>
        <title>ESP32 8 Channel Relay Module</title>
        <script>
        var State1;
        var State2;
        var State3;
        var State4;
        var State5;
        var State6;
        var State7;
        var State8;
      function DisplayCurrentTime() {
          var dt = new Date();
  var weekday = new Array(7);
  weekday[0] = "Sunday";
  weekday[1] = "Monday";
  weekday[2] = "Tuesday";
  weekday[3] = "Wednesday";
  weekday[4] = "Thursday";
  weekday[5] = "Friday";
  weekday[6] = "Saturday";
  var dow = weekday[dt.getDay()];
document.getElementById("datetime").innerHTML = (dow) +" "+ (dt.toLocaleString());
        setTimeout('DisplayCurrentTime()', 1000);
      };
        function GetState()
        {
          var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
                            State1 = this.responseXML.getElementsByTagName("analog")[0].childNodes[0].nodeValue;
                            State2 = this.responseXML.getElementsByTagName("analog")[1].childNodes[0].nodeValue;
                            State3 = this.responseXML.getElementsByTagName("analog")[2].childNodes[0].nodeValue;
                            State4 = this.responseXML.getElementsByTagName("analog")[3].childNodes[0].nodeValue;
                            State5 = this.responseXML.getElementsByTagName("analog")[4].childNodes[0].nodeValue;
                            State6 = this.responseXML.getElementsByTagName("analog")[5].childNodes[0].nodeValue;
                            State7 = this.responseXML.getElementsByTagName("analog")[6].childNodes[0].nodeValue;
                            State8 = this.responseXML.getElementsByTagName("analog")[7].childNodes[0].nodeValue;
    }
    if (State1 == 0) {
    document.getElementById("statled1").innerHTML = "ON";
    var elem = document.getElementById("statled1");
    elem.style.color = "green";
    elem.style.background = "white";
    elem.style.fontSize = "32px";
    elem.style.fontWeight = "bold";
  } else {
    document.getElementById("statled1").innerHTML = "OFF";
    var elem = document.getElementById("statled1");
    elem.style.color = "red";
    elem.style.background = "white";
    elem.style.fontSize = "32px";
    elem.style.fontWeight = "bold";
  }
    if (State2 == 0) {
    document.getElementById("statled2").innerHTML = "ON";
    var elem = document.getElementById("statled2");
    elem.style.color = "green";
    elem.style.background = "white";
    elem.style.fontSize = "32px";
    elem.style.fontWeight = "bold";
  } else {
    document.getElementById("statled2").innerHTML = "OFF";
    var elem = document.getElementById("statled2");
    elem.style.color = "red";
    elem.style.background = "white";
    elem.style.fontSize = "32px";
    elem.style.fontWeight = "bold";
  }
    if (State3 == 0) {
    document.getElementById("statled3").innerHTML = "ON";
    var elem = document.getElementById("statled3");
    elem.style.color = "green";
    elem.style.background = "white";
    elem.style.fontSize = "32px";
    elem.style.fontWeight = "bold";
  } else {
    document.getElementById("statled3").innerHTML = "OFF";
    var elem = document.getElementById("statled3");
    elem.style.color = "red";
    elem.style.background = "white";
    elem.style.fontSize = "32px";
    elem.style.fontWeight = "bold";
  }
    if (State4 == 0) {
    document.getElementById("statled4").innerHTML = "ON";
    var elem = document.getElementById("statled4");
    elem.style.color = "green";
    elem.style.background = "white";
    elem.style.fontSize = "32px";
    elem.style.fontWeight = "bold";
  } else {
    document.getElementById("statled4").innerHTML = "OFF";
    var elem = document.getElementById("statled4");
    elem.style.color = "red";
    elem.style.background = "white";
    elem.style.fontSize = "32px";
    elem.style.fontWeight = "bold";
  }
    if (State5 == 0) {
    document.getElementById("statled5").innerHTML = "ON";
    var elem = document.getElementById("statled5");
    elem.style.color = "green";
    elem.style.background = "white";
    elem.style.fontSize = "32px";
    elem.style.fontWeight = "bold";
  } else {
    document.getElementById("statled5").innerHTML = "OFF";
    var elem = document.getElementById("statled5");
    elem.style.color = "red";
    elem.style.background = "white";
    elem.style.fontSize = "32px";
    elem.style.fontWeight = "bold";
  }
    if (State6 == 0) {
    document.getElementById("statled6").innerHTML = "ON";
    var elem = document.getElementById("statled6");
    elem.style.color = "green";
    elem.style.background = "white";
    elem.style.fontSize = "32px";
    elem.style.fontWeight = "bold";
  } else {
    document.getElementById("statled6").innerHTML = "OFF";
    var elem = document.getElementById("statled6");
    elem.style.color = "red";
    elem.style.background = "white";
    elem.style.fontSize = "32px";
    elem.style.fontWeight = "bold";
  }
    if (State7 == 0) {
    document.getElementById("statled7").innerHTML = "ON";
    var elem = document.getElementById("statled7");
    elem.style.color = "green";
    elem.style.background = "white";
    elem.style.fontSize = "32px";
    elem.style.fontWeight = "bold";
  } else {
    document.getElementById("statled7").innerHTML = "OFF";
    var elem = document.getElementById("statled7");
    elem.style.color = "red";
    elem.style.background = "white";
    elem.style.fontSize = "32px";
    elem.style.fontWeight = "bold";
  }
    if (State8 == 0) {
    document.getElementById("statled8").innerHTML = "ON";
    var elem = document.getElementById("statled8");
    elem.style.color = "green";
    elem.style.background = "white";
    elem.style.fontSize = "32px";
    elem.style.fontWeight = "bold";
  } else {
    document.getElementById("statled8").innerHTML = "OFF";
    var elem = document.getElementById("statled8");
    elem.style.color = "red";
    elem.style.background = "white";
    elem.style.fontSize = "32px";
    elem.style.fontWeight = "bold";
  }
  }
  xhttp.open("GET", "redstate", true);
  xhttp.send();
            setTimeout('GetState()', 4000);
            
        };
      document.addEventListener('DOMContentLoaded', function() {
        DisplayCurrentTime(),GetState();
      }, false);
    </script>
      <style>

  body {
  text-align: center;
  background-color: #ffaaaa;
  }
  table {
  text-align: center;
  border: 2px solid #ff00ff;
  background-color: #ffffff;
  width:100%;
  color: #0000ff;
  -moz-border-radius: 7px;
  -webkit-border-radius: 7px;
  }
  td {
  border: 2px solid #ff0000;
  background-color: #ffff00;
  padding: 16px;
  -moz-border-radius: 7px;
  -webkit-border-radius: 7px;
  }
  .button-led {
  padding: 5px 5px 5px 5px;
  width: 100%;
  border: #fbfb00 solid 3px;
  background-color: #ff00ff;
  color:white;
  font-size:33px;
  padding-bottom:5px;
  font-weight:700;
  -moz-border-radius: 7px;
  -webkit-border-radius: 7px;
  }
  .button-led:hover {
  background-color: #0000ff;
  color: #ff93bd;
  }
  .dtime {
  padding: 5px 5px 5px 5px;
  width: 100%;
  border: #fbfb00 solid 3px;
  background-color: #ffffff;
  color:red;
  font-size:30px;
  padding-bottom:5px;
  font-weight:700;
  -moz-border-radius: 7px;
  -webkit-border-radius: 7px;
  }
  </style>
    </head>
    <body>
   <td>
   <b class="dtime"><span id="datetime"></span></b>
   </td>
    <table>
   <tr>
        <td>
  <form action="/LED1" method="POST">
      <button type="submit" class="button-led">Relay1 <span id="statled1"></span></button>
  </form></td>
        <td style='width:40%'>
  <form action="/LED2" method="POST">
      <button type="submit" class="button-led">Relay2 <span id="statled2"></span></button>
  </form></td>
  </tr>
   <tr>
        <td style='width:40%'>
  <form action="/LED3" method="POST">
      <button type="submit" class="button-led">Relay3 <span id="statled3"></span></button>
  </form></td>
        <td style='width:40%'>
  <form action="/LED4" method="POST">
      <button type="submit" class="button-led">Relay4 <span id="statled4"></span></button>
  </form></td>
  </tr>
   <tr>
        <td>
  <form action="/LED5" method="POST">
      <button type="submit" class="button-led">Relay5 <span id="statled5"></span></button>
  </form></td>
        <td style='width:40%'>
  <form action="/LED6" method="POST">
      <button type="submit" class="button-led">Relay6 <span id="statled6"></span></button>
  </form></td>
  </tr>
   <tr>
        <td style='width:40%'>
  <form action="/LED7" method="POST">
      <button type="submit" class="button-led">Relay7 <span id="statled7"></span></button>
  </form></td>
        <td style='width:40%'>
  <form action="/LED8" method="POST">
      <button type="submit" class="button-led">Relay8 <span id="statled8"></span></button>
  </form></td>
  </tr>
   <tr>
        <td style='width:40%'>
  <form action="/allon" method="POST">
      <button type="submit" class="button-led">All Relay ON</button>
  </form></td>
        <td style='width:40%'>
  <form action="/alloff" method="POST">
      <button type="submit" class="button-led">All Relay OFF</button>
  </form></td>
  </tr>
    </table>
    </body>
</html>
)=====";
