/** 
 * @file    : SNSensorTypes_test.cpp
 * @author  : musahl
 * @date    : 16 Nov 2020
 * @version : 0.0
 * @brief   : Short description
 */
#include <SNSensorTypes.h>

#include <catch2/catch.hpp>
#include <memory>
#include <ios>
#include <chrono>
#include <memory.h>
#include <asio.hpp>

TEST_CASE( "SNSensorInformation", "[SNSensorTypes]" ) {

  SECTION( "fail on invalid input" ) {
    std::string data = "FFFF";
    SNSensorInformation invalidInfo(data);

    REQUIRE(invalidInfo.getType() == SNSensorTypes::s_InvalidType);

  }

}
