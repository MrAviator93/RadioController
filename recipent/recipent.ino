#include <SoftwareSerial.h>
#include <Servo.h>

//http://tutorial.cytron.io/2015/09/04/softwareserial-conflicting-with-servo-interrupt/

//Software serial: TX = digital pin 10, RX = digital pin 11
SoftwareSerial HC12(10, 11);

typedef unsigned short  uint16;
typedef unsigned int    uint32;

const int TRANSMITTER_ID = 9999; //[0] = 15, [1] = 39
const int END_CHECK_VALUE = 6647; //[0] = 247, [1] = 25


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

ControlParams   InputControls;

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);

  //ServoTest1.attach(5);
  //ServoTest1.writeRaw(100);
}

bool isLegitMessage(const DeviceMSG& message)
{
   //Serial.println("Check vals.");
   //Serial.println(message.uniqueID, DEC);
   //Serial.println(message.endCheckVal, DEC);
   //Serial.println(message.deviceID, DEC);
   if (message.uniqueID == TRANSMITTER_ID && message.deviceID == 70 && message.endCheckVal == END_CHECK_VALUE)
   {
    return true;
   }
   return false;
}

int ControlItterations = 0;

void applyControls(const ControlParams& ctrl)
{
   // scale it to use it with the servo (value between 0 and 180)
   //ServoTest1.write(map(ctrl.throttle, 0, 1024, 0, 180));  
   //ServoTest1.writeRaw(ctrl.throttle);
   Serial.println(ctrl.throttle, DEC);
}

void loop() 
{
   DeviceMSG deviceMSG;
   byte byteBuffer[sizeof(deviceMSG)]; //here should be sizeof(DeviceMSG)
   memset(&byteBuffer, 0x00, sizeof(deviceMSG)); //here should be sizeof(DeviceMSG)

  applyControls(InputControls);
   
  if (HC12.available())
  {  
    for (int i = 0; i < sizeof(deviceMSG); i++)
    {
        if (i == 0) {
          byteBuffer[0] = HC12.read();
          while(byteBuffer[0] != 15)
          {
            byteBuffer[0] = HC12.read();   
          }
        }
        else {
            byteBuffer[i] = HC12.read();   
        }
        //Serial.print(byteBuffer[i], DEC);
        //Serial.print("\n");
        delay(1);
    }

    memcpy(&deviceMSG, &byteBuffer, sizeof(DeviceMSG));

    if(isLegitMessage(deviceMSG))
    {
      Serial.print("Legit Message\n");
      //applyControls(deviceMSG.ctrlParam);
      InputControls = deviceMSG.ctrlParam;
    }

    delay(10);
 
  }
}
