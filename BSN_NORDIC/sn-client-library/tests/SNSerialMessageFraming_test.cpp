/*
 * SNSerialMessageFraming_test.cpp
 *
 *  Created on: 07.04.2021
 *      Author: musahl
 */

#include "SNSerialMessageFraming.h"
#include "SNUtils.h"
#include <iostream>
#include <catch2/catch.hpp>

TEST_CASE( "SNSerialMessageFraming", "[SNSerialMessageFraming]" ) {

  SECTION( "Standard Constructor working" ) {
    std::unique_ptr<SNSerialMessageFraming> snSerialFraming(new SNSerialMessageFraming());
    REQUIRE_FALSE(snSerialFraming == nullptr);

    SECTION( "no crc errors at statup" ) {
      REQUIRE(snSerialFraming->getCRCErrorsSinceStart() == 0);
    }

    SECTION( "random buffer test" ) {
      std::string randombuffer=SNUtils::getRandomBinaryData(6);
      REQUIRE(randombuffer.size() > 0);
      std::cout << SNUtils::toHex(randombuffer) << std::endl;
      REQUIRE(true);
    }

    SECTION( "messageToStream tests" ) {
      std::string testString("");
      std::string streamData = snSerialFraming->messageToStream(testString);
      REQUIRE(streamData.size() == 4);
      testString += "abcd";
      streamData = snSerialFraming->messageToStream(testString);
      std::cout << testString << std::endl;
      REQUIRE(streamData.size() == 4+4);
      std::cout << streamData << std::endl;

    }

    SECTION( "streamToMessage tests" ) {
      std::string testString("abcd");
      std::vector<std::string> messageData = snSerialFraming->streamToMessage(testString);
      REQUIRE(messageData.size() == 0);

      std::string streamData = snSerialFraming->messageToStream(testString);
      std::cout << testString << std::endl;
      REQUIRE(streamData.size() == testString.size()+4);

      std::string messageChunks("");

      for(int messagesInBuffer = 1; messagesInBuffer <= 10; messagesInBuffer++) {
        messageChunks+=streamData;
        messageData = snSerialFraming->streamToMessage(messageChunks);
        REQUIRE(messageData.size() == messagesInBuffer);
      }

      messageChunks="";
      for(int messagesInBuffer = 1; messagesInBuffer <= 256; messagesInBuffer++) {
        messageChunks+=streamData;
        if(messagesInBuffer%5 == 0)
          messageChunks+="}}{{}{";
        messageChunks+=SNUtils::getRandomBinaryData(messagesInBuffer);
        messageData = snSerialFraming->streamToMessage(messageChunks);
        REQUIRE(messageData.size() == messagesInBuffer);
      }
    }
  }
}
