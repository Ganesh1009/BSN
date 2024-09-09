/** 
 * @file    : SNProtocol_test.cpp
 * @author  : musahl
 * @date    : 8 Jul 2020
 * @version : 0.0
 * @brief   : Short description
 */
#include <SNProtocol.h>

#include <catch2/catch.hpp>
#include <memory>
#include <ios>
#include <chrono>
#include <memory.h>
#include <asio.hpp>

TEST_CASE( "SNProtocolBasicTests", "[SNProtocol]" ) {

  SECTION( "Standard Constructor working" ) {
    std::unique_ptr<SNProtocol> snProtocol(new SNProtocol());
    REQUIRE_FALSE(snProtocol == nullptr);
  }
}

TEST_CASE( "SNProtocolCommandTests", "[SNProtocol]" ) {
  std::unique_ptr<SNProtocol> snProtocol(new SNProtocol());
  REQUIRE_FALSE(snProtocol == nullptr);

  SECTION( "parsing invalid messages" ) {
    std::string data;
    std::unique_ptr<SNProtocolEvent> result = snProtocol->parseMessage(reinterpret_cast<const uint8_t*>(data.c_str()),data.length());
    REQUIRE(result->type == SNProtocolEvent::Invalid);
    data="abcdefg";
    result = snProtocol->parseMessage(reinterpret_cast<const uint8_t*>(data.c_str()),data.length());
    REQUIRE(result->type == SNProtocolEvent::Raw);
  }

  SECTION( "parsing detect messages" ) {
    std::string data("detect 0110,0120,0130,8012");
    std::unique_ptr<SNProtocolEvent> result = snProtocol->parseMessage(reinterpret_cast<const uint8_t*>(data.c_str()),data.length());
    REQUIRE(result->type == SNProtocolEvent::Detect);
  }

  SECTION( "parsing state messages" ) {
    std::string data("state idle");
    std::unique_ptr<SNProtocolEvent> result = snProtocol->parseMessage(reinterpret_cast<const uint8_t*>(data.c_str()),data.length());
    REQUIRE(result->type == SNProtocolEvent::State);
  }

  SECTION( "parsing eeprom messages" ) {
    std::string data("eeprom 0110 \x00\x04TEST");
    std::unique_ptr<SNProtocolEvent> result = snProtocol->parseMessage(reinterpret_cast<const uint8_t*>(data.c_str()),data.length());
    REQUIRE(result->type == SNProtocolEvent::Eeprom);

    //std::unique_ptr<SNProtocolEventEEPROMData> eepromEvent = reinterpret_cast<std::unique_ptr<SNProtocolEventEEPROMData>>(result);
//    std::string  output((const char*)eepromEvent->getEEPROMData().get());
    UNSCOPED_INFO("The eeprom is " << data );
    //CHECK(false);
  }

  SECTION( "parsing option messages" ) {
    std::string data("option_sr 100");
    std::unique_ptr<SNProtocolEvent> result = snProtocol->parseMessage(reinterpret_cast<const uint8_t*>(data.c_str()),data.length());
    REQUIRE(result->type == SNProtocolEvent::Option);
  }

}
