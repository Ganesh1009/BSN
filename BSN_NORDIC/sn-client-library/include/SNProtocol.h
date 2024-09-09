/** 
 * @file    : SNProtocol.h
 * @author  : musahl
 * @date    : 8 Jul 2020
 * @version : 0.1
 * @brief   : Protocol class for parsing messages received from a SNTransport, returns SNProtocolEvent that needs to be handled properly
 */
#ifndef SRC_SNPROTOCOL_H_
#define SRC_SNPROTOCOL_H_

#include <SNSensorTypes.h>

#include <stdint.h>
#include <vector>
#include <exception>

constexpr auto MIN_COMMAND_LENGTH = (4);

/**
 * @brief Event Base Class that is returned by the SNProtocol->parseMessage Function the type gives information on what event was detected
 * 
 */
class SNProtocolEvent {
 public:
  /**
  * @brief SNPRotocolEvent::Type enum 
  */

  enum Type {
    DataFrame,
    Detect,
    Eeprom,
    Invalid,
    Option,
    Raw,
    State
  } type = Invalid;
  SNProtocolEvent() = default;
  SNProtocolEvent(enum Type);
  virtual ~SNProtocolEvent() = default;
  virtual std::vector<std::shared_ptr<SNSensorInformation>> * getSensorInfo() { return nullptr;};
  virtual SNSensorDataSet * getSensorDataSet() { return nullptr;}
  virtual std::string getEEPROMData() { return ""; }

  friend std::ostream& operator<<(std::ostream& os, const SNProtocolEvent& event);
};

class SNProtocolEventDetect : public SNProtocolEvent {
 public:
  SNProtocolEventDetect();
  virtual ~SNProtocolEventDetect() = default;
  std::vector<std::shared_ptr<SNSensorInformation>> * getSensorInfo() {
    return &detectedSensors;
  }
  void addSensorInfo(std::shared_ptr<SNSensorInformation> infoToAdd) {
    detectedSensors.push_back(infoToAdd);
  }
 private:
  std::vector<std::shared_ptr<SNSensorInformation>> detectedSensors;
};

class SNProtocolEventSensorData : public SNProtocolEvent {
 public:
  SNProtocolEventSensorData();
  virtual ~SNProtocolEventSensorData() = default;
  SNSensorDataSet * getSensorDataSet() {
    return &dataSet;
  }
  int_fast32_t addSensorData(uint8_t const *data, uint_fast32_t const length, const SNHarnessTypes harness);
 private:
  SNSensorDataSet dataSet;
};

class SNProtocolEventState : public SNProtocolEvent {
 public:
  SNProtocolEventState(std::string &stateAsString);
  virtual ~SNProtocolEventState() = default;
  const std::string getState() {
    return clientState;
  }
 private:
  std::string clientState;
};

class SNProtocolEventEEPROMData : public SNProtocolEvent {
 public:
  SNProtocolEventEEPROMData(uint8_t const *data, uint_fast32_t const length);
  virtual ~SNProtocolEventEEPROMData() = default;
  std::string getEEPROMData() {
    return eepromData;
  } 
 private:
  std::string eepromData;
};

class SNProtocol {
 public:
  SNProtocol();
  virtual ~SNProtocol();
  std::unique_ptr<SNProtocolEvent> parseMessage(const uint8_t *const data,
                               const uint_fast32_t length);
  SNHarnessTypes setHarnessType(SNHarnessTypes harness);
//
  std::string getMessage_Discovery();
  std::string getMessage_startMeasuring();
  std::string getMessage_stopMeasuring();
  std::string getMessage_setOption(std::string &sensorID, std::string optionName, std::string optionValue);
  std::string getMessage_getOption(std::string &sensorID, std::string optionName);
  std::string getMessage_writeEEPROMData(std::string &sensorID, const uint8_t *data, const uint_fast32_t length);
  std::string getMessage_getEEPROMData(std::string &sensorID);

//  bool validateEEPROMS();
//  bool startMeasuring();
//  bool stopMeasuring();
//  bool setOption(std::string optionName, std::string optionValue, std::string sensorID = "FFFF");
//  std::string getOption(std::string optionName, std::string sensorID);
//  bool writeEEPROMData(std::string& sensorID,const uint8_t *data, const uint_fast32_t length);

 private:
  std::unique_ptr<SNProtocolEvent> parseSensorData(uint8_t const *data,
                                  uint_fast32_t const length) const;
  std::unique_ptr<SNProtocolEvent> parseState(uint8_t const *const data,
                             uint_fast32_t const length) const;
  std::unique_ptr<SNProtocolEvent> parseOption(uint8_t const *data,
                              uint_fast32_t const length) const;
  std::unique_ptr<SNProtocolEvent> parseDetect(uint8_t const *data,
                              uint_fast32_t const length) const;
  std::unique_ptr<SNProtocolEvent> parseEeprom(uint8_t const *data,
                              uint_fast32_t const length) const;
  std::unique_ptr<SNProtocolEvent> parseRAW(uint8_t const *data,
                           uint_fast32_t const length) const;

  std::string getSpinalAddress(SNSensorInformation &info);

  SNHarnessTypes harness;
};

#endif /* SRC_SNPROTOCOL_H_ */
