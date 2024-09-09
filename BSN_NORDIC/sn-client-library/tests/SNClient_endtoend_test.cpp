/*
 * SNClient_endtoend_test.cpp
 *
 *  Created on: 9 Jun 2020
 *      Author: musahl
 */

//#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>
#include "SNClient.h"
#include "SNClientImpl.h"
#include "SNClientTransportSerialPort.h"
#include <memory>
#include <future>
#include <ios>

#define SN_LIBRARY_TEST_TTY       "/dev/ttyACM0"
#define SN_LIBRARY_ASYNC_TIMEOUT_MS (500)

class testListener : public SNClientListener {
  void onMessageReceived(const uint8_t *data, const uint_fast16_t length) {
    std::ostream output(&message_received);
    output.write(reinterpret_cast<const char*>(data), static_cast<const uint32_t>(length));
    message_received.commit(length);
    eventreceived = true;
  }
  void onSensorData(SNSensorDataSet *sensorData) {
    eventreceived = true;
  }
  void onEventReceived(std::shared_ptr<SNProtocolEvent> event) {
    eventreceived = true;
  }
 public:
  bool eventreceived = false;
  asio::streambuf message_received;
  SNSensorDataSet *sensorData;
  std::string *keyThatChanged;
};

bool waitForListener(testListener &listener, uint32_t durationInMs) {
  auto start = std::chrono::high_resolution_clock::now();
  while (!listener.eventreceived) {
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    auto end = std::chrono::high_resolution_clock::now();
    if (std::chrono::duration<double, std::milli>(end - start).count() > durationInMs)
      return false;
  }
  //reset the eventreceived flag
  listener.eventreceived=false;
  return true;
}

TEST_CASE( "SNClientBasicTests - Constructor", "[SNClient]" ) {

  SECTION( "Standard Constructor working" ) {
    std::unique_ptr<SNClient> snClient(new SNClient());
    REQUIRE_FALSE(snClient == nullptr);
  }

  SECTION( "Constructor with different ttyName" ) {
    std::unique_ptr<SNClient> snClient = std::make_unique <SNClient> (SN_LIBRARY_TEST_TTY);
    REQUIRE_FALSE(snClient == nullptr);
  }

  SECTION( "Constructor with wrong ttyName" ) {
    REQUIRE_THROWS(std::make_unique<SNClient>(SN_LIBRARY_TEST_TTY"IMWRONG"));
  }

  SECTION( "Constructor with different transport " ) {
    std::unique_ptr<SNClientTransport> transportSerial = std::make_unique<SNClientTransportSerialPort> (SN_LIBRARY_TEST_TTY);
    std::unique_ptr<SNClient> snClient(new SNClient(std::move(transportSerial)));
    REQUIRE_FALSE(snClient == nullptr);
  }
}

TEST_CASE( "SNClientBasicTests - Listener", "[SNClient]" ) {
  std::unique_ptr<SNClient> snClient(new SNClient(SN_LIBRARY_TEST_TTY));
  REQUIRE_FALSE(snClient == nullptr);
  snClient->setDFKIProtocol(true);

  SECTION( "Basic listener is working - test with detect" ) {
    testListener listentoMe;
    listentoMe.willReceiveRawData = true;
    snClient->attachListener(listentoMe);
    uint8_t detect[10] = "d";
    snClient->sendRawMessage(detect, 2);
    REQUIRE(waitForListener(listentoMe, SN_LIBRARY_ASYNC_TIMEOUT_MS));
  }

  SECTION( "listener can be unmounted" ) {
    testListener listentoMe;
    listentoMe.willReceiveRawData = true;
    snClient->attachListener(listentoMe);
    uint8_t detect[10] = "d";
    snClient->sendRawMessage(detect, 2);
    REQUIRE(waitForListener(listentoMe, SN_LIBRARY_ASYNC_TIMEOUT_MS));
    snClient->detachListener(listentoMe);
    snClient->sendRawMessage(detect, 2);
    REQUIRE(waitForListener(listentoMe, SN_LIBRARY_ASYNC_TIMEOUT_MS) == false);
  }
}

TEST_CASE( "SNClientBasicTests - Control", "[SNClient]" ) {
  std::unique_ptr<SNClient> snClient = std::make_unique <SNClient> (SN_LIBRARY_TEST_TTY);
  REQUIRE_FALSE(snClient == nullptr);
  testListener listentoMe;
  listentoMe.willReceiveRawData = true;
  snClient->attachListener(listentoMe);

  REQUIRE(snClient->getstate() == SNCLientState_Uninitalized);
  REQUIRE(snClient->getSensors().size() == 0);

  SECTION( "Detect Sensors" ) {
    REQUIRE(snClient->detectSensors() == true);
    REQUIRE(snClient->getstate() == SNCLientState_Discovery);
    waitForListener(listentoMe, SN_LIBRARY_ASYNC_TIMEOUT_MS);
    REQUIRE(listentoMe.message_received.size() != 0);
    REQUIRE(snClient->getstate() == SNCLientState_Idle);
    REQUIRE(snClient->getSensors().size() == 0);
  }

  SECTION( "Request Client Version" ) {
    REQUIRE(snClient->getVersion() > 0);
    REQUIRE(snClient->isCompatibleVersion(0xff000000) == false);
    REQUIRE(snClient->isCompatibleVersion(snClient->getVersion()+500) == false);
    REQUIRE(snClient->isCompatibleVersion(snClient->getVersion()));
  }

  SECTION( "Start Measuring" ) {

    REQUIRE(snClient->detectSensors() == true);
    REQUIRE(snClient->getstate() == SNCLientState_Discovery);
    SECTION( "fails from discovery state" ) {
      REQUIRE(snClient->startMeasuring() == false);
      REQUIRE(snClient->getstate() == SNCLientState_Discovery);
    }
    waitForListener(listentoMe, SN_LIBRARY_ASYNC_TIMEOUT_MS);
    REQUIRE(listentoMe.message_received.size() != 0);
    REQUIRE(snClient->getstate() == SNCLientState_Idle);
    REQUIRE(snClient->getSensors().size() == 0);

    SECTION( "works from idle state" ) {
      REQUIRE(snClient->startMeasuring() == true);
      waitForListener(listentoMe, SN_LIBRARY_ASYNC_TIMEOUT_MS);
      REQUIRE(snClient->getstate() == SNCLientState_Running);
    }
    SECTION( "works from running state" ) {
      REQUIRE(snClient->startMeasuring() == true);
      waitForListener(listentoMe, SN_LIBRARY_ASYNC_TIMEOUT_MS);
      REQUIRE(snClient->getstate() == SNCLientState_Running);
      REQUIRE(snClient->startMeasuring() == true);
      waitForListener(listentoMe, SN_LIBRARY_ASYNC_TIMEOUT_MS);
      REQUIRE(snClient->getstate() == SNCLientState_Running);
    }
  }

  SECTION( "Stop Measuring" ) {

    REQUIRE(snClient->detectSensors() == true);
    REQUIRE(snClient->getstate() == SNCLientState_Discovery);

    waitForListener(listentoMe, SN_LIBRARY_ASYNC_TIMEOUT_MS);
    REQUIRE(listentoMe.message_received.size() != 0);
    REQUIRE(snClient->getstate() == SNCLientState_Idle);
    REQUIRE(snClient->getSensors().size() == 0);

    SECTION( "works from running state" ) {
      REQUIRE(snClient->startMeasuring() == true);
      waitForListener(listentoMe, SN_LIBRARY_ASYNC_TIMEOUT_MS);
      REQUIRE(snClient->getstate() == SNCLientState_Running);
      REQUIRE(snClient->stopMeasuring() == true);
      waitForListener(listentoMe, SN_LIBRARY_ASYNC_TIMEOUT_MS);
      REQUIRE(snClient->getstate() == SNCLientState_Idle);
    }
  }
}

