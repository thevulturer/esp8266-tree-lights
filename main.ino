#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Defining the LED pin as pin 5 of the ESP8266
#define ledPin 5

// Defining variables containing Wi-Fi credentials
const char* ssid = "SSID";
const char* pass = "PASSWORD";

// Defining the web server on port 80 (HTTP)
AsyncWebServer server(80);

// Defining default parameter values
String minSliderValue = "0";
String maxSliderValue = "130";
String delaySliderValue = "5";
String brightSliderValue = "130";
String modeValue = "1";

// Defining a variable that will help when recieving input from the web interface
const char* PARAM_INPUT = "value";

// Defining a variable containing the website's code
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">

    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/css/bootstrap.min.css" rel="stylesheet">
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/js/bootstrap.bundle.min.js"></script> 

    <link rel="icon" type="image/x-icon" href="https://upload.wikimedia.org/wikipedia/commons/thumb/d/d1/Xmas_tree.svg/2048px-Xmas_tree.svg.png">
    <title>Tree lightning</title>
    <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 2.3rem;}
    p {font-size: 1.9rem;}
    %MODE%
    </style>
</head>

<body>

    <nav class="navbar navbar-expand-sm bg-light">
        <div class="container-fluid">
            <ul class="navbar-nav">
                <li class="nav-item">
                    <a class="nav-link active" href="#">Lightning Efects</a>
                </li>
                <li class="nav-item">
                    <a class="nav-link" href="/schedules.html">Schedules</a>
                </li>
            </ul>
        </div>  
    </nav>

    <div class="container-fluid mt-3">
        <div class="row">
            <div class="col-md-9">
                <div id="dynamic" class="container">
                    <h2>Minimum brightness</h2>
                    <p><span id="textMinSliderValue">%MINSLIDERVALUE%</span></p>
                    <p><input type="range" class="form-range" onchange="updateMinSlider(this)" id="minSlider" min="0" max="200" value="%MINSLIDERVALUE%" step="10" class="slider"></p>

                    <h2>Maximum brightness</h2>
                    <p><span id="textMaxSliderValue">%MAXSLIDERVALUE%</span></p>
                    <p><input type="range" class="form-range" onchange="updateMaxSlider(this)" id="maxSlider" min="0" max="200" value="%MAXSLIDERVALUE%" step="10" class="slider"></p>

                    <h2>Delay</h2>
                    <p><span id="textDelaySliderValue">%DELAYSLIDERVALUE%</span></p>
                    <p><input type="range" class="form-range" onchange="updateDelaySlider(this)" id="delaySlider" min="0" max="60" value="%DELAYSLIDERVALUE%" step="1" class="slider"></p>
                </div>

                <div id="static" class="container">
                    <h2>Brightness</h2>
                    <p><span id="textBrightSliderValue">%BRIGHTSLIDERVALUE%</span></p>
                    <p><input type="range" class="form-range" onchange="updateBrightSlider(this)" id="brightSlider" min="0" max="200" value="%BRIGHTSLIDERVALUE%" step="10" class="slider"></p>
                </div>
            </div>

            <div class="col-md-3">
                <div class="container">
                    <label for="lightning-mode">Lightning mode</label>
                    <select id="ligthning-mode" class="form-select">
                        <option %DYNSEL% onclick="hideStatic()">Dynamic</option>
                        <option %STASEL% onclick="hideDynamic()">Static</option>
                    </select> 
                </div>
            </div>
        </div>
    </div>

    <script>
function updateMinSlider(element) {
  var minSliderValue = document.getElementById("minSlider").value;
  document.getElementById("textMinSliderValue").innerHTML = minSliderValue;

  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/minSlider?value="+minSliderValue, true);
  xhr.send();
}

function updateMaxSlider(element) {
  var maxSliderValue = document.getElementById("maxSlider").value;
  document.getElementById("textMaxSliderValue").innerHTML = maxSliderValue;

  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/maxSlider?value="+maxSliderValue, true);
  xhr.send();
}

function updateDelaySlider(element) {
  var delaySliderValue = document.getElementById("delaySlider").value;
  document.getElementById("textDelaySliderValue").innerHTML = delaySliderValue;

  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/delaySlider?value="+delaySliderValue, true);
  xhr.send();
}

function updateBrightSlider(element) {
  var brightSliderValue = document.getElementById("brightSlider").value;
  document.getElementById("textBrightSliderValue").innerHTML = brightSliderValue;
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/brightSlider?value="+brightSliderValue, true);
  xhr.send();
}

function hideDynamic() {
  document.getElementById("dynamic").style.display = "none";
  document.getElementById("static").style.display = "inline";
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/mode?value=2", true);
  xhr.send();
}

function hideStatic() {
  document.getElementById("static").style.display = "none";
  document.getElementById("dynamic").style.display = "inline";
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/mode?value=1", true);
  xhr.send();
}
</script>

</body>
</html>
)rawliteral";

// Replaces placeholders with proper values in the web interface
String processor(const String& var){
  //Serial.println(var);
  if (var == "MINSLIDERVALUE"){
    return minSliderValue;
  }

  if (var == "MAXSLIDERVALUE"){
    return maxSliderValue;
  }

  if (var == "DELAYSLIDERVALUE"){
    return delaySliderValue;
  }

  if (var == "BRIGHTSLIDERVALUE"){
    return brightSliderValue;
  }

  if (var == "MODE") {
    if (modeValue == "1") {
      return "#static {display: none;}";
    }
    else {
      return "#dynamic {display: none;}";
    }
  }

  if (var == "DYNSEL") {
    if (modeValue == "1") {
      return "selected='selected'";
    }
    else {
      return "";
    }
  }

  if (var == "STASEL") {
    if (modeValue == "2") {
      return "selected='selected'";
    }
    else {
      return "";
    }
  }

  return String();
}



void setup() {

  // Enabling serial via USB for diagnostics
  Serial.begin(115200);
  Serial.println();

  // Connecting to Wi-Fi
  WiFi.begin(ssid, pass);

  // While connecting to Wi-Fi send dots to serial
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  // Sending the IP adress to serial
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  // Setting the LED pin to OUTPUT
  pinMode(ledPin, OUTPUT);

  // Tell the server where the website code is and pass the processor function
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/minSlider?value=<inputMessage>
  server.on("/minSlider", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    // GET input1 value on <ESP_IP>/minSlider?value=<inputMessage>
    if (request->hasParam(PARAM_INPUT)) {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      minSliderValue = inputMessage;
    }
    else {
      inputMessage = "No message sent";
    }
    // Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });

  // Send a GET request to <ESP_IP>/maxSlider?value=<inputMessage>
  server.on("/maxSlider", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    // GET input1 value on <ESP_IP>/maxSlider?value=<inputMessage>
    if (request->hasParam(PARAM_INPUT)) {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      maxSliderValue = inputMessage;
    }
    else {
      inputMessage = "No message sent";
    }
    // Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });

  // Send a GET request to <ESP_IP>/delaySlider?value=<inputMessage>
  server.on("/delaySlider", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    // GET input1 value on <ESP_IP>/delaySlider?value=<inputMessage>
    if (request->hasParam(PARAM_INPUT)) {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      delaySliderValue = inputMessage;
    }
    else {
      inputMessage = "No message sent";
    }
    // Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });

  // Send a GET request to <ESP_IP>/brightSlider?value=<inputMessage>
  server.on("/brightSlider", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    // GET input1 value on <ESP_IP>/brightSlider?value=<inputMessage>
    if (request->hasParam(PARAM_INPUT)) {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      brightSliderValue = inputMessage;
    }
    else {
      inputMessage = "No message sent";
    }
    // Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });

  // Send a GET request to <ESP_IP>/mode?value=<inputMessage>
  server.on("/mode", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    // GET input1 value on <ESP_IP>/mode?value=<inputMessage>
    if (request->hasParam(PARAM_INPUT)) {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      modeValue = inputMessage;
    }
    else {
      inputMessage = "No message sent";
    }
    // Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });

  // Start the web server
  server.begin();
}



void loop() {
  if (modeValue == "1") {
    for (int i = minSliderValue.toInt(); i < maxSliderValue.toInt(); i++) {
      analogWrite(ledPin, i);
      delay(delaySliderValue.toInt());
    } 

    for (int i = maxSliderValue.toInt(); i > minSliderValue.toInt(); i--) {
      analogWrite(ledPin, i);
      delay(delaySliderValue.toInt());
    } 
  }
  else {
    analogWrite(ledPin, brightSliderValue.toInt());
  }
  

}
