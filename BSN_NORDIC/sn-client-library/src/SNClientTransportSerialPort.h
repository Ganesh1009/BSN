/**
 * @file    : SNClientTransportSerialPort.h
 * @author  : musahl
 * @date    : 5 Jul 2020
 * @version : 0.0
 * @brief   : Short description
 */
#ifndef SRC_SNCLIENTTRANSPORTSERIALPORT_H_
#define SRC_SNCLIENTTRANSPORTSERIALPORT_H_

#include "SNClientTransport.h"
#include "SNSerialMessageFraming.h"
#include <string>
#include <memory>
#include <utility>
#include <asio.hpp>
#include <thread>
 

class SNClientTransportSerialPort : 
  public SNClientTransport {
  // using ERROR = marsUtils::Log<marsUtils::LogLevel::ERROR, SNClientTransport>;
  // using DEBUG = marsUtils::Log<marsUtils::LogLevel::DEBUG, SNClientTransport>;
  // using INFO = marsUtils::Log<marsUtils::LogLevel::INFO, SNClientTransport>;
 public:
  explicit SNClientTransportSerialPort(const std::string &deviceName =
                                           "/dev/ttyACM0",
                                       uint32_t baudRate = 1000000);
  ~SNClientTransportSerialPort();

  bool sendMessage(const uint8_t *const data, const uint_fast32_t length);
  bool sendMessage(const std::string &message);
  bool sendRawMessage(const uint8_t *data, const uint_fast32_t length);

 private:
  void run();
  void onReceive(const asio::error_code &error,
                 size_t const bytesReceived);
  asio::io_service io;
  asio::serial_port port;
  SNSerialMessageFraming framing;
  std::thread *thread;
  uint16_t readBufferLength;
  uint8_t *readBuffer;

};
#endif /* SRC_SNCLIENTTRANSPORTSERIALPORT_H_ */
