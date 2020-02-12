#include "WebsocketInterpreter.h"
#include "FunctionCommands.h"

int isrCount = 0;
void IRAM_ATTR onTimer()
{
  digitalWrite(4, isrCount++%2);
}

hw_timer_t* timer = NULL;
void webSocketDataInterpreter(WebSocketsServer &WEBSOCKETOBJECT, String WEBSOCKETDATA)
{
  String topLevelToken = "";
  String subLevelToken = "";
  String serialClear = "SERIAL UART CLEAR";
  Serial.println("New data received: " + WEBSOCKETDATA);
  
  //I2C related tasks
  if(WEBSOCKETDATA.startsWith("I2C"))
  {
    //Look at start of line for tokens, add +1 to length to account for space
    topLevelToken = "I2C";
    String i2cCommand = WEBSOCKETDATA.substring(topLevelToken.length()+1);
    if(i2cCommand.startsWith("SCAN"))
    {
      WEBSOCKETOBJECT.broadcastTXT(serialClear);
      scanI2CAddressAndRegisters(WEBSOCKETOBJECT);
    }
    if(i2cCommand.startsWith("FIND DEVICES"))
    {
      WEBSOCKETOBJECT.broadcastTXT(serialClear);
      scanI2CAddress(WEBSOCKETOBJECT);
    }    
    if(i2cCommand.startsWith("READ DEVICE"))
    {
      WEBSOCKETOBJECT.broadcastTXT(serialClear);
      //Look at start of line for subtokens, add +1 to length to account for space
      subLevelToken = "READ DEVICE";
      byte deviceToRead = i2cCommand.substring(subLevelToken.length()+1).toInt();
      scanI2CRegisters(WEBSOCKETOBJECT, deviceToRead);
    }
  }
  //Function Generator related tasks
  if(WEBSOCKETDATA.startsWith("FUNCTION"))
  {
    //Look at start of line for tokens, add +1 to length to account for space
    topLevelToken = "FUNCTION";
    String command = WEBSOCKETDATA.substring(topLevelToken.length()+1);
    if(command.startsWith("MODE"))
    {
      Serial.println("Got mode set");
      subLevelToken = "MODE";
      byte deviceToWrite = command.substring(subLevelToken.length()+1).toInt();

      String mode = command.substring(subLevelToken.length()+3);
      if (mode.startsWith("SINE"))
      {
        Serial.println("Setting sine wave");
        Serial.print((int)deviceToWrite);

        func[deviceToWrite-1] = 0;
      }
      else if (mode.startsWith("SQUARE"))
      {
        Serial.println("Setting square wave");
        Serial.print((int)deviceToWrite);

        func[deviceToWrite-1] = 1;
      }
      else if (mode.startsWith("SAWTOOTH"))
      {
        Serial.println("Setting sawtooth wave");
        Serial.print((int)deviceToWrite);

        func[deviceToWrite-1] = 2;
      }
      else
      {
        // turn it off
        func[deviceToWrite-1] = 3;
      }
    }
    else if (command.startsWith("AMPLITUDE"))
    {
      Serial.println("Got amplitude set");

      subLevelToken = "AMPLITUDE";
      byte deviceToWrite = command.substring(subLevelToken.length()+1).toInt();

      float a = command.substring(subLevelToken.length()+3).toFloat();

      Serial.println(deviceToWrite);
      Serial.println(a);
      amplitude[deviceToWrite-1] = (int)(255.0*a/3.3);
      Serial.println(amplitude[deviceToWrite-1]);
    }
    else if (command.startsWith("PERIOD"))
    {
      // in milliseconds
      Serial.println("Got PERIOD set");

      subLevelToken = "PERIOD";
      byte deviceToWrite = command.substring(subLevelToken.length()+1).toInt();

      float a = command.substring(subLevelToken.length()+3).toFloat()*1000.0;// go from ms to micro seconds

      Serial.println(deviceToWrite);
      Serial.println(a);
      period[deviceToWrite-1] = max(a, 1.0f);
      Serial.println(period[deviceToWrite-1]);
    }
  }
  //Oscilloscope related tasks
  if(WEBSOCKETDATA.startsWith("SCOPE"))
  {
    //Look at start of line for tokens, add +1 to length to account for space
    topLevelToken = "SCOPE";
    String scopeCommand = WEBSOCKETDATA.substring(topLevelToken.length()+1);
    if(scopeCommand.startsWith("CHANNEL 1"))
    {
      //Look at start of line for subtokens, add +1 to length to account for space
      subLevelToken = "CHANNEL 1";
      setChannelMode1(scopeCommand.substring(subLevelToken.length()+1));
    }
    if(scopeCommand.startsWith("CHANNEL 2"))
    {
      //Look at start of line for subtokens, add +1 to length to account for space
      subLevelToken = "CHANNEL 2";
      setChannelMode2(scopeCommand.substring(subLevelToken.length()+1));
    }
    if(scopeCommand.startsWith("TIMESCALE"))
    {
      //Look at start of line for subtokens, add +1 to length to account for space
      subLevelToken = "TIMESCALE";
      setMsTimer(scopeCommand.substring(subLevelToken.length()+1).toInt());
    }
    if(scopeCommand.startsWith("DATALOG"))
    {
      //Look at start of line for subtokens, add +1 to length to account for space
      subLevelToken = "DATALOG";
      if(scopeCommand.substring(subLevelToken.length()+1) == "ON")
      {
        setDataLog(true);
      }
      else
      {
        setDataLog(false);
      }
    }
  }
  //Terminal related tasks
  if(WEBSOCKETDATA.startsWith("TERMINAL"))
  {
    //Look at start of line for tokens, add +1 to length to account for space
    topLevelToken = "TERMINAL";
    String terminalCommand = WEBSOCKETDATA.substring(topLevelToken.length()+1);
    if(terminalCommand.startsWith("CONNECT"))
    {
      //Look at start of line for subtokens, add +1 to length to account for space
      subLevelToken = "CONNECT";
      if(terminalCommand.substring(subLevelToken.length()+1) == "ON")
      {
        //CONNECT TO SERIAL
      }
      else
      {
        //DISCONNECT FROM SERIAL
      }
    }
  }
}
