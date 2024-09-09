/**
 * @file SNClientImpl.h
 *
 * @date 9 Jun 2020
 * @author Mathias Musahl <mathias.musahl@dfki.de>
 */

#ifndef SRC_SNCLIENTIMPL_H_
#define SRC_SNCLIENTIMPL_H_

#include "SNClient.h"
#include "SNClientTransport.h"
#include "SNClientTransportSerialPort.h"
#include "SNProtocol.h"

#include "Debug.h"

#include <functional>
#include <memory>
#include <map>
#include <vector>

typedef union version {
  uint32_t version;
  struct {
    uint8_t build;
    uint8_t patch;
    uint8_t minor;
    uint8_t major;
  };
} version_t;

class SNClient::impl : public SNClientTransportListener {

  // using DEBUG = marsUtils::Log<marsUtils::LogLevel::DEBUG, SNClient::impl>;
  // using INFO = marsUtils::Log<marsUtils::LogLevel::INFO, SNClient::impl>;

 public:
  explicit impl(std::unique_ptr<SNClientTransport> alternativeTransport = std::make_unique<SNClientTransportSerialPort>());
  virtual ~impl();

  const uint32_t getVersion() const;
  const bool isCompatibleVersion(uint32_t version);

  void setDFKIProtocol(bool active);
  bool isDFKIProtocolActive(void);

  void attachListener(SNClientListener &listener);
  void detachListener(SNClientListener &listener);
  void notifyListeners(const uint8_t *const data, const uint_fast32_t length);
  void notifyListeners(std::unique_ptr<SNProtocolEvent> event);

	const SNCLientState getstate() const;
  bool setState(const SNCLientState newState);
  bool setOption(std::string optionName, std::string optionValue, std::string sensorID = "FFFF");
  std::string getOption(std::string optionName, std::string sensorID);

  bool writeEEPROMData(std::string& sensorID,const uint8_t *data, const uint_fast32_t length);
  bool requestEEPROMData(std::string& sensorID);

  bool sendRawMessage(const uint8_t *const data, const uint_fast32_t length);
  bool sendStringMessage(const std::string & message);

  void onMessageReceived(const uint8_t *data, const uint_fast32_t length);

  const std::vector<std::shared_ptr<SNSensorInformation>> getSensors() const;

 private:

  bool startDiscovery();
  bool validateEEPROMS();
  bool startMeasuring();
  bool stopMeasuring();
  bool handleState(SNCLientState newState);
  bool handleEvent(std::unique_ptr<SNProtocolEvent> newEvent);

  bool DFKIProtocolActive;
  version_t version;
  SNProtocol protocol;

  std::vector<SNClientListener*> listeners;
  std::vector<std::shared_ptr<SNSensorInformation>> sensors;
  std::shared_ptr<SNSensorInformation> sensorWaitingForEEPROMData;
  SNCLientState state;
  std::map<SNCLientState, std::map<SNCLientState, std::function<bool()>>> stateTransistion;
  std::unique_ptr<SNClientTransport> transport;

};

#endif /* SRC_SNCLIENTIMPL_H_ */
