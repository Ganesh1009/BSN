/** 
 * @file    : SNCLientTransportSerialPort_test.cpp
 * @author  : musahl
 * @date    : 5 Jul 2020
 * @version : 0.0
 * @brief   : Short description
 */
//#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>
#include "SNClientTransportSerialPort.h"
#include <memory>
#include <ios>

#define SN_LIBRARY_TEST_TTY "/dev/ttyACM0"

class transportTestListener : public SNClientTransportListener {
  void onMessageReceived(const uint8_t *data, const uint_fast16_t length) {
    std::ostream output(&message_received);
    output.write(reinterpret_cast<const char*>(data), static_cast<const uint32_t>(length));
    message_received.commit(length);
    eventreceived = true;
  }
 public:
  bool eventreceived = false;
  asio::streambuf message_received;
};

bool waitForTransportListener(transportTestListener &listener, uint32_t durationInMs) {
  auto start = std::chrono::high_resolution_clock::now();
  while (!listener.eventreceived) {
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    auto end = std::chrono::high_resolution_clock::now();
    if (std::chrono::duration<double, std::milli>(end - start).count() > durationInMs)
      return false;
  }
  //reset the eventreceived flag
  listener.eventreceived = false;
  return true;
}

TEST_CASE( "SNTransportBasicTests", "[SNTransport]" ) {

  SECTION( "Constructor with different ttyName" ) {
    std::unique_ptr<SNClientTransport> snTransport = std::make_unique<SNClientTransportSerialPort>(SN_LIBRARY_TEST_TTY);
    REQUIRE_FALSE(snTransport == nullptr);
  }

  SECTION( "Constructor with wrong ttyName" ) {
    REQUIRE_THROWS(std::make_unique<SNClientTransportSerialPort>(SN_LIBRARY_TEST_TTY"IMWRONG"));
  }

  std::unique_ptr<SNClientTransport> snTransport = std::make_unique < SNClientTransportSerialPort
      > (SN_LIBRARY_TEST_TTY);
  REQUIRE_FALSE(snTransport == nullptr);

  SECTION( "listener interface" ) {
    transportTestListener listentoMe;
    snTransport->attachListener(listentoMe);
    std::string data("d");
    snTransport->sendMessage(reinterpret_cast<const uint8_t*>(data.c_str()), data.length());
    REQUIRE(waitForTransportListener(listentoMe, 100));
    snTransport->detachListener(listentoMe);
    snTransport->sendMessage(reinterpret_cast<const uint8_t*>(data.c_str()), data.length());
    REQUIRE(waitForTransportListener(listentoMe, 100) == false);
  }

  SECTION( "sendig data - no listener" ) {
    std::string data("xSOMEBINARYDATA");
    snTransport->sendMessage(reinterpret_cast<const uint8_t*>(data.c_str()), data.length());
    snTransport->sendMessage(reinterpret_cast<const uint8_t*>(data.c_str()), data.length());
    snTransport->sendMessage(reinterpret_cast<const uint8_t*>(data.c_str()), data.length());
  }
}
