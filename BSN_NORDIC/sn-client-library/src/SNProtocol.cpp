/** 
 * @file    : SNProtocol.cpp
 * @author  : musahl
 * @date    : 8 Jul 2020
 * @version : 0.0
 * @brief   : Short description
 */
#include <SNProtocol.h>
#include <SNSensorTypes.h>

#include <iomanip>
#include <string.h>
#include <strings.h>

#include "Debug.h"

#undef SPINALDEBUG
//c#define SPINALDEBUG

#define MESSAGE_TIMESTAMP_SIZE (4)
#define MESSAGE_SPINALID_SIZE (3)
#define MESSAGE_SENSORID_SIZE (2)
#define MESSAGE_IMU_LENGTH (20) 
#define MESSAGE_ADC_LENGTH (2)

SNProtocol::SNProtocol() = default;
SNProtocol::~SNProtocol() = default;

SNProtocolEvent::SNProtocolEvent(enum Type type) : type(type)
{
}

SNHarnessTypes SNProtocol::setHarnessType(SNHarnessTypes harness) {
  this->harness=harness;
  return harness;
}

std::string SNProtocol::getMessage_Discovery() {
  return std::string("state_set detect");
 }

std::string SNProtocol::getMessage_startMeasuring() {
  return std::string("state_set running FFFF");
}

std::string SNProtocol::getMessage_stopMeasuring() {
  return std::string("state_set idle FFFF");
}

std::string SNProtocol::getMessage_setOption(std::string &sensorID, std::string optionName, std::string optionValue ) {
  std::string optionSetCommand = "option_"+ optionName;
  optionSetCommand += "_set " + optionValue + " " + sensorID;
  return optionSetCommand;
}

std::string SNProtocol::getMessage_getOption(std::string &sensorID, std::string optionName ) {
  std::string optionGetCommand = "option_"+ optionName;
  optionGetCommand += "_get " +  sensorID;
  return optionGetCommand;
}

std::string SNProtocol::getMessage_writeEEPROMData(std::string &sensorID, const uint8_t *data, const uint_fast32_t length) {
  SNSensorInformation info(sensorID);
  std::string mappedAddress=getSpinalAddress(info);
  std::cout << "writing " << sensorID << " ["<< mappedAddress <<"] " << length << " bytes" << std::endl;
  std::string eepromWriteCommand="eeprom_set "+ mappedAddress +" ";
  char charToAdd = (char)(length&0xFF);
  eepromWriteCommand.append(1,charToAdd);
  charToAdd = (char)((length >> 8)&0xFF);
  eepromWriteCommand.append(1,charToAdd);
  eepromWriteCommand.append((const char *)data,length);
  return eepromWriteCommand;
}
std::string SNProtocol::getMessage_getEEPROMData(std::string &sensorID) {
  SNSensorInformation info(sensorID);
  std::string mappedAddress=getSpinalAddress(info);
  //std::cout << "reading " << sensorID << " ["<< mappedAddress <<"] " << std::endl;
  std::string eepromGetCommand="eeprom_get "+ mappedAddress;
  return eepromGetCommand;
}


std::unique_ptr<SNProtocolEvent> SNProtocol::parseMessage(const uint8_t *const data, const uint_fast32_t length)
{
  if (length > MIN_COMMAND_LENGTH)
    switch (data[0])
    {
    case 'x':
      return parseSensorData(data, length);
      break;
    case 'd':
      return parseDetect(data, length);
      break;
    case 'e':
      return parseEeprom(data, length);
      break;
    case 's':
      return parseState(data, length);
      break;
    case 'o':
      return parseOption(data, length);
      break;
    default:
      DEBUG() << "unknown command - just forwarding here";
      return parseRAW(data, length);
      break;
    }
  DEBUG() << "invalid command";
  return std::make_unique<SNProtocolEvent>();
}

std::string SNProtocol::getSpinalAddress(SNSensorInformation &info) {
  switch(harness) {
    case s_BIONICUpperBodyREDUCED:
        switch(info.getPosition()){
          case SNSensorPosition::LeftUpperArm:
            return "8051";
          case SNSensorPosition::RightUpperArm:
            return "8052";
          case SNSensorPosition::Pelvis:
            return "8091";
          case SNSensorPosition::Torso:
            return "8092";
          case SNSensorPosition::Head:
            return "8093";
          default:
            return "FFFF";
            break;
        }
      break;
    case s_BIONICLowerBody:
        switch(info.getPosition()){
          case SNSensorPosition::LeftUpperLeg:
            return "8051";
          case SNSensorPosition::LeftLowerLeg:
            return "8052";
          case SNSensorPosition::LeftFoot:
            return "8052";
          case SNSensorPosition::Pelvis:
            return "8091";
          case SNSensorPosition::RightUpperLeg:
            return "8092";
          case SNSensorPosition::RightLowerLeg:
            return "8093";
          case SNSensorPosition::RightFoot:
            return "8094";
          default:
            return "FFFF";
            break;
        }
      break;
    default:
      return info.getID();
      break;
  }
}

uint8_t SpinalGetPosition(uint8_t ID, SNHarnessTypes harness=s_BIONICUpperBodyREDUCED)
{
  switch (ID)
  {    
  case 0x51:
    if(harness == s_BIONICUpperBodyREDUCED)
      return SNSensorPosition::LeftUpperArm;
    else
      return SNSensorPosition::LeftUpperLeg;
    break;
  case 0x52:
    if(harness == s_BIONICUpperBodyREDUCED)
      return SNSensorPosition::RightUpperArm;
    else
      return SNSensorPosition::LeftLowerLeg;
    break;
  case 0x53:
    if(harness == s_BIONICUpperBodyREDUCED)
      return SNSensorPosition::MAXPosition;
    else
      return SNSensorPosition::LeftFoot;
    break;
  case 0x91: 
      return SNSensorPosition::Pelvis;
  case 0x92:
    if(harness == s_BIONICUpperBodyREDUCED)
      return SNSensorPosition::Torso;
    else
      return SNSensorPosition::RightUpperLeg; 
  case 0x93:
    if(harness == s_BIONICUpperBodyREDUCED)
      return SNSensorPosition::Head;
    else
      return SNSensorPosition::RightLowerLeg; 
  case 0x94:
    if(harness == s_BIONICUpperBodyREDUCED)
      return SNSensorPosition::MAXPosition;
    else
      return SNSensorPosition::RightFoot; 
    break;
  default:
    return SNSensorPosition::MAXPosition;
    break;
  }
}
uint8_t SpinalGetTypeByLength(uint8_t length) {
  switch (length) {
    case 24:
      return SNSensorTypes::s_SPINAL;
      break;
    case 20:
      return SNSensorTypes::s_imu;
      break;
    case 2:
      return SNSensorTypes::s_adc;
      break;
    default:
      return SNSensorTypes::s_InvalidType;
  }
}

std::string SpinalDetectMapping(std::string detectString, const SNHarnessTypes harness){
  std::string result;
  if (detectString.rfind("80", 0) == 0) { 
    // spinal sensor!
    unsigned int sensorInfoAsInt;
    std::stringstream ss;
    ss << std::hex << detectString.substr(2,2);
    //std::cout << ss.str() << std::endl;
    ss >> sensorInfoAsInt;
    //std::cout << std::hex << sensorInfoAsInt << std::endl;
    uint16_t mappedValue = (SpinalGetPosition(sensorInfoAsInt, harness) << 2) | (SNSensorTypes::s_SPINAL << 8);
    //std::cout << std::hex << mappedValue << std::endl;
    ss.str("");
    ss.clear();
    ss << std::hex << mappedValue;
    return ss.str();
  } 
    return detectString;
}

uint16_t SpinalIDMapping(const uint8_t *data, const SNHarnessTypes harness)
{
  uint16_t returnValue = 0;
  switch (data[0] & 0x80)
  {
    default:
      returnValue = (SpinalGetPosition(data[1], harness) << 2) | (SpinalGetTypeByLength(data[2]) << 8);
    break;
  }
  return returnValue;
}

int_fast32_t SNProtocolEventSensorData::addSensorData(const uint8_t *data, const uint_fast32_t length, const SNHarnessTypes harness=s_BIONICUpperBodyREDUCED)
{
  if (data == nullptr)
    return -1;

  uint32_t sensorTimeStamp = *((uint32_t*) data);
  //std::cout << "RECEIVED A SENSORMESSAGE at " << std::dec << sensorTimeStamp << " ms" << std::endl;

  uint16_t sensorID = 0;
  const uint8_t *readPointer = &data[MESSAGE_TIMESTAMP_SIZE];
  uint32_t remainingBytes = length - MESSAGE_TIMESTAMP_SIZE;

#ifdef SPINALDEBUG
  std::cout << "remainingBytes = " << std::dec << remainingBytes << std::endl;
  for (int i = 0; i < remainingBytes; i++)
  {
    if (i % 16 == 0)
      std::cout << std::endl;
    std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)readPointer[i] << " ";
  }
  std::cout << std::endl;
#endif

  while (remainingBytes > 3)
  {

    if (readPointer[0] & SNSensorTypes::s_spinaladdress) {
// SPINALIDs
#ifdef SPINALDEBUG
      std::cout << "spinal sensor : " << std::hex << (int) readPointer[0] << (int) readPointer[1]
          << (int) readPointer[2] << std::endl;
#endif
      sensorID = SpinalIDMapping(readPointer, harness);
#ifdef SPINALDEBUG
      std::cout << "sensorID : " << sensorID << std::endl;
#endif
      readPointer += MESSAGE_SPINALID_SIZE;
    } else {
      // SensorIDs
      sensorID = readPointer[0] << 8 | readPointer[1];
      readPointer += MESSAGE_SENSORID_SIZE;
    }

    SNSensorInformation sensorInfo(sensorID);
#ifdef SPINALDEBUG
    std::cout << sensorInfo << std::endl;
#endif
    std::shared_ptr<SNSensorData> sensorToAdd = nullptr;

    switch (sensorInfo.getType()) {
      case SNSensorTypes::s_SPINAL: {
#ifdef SPINALDEBUG
        std::cout << "SPINAL packet" << std::endl;
#endif

        //FIRST IMU DATA
        sensorID = (sensorID & 0xff) | (SNSensorTypes::s_imu<<8);
        SNSensorInformation imuInfo(sensorID);

        sensorToAdd = std::make_shared<SNSensorData_IMU>(imuInfo, readPointer, MESSAGE_IMU_LENGTH);
        readPointer += MESSAGE_IMU_LENGTH;
        if (sensorToAdd != nullptr) {
          sensorToAdd->time = sensorTimeStamp;
          this->dataSet.push_back(sensorToAdd);
        }
        //ADC1 DATA
        sensorID = (sensorID & 0xff) | (SNSensorTypes::s_adc<<8);
        SNSensorInformation adc0Info(sensorID);

        sensorToAdd = std::make_shared<SNSensorData_ADC>(adc0Info, readPointer, MESSAGE_ADC_LENGTH);
        readPointer += MESSAGE_ADC_LENGTH;
        if (sensorToAdd != nullptr) {
          sensorToAdd->time = sensorTimeStamp;
          this->dataSet.push_back(sensorToAdd);
        }
        //ADC2 DATA
        sensorID = (sensorID & 0xff) | (SNSensorTypes::s_adc<<8);
        SNSensorInformation adc1Info(sensorID);

        sensorToAdd = std::make_shared<SNSensorData_ADC>(adc1Info, readPointer, MESSAGE_ADC_LENGTH);
        readPointer += MESSAGE_ADC_LENGTH;
        if (sensorToAdd != nullptr) {
          sensorToAdd->time = sensorTimeStamp;
          this->dataSet.push_back(sensorToAdd);
        }
        sensorToAdd=nullptr;
      }
        break;
      case SNSensorTypes::s_imu: {
#ifdef SPINALDEBUG
      std::cout << "imu sensor" << std::endl;
#endif
      sensorToAdd = std::make_shared<SNSensorData_IMU>(sensorInfo, readPointer, MESSAGE_IMU_LENGTH);
      readPointer += MESSAGE_IMU_LENGTH;
    }
    break;
    case SNSensorTypes::s_adc:
#ifdef SPINALDEBUG
      std::cout << "adc sensor" << std::endl;
#endif
      sensorToAdd = std::make_shared<SNSensorData_ADC>(sensorInfo, readPointer, MESSAGE_ADC_LENGTH);
      readPointer += MESSAGE_ADC_LENGTH;
      break;
    default:
      std::cout << "unknown sensor" << std::endl;
      throw unknownSensorTypeException();
    }

    if (sensorToAdd != nullptr)
    {
      sensorToAdd->time = sensorTimeStamp;
      this->dataSet.push_back(sensorToAdd);
    }

    remainingBytes = length - (uint32_t)((uint64_t)readPointer - (uint64_t)&data[0]);
    if (remainingBytes > 10000)
      remainingBytes = 0;
#ifdef SPINALDEBUG
    std::cout << "remainingBytes = " << std::dec << remainingBytes << std::endl;
    for (int i = 0; i < remainingBytes; i++)
    {
      if (i % 16 == 0)
        std::cout << std::endl;
      std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)readPointer[i] << " ";
    }
    std::cout << std::endl;
#endif
  }
  return remainingBytes;
}

std::unique_ptr<SNProtocolEvent> SNProtocol::parseSensorData(uint8_t const *data,
                                                             uint_fast32_t const length) const
{
  try
  {
    std::unique_ptr<SNProtocolEventSensorData> sensorData = std::make_unique<SNProtocolEventSensorData>();
    sensorData->addSensorData(&data[1], length - 1);
    return sensorData;
  }
  catch (std::exception const &e)
  {
    std::cerr << e.what() << std::endl;
    return std::make_unique<SNProtocolEvent>(SNProtocolEvent::Type::Invalid);
  }
}

std::unique_ptr<SNProtocolEvent> SNProtocol::parseOption(uint8_t const *data,
                                                         uint_fast32_t const length) const
{
  return std::make_unique<SNProtocolEvent>(SNProtocolEvent::Type::Option);
}

std::unique_ptr<SNProtocolEvent> SNProtocol::parseState(uint8_t const *const data,
                                                        uint_fast32_t const length) const
{
  if (memcmp(data, "state ", 7) == 0){
    std::string stateAsString(reinterpret_cast<const char*>(&data[6]),length-6);
    return std::make_unique<SNProtocolEventState>(stateAsString);
  }

  return std::make_unique<SNProtocolEvent>(SNProtocolEvent::Type::State);
}

std::unique_ptr<SNProtocolEvent> SNProtocol::parseDetect(uint8_t const *data,
                                                         uint_fast32_t const length) const
{
  if (memcmp(data, "detect ", 7) == 0)
  {
    std::stringstream ss((const char *)&data[7]);
    std::unique_ptr<SNProtocolEventDetect> detectEvent = std::make_unique<SNProtocolEventDetect>();

    while (ss.good())
    {
      std::string substr;
      std::getline(ss, substr, ',');
      if (substr.length())
      {
        //std::cout << substr << std::endl;
        detectEvent->addSensorInfo(std::make_shared<SNSensorInformation>(SpinalDetectMapping(substr,harness)));
      }  
    }
    return detectEvent;
  }
  return std::make_unique<SNProtocolEvent>(SNProtocolEvent::Type::Invalid);
}

std::unique_ptr<SNProtocolEvent> SNProtocol::parseEeprom(uint8_t const *data,
                                                         uint_fast32_t const length) const
{
  if (memcmp(data, "eeprom ", 7) == 0)
  { 
    std::unique_ptr<SNProtocolEventEEPROMData> eepromEvent = std::make_unique<SNProtocolEventEEPROMData>(&data[14],length - 14); 
    return eepromEvent;
  }
  return std::make_unique<SNProtocolEvent>(SNProtocolEvent::Type::Invalid); 
}

std::unique_ptr<SNProtocolEvent> SNProtocol::parseRAW(uint8_t const *data,
                                                      uint_fast32_t const length) const
{
  return std::make_unique<SNProtocolEvent>(SNProtocolEvent::Type::Raw);
}

SNProtocolEventDetect::SNProtocolEventDetect()
{
  type = SNProtocolEvent::Type::Detect;
}

SNProtocolEventSensorData::SNProtocolEventSensorData()
{
  type = SNProtocolEvent::Type::DataFrame;
}

SNProtocolEventEEPROMData::SNProtocolEventEEPROMData(uint8_t const *data, uint_fast32_t const length) {
  if(data == nullptr){
    type=SNProtocolEvent::Type::Invalid;
  }else{
    type = SNProtocolEvent::Type::Eeprom;
    eepromData.append((const char*)data, length);
    //std::cout << eepromData << std::endl;
  }
}

std::ostream& operator<<(std::ostream& os, const SNProtocolEvent& event)
{
  os << "ProtocolEvent --- " << event.type;
  return os;
}

SNProtocolEventState::SNProtocolEventState(std::string &stateAsString) :
    clientState(stateAsString) {
  type = SNProtocolEvent::Type::State;
}
