/*
 * SNSensorTypes.cpp
 *
 *  Created on: 3 Jul 2020
 *      Author: musahl
 */
#include "SNSensorTypes.h"

#include <string>
#include <ostream>
#include <iomanip>

#include <asio.hpp>

#include "Debug.h"


bool SNSensorDataSet::haveSensor(const std::string &name) const {
  for (const auto  p : *this)
    if (p->getID() == name)
      return true;
  return false;
}

bool SNSensorDataSet::getSensor(std::shared_ptr<SNSensorData> dat,
                                const std::string &name) const {
  if (haveSensor(name))
  {
    dat = getSensor(name);
    return true;
  }
  return false;
}

std::shared_ptr<SNSensorData> SNSensorDataSet::getSensor(
    const std::string &name) const {
  for (const auto  p : *this)
    if (p->getID() == name)
      return p;
  return std::make_shared<SNSensorData>();
}

int SNSensorDataSet::getSensorIndex(const std::string &name) const {
  for (int i = 0; i < static_cast<int>(size()); ++i)
    if (at(i)->getID() == name)
      return i;
  return -1;
}

SNSensorInformation::SNSensorInformation(std::string sensorInfo) :
  type(s_imu),
  position(Pelvis){
  unsigned int sensorInfoAsInt;
  std::stringstream ss;
  ss << std::hex << sensorInfo;
  ss >> sensorInfoAsInt;
  type=createSNSensorType((int)((sensorInfoAsInt >> 8)&0xFF));
  position=static_cast<enum SNSensorPosition>((int)((sensorInfoAsInt >> 2)&0x3F));
  id=std::string(4 - sensorInfo.length(), '0') + sensorInfo;
}
SNSensorInformation::SNSensorInformation(uint16_t sensorInfo) :
  type(s_imu),
  position(Pelvis){
  type=createSNSensorType((int)((sensorInfo >> 8)&0xFF));
  position=static_cast<enum SNSensorPosition>((int)((sensorInfo >> 2)&0x3F));
  std::stringstream ss;
  ss << std::hex << sensorInfo;
  id=std::string(4 - ss.str().length(), '0') + ss.str();
}


const char * getSNSensorPosition(enum SNSensorPosition position){
  const char* position2str[] = {
    "Pelvis",
    "RightUpperLeg",
    "RightLowerLeg",
    "RightTalus",
    "RightFoot",
    "RightToes",
    "LeftUpperLeg",
    "LeftLowerLeg",
    "LeftTalus",
    "LeftFoot",
    "LeftToes",
    "Torso",
    "Head",
    "LeftUpperArm",
    "LeftLowerArm",
    "LeftHand",
    "RightUpperArm",
    "RightLowerArm",
    "RightHand",
  };
  if(position < MAXPosition)
    return position2str[position];
  else
    return "unknown";
}

const char * getSNSensorType(enum SNSensorTypes type){
  const char* type2str[] = {
      "Invalid",
      "(M)IMU",
      "Environmental",
      "HRS",
      "Insoles",
      "Respiration",
      "Kinematics",
      "ADC",
      "SPINAL"
  };
  if(type < SNSensorTypes::s_MaxType)
    return type2str[type];
  else  
      return "Unknown";
}

enum SNSensorTypes createSNSensorType(uint16_t type){
  //validate sensortypes
  if(type < SNSensorTypes::s_MaxType)
    return static_cast<enum SNSensorTypes>((int)type);
  else  
    return SNSensorTypes::s_InvalidType;
}


std::ostream& operator<<(std::ostream& os, const SNSensorInformation& sensorInfo)
{
  os << "SensorInfo --- " << 
  "ID: " << sensorInfo.id << 
  ", Type: " << sensorInfo.type 
  << " => " <<  getSNSensorType(sensorInfo.type) << 
  ", Position: " << sensorInfo.position 
  << " => "  << getSNSensorPosition(sensorInfo.position);
  return os;
}

std::ostream& operator<<(std::ostream& os, const SNSensorData& sensorData){
  os << "SensorData --- " << 
  "ID: " << sensorData.getID() << 
  ", Type: " << sensorData.getType()
  << " => " <<  getSNSensorType(sensorData.getType()) << 
  ", Time: " << sensorData.time ;
  return os;
}

std::ostream& operator<<(std::ostream& os, const SNSensorData_IMU& sensorData){
  os << (SNSensorData) sensorData ;
  os << ", T: " << std::dec << std::setw(6) <<  sensorData.temperature << ", ";
  os << "A: [" 
  << std::dec << std::setw(6) << sensorData.acc[0]  << ", "
  << std::dec << std::setw(6) << sensorData.acc[1]  << ", "
  << std::dec << std::setw(6) << sensorData.acc[2] << "], ";
  os << "G: ["
  << std::dec << std::setw(6) << sensorData.gyr[0] << ", " 
  << std::dec << std::setw(6) << sensorData.gyr[1] << ", "
  << std::dec << std::setw(6) << sensorData.gyr[2] << "], ";
  os << "M: [" 
  << std::dec << std::setw(6) << sensorData.mag[0] << ", " 
  << std::dec << std::setw(6) << sensorData.mag[1] << ", "
  << std::dec << std::setw(6) << sensorData.mag[2] << "]";
  return os;
}

#pragma pack(push, 1)
typedef struct imu_raw_s { 
    int16_t acc[3];
    int16_t temp;
    int16_t gyr[3];
    int16_t mag[3];
} imu_raw_st;
#pragma pack(pop)

SNSensorData_IMU::SNSensorData_IMU(SNSensorInformation info, const uint8_t *data, const uint_fast32_t length){
  type=info.getType();
  id=info.getID();
  imu_raw_st data_as_imu=*(reinterpret_cast<const imu_raw_st *>(data));
  temperature = (float) (int16_t) ntohs(data_as_imu.temp);
  for(int i=0;i<3;i++){
    acc[i] = (float) (int16_t) ntohs(data_as_imu.acc[i]);
    gyr[i] = (float) (int16_t) ntohs(data_as_imu.gyr[i]);
    mag[i] = (float) (int16_t) ntohs(data_as_imu.mag[i]);
  }
}

SNSensorData_ADC::SNSensorData_ADC(SNSensorInformation info, const uint8_t *data, const uint_fast32_t length){
  type=info.getType();
  id=info.getID();
  const int16_t * measurement=reinterpret_cast<const int16_t *>(data);
  value = (float) *measurement;
}
