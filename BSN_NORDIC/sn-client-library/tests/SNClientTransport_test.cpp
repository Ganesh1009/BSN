/** 
 * @file    : SNCLientTransportSerialPort_test.cpp
 * @author  : musahl
 * @date    : 5 Jul 2020
 * @version : 0.0
 * @brief   : Short description
 */
//#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>
#include "SNClientTransport.h"
#include <memory>
#include <ios>
#include <iomanip>
#include <thread>
#include <asio.hpp>

#include "Debug.h"

#include "SNClientTransportMockup.hpp" 

class transportTestListener : public SNClientTransportListener {
  void onMessageReceived(const uint8_t *data, const uint_fast16_t length) {
    std::ostream output(&message_received);
    output.write(reinterpret_cast<const char*>(data), static_cast<const uint32_t>(length));
    message_received.commit(length);
    eventreceived = true;
    std::cout << std::endl
              << "received " << std::dec << length << " bytes:" << std::endl;
    std::cout << "t:'";
    std::cout.write(reinterpret_cast<const char *>(data), length);
    std::cout << "'" << std::endl
              << "b:";
    for (int i = 0; i < length; ++i)
      std::cout << std::hex << std::setfill('0') << std::setw(2) << reinterpret_cast<int>((int)data[i]) << " ";
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

  std::unique_ptr<SNClientTransport> snTransport = std::make_unique<SNClientTransportMockup>();
  REQUIRE_FALSE(snTransport == nullptr);

  SECTION( "listener interface" ) {
    transportTestListener listentoMe;
    snTransport->attachListener(listentoMe);
    std::string data("d"); 
    SNClientTransportMockup *snTransportMockup = reinterpret_cast<SNClientTransportMockup*>(snTransport.get());
    snTransportMockup->setNextResponse(reinterpret_cast<const uint8_t*>(data.c_str()), data.length());
    snTransport->sendMessage(reinterpret_cast<const uint8_t*>(data.c_str()), data.length());
    REQUIRE(waitForTransportListener(listentoMe, 100));
    snTransport->detachListener(listentoMe);
    snTransport->sendMessage(reinterpret_cast<const uint8_t*>(data.c_str()), data.length());
    REQUIRE_FALSE(waitForTransportListener(listentoMe, 100));
  }

  SECTION( "sendig data - no listener" ) {
    std::string data("xSOMEBINARYDATA");
    transportTestListener listentoMe;
    snTransport->sendMessage(reinterpret_cast<const uint8_t*>(data.c_str()), data.length());
    REQUIRE_FALSE(waitForTransportListener(listentoMe, 100));
    snTransport->sendMessage(reinterpret_cast<const uint8_t*>(data.c_str()), data.length());
    snTransport->sendMessage(reinterpret_cast<const uint8_t*>(data.c_str()), data.length());
  }
}
