/*
 * SNSerialMessageFraming.cpp
 *
 *  Created on: 07.04.2021
 *      Author: musahl
 */

#include "SNSerialMessageFraming.h"
#include "SNUtils.h"
#include "bsn_crc16.h"
#include "Debug.h"

#include <iostream>
#include <iomanip>

#define BSN_MSG_START_BYTE 0x7D
#define BSN_MSG_ESC_BYTE 0x7E
#define BSN_MSG_END_BYTE 0x7D

SNSerialMessageFraming::SNSerialMessageFraming()
    :
    synchronized(false),
    crc_errors_since_start(0) {
  // TODO Auto-generated constructor stub

}

SNSerialMessageFraming::~SNSerialMessageFraming() {
  // TODO Auto-generated destructor stub
}

int32_t SNSerialMessageFraming::findMessage(const std::string &inBuffer, std::string &outBuffer) {

  bool prev_escape = false;

  uint8_t orig_char;

  size_t startIndex = inBuffer.find(BSN_MSG_START_BYTE);

  if (startIndex == std::string::npos) {
    // no start found => caller can remove bytes from inbuffer
    //DEBUG() << "findMessage("<< inBuffer.size()<< ") no start! Removing all bytes" << inBuffer.size() << std::endl;
    return -inBuffer.size();
  } else {

    //DEBUG() << "findMessage(" << messagebuffer << ") foundStart: " << foundStart << std::endl;
    if (startIndex == (inBuffer.size() - 1)) {
      //start byte is the last byte of the string => leave here
      return -startIndex;
    }

    // >>>>>>>>>>>>>>>>>>>>>>>>
    // valid start index found
    // >>>>>>>>>>>>>>>>>>>>>>>>

    size_t endIndex = inBuffer.find(BSN_MSG_END_BYTE, startIndex + 1);

    //DEBUG() << "findMessage(" << messagebuffer << ") foundEnd: " << foundEnd << std::endl;

    if (endIndex != std::string::npos) {

      // >>>>>>>>>>>>>>>>>>>>>>>
      // valid end index found
      // >>>>>>>>>>>>>>>>>>>>>>>

      // handle escaping here
      for (int i = startIndex + 1; i < endIndex; i++) {
        if (prev_escape) {
          prev_escape = false;
          orig_char = inBuffer[i] ^ 0x20;
          outBuffer.push_back(orig_char);
        } else if (inBuffer[i] == BSN_MSG_ESC_BYTE) {
          prev_escape = true;
        } else {
          orig_char = inBuffer[i];
          outBuffer.push_back(orig_char);
        }
      }
      //DEBUG() << "findMessage(" << messagebuffer << ") outBuffer: " << outBuffer << std::endl;

      // at least the CRC16 was in the message -> potentially a valid message
      if (outBuffer.size() >= 2) {
        // get the CRC from the last 2 bytes of the message buffer
        std::string crcerrocopy(outBuffer);
        uint8_t crc_low = outBuffer.back();
        outBuffer.pop_back();
        uint8_t crc_high = outBuffer.back();
        outBuffer.pop_back();
        uint16_t crc = crc_low | (crc_high << 8);
        //calculate the crc of the message after escaping it
        uint16_t crc_floating = 0;
        uint16_t crc_buff = crc16_buff(outBuffer.data(),outBuffer.size());
        for(uint8_t character : outBuffer) {
          crc_floating = crc16_floating(character,crc_floating);
        }

        if (crc != crc_floating) {
          //crc error -> dont trust the buffer
          crc_errors_since_start++;
////
//          ERROR() << "CRC ERROR (" << std::hex << std::setfill('0') << std::setw(2) << crc_floating << " != " << crc_buff << " != " << (uint32_t) crc << " , removing " << std::dec << (uint32_t) endIndex << "bytes" << std::endl;
//          ERROR() << "i: " << std::endl << SNUtils::toHex(inBuffer) << std::endl;
//          ERROR() << "o: " << std::endl << SNUtils::toHex(outBuffer) << std::endl;
//          ERROR() << "o: " << std::endl << SNUtils::toHex(crcerrocopy) << std::endl;

          //don't remove the last END Byte because it could be the next start
          outBuffer.clear();
          return endIndex;
        }

//        DEBUG() << "findMessage(" << inBuffer.size() << ") Removing " << endIndex + 1 << std::endl;

        // >>>>>>>>>>>>>>>>>>>>>>>>>
        // correct message parsing!
        // remove all data that was parsed
        // >>>>>>>>>>>>>>>>>>>>>>>>>
        return endIndex + 1;
      } else {
//        ERROR() << "findMessage(" << inBuffer.size() << ") too small removing " << endIndex << std::endl;
        outBuffer.clear();
        //don't remove the last END Byte because it could be the next start
        return endIndex;
      }
    }
  }
  // no end found => caller can remove bytes up to the start position from inbuffer
//  DEBUG() << "findMessage(" << inBuffer.size() << ") no end! Removing " << startIndex << std::endl;
  return -startIndex;
}
int32_t SNSerialMessageFraming::packMessage(const std::string &inBuffer, std::string &outBuffer) {
  outBuffer.push_back(BSN_MSG_START_BYTE);
  uint16_t crc = crc16_buff(inBuffer.data(), inBuffer.size());
  std::string fullmessage(inBuffer);
  uint8_t crc_high = ((crc) >> 8) & 0xFF;
  uint8_t crc_low = crc & 0xFF;
  fullmessage.push_back(crc_high);
  fullmessage.push_back(crc_low);
  // reset the escape set flag
  bool prev_escape = false;
  for (char const &character : fullmessage) {
    if (prev_escape) {
      prev_escape = false;
      outBuffer.push_back(character ^ 0x20);
    } else if (character == BSN_MSG_START_BYTE || character == BSN_MSG_ESC_BYTE || character == BSN_MSG_END_BYTE) {
      prev_escape = true;
      outBuffer.push_back(BSN_MSG_ESC_BYTE);
      continue;
    } else {
      outBuffer.push_back(character);
    }
  }
  outBuffer.push_back(BSN_MSG_END_BYTE);

  return outBuffer.size();
}

std::vector<std::string> SNSerialMessageFraming::streamToMessage(const std::string &newSerialData) {

  std::vector < std::string > messages;

  // 1. add incoming bytes to buffer
  messagebuffer.append(newSerialData);
//  DEBUG() << "streamToMessage("<<newSerialData.size()<<"):" << messagebuffer << std::endl;
//  DEBUG()  << std::dec << "messagebuffer (" << messagebuffer.size() << "): " << SNUtils::toHex(messagebuffer)
//      << std::endl;

  // 2. message handling
  while (messagebuffer.size() > 0) {
    std::string newMessage("");
    //finde a message
    int32_t message_size = findMessage(messagebuffer, newMessage);
//    DEBUG()  << std::dec << "findMessage(" << messagebuffer.size() << ") returned: " << message_size
//        << " bytes to remove and message: " << newMessage.size() << std::endl;
//
//    DEBUG()  << std::dec << "messagebuffer (" << messagebuffer.size() << "): " << SNUtils::toHex(messagebuffer)
//        << std::endl;
    //remove from handled data from buffer
    if(message_size <= 0) {
      messagebuffer.erase(0, -message_size);
      break;
    }
    else
      messagebuffer.erase(0, message_size);
//
//    DEBUG()  << std::dec << "messagebuffer (" << messagebuffer.size() << "): " << SNUtils::toHex(messagebuffer)
//        << std::endl;

    //no message found -> exit here
    if (newMessage.length() != 0)
      messages.push_back(newMessage);
  }
//  DEBUG() << "streamToMessage DONE :" << std::endl;
//  DEBUG()  << std::dec << "messagebuffer (" << messagebuffer.size() << "): " << SNUtils::toHex(messagebuffer)
//      << std::endl;
  return messages;
}

std::string SNSerialMessageFraming::messageToStream(const std::string &messageToFrame) {
  std::string streamBuffer("");
  packMessage(messageToFrame, streamBuffer);
  return streamBuffer;
}
