#include "ScopeCommands.h"

String channelModeOutput1;
String channelModeOutput2;
bool toggledChannelOffFlag1;
bool toggledChannelOffFlag2;
byte ADCAddress = 54;

void scopeInit(void)
{
  setChannelMode1("OFF");
  setChannelMode1("OFF");
  toggledChannelOffFlag1 = false;
  toggledChannelOffFlag2 = false;
  channelModeOutput1 = "";
  channelModeOutput2 = "";
  setUartScopeData("0");
  ADCInit();
}
void scopeHandler(WebSocketsServer &WEBSOCKETOBJECT)
{
  //Channel 1
  if((getChanneMode1()=="4V ADC")||(getChanneMode1()=="64V ADC"))
  {
    toggledChannelOffFlag1 = false;
    channelModeOutput1 = "SCOPE ADC DATACHANNEL1";
    channelModeOutput1 += getADCScopeData1();    
    WEBSOCKETOBJECT.broadcastTXT(channelModeOutput1);
    if(getDataLog())
    {
      Serial.print("CHANNEL1, ");
      Serial.println(getADCScopeData1());
    }
    clearADCScopeData1();
  }
  if(getChanneMode1()=="UART")
  {
    toggledChannelOffFlag1 = false;
    channelModeOutput1 = "SCOPE UART DATACHANNEL1";
    channelModeOutput1 += getUartScopeData();
    WEBSOCKETOBJECT.broadcastTXT(channelModeOutput1);
    clearUartScopeData();
  }
  if(getChanneMode1()=="OFF")
  {
    if(!toggledChannelOffFlag1)
    {
      toggledChannelOffFlag1 = true;
      channelModeOutput1 = "SCOPE OFF DATACHANNEL1";
      channelModeOutput1 += " 0";
      WEBSOCKETOBJECT.broadcastTXT(channelModeOutput1);
    }
  }
  //Channel 2
  if((getChanneMode2()=="4V ADC")||(getChanneMode2()=="64V ADC"))
  {
    toggledChannelOffFlag2 = false;
    channelModeOutput2 = "SCOPE ADC DATACHANNEL2";
    channelModeOutput2 += getADCScopeData2();    
    WEBSOCKETOBJECT.broadcastTXT(channelModeOutput2);
    if(getDataLog())
    {
      Serial.print("CHANNEL2, ");
      Serial.println(getADCScopeData2());
    }
    clearADCScopeData2();
  }
  if(getChanneMode2()=="UART")
  {
    toggledChannelOffFlag2 = false;
    channelModeOutput2 = "SCOPE UART DATACHANNEL2";
    channelModeOutput2 += getUartScopeData();
    WEBSOCKETOBJECT.broadcastTXT(channelModeOutput2);
    clearUartScopeData();
  }
  if(getChanneMode2()=="OFF")
  {
    if(!toggledChannelOffFlag2)
    {
      toggledChannelOffFlag2 = true;
      channelModeOutput2 = "SCOPE OFF DATACHANNEL2";
      channelModeOutput2 += " 0";
      WEBSOCKETOBJECT.broadcastTXT(channelModeOutput2);
    }
  }
}
void ADCInit(void)
{
  byte internalError;
  byte ADCSetupByte = 210;
	byte ADCConfigByte = 97;
  Wire.beginTransmission(ADCAddress);
  Wire.write(ADCSetupByte);
  Wire.write(ADCConfigByte);
  internalError = Wire.endTransmission();
	if (internalError == 0)
	{
		Serial.println("ADC Initialized");
	}
}
char active_channel = 0;
void setADCChannel(int CHANNEL)
{   
    analogReadResolution(12);
    analogSetWidth(12);
    analogSetCycles(8);
    analogSetSamples(1);
    analogSetClockDiv(1);
    analogSetAttenuation(ADC_11db);
}
int ADCRead(char channel)
{
	//adcStart(2);
  int value = analogRead(channel);
  return value;//adcEnd(2);
}
void ADCHandler(void)
{
    if(getChanneMode1()=="4V ADC")
    {
        setADCChannel(0);
        addADCScopeData1(String(ADCRead(35)));
    }
    if(getChanneMode1()=="64V ADC")
    {
        setADCChannel(0);
        addADCScopeData1(String(ADCRead(35)));
    }
    if(getChanneMode2()=="4V ADC")
    {
        setADCChannel(1);
        addADCScopeData2(String(ADCRead(34)));
    }
    if(getChanneMode2()=="64V ADC")
    {
        setADCChannel(1);
        addADCScopeData2(String(ADCRead(34)));
    }
}

