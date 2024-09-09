/*
 * SNClient_test.cpp
 *
 *  Created on: 9 Jun 2020
 *      Author: musahl
 */

//#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>
#include "SNClient.h"
#include "SNClientImpl.h"
#include "SNClientTransportMockup.hpp"
#include <memory>
#include <future>
#include <ios>

#define SN_LIBRARY_ASYNC_TIMEOUT_MS (500)
class testListener : public SNClientListener
{
  void onMessageReceived(const uint8_t *data, const uint_fast16_t length)
  {
    std::ostream output(&message_received);
    output.write(reinterpret_cast<const char *>(data), static_cast<const uint32_t>(length));
    message_received.commit(length);
    eventreceived = true;
  }
  void onSensorData(SNSensorDataSet *sensorData)
  {
    eventreceived = true;
  }
  void onEventReceived(std::shared_ptr<SNProtocolEvent> event)
  {
    eventreceived = true;
  }

public:
  bool eventreceived = false;
  asio::streambuf message_received;
  SNSensorDataSet *sensorData;
  std::string *keyThatChanged;
};

bool waitForListener(testListener &listener, uint32_t durationInMs)
{
  auto start = std::chrono::high_resolution_clock::now();
  while (!listener.eventreceived)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    auto end = std::chrono::high_resolution_clock::now();
    if (std::chrono::duration<double, std::milli>(end - start).count() > durationInMs)
      return false;
  }
  //reset the eventreceived flag
  listener.eventreceived = false;
  return true;
}

TEST_CASE("SNClientBasicTests - Constructor", "[SNClient]")
{
  SECTION("Constructor with different transport ")
  {
    std::unique_ptr<SNClientTransport> transportMockup = std::make_unique<SNClientTransportMockup>();
    std::unique_ptr<SNClient> snClient(new SNClient(std::move(transportMockup)));
    REQUIRE_FALSE(snClient == nullptr);
  }
}


TEST_CASE("SNClientBasicTests - Listener", "[SNClient]")
{
  std::string welcomemessage = "bionic V0.0.6\nhardware haptic_sensor_node\ntag upper\nserialnr 0\n\n";
  std::unique_ptr<SNClientTransport> transportMockup = std::make_unique<SNClientTransportMockup>(welcomemessage);
  SNClientTransportMockup *snTransportMockup = reinterpret_cast<SNClientTransportMockup*>(transportMockup.get());
  std::unique_ptr<SNClient> snClient(new SNClient(std::move(transportMockup)));
  snTransportMockup->setNextResponse(reinterpret_cast<const uint8_t*>(welcomemessage.c_str()), welcomemessage.length(),true);
  //transportMockup->sendMessage(reinterpret_cast<const uint8_t*>(welcome.c_str()), welcome.length());
  REQUIRE_FALSE(snClient == nullptr);
  //snClient->setDFKIProtocol(true);

/*
  SECTION("Basic listener is working - test with detect")
  {
    testListener listentoMe;
    listentoMe.willReceiveRawData = true;
    snClient->attachListener(listentoMe);
    std::string data = "detect 8051";
    snTransportMockup->setNextResponse(reinterpret_cast<const uint8_t*>(data.c_str()), data.length(),true);
    //snClient->sendRawMessage(reinterpret_cast<const uint8_t*>(data.c_str()), data.length());
    REQUIRE(waitForListener(listentoMe, SN_LIBRARY_ASYNC_TIMEOUT_MS));
  }

  SECTION("listener can be unmounted")
  {
    testListener listentoMe;
    listentoMe.willReceiveRawData = true;
    snClient->attachListener(listentoMe);
    std::string data = "detect 8051";
    snClient->sendRawMessage(reinterpret_cast<const uint8_t*>(data.c_str()), data.length());
    REQUIRE(waitForListener(listentoMe, SN_LIBRARY_ASYNC_TIMEOUT_MS));
    snClient->detachListener(listentoMe);
    snClient->sendRawMessage(reinterpret_cast<const uint8_t*>(data.c_str()), data.length());
    REQUIRE(waitForListener(listentoMe, SN_LIBRARY_ASYNC_TIMEOUT_MS) == false);
  }
  */
}

TEST_CASE("SNClientBasicTests - Control", "[SNClient]")
{
  std::unique_ptr<SNClientTransport> transportMockup = std::make_unique<SNClientTransportMockup>();
  SNClientTransportMockup *snTransportMockup = reinterpret_cast<SNClientTransportMockup *>(transportMockup.get());
  std::unique_ptr<SNClient> snClient(new SNClient(std::move(transportMockup)));
  REQUIRE_FALSE(snClient == nullptr);
  testListener listentoMe;
  listentoMe.willReceiveRawData = true;
  snClient->attachListener(listentoMe);

  REQUIRE(snClient->getstate() == SNCLientState_Uninitalized);
  REQUIRE(snClient->getSensors().size() == 0);

  // SECTION("Detect Sensors")
  // {
  //   std::string fakedresponse("detect 0101,0110,0201,0300");
  //   //preload response here from internal transport
  //   snTransportMockup->setNextResponse(reinterpret_cast<const uint8_t *>(fakedresponse.c_str()), fakedresponse.length());
  //   waitForListener(listentoMe, SN_LIBRARY_ASYNC_TIMEOUT_MS);
  //   REQUIRE(snClient->detectSensors() == true);
  //   REQUIRE(snClient->getstate() == SNCLientState_Discovery);
  //   REQUIRE(listentoMe.message_received.size() != 0);
  //   fakedresponse = "state idle";
  //   snTransportMockup->setNextResponse(reinterpret_cast<const uint8_t *>(fakedresponse.c_str()), fakedresponse.length(),true);
  //   REQUIRE(snClient->getstate() == SNCLientState_Idle);
  //   REQUIRE(snClient->getSensors().size() == 0);
  // }

  SECTION("Request Client Version")
  {
    REQUIRE(snClient->getVersion() > 0);
    REQUIRE(snClient->isCompatibleVersion(0xff000000) == false);
    REQUIRE(snClient->isCompatibleVersion(snClient->getVersion() + 500) == false);
    REQUIRE(snClient->isCompatibleVersion(snClient->getVersion()));
  }
/*
  SECTION("Start Measuring")
  {

    REQUIRE(snClient->detectSensors() == true);
    REQUIRE(snClient->getstate() == SNCLientState_Discovery);
    SECTION("fails from discovery state")
    {
      REQUIRE(snClient->startMeasuring() == false);
      REQUIRE(snClient->getstate() == SNCLientState_Discovery);
    }
    waitForListener(listentoMe, SN_LIBRARY_ASYNC_TIMEOUT_MS);
    REQUIRE(listentoMe.message_received.size() != 0);
    REQUIRE(snClient->getstate() == SNCLientState_Idle);
    REQUIRE(snClient->getSensors().size() == 0);

    SECTION("works from idle state")
    {
      REQUIRE(snClient->startMeasuring() == true);
      waitForListener(listentoMe, SN_LIBRARY_ASYNC_TIMEOUT_MS);
      REQUIRE(snClient->getstate() == SNCLientState_Running);
    }
    SECTION("works from running state")
    {
      REQUIRE(snClient->startMeasuring() == true);
      waitForListener(listentoMe, SN_LIBRARY_ASYNC_TIMEOUT_MS);
      REQUIRE(snClient->getstate() == SNCLientState_Running);
      REQUIRE(snClient->startMeasuring() == true);
      waitForListener(listentoMe, SN_LIBRARY_ASYNC_TIMEOUT_MS);
      REQUIRE(snClient->getstate() == SNCLientState_Running);
    }
  }

  SECTION("Stop Measuring")
  {

    REQUIRE(snClient->detectSensors() == true);
    REQUIRE(snClient->getstate() == SNCLientState_Discovery);

    waitForListener(listentoMe, SN_LIBRARY_ASYNC_TIMEOUT_MS);
    REQUIRE(listentoMe.message_received.size() != 0);
    REQUIRE(snClient->getstate() == SNCLientState_Idle);
    REQUIRE(snClient->getSensors().size() == 0);

    SECTION("works from running state")
    {
      REQUIRE(snClient->startMeasuring() == true);
      waitForListener(listentoMe, SN_LIBRARY_ASYNC_TIMEOUT_MS);
      REQUIRE(snClient->getstate() == SNCLientState_Running);
      REQUIRE(snClient->stopMeasuring() == true);
      waitForListener(listentoMe, SN_LIBRARY_ASYNC_TIMEOUT_MS);
      REQUIRE(snClient->getstate() == SNCLientState_Idle);
    }
  }*/
}
