/***********************************************************************
FILE RECIPENT.INO CONTAINS RECIPENT IMPLEMENTATION AND ENTRY POINT

TODO: THIS FILE & IMPLEMENTATION MUST BE REVIEWED

FILE UTMOST REVIEW DONE ON (03.05.2021) BY ARTUR K. 
***********************************************************************/

#include "CoreDefinitions.hpp"

#include <SoftwareSerial.h>
#include <Servo.h>

//http://tutorial.cytron.io/2015/09/04/softwareserial-conflicting-with-servo-interrupt/

// Software serial: TX = digital pin 10, RX = digital pin 11
SoftwareSerial HC12(10, 11);

typedef unsigned short  uint16;
typedef unsigned int    uint32;

struct ControlParams
{
  uint16 throttle;
  uint16 yaw;
  uint16 elevator;
  uint16 aileron;
};

struct AdditionalControlParams
{
  uint16 value;
};

struct DeviceMSG
{
  uint16                    uniqueID;
  uint16                    deviceID;
  ControlParams             ctrlParam;
  AdditionalControlParams   adCtrlParam;
  uint16                    endCheckVal;
};

ControlParams InputControls;

void setup() 
{
  Serial.begin(9600);
  HC12.begin(9600);
}

bool isLegitMessage(const DeviceMSG& message);
void applyControls(const ControlParams& ctrl);

void loop() 
{
  static DeviceMSG deviceMSG;
  static byte byteBuffer[ sizeof( DeviceMSG ) ];
  memset( &byteBuffer, 0x00, sizeof( DeviceMSG ) );

  auto startTime = millis();

  // Wait for the response
  while ( HC12.available() < sizeof( DeviceMSG ) )
  {
    // Timeout mechanism
    if ( millis() - startTime >= 10 )
    {
      // Return as there are no control inputs to read!
      // However we may add sending some info later!
      return;
    }

    // Delay for 1ms
    delay( 1 );
  }
   
  // TODO: Rewrite this entire part of a function!
  if (HC12.available())
  {  
    for (int i = 0; i < sizeof(DeviceMSG); i++)
    {
        if (i == 0) 
        {
          byteBuffer[0] = HC12.read();
          while(byteBuffer[0] != 15)
          {
            byteBuffer[0] = HC12.read();   
          }
        }
        else 
        {
            byteBuffer[i] = HC12.read();   
        }
        
        delay(1);
    }

    memcpy( &deviceMSG, &byteBuffer, sizeof( DeviceMSG ) );

    if( isLegitMessage( deviceMSG ) )
    {
      Serial.print("Legit Message\n");
      //applyControls(deviceMSG.ctrlParam);
      InputControls = deviceMSG.ctrlParam;
    }

    delay(10);
  }


  applyControls( InputControls );
}

bool isLegitMessage(const DeviceMSG& message)
{
   if ( message.uniqueID == RC::TRANSMITTER_ID && message.deviceID == 70 && message.endCheckVal == RC::END_CHECK_VALUE )
   {
    return true;
   }
   
   return false;
}

void applyControls(const ControlParams& ctrl)
{
   Serial.println(ctrl.throttle, DEC);
}