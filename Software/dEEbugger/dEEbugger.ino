#include <Arduino.h>
#include <WebSocketsServer.h>
//#include <Hash.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
//#include <DNSServer.h>
//#include <WiFiManager.h>
#include <Wire.h>
#include "miniDB.h"
#include "websiteHTML.h"
#include "WebsocketInterpreter.h"
#include "FunctionCommands.h"

#define APMODE_BOOT_PIN 4

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
void handleRoot();
void handleNotFound();
void serialEvent();

boolean bootAPFlag = true;

String inputString = "";
int uartIntData = -1;
boolean stringComplete = false;
String webSocketData = "";

unsigned long oldTime = 0;
unsigned long oldTimeADC = 0;
unsigned long currentTime = 0;

const char *ssid = "dEEbugger";
const char *password = "DEBUGGIN4DAYZ";

//ESP8266WiFiMulti WiFiMulti;

WiFiServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void loop2(void*)
{
  //update the DAC
  while (true)
  {
    functionGeneratorUpdate(25, 0);
    functionGeneratorUpdate(26, 1);
  }
}
void setup()
{  
  Serial.begin(115200);
  if(false)//digitalRead(APMODE_BOOT_PIN))
  {
    WiFi.disconnect();
    WiFi.softAP(ssid, password);
    Serial.println();
    Serial.println("Booting in AP mode");
    Serial.println("Go to 192.168.4.1 to access the dEEbugger");
    Serial.println("NOTE: OTA is NOT available in AP mode");
  }
  else
  {
    Serial.println();
    Serial.println("Booting in client mode");
    Serial.println("OTA is available");
    //WiFiManager wifiManager;
    //wifiManager.autoConnect(ssid,password);
    // Connect to WiFi network
    WiFi.begin(ssid, password);
    Serial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.print("IP address: ");  
    if (!MDNS.begin("dEEbugger"))
    {
      Serial.println("Error setting up MDNS responder!");
      while(1)
      { 
        delay(1000);
      }
    }
    Serial.println("mDNS responder started");
  
    Serial.print("Connect to http://dEEbugger.local or http://");
    Serial.println(WiFi.localIP());
    
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("ws", "tcp", 81); 
  }

  //server.on("/", handleRoot);
  //server.onNotFound(handleNotFound);
  server.begin();

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  
  Wire.begin(); 

  scopeInit();
  setMsTimer(40);
  xTaskCreatePinnedToCore(loop2, "loop2", 4096, NULL, 1, NULL, 1);
}

void loop()
{  
  currentTime = millis();
  serialEvent();
  webSocket.loop();
  //server.handleClient();

  //handle client
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.printf("Content-Length: %u\r\n", sizeof(INDEX_HTML));
          client.println();

          client.print(INDEX_HTML);
          client.println();
          client.flush();
          break;
        }
      }
    }
    client.stop();
  }
  if((currentTime - oldTimeADC)>=1)
  {
    ADCHandler();
    oldTimeADC = currentTime;
  }
  if(webSocketData!="")
  {
    webSocketDataInterpreter(webSocket, webSocketData);
    webSocketData = "";
  }
  if((currentTime - oldTime)>=getMsTimer())
  {
    scopeHandler(webSocket);
    webSocketData = "";
    oldTime = currentTime;
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length)
{
  switch(type)
    {
    case WStype_DISCONNECTED:
        Serial.printf("[%u] Disconnected!\r\n", num);
        break;
    case WStype_CONNECTED:
        {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\r\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        }
        break;
    case WStype_TEXT:
         webSocketData = String((const char *)payload);
        break;
    case WStype_BIN:
        Serial.printf("[%u] get binary length: %u\r\n", num, length);
        //hexdump(payload, length);

        // echo data back to browser
        webSocket.sendBIN(num, payload, length);
        break;
    default:
        Serial.printf("Invalid WStype [%d]\r\n", type);
        break;
    }
}

void handleRoot()
{
  //server.send_P(200, "text/html", INDEX_HTML);
}

void handleNotFound()
{
  /*String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);*/
}

void serialEvent()
{
  while (Serial.available())
  {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      stringComplete = true;
      return;
    } else {
      inputString += inChar;
    }
  }
  if (stringComplete) {
    if(getUartScopeFlag())
    {
      uartIntData = inputString.toInt();
      if(uartIntData)
      {
        setUartScopeData(inputString);
        Serial.println(inputString);
      }
      else if(inputString == "0")
      {
        setUartScopeData(inputString);
        Serial.println(inputString);
      }
      else
      {
        String line = "SERIAL UART " + inputString;
        webSocket.broadcastTXT(line);
        Serial.println(line);
      }
    }
    else{
      String line = "SERIAL UART " + inputString;
      webSocket.broadcastTXT(line);
      Serial.println(line);
    }
    inputString = "";
    stringComplete = false;
  }
}
