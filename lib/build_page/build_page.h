#include <Arduino.h>

char page[] PROGMEM = R"(
  <!DOCTYPE html><html>
  <head><title>PV router</title>
  <style>
  body {font-family:Arial}
  td {vertical-align: top;}
  textarea {font-family:Arial; width:300px; height:50px}
  </style></head>
  <body id='initialise'>
  <form action="/heater" method="POST">
    <input type="submit" name="heater" value="ON">
    <input type="submit" name="heater" value="OFF">
  </form>
  <br></br>
  <form action="/ota_update" method="GET">
    <input type="submit" name="ota_update" value="Update">
  </form>
  <h2>PV_router data</h2>
  <table><tr>
  <td>from serial</td>
  </tr><tr>
  <td><textarea id='rxText'
  onfocus='this.value=""'></textarea><br></td>
  </tr></table>

  <script>
  var wskt;
  document.getElementById('initialise').onload = function()
  {init()};
  function init()// open WebSocket
  {
    wskt = new WebSocket('ws://' + window.location.hostname + ':81/');
    wskt.onmessage = function(rx)
    {  // client receive message
      var obj = rx.data;
      var js = JSON.parse(obj);
      document.getElementById('rxText').value +=
        "InPower : " + js.realPower1.toFixed(0) + " W - " + "DivertedEnerg : " + js.divertedEnergy.toFixed(2) + " Wh\n"
        + "realPower2 : " + js.realPower2.toFixed(0) + " W\n";
    };
  }
  </script>
  </body></html>
)";

// JSON example :
// '{"realPower1": 200.0, "realPower2": -80.0, "divertedEnergy": 0.2}'