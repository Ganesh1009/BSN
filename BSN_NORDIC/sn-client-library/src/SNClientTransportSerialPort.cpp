/** 
 * @file    : SNClientTransportSerialPort.cpp
 * @author  : musahl
 * @date    : 5 Jul 2020
 * @version : 0.0
 * @brief   : Short description
 */
#include "SNClientTransportSerialPort.h"
#include <thread>
#include <future>
#include <iostream>
#include <exception>
#include <functional>
#include <stdio.h>
#include <iomanip>
#include <asio/use_future.hpp>

#include "Debug.h"

constexpr auto SNCLIENT_SERIALPORT_BUFFERSIZE = (8192);

SNClientTransportSerialPort::SNClientTransportSerialPort(const std::string &deviceName, uint32_t baudRate)
    :
    io(),
    port(io),
    thread(nullptr),
    readBufferLength(SNCLIENT_SERIALPORT_BUFFERSIZE),
    readBuffer(nullptr) {

  readBuffer = new uint8_t[readBufferLength];

  try{
    port.open(deviceName);
    // inner catch, OSX doesnt allow setting baudrate on a usb serial
    try{
      port.set_option(asio::serial_port_base::baud_rate(baudRate));
    }
    catch(std::exception const &e) { 
        ERROR() << this << "guess we are on a mac... : " << e.what() ;
    } 

     
  //start io thread
    thread = new std::thread([this](){run();});
  } catch (std::exception const &e) {
    delete [] readBuffer;
    throw;
  }

  DEBUG() << this << " created - buffersize: " << readBufferLength << std::endl;
}

SNClientTransportSerialPort::~SNClientTransportSerialPort() {
  // clear list of listeners
  // stop everything here in the constructor -> hope this roks :-S
  if (thread != nullptr) {
    io.stop();
    thread->join();
    delete thread;
    thread = nullptr;
    DEBUG() << "thread stopped" ;
  }
  try {
    port.close();
  } catch (std::exception const &e) {
    DEBUG() << e.what() ;
  }
  delete[] readBuffer;
  readBuffer = nullptr;
  DEBUG() << this << " deleted - buffersize: " << readBufferLength << std::endl;
}

void SNClientTransportSerialPort::run() {

  port.async_read_some(
      asio::buffer(readBuffer, readBufferLength),
      [this](const asio::error_code &error, size_t const bytesReceived){onReceive(error,bytesReceived);});

  DEBUG() << "running" << std::endl;
  io.run();
  DEBUG() << "stopping" << std::endl;
}

bool SNClientTransportSerialPort::sendMessage(const uint8_t *const data,
                                              const uint_fast32_t length) {
  std::string messageToSend(reinterpret_cast<const char *>(data),length);
  return sendMessage(messageToSend);

}

bool SNClientTransportSerialPort::sendMessage(const std::string &message) {
  std::string streamToSend = framing.messageToStream(message);
  return sendRawMessage(reinterpret_cast<const uint8_t *>(streamToSend.data()),streamToSend.size());
}

bool SNClientTransportSerialPort::sendRawMessage(const uint8_t *data, const uint_fast32_t length) {

  std::string debugMessage(reinterpret_cast<const char *>(data),length);
  DEBUG() << this << " message >> " << std::endl << debugMessage << std::endl ;
  asio::write(port,asio::buffer(data, length));
  return true;
}

void SNClientTransportSerialPort::onReceive(
    const asio::error_code &error, size_t const bytesReceived) {
//
//  DEBUG() << this << " message << " << std::endl ;
//  DEBUG() << "t:'";
//  DEBUG().write(reinterpret_cast<const char *>(readBuffer), bytesReceived);
//  DEBUG() << "'" << std::endl
//            << "b:";
//  for (int i = 0; i < bytesReceived; ++i)
//    std::cout << std::hex << std::setfill('0') << std::setw(2) << reinterpret_cast<int>((int)readBuffer[i]) << " ";
//  DEBUG() << std::endl ;

  if(connected == false) {
    //still waiting for handshake -> parse first message as welcome message
    std::string key, val;
    std::istringstream iss((char*) readBuffer);

    bool headerFound = false;

    std::string greeting;
    while(std::getline(iss, greeting)){
      if (greeting.rfind("bionic", 0) == 0) {
        std::cout << "connection established: " << greeting << std::endl;
        headerFound = true;
      } else if(headerFound) {
        std::cout << greeting << std::endl;
        notifyListeners(reinterpret_cast<const uint8_t *>(greeting.data()), greeting.length());
      }
    }

    if(headerFound == false) {
      port.close();
      throw std::runtime_error("invalid BSN");
    }
    connected = true;
  }


  std::string message(reinterpret_cast<const char *>(readBuffer), bytesReceived);
  std::vector<std::string> messages = framing.streamToMessage(message);

  for(std::string message : messages) {
    notifyListeners(reinterpret_cast<const uint8_t *>(message.data()), message.size());
  }
//
//  framing.streamToMessage
//  bsnmsg_stream_to_message(static_cast<const uint8_t*>(readBuffer), static_cast<const uint32_t>(bytesReceived),
//                           SNClientTransportSerialPort::onReceiveCWrapper);
//

  port.async_read_some(
      asio::buffer(readBuffer, readBufferLength),
      [this](const asio::error_code &error, size_t const bytesReceived){onReceive(error,bytesReceived);});
}
//
//int SNClientTransportSerialPort::onReceiveCWrapper(const uint8_t *const buffer,
//                                                   const uint32_t length) {
//  if (SNClientTransportSerialPortHelperPointer)
//    SNClientTransportSerialPortHelperPointer->notifyListeners(buffer, length);
//  return 0;
//}
