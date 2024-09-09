/*
 * SNClientImpl.cpp
 *
 *  Created on: 9 Jun 2020
 *      Author: musahl
 */

#include "SNClientImpl.h"

#include "SNClientTransportSerialPort.h"
#include "SNUtils.h"

#include "config.h"

#include <algorithm>

#include <functional>
#include <utility>
#include <memory>
#include <string>
#include <sstream> 
#include <iomanip>
#include <vector>

#include "Debug.h"


SNClient::impl::impl(std::unique_ptr<SNClientTransport> alternativeTransport)
    :
    DFKIProtocolActive(false),
    state(SNCLientState_Uninitalized),
    transport(std::move(alternativeTransport)) {

  version.major = PROJECT_VER_MAJOR;
  version.minor = PROJECT_VER_MINOR;
  version.patch = PROJECT_VER_PATCH;
  version.build = PROJECT_VER_BUILD;

  transport->attachListener(*this);

  //has to run discovery when uninitialized
  stateTransistion[SNCLientState_Uninitalized][SNCLientState_Discovery] = std::function<bool()>(std::bind(&SNClient::impl::startDiscovery, this));
  stateTransistion[SNCLientState_Uninitalized][SNCLientState_Running] = std::function<bool()>(std::bind(&SNClient::impl::startMeasuring, this));
  stateTransistion[SNCLientState_Uninitalized][SNCLientState_Idle] = std::function<bool()>(std::bind(&SNClient::impl::stopMeasuring, this));

  //from idle mode you can switch to running or trigger a new discovery
  stateTransistion[SNCLientState_Idle][SNCLientState_Discovery] = std::function<bool()>(std::bind(&SNClient::impl::startDiscovery, this));
  stateTransistion[SNCLientState_Idle][SNCLientState_Running] = std::function<bool()>(std::bind(&SNClient::impl::startMeasuring, this));
  stateTransistion[SNCLientState_Idle][SNCLientState_Idle] = std::function<bool()>(std::bind(&SNClient::impl::stopMeasuring, this));
  stateTransistion[SNCLientState_Idle][SNCLientState_Discovery_getEEPROMs] = std::function<bool()>(std::bind(&SNClient::impl::validateEEPROMS, this));

  //discovery done -> switch to idle
  stateTransistion[SNCLientState_Discovery][SNCLientState_Idle] = std::function<bool()>(std::bind(&SNClient::impl::stopMeasuring, this));
  stateTransistion[SNCLientState_Discovery][SNCLientState_Discovery_getEEPROMs] = std::function<bool()>(std::bind(&SNClient::impl::validateEEPROMS, this));
  stateTransistion[SNCLientState_Discovery][SNCLientState_Running] = std::function<bool()>(std::bind(&SNClient::impl::startMeasuring, this));

  //discovery eeprom detection done -> switch to idle
  stateTransistion[SNCLientState_Discovery_getEEPROMs][SNCLientState_Discovery] = std::function<bool()>(std::bind(&SNClient::impl::startDiscovery, this));
  stateTransistion[SNCLientState_Discovery_getEEPROMs][SNCLientState_Idle] = std::function<bool()>(std::bind(&SNClient::impl::stopMeasuring, this));
  stateTransistion[SNCLientState_Discovery_getEEPROMs][SNCLientState_Running] = std::function<bool()>(std::bind(&SNClient::impl::startMeasuring, this));
 
  //running -> idle or discovery
  stateTransistion[SNCLientState_Running][SNCLientState_Idle] = std::function<bool()>(std::bind(&SNClient::impl::stopMeasuring, this));
  stateTransistion[SNCLientState_Running][SNCLientState_Discovery] = std::function<bool()>(std::bind(&SNClient::impl::startDiscovery, this));
  
  //invalid
  stateTransistion[SNCLientState_Invalid][SNCLientState_Idle] = std::function<bool()>(std::bind(&SNClient::impl::stopMeasuring, this));

  DEBUG() << this << " created " << std::endl;
//
//  std::cout << "== " << PROJECT_NAME << " - V" << (int) version.major << "." << (int) version.minor << "."
//            << (int) version.patch << "+" << (int) version.build << " ==" << std::endl;
//  std::cout << " - " << __DATE__ << " - " << __TIME__ << std::endl;
//  std::cout << " - " << GIT_BRANCH << " - " << GIT_VERSION << std::endl;
}

SNClient::impl::~impl() = default;

void SNClient::impl::attachListener(SNClientListener &listener) {
  listeners.push_back(&listener);
}
void SNClient::impl::detachListener(SNClientListener &listener) {
  // TODO!!!
  std::vector<SNClientListener*>::iterator it;

  it = std::find(listeners.begin(), listeners.end(), &listener);

  if (it != listeners.end())
    listeners.erase(it);
}

void SNClient::impl::notifyListeners(const uint8_t *const data, const uint_fast32_t length) {
  {
    for (SNClientListener *const listener : listeners) {
      if (listener->willReceiveRawData)
        listener->onMessageReceived(data, length);
    }
  }
}

void SNClient::impl::notifyListeners(std::unique_ptr<SNProtocolEvent> event) {
  std::shared_ptr<SNProtocolEvent> publicEvent = std::move(event);
  for (SNClientListener *const listener : listeners) {
    listener->onEventReceived(publicEvent);
  }
}

const SNCLientState SNClient::impl::getstate() const {
  return state;
}

bool SNClient::impl::setState(const SNCLientState newState) {
  // set new state
  if (newState < SNCLientState_MAX) {
    return handleState(newState);
  } else {
    INFO() << "ignoring invalid state " << newState;
    return false;
  }
}

bool SNClient::impl::setOption(std::string optionName, std::string optionValue, std::string sensorID) {
  std::string optionSetCommand = protocol.getMessage_setOption(sensorID,optionName,optionValue);
  sendStringMessage(optionSetCommand);

  if(sensorID != "FFFF")
    getOption(optionName,sensorID);
  return true;
}

std::string SNClient::impl::getOption(std::string optionName, std::string sensorID){
  std::string optionGetCommand = protocol.getMessage_getOption(sensorID,optionName);
  sendStringMessage(optionGetCommand);
  std::cerr << "NEED TO FIND A BETTER WAY! => CANT REPLY HERE!" << std::endl;
  return "wait for callback";
}

bool SNClient::impl::handleState(const SNCLientState newState) {
  //do nothing on same state
  if (newState == state)
    return true;

  bool returnValue = false;
  if (stateTransistion[state].count(newState)) {
    returnValue = stateTransistion[state][newState]();
  }

  if (returnValue)
    state = newState;

  return returnValue;
}

bool SNClient::impl::requestEEPROMData(std::string& sensorID){

  if(sensorID.length() > 4)
  {
    std::cerr << "wrong ID format, need 4 character wide HEX representation with leading zeros (e.g. 8051) !!! " << sensorID << std::endl;
    return false;
  }
  else{
    std::string address = std::string(4 - sensorID.length(), '0') + sensorID;
    bool sensor_found=false;
    sensorWaitingForEEPROMData = nullptr;

    for(auto sensor: getSensors()) {
      std::string paddedID= std::string(4 - sensor->getID().length(), '0') + sensor->getID();
      if(paddedID == address){
        sensor_found=true;
        sensorWaitingForEEPROMData = sensor;
      }
    }

    if(!sensor_found)
      return false;

    std::string eepromWriteCommand = protocol.getMessage_getEEPROMData(address);
    return sendStringMessage(eepromWriteCommand);
  }
}

bool SNClient::impl::writeEEPROMData(std::string& sensorID,const uint8_t *data, const uint_fast32_t length){

  if(sensorID.length() > 4)
  {
    std::cerr << "wrong ID format, need 4 character wide HEX representation with leading zeros (e.g. 8051) !!! " << sensorID << std::endl;
    return false;
  }
  else{
    std::string address = std::string(4 - sensorID.length(), '0') + sensorID;
    std::string eepromWriteCommand = protocol.getMessage_writeEEPROMData(address,data,length);
    return sendStringMessage(eepromWriteCommand);
  }
}

bool SNClient::impl::handleEvent(std::unique_ptr<SNProtocolEvent> newEvent) {
  if (newEvent->type == SNProtocolEvent::Type::Detect) {
    //std::cout << "Detect Event received" << std::endl;
    sensors=(*newEvent->getSensorInfo());
//    for (auto const sensorInfo : sensors) {
//      //std::cout << sensorInfo->getID() << std::endl;
//    }
    notifyListeners(std::move(newEvent));

    return true;
  }
  if (newEvent->type == SNProtocolEvent::Type::Eeprom) {
    //std::cout << "Eeprom Event received adding to sensor: " << sensorWaitingForEEPROMData << std::endl;
    if(sensorWaitingForEEPROMData != nullptr){
      sensorWaitingForEEPROMData->setEEPROMData(newEvent->getEEPROMData());
    }

    notifyListeners(std::move(newEvent));
    sensorWaitingForEEPROMData = nullptr;
    return true;
  }
  if (newEvent->type == SNProtocolEvent::Type::State) {
    //std::cout << "State Event received" << std::endl;
    notifyListeners(std::move(newEvent));
    return true;
  }
  if (newEvent->type == SNProtocolEvent::Type::DataFrame) {
    //std::cout << "DataFrame Event received" << std::endl;
    notifyListeners(std::move(newEvent));
    return true;
  }
  return false;
}

const std::vector<std::shared_ptr<SNSensorInformation>> SNClient::impl::getSensors() const {
  return sensors;
}

bool SNClient::impl::sendStringMessage(const std::string & message){
  //std::cout << SNUtils::toHex(message) << std::endl;
  return sendRawMessage(reinterpret_cast<const uint8_t *>(message.c_str()),message.length());
}

bool SNClient::impl::sendRawMessage(const uint8_t *const data, const uint_fast32_t length) {
  //std::cout << " length:" << length << "data: " << data   << std::endl;
  return transport->sendMessage(data, length);
}

void SNClient::impl::onMessageReceived(const uint8_t *data, const uint_fast32_t length) {
  //TODO parse the incoming messages here and add an eventhandler for it
  static bool harnessKnown=false;
  //TODO need to implement discovery mechanism and parse incoming sensordata
  if(transport->isConnected() && harnessKnown) {
    std::unique_ptr<SNProtocolEvent> result = protocol.parseMessage(data, length);
    handleEvent(std::move(result));
    notifyListeners(data, length);
  }
  else {
    //string data with key value pairs!
    std::string stateAsString(reinterpret_cast<const char*>(data),length);
    std::vector<std::string> words = SNUtils::wordsFromString(stateAsString);

    //metadata[results[0]] = results[1];
    if(words.size() == 2)
    {
      //std::cout << words[0] << ":" << words[1] << std::endl;
      
      if(words[0] == "tag") {
        if(words[1] == "lower")
        {
          std::cout << "BIONIC Lower Body Harness detected" << std::endl;
          protocol.setHarnessType(s_BIONICLowerBody);
        }
        else if(words[1] == "upper")
        {
          protocol.setHarnessType(s_BIONICUpperBodyREDUCED);
          std::cout << "BIONIC Upper Body Harness detected" << std::endl;
        }
        else
        {
          protocol.setHarnessType(s_unknown);
          std::cout << "Unknown Harness detected" << std::endl;
        }
        harnessKnown = true;
      }
    } 
  }

  DEBUG() << " message << " << data << std::endl;
}

bool SNClient::impl::stopMeasuring() {
  static const std::string stopCommand = protocol.getMessage_stopMeasuring();
  DEBUG() << stopCommand << std::endl;
  return sendStringMessage(stopCommand);
//  return transport->sendMessage((const uint8_t*) (stopCommand.c_str()), stopCommand.length());
}

bool SNClient::impl::startMeasuring() {
  static const std::string startCommand = protocol.getMessage_startMeasuring();
  DEBUG() << startCommand << std::endl;
  return sendStringMessage(startCommand);
//  return transport->sendMessage((const uint8_t*) (startCommand.c_str()), startCommand.length());
}

bool SNClient::impl::startDiscovery() {
  static const std::string detectCommand = protocol.getMessage_Discovery();
  DEBUG() << detectCommand << std::endl;
  return sendStringMessage(detectCommand);
//  return transport->sendMessage((const uint8_t*) (detectCommand.c_str()), detectCommand.length());
}

bool SNClient::impl::validateEEPROMS() {
  std::cerr << "EEPROMSTATE!!!" << std::endl;
  return false;
//  static const std::string detectCommand("state_set detect");
//  return transport->sendMessage((const uint8_t*) (detectCommand.c_str()), detectCommand.length());
}

const uint32_t SNClient::impl::getVersion() const {
  return version.version;
}

const bool SNClient::impl::isCompatibleVersion(uint32_t versionToCheck) {
  version_t const versiontocheck = { versionToCheck };
  if (versiontocheck.major == version.major) {
    return (versionToCheck <= version.version);
  }
  return false;
}

void SNClient::impl::setDFKIProtocol(bool active) {
  DFKIProtocolActive = active;
  if(active)
  {
    static const std::string protocolTypeCommand("usbnew=false");
    transport->sendMessage((const uint8_t*) (protocolTypeCommand.c_str()), protocolTypeCommand.length());
  }
  else{
    static const std::string protocolTypeCommand("usbnew=true");
    transport->sendMessage((const uint8_t*) (protocolTypeCommand.c_str()), protocolTypeCommand.length());
  }
}

bool SNClient::impl::isDFKIProtocolActive(void) {
  return DFKIProtocolActive;
}
