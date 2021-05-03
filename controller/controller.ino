/***********************************************************************
FILE CONTROLLER.INO CONTAINS CONTROLLER IMPLEMENTATION AND ENTRY POINT

FILE UTMOST REVIEW DONE ON (03.05.2021) BY ARTUR K. 
***********************************************************************/

#include "CoreDefinitions.hpp"

#include <SoftwareSerial.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Pins for different microcontrollers:
// Nano: SDA (A4); SCL(A5);
// MEGA: SDA (20); SCL(21);
// Leonardo: SDA (20); SCL(21); 

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
constexpr uint8_t OLED_RESET_PIN = 4; // Reset pin # (or -1 if sharing Arduino reset pin)
constexpr uint8_t SCREEN_WIDTH = 128; // OLED display width, in pixels
constexpr uint8_t SCREEN_HEIGHT = 32; // OLED display height, in pixels
constexpr uint8_t RADIO_RX_PIN = 10;
constexpr uint8_t RADIO_TX_PIN = 11;

// Control input pin declarations
constexpr uint8_t THROTTLE_PIN = A2;
constexpr uint8_t YAW_PIN = 0;
constexpr uint8_t ELEVATOR_PIN = A3;
constexpr uint8_t AILERON_PIN = A6;

void getControlInput( RC::ControllerMsg& msg );
bool receiveMessage();
void displayControlLevels( const RC::ControllerMsg& msg, bool goodTX, bool goodRX );
void setupDisplay();

// Global variable definitions
Adafruit_SSD1306 display( SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET_PIN );
SoftwareSerial HC12( RADIO_RX_PIN, RADIO_TX_PIN );

void setup() 
{
  pinMode( RADIO_RX_PIN, INPUT );
  pinMode( RADIO_TX_PIN, OUTPUT ); 

  Serial.begin( 9600 ); // Needed only for debug purposes
  HC12.begin( 9600 );

  setupDisplay();
}

void loop()
{ 
  static byte byteBuffer[ sizeof( RC::ControllerMsg ) ];
  static RC::ControllerMsg controlMsg;

  getControlInput( controlMsg );

  // Send a message with control parameters
  memset( &byteBuffer, 0x00, sizeof( RC::ControllerMsg ) );
  memcpy( &byteBuffer, &controlMsg, sizeof( RC::ControllerMsg ) ); 
  
  auto bytesSent = HC12.write( byteBuffer, sizeof( RC::ControllerMsg ) );
  auto rslt = receiveMessage();
  displayControlLevels( controlMsg, ( bytesSent == sizeof( RC::ControllerMsg ) ), rslt );
}

void getControlInput( RC::ControllerMsg& msg )
{
   msg.throttle = analogRead(THROTTLE_PIN);
   msg.yaw = analogRead(YAW_PIN);
   msg.elevator = analogRead(ELEVATOR_PIN);
   msg.aileron = analogRead(AILERON_PIN);
   msg.updateCheckSum();
}

void setupDisplay()
{
  if( !display.begin( SSD1306_SWITCHCAPVCC, 0x3C ) ) // Address 0x3C for 128x32
  { 
    Serial.println( F( "SSD1306 allocation failed" ) );
    //for(;;); // Don't proceed, loop forever
  }

  RC::ControllerMsg controlMsg;
  displayControlLevels( controlMsg, false, false );
}

void displayControlLevels( const RC::ControllerMsg& msg, bool goodTX, bool goodRX )
{
  display.clearDisplay();
  display.setTextSize( 1 );             // Normal 1:1 pixel scale
  display.setTextColor( WHITE );        // Draw white text
  display.setCursor( 0, 0 );             // Start at top-left corner
  
  display.print( "Throttle: " );
  display.print( map( msg.throttle, 0, 1023, 0, 100 ) );
  display.print( "%");
 
  // Yaw
  display.print( "\nX: " );
  display.print( 0 /* map(STATIC_YAW, 0, 1024, -100.0, 100.0) */ );
  display.print( "%" );

  // Elevator
  display.print( "\nY: " );
  display.print( map( msg.elevator, 0, 1023, -100.0, 100.0 ) );
  display.print( "%" );

  // Aileron
  display.print( "\nZ: " ); 
  display.print( map( msg.aileron, 0, 1023, -100.0, 100.0 ) );
  display.print( "%" );
  
  display.drawLine( display.width() - 32, 2, display.width() - 32, display.height() - 2, WHITE );

  display.setCursor( display.width() - 25, 0 ); 
  //display.setTextColor( BLACK, WHITE ); // Draw 'inverse' coloured text
  display.println( "RX" );

  display.setCursor( display.width() - 10, 0 ); 
  display.println( ( goodRX ? "C" : "F" ) ); // C refers for connected, F for fail

  display.setCursor( display.width() - 25, 10 ); 
  display.println( "TX" );

  display.setCursor( display.width() - 10, 10 ); 
  display.println( ( goodTX ? "C" : "F" ) ); // C refers for connected, F for fail
  
  //display.setTextSize( 2 );             // Draw 2X-scale text
  //display.setTextColor( WHITE );
  //display.print( F( "0x" ) ); 
  //display.println( 0xDEADBEEF, HEX );

  display.display();
}

bool receiveMessage()
{
  auto startTime = millis();

  // Wait for the response
  while ( HC12.available() < sizeof( RC::AirVehicleMsg ) )
  {
    // Timeout mechanism
    if ( millis() - startTime >= 10 )
    {
      return false;
    }

    // Delay for 1ms
    delay( 1 );
  }

  // Read the message
  if ( HC12.available() > 0 )
  {
    RC::AirVehicleMsg airVehicleMsg;
    byte* pBuffer = reinterpret_cast< byte* >( &airVehicleMsg );

    pBuffer[ 0 ] = HC12.read(); // 0xDA
    pBuffer[ 1 ] = HC12.read(); // 0x08
    pBuffer[ 2 ] = HC12.read(); // 0xD2
    pBuffer[ 3 ] = HC12.read(); // 0x04
    
    startTime = millis();
    while ( pBuffer[ 0 ] != 0xDA && pBuffer[ 1 ] != 0x08 && pBuffer[ 2 ] != 0xD2 && pBuffer[ 3 ] != 0x04 )
    {
      // Timeout mechanism
      if ( millis() - startTime >= 10 )
      {
        // TODO: maybe flush the HC12 buffer ??
        return false;
      }

      pBuffer[ 0 ] = HC12.read();
      pBuffer[ 1 ] = HC12.read();
      pBuffer[ 2 ] = HC12.read();
      pBuffer[ 3 ] = HC12.read();
    }

    for ( int i = 4; i < sizeof( RC::AirVehicleMsg ); i++ )
    {
      pBuffer[ i ] = HC12.read();
    }

    if ( airVehicleMsg.signatureA == 2266 && airVehicleMsg.signatureB == 1234 ) //&& airVehicleMsg.checkSum == 16
    {
      // Good message, do something?
      Serial.println( "Good signature, do something?" );
      return true;
    } 
  }

  return false;
}
