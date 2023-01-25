#include <Arduino.h>

char page[] PROGMEM = R"(
  <!DOCTYPE html>
  <html>
    <head>
      <title>PV router</title>
      <style>
      body {font-family:Arial}
      td {vertical-align: top;}
      textarea {font-family:Arial; width:300px; height:50px}
      </style>
    </head>
    <body id='initialise'>
    <button onclick='clickHeaterON()'>Heater ON</button>
    <button onclick='clickHeaterOFF()'>Heater OFF</button>
    <br></br>
    <button onclick='clickUpdate()'>UPDATE</button>
    <br></br>
    <button onclick='clickResetDivE()'>Reset Div Energy</button>
    <br></br>
    <h2>PV_router data</h2>
    <table>
      <tr>
        <td>from serial</td>
      </tr>
      <tr>
        <td>
          <textarea id='rxText' onfocus='this.value=""'></textarea>
          <br>
        </td>
        <td>
          <textarea id='errorText' onfocus='this.value=""'></textarea>
          <br>
        </td>
      </tr>
    </table>

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
        try {
          var js = JSON.parse(obj);
          document.getElementById('rxText').value +=
            "InPower : " + js.realPower1.toFixed(0) + " W - " + "DivertedEnerg : " + js.divertedEnergy.toFixed(2) + " Wh\n"
            + "realPower2 : " + js.realPower2.toFixed(0) + " W\n";
        }
        catch(e)
        {
          document.getElementById('errorText').value += obj + "\n";
        }
      };
    }

    function clickUpdate()
    {
      wskt.send('9');
    };

    function clickHeaterON()
    {
      wskt.send('1');
      document.getElementById('errorText').value += "Heater ON\n";
    };

    function clickHeaterOFF()
    {
      wskt.send('0');
      document.getElementById('errorText').value += "Heater OFF\n";
    };

    function clickResetDivE()
    {
      wskt.send('8');
      document.getElementById('errorText').value += "Reset Div Energy\n";
    };

    </script>
    </body>
  </html>
)";

// JSON example :
// '{"realPower1": 200.0, "realPower2": -80.0, "divertedEnergy": 0.2}'