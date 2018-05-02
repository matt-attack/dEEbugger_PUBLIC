#include <Arduino.h>
#include <Wire.h>

#include <WiFi.h>
#include <WebSocketsServer.h>
//#include <Hash.h>

void scanI2CAddressAndRegisters(WebSocketsServer &WEBSOCKETOBJECT);
void scanI2CAddress(WebSocketsServer &WEBSOCKETOBJECT);
void scanI2CRegisters(WebSocketsServer &WEBSOCKETOBJECT, byte ADDRESS);
