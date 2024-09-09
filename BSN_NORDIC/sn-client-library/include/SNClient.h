/**
 * @file SNClient.h
 *
 * @date 9 Jun 2020
 * @author Mathias Musahl <mathias.musahl@dfki.de>
 */

#ifndef SRC_SNCLIENT_H_
#define SRC_SNCLIENT_H_

#include "SNSensorTypes.h"
#include "SNClientTransport.h"

//#include <Logging.h>

#include <vector>
#include <string>
#include <memory>
#include <experimental/propagate_const>

class SNProtocolEvent;

/**
 * @brief listener class for receiving events from the client, WARNING! MIGHT CHANGE SOON!
 */
class SNClientListener {
 public:
  /**
   * if implemented gets called whenever a new message is received
   * @param data : raw message data
   * @param length : length of the buffer
   */
  virtual void onMessageReceived(const uint8_t *data, const uint_fast16_t length) {};

  /**
   * called if a new dataset has be          std::cout << *((SNSensorData_IMU*)sensorData.get()) << std::endl;
 received
   * @param sensorData
   */
  virtual void onEventReceived(std::shared_ptr<SNProtocolEvent> event){};

  /**
   * @brief if set to true all received data will be forwarded to the listener
   */
  bool willReceiveRawData = false;

  /**
   * TODO: create option for filtering
   */
};

enum SNCLientState {
  SNCLientState_Invalid,
  SNCLientState_Uninitalized,
  SNCLientState_Discovery,
  SNCLientState_Discovery_getEEPROMs,
  SNCLientState_Idle,
  SNCLientState_Running,
  SNCLientState_MAX
};

class SNClient {

  //using DEBUG = marsUtils::Log<marsUtils::LogLevel::DEBUG, SNClient>;

 public:
  explicit SNClient(const std::string &deviceName = "/dev/ttyACM0", uint32_t baudRate = 1000000);
  explicit SNClient(std::unique_ptr<SNClientTransport>);
  ~SNClient();

  SNClient(SNClient&&) = default;
  SNClient& operator =(SNClient&&) = default;

  /**
   * @brief returns the version of the client library
   * @return
   */
  const uint32_t getVersion();
  /**
   * @brief returns the version of the client library
   * @return
   */
  const bool isCompatibleVersion(uint32_t version);

  /**
   * @brief triggers a new detection phase of the sensors
   */
  bool detectSensors();

  /**
   * @brief function for writing the eeprom of a sensors to it's flash !!! ATTENTION this will overwrite factory settings !!!
   * @param sensorID: if od the sensor
   * @param data: data to write (max 1024 bytes)
   * @param length: length of the data to write
   * @return
   */
  bool writeEEPROMData(std::string& sensorID,const uint8_t *data, const uint_fast32_t length);

  /**
   * @brief requests the eeprom informatino for a sensor
   * @param sensorID
   * @return
   */
  bool requestEEPROMData(std::string& sensorID);

  /**
   * @brief starts measurement with the currently detected sensors
   * @return
   */
  bool startMeasuring();

  /**
   * @brief stop measurement
   * @return
   */
  bool stopMeasuring();

  /**
   * @brief Set the Option in network
   * 
   * @param option 
   * @param value 
   * @return true 
   * @return false 
   */
  bool setOption(std::string optionName, std::string optionValue, std::string sensorID = "FFFF");
  /**
   * @brief Get the Option information from sensor   * 
   * @param optionName 
   * @param sensorID 
   * @return std::string : option value
   */
  std::string getOption(std::string optionName, std::string sensorID);

  /**
   * @brief returns the detected sensors
   */
  const std::vector<std::shared_ptr<SNSensorInformation>> getSensors();

  /**
   * @brief mount a SNClientListener here to get asynchronous feedback whenever new events get triggered.
   * @param listener : listener of type SNClientListener
   */
  void attachListener(SNClientListener &listener);

  /**
   * @brief unmount a SNClientListener
   * @param listener : listener of type SNClientListener
   */
  void detachListener(SNClientListener &listener);

  /**
   * @brief the state of the client - make sure it is in idle mode before starting measurement mode
   * @return
   */
  SNCLientState getstate();


  /**
   * @brief just for debugging purposes. here you can send a message to the SN Hub. it will be forwarded to the SNClientTransport that was configured for this client. accoridng to the implementation it will add additional message framing/chunking as needed for communciating with the Hub
   * @param data : buffer with binary data that should be send - make sure it's valid until everything was send.
   * @param length : length of the message to transmit
   */
	void sendRawMessage(const uint8_t *data, const uint_fast32_t length);

	/**
	 * @brief implements old DFKI protocol to be compatible with previous systems
	 * @param active : true: old protocol active
	 */
	void setDFKIProtocol(bool active);

	/**
	 * @brief returns if the old DFKI protocol is being used or the new BIONIC
	 * @return
	 */
	bool isDFKIProtocolActive(void);

 private:

  class impl;
  std::experimental::propagate_const<std::unique_ptr<impl>> pimpl;

};

#endif /* SRC_SNCLIENT_H_ */
