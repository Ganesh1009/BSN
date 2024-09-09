/**
 * @file SNSensorTypes.h
 *
 * @date 9 Jun 2020
 * @author Mathias Musahl <mathias.musahl@dfki.de>
 */

#ifndef INCLUDE_SNSENSORTYPES_H_
#define INCLUDE_SNSENSORTYPES_H_

#include <string>
#include <vector>
#include <memory>

enum SNSensorTypes {
  s_InvalidType,
  s_imu,
  s_environmental,
  s_HRS,
  s_insoles,
  s_respiration,
  s_kinematics,
  s_adc,
  s_SPINAL,
  s_MaxType,
  s_spinaladdress = 0x80
};

enum SNSensorPosition {
  Pelvis = 0,
  RightUpperLeg = 1,
  RightLowerLeg = 2,
  RightTalus = 3,
  RightFoot = 4,
  RightToes = 5,
  LeftUpperLeg = 6,
  LeftLowerLeg = 7,
  LeftTalus = 8,
  LeftFoot = 9,
  LeftToes = 10,
  Torso = 11,
  Head = 12,
  LeftUpperArm = 13,
  LeftLowerArm = 14,
  LeftHand = 15,
  RightUpperArm = 16,
  RightLowerArm = 17,
  RightHand = 18,
  MAXPosition = 19
};

enum SNHarnessTypes {
  s_BIONICUpperBodyREDUCED,
  s_BIONICLowerBody,
  s_unknown
};

/**
 * @brief custom exception for unknown sensordata
 * 
 */
class unknownSensorTypeException: public std::exception
{
  virtual const char* what() const throw()
  {
    return "Parsing unknown SensorData";
  }
};


const char * getSNSensorPosition(enum SNSensorPosition position);
const char * getSNSensorType(enum SNSensorTypes type);
enum SNSensorTypes createSNSensorType(uint16_t type);


class SNSensorInformation{
 public:
  inline const std::string& getID() const { return id; }
  inline const enum SNSensorTypes getType() const { return type; }
  inline const enum SNSensorPosition getPosition() const { return position; }
  SNSensorInformation(std::string sensorInfo);
  SNSensorInformation(uint16_t sensorInfo);
  friend std::ostream& operator<<(std::ostream& os, const SNSensorInformation& sensorInfo);
  std::string& getEEPROMData() {
    return eepromData;
  }
  void setEEPROMData(std::string eepromDataToSet) {
    eepromData = eepromDataToSet;
  }

 private:
  enum SNSensorTypes type;
  enum SNSensorPosition position;
  std::string id;
  std::string eepromData;

};

//string operators
std::ostream& operator<<(std::ostream& os, const SNSensorInformation& sensorInfo);

class SNSensorData {
 public:
  inline const std::string& getID() const { return id; }
  inline const enum SNSensorTypes getType() const { return type; }
  friend std::ostream& operator<<(std::ostream& os, const SNSensorData& sensorData);
  uint32_t time;

 protected:
  enum SNSensorTypes type;
  std::string id;
};

std::ostream& operator<<(std::ostream& os, const SNSensorData& sensorData);

class SNSensorData_IMU : public SNSensorData {
 public:
  SNSensorData_IMU(SNSensorInformation info, const uint8_t *data, const uint_fast32_t length);
  float acc[3];
  float gyr[3];
  float mag[3];
  float temperature;
  friend std::ostream& operator<<(std::ostream& os, const SNSensorData_IMU& sensorData);
};

std::ostream& operator<<(std::ostream& os, const SNSensorData_IMU& sensorData);

class SNSensorData_Environmental : public SNSensorData {
 public:
  float temperature;
  float humidity;
  float barometric_pressure;
};

class SNSensorData_Heartrate : public SNSensorData {
 public:
  float rate;
};
class SNSensorData_RespiratoryRate : public SNSensorData {
 public:
  float rate;
};

class SNSensorData_ADC : public SNSensorData {
  public:
    SNSensorData_ADC(SNSensorInformation info, const uint8_t *data, const uint_fast32_t length);
    float value;
};

class SNSensorData_Insoles : public SNSensorData {
 public:
  float center_of_pressure[2];
  float pads[16];
  float temperature;
  uint32_t total_force;
  uint32_t battery_level;
};

class SNSensorDataSet : public std::vector<std::shared_ptr<SNSensorData>>
{
 public:
  bool haveSensor(const std::string& name) const;
  bool getSensor(std::shared_ptr<SNSensorData> dat, const std::string& name) const;
  std::shared_ptr<SNSensorData> getSensor(const std::string& name) const;
  int getSensorIndex(const std::string& name) const;
};


#endif /* INCLUDE_SNSENSORTYPES_H_ */
