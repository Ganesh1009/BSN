/*
 * SNSerialMessageFraming.h
 *
 *  Created on: 07.04.2021
 *      Author: musahl
 */

#ifndef EXTERN_SHARED_LIBRARY_MESSAGING_SNSERIALMESSAGEFRAMING_H_
#define EXTERN_SHARED_LIBRARY_MESSAGING_SNSERIALMESSAGEFRAMING_H_

#include <string>
#include <vector>

class SNSerialMessageFraming {
public:
	SNSerialMessageFraming();
	virtual ~SNSerialMessageFraming();

	std::vector<std::string> streamToMessage(const std::string &newSerialData);
	std::string messageToStream(const std::string &messageToFrame);

	const uint32_t getCRCErrorsSinceStart() {
		return crc_errors_since_start;
	}

	const uint32_t getBufferSize() {
	  return messagebuffer.size();
	}

private:
	int32_t findMessage(const std::string &inBuffer,std::string &outBuffer);
	int32_t packMessage(const std::string &inBuffer,std::string &outBuffer);
	bool synchronized;
	std::string messagebuffer;
	uint32_t crc_errors_since_start;
};

#endif /* EXTERN_SHARED_LIBRARY_MESSAGING_SNSERIALMESSAGEFRAMING_H_ */
