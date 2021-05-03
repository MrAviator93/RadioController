/***********************************************************************
FILE COREDEFINITIONS.HPP CONTAINS GENERAL DEFINITIONS FOR THE WHOLE PROJECT

FILE UTMOST REVIEW DONE ON (03.05.2021) BY ARTUR K. 
***********************************************************************/

#ifndef RC_CORE_DEFINITIONS_HPP__
#define RC_CORE_DEFINITIONS_HPP__

namespace RC
{
constexpr uint16_t TRANSMITTER_ID = 9999;  // [0] = 15,  [1] = 39
constexpr uint16_t END_CHECK_VALUE = 6647; // [0] = 247, [1] = 25

//-----------------------------------------------------------------------
// CONTROLLER MESSAGE
//
// Message sent from the controller to the receiver.
//-----------------------------------------------------------------------
struct ControllerMsg
{
  ControllerMsg() : 
    signatureA{ 2266 },
    signatureB{ 1234 } 
    { }
  
  ~ControllerMsg() = default;

  void updateCheckSum()
  {
    checkSum = throttle + yaw + elevator + aileron;
  }
  
  // Signature
  uint16_t signatureA;
  uint16_t signatureB;

  // Control parameters
  uint16_t throttle{};
  uint16_t yaw{};
  uint16_t elevator{};
  uint16_t aileron{};
  
  // Checksum
  uint32_t checkSum{};
};

//-----------------------------------------------------------------------
// AIR VEHICLE MESSAGE
//
// Message sent from the receiver to the controller.
//-----------------------------------------------------------------------
struct AirVehicleMsg
{
  AirVehicleMsg() :
    signatureA{ 2266 },
    signatureB{ 1234 }
    {}
  
  // Signatures
  uint16_t signatureA;
  uint16_t signatureB;

  // Put parameters here (airspeed, altitude, deflections, e.g.)

  uint32_t checkSum{};
};

} // RC
#endif // RC_CORE_DEFINITIONS_HPP__