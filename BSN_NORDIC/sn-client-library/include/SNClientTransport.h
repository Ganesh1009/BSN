/*
 * SNClientTransport.h
 *
 *  Created on: 5 Jul 2020
 *      Author: musahl
 */

#ifndef SRC_SNCLIENTTRANSPORT_H_
#define SRC_SNCLIENTTRANSPORT_H_

//#include <Logging.h>

#include <stdint.h>
#include <vector>
#include <queue>          // std::queue
#include <string>         // std::string


class SNClientTransportListener {
 public:
  /**
   * @brief this listener gets called whenever there's a new message available
   * @param data : message buffer
   * @param length : length of message
   */
  virtual void onMessageReceived(const uint8_t *data, const uint_fast32_t length) = 0;
};

class SNClientTransport {
  // using DEBUG = marsUtils::Log<marsUtils::LogLevel::DEBUG, SNClientTransport>;
  // using INFO = marsUtils::Log<marsUtils::LogLevel::INFO, SNClientTransport>;
 public:
  SNClientTransport() = default;
  virtual ~SNClientTransport() = default;
  /**
   * @brief sends a message to the sn
   * @param data : message buffer
   * @param length : length of message
   * @return true if message can be send, false on error
   */
  virtual bool sendMessage(const uint8_t *data, const uint_fast32_t length) = 0;

  /**
   * @brief here you can add your callback for receiving messages
   * @param listener : listener to add
   */
  void attachListener(SNClientTransportListener &listener);

  /**
   * @brief detach a listener
   * @param listener
   */
  void detachListener(SNClientTransportListener &listener);

  /**
   * @brief is the connection being established properly?
   * @return
   */
  const bool isConnected(void) {
	  return connected;
  }

 protected:
  /**
   * @brief can be used to send a message to all listeners
   * @param data
   * @param length
   * @return
   */
  bool notifyListeners(const uint8_t *data, const uint_fast32_t length);

  bool connected;

 private:
  std::vector<SNClientTransportListener*> listeners;
  std::queue<std::string> pendingMessages;
};

#endif /* SRC_SNCLIENTTRANSPORT_H_ */
