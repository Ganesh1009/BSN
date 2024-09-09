/**
 * @file    bsn_msg_mcu.h
 * @brief   Implementation of a message library that can be used for transmitting messages via a stream, including SYNC / ESCAPE and bytes and CRC16 to validate the messages being send and received
 * @date    21 Sep 2018
 * @author  Mathias Musahl <mathias.musahl@dfki.de>
 */

#ifndef BSN_MSG_MCU_H_
#define BSN_MSG_MCU_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*messageCallback_t) (const uint8_t * buffer, const uint32_t length);

/**
 * @brief initializes three buffers with the given internalBufferSize
 * @param internalBufferSize
 */
void bsnmsg_initialize(uint32_t internalBufferSize);

/**
 * @brief needs to be called to free the memory
 */
void bsnmsg_deinitialize();

/**
 * @brief adds the incoming data to the internal buffer and fires the receivedCallBack function whenever a message was found
 * @param buffer
 * @param length
 * @param receivedCallback
 * @return
 */
int bsnmsg_stream_to_message(const uint8_t * buffer, const uint32_t length, messageCallback_t receivedCallback);

/**
 * @brief converts the given buffer to a message and calls the sendCallback function for sending it via your socket / serial or whatever you're gonna use here
 * @param buffer
 * @param length
 * @param sendCallback
 * @return
 */
int bsnmsg_message_to_stream(const uint8_t * buffer, const uint32_t length, messageCallback_t sendCallback);

/**
 * @brief just for printing buffers
 * @param buffer
 * @param length
 */
void bsn_msg_printhex (const uint8_t * buffer, const uint32_t length);

/**
 * @brief function returns the number of crc errors since start
 * @return number of crc errors
 */
uint32_t bsnmsg_get_crc_errors_since_start(void);

#ifdef __cplusplus
}
#endif
#endif /* BSN_MSG_MCU_H_ */
