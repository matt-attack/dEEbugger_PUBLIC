#include "FunctionCommands.h"

void functionGeneratorInit(void)
{
  
}
void functionGeneratorHandler(WebSocketsServer &WEBSOCKETOBJECT)
{
  
}

int func[2] = {0, 1};//sin
float freq = 2;
int offset[2] = { 0, 0};//in 1/255 increments
int amplitude[2] = {255, 255};
int period[2] = {1000000.0/freq, 1000000.0/freq};//in microseconds
void functionGeneratorUpdate(int channel, int id)
{
  float step = micros()%period[id];
  if (func[id] == 0)
  {
    float t = 3.14159265f*2.0f*(step/(float)period[id]);
    dacWrite(channel, sin(t)*127.5f + 127.5f);
  }
  else if (func[id] == 1)
  {
    //square
    if (step > period[id]/2)
      dacWrite(channel, offset[id]);
    else
      dacWrite(channel, offset[id]+amplitude[id]);
  }
}
/*void ADCInit(void)
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
void setADCChannel(int CHANNEL)
{   
    byte internalError, ADCConfigByte;
    //Select correct channel
    switch(CHANNEL)
    {
        case 0:
            ADCConfigByte = 97;
            break;
        case 1:
            ADCConfigByte = 99;
            break;
        default:
            ADCConfigByte = 97;
            break;
    }
    
    //analogInit();
    analogReadResolution(12);
    analogSetWidth(12);
    analogSetCycles(8);
    analogSetSamples(1);
    analogSetClockDiv(1);
    analogSetAttenuation(ADC_11db);

    adcAttachPin(2);
}
int ADCRead(void)
{
	//adcStart(2);
  int value = analogRead(35);
  return value;//adcEnd(2);
}
void ADCHandler(void)
{
    if(getChanneMode1()=="4V ADC")
    {
        setADCChannel(0);
        addADCScopeData1(String(ADCRead()));
    }
    if(getChanneMode1()=="64V ADC")
    {
        setADCChannel(1);
        addADCScopeData1(String(ADCRead()));
    }
    if(getChanneMode2()=="4V ADC")
    {
        setADCChannel(0);
        addADCScopeData2(String(ADCRead()));
    }
    if(getChanneMode2()=="64V ADC")
    {
        setADCChannel(1);
        addADCScopeData2(String(ADCRead()));
    }
}*/

