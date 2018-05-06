#include <Arduino.h>
#include <Wire.h>

#include <WiFi.h>
#include <WebSocketsServer.h>
//#include <Hash.h>
#include "I2CCommands.h"
#include "miniDB.h"

extern int func[2];
void functionGeneratorInit(void);
void functionGeneratorHandler(WebSocketsServer &WEBSOCKETOBJECT);
void FunctionGeneratorInit(void);
void functionGeneratorUpdate(int channel, int id);
//void setADCChannel(int CHANNEL);
//int ADCRead(void);
//void ADCHandler(void);
