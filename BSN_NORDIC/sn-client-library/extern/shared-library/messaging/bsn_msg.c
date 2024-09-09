/**
 * @file bsn_msg_mcu.c
 *
 * @date 21 Sep 2018
 * @author Mathias Musahl
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bsn_msg.h"
#include "bsn_crc16.h"

#define BSN_MSG_START_BYTE 0x7D
#define BSN_MSG_ESC_BYTE 0x7E
#define BSN_MSG_END_BYTE 0x7D

#undef BSN_MSG_DEBUG
//#define BSN_MSG_DEBUG

/*
 * LOCAL PROTOTYPES
 */

static int32_t bsn_find_message(const uint8_t *src, uint32_t src_len, uint8_t *dest, uint32_t *dest_len);

static int32_t bsn_compress_message(const uint8_t *src, uint32_t src_len, uint8_t *dest, uint32_t *dest_len);

/*
 * LOCAL VARIABLES
 */

static uint8_t *messagebuffer = 0;
static uint32_t messagebuffer_length = 0;
static uint32_t messagebuffer_size = 0;
static uint8_t *incoming_msg;
static uint32_t incoming_msg_length = 0;
static uint8_t *outgoing_msg;
static uint32_t outgoing_msg_length = 0;
static uint32_t crc_errors_since_start = 0;

void bsnmsg_initialize(uint32_t internalBufferSize) {
  messagebuffer_size = internalBufferSize;
  messagebuffer = (uint8_t*) malloc(messagebuffer_size);
  incoming_msg = (uint8_t*) malloc(messagebuffer_size);
  incoming_msg_length = 0;
  outgoing_msg = (uint8_t*) malloc(messagebuffer_size);
  outgoing_msg_length = 0;
}

void bsnmsg_deinitialize() {
  free(messagebuffer);
  free(incoming_msg);
  free(outgoing_msg);
  messagebuffer = NULL;
  incoming_msg = NULL;
  outgoing_msg = NULL;
  messagebuffer_size = 0;
  incoming_msg_length = 0;
  outgoing_msg_length = 0;
}

void bsn_msg_printhex(const uint8_t *buffer, const uint32_t length) {
  int i = 0;
  for (i = 0; i < length; i++) {
    if ((i & 0xF) == 0)
      printf("\n %d: ", i);
    printf("%02X ", buffer[i]);
  }
  printf("\n");
}

int32_t bsn_find_message(const uint8_t *src, uint32_t src_len, uint8_t *dest, uint32_t *dest_len) {
  if(src == NULL || dest == NULL || dest_len == NULL)
    return (-10);
  const uint8_t *src_start = src;
  const uint8_t *msg_start = src;
  const uint8_t *msg_end = src;
  const uint8_t *src_end = src + src_len;
  const uint8_t *dest_start = dest;

  // we need some memory to find the crc before the actual CRC has been received
  uint16_t crc_checks_array[3] = { 0 };

  // find a start byte
  while (msg_start < src_end && *msg_start != BSN_MSG_START_BYTE)
    ++msg_start;

  //no start found -> set return value -1 to delete buffer content
  if (*msg_start != BSN_MSG_START_BYTE) {
    //remove all bytes! DONT TRY AGAIN
    *dest_len = src_len;
    return (-1);
  }

  // in case of error dest_len will be used to remove this amount of byte from buffer -> set to trailing bytes!
  *dest_len = msg_start - src_start;

  if (msg_start < src_end) {
    // Loop through the data
    msg_end = msg_start + 1;
  } else {
    return (-1);
  }

  // find a end byte
  while (msg_end < src_end && *msg_end != BSN_MSG_END_BYTE)
    ++msg_end;

  //no start found -> set return value -1 to delete buffer content
  if (*msg_end != BSN_MSG_END_BYTE || msg_end == src_end) {
    return (-1);
  }

  // in case of error dest_len will be used to remove this amount of byte from buffer -> set to trailing bytes!
  *dest_len = msg_end - src_start;

  // reset the escape set flag
  uint_fast8_t prev_escape = 0;
  uint8_t orig_char;
  //set src position to start of message +1
  src = msg_start + 1;

  // stop on end char, unescape everthing until then
  while (src < msg_end) {
    if (prev_escape) {
      prev_escape = 0;
      orig_char = (*src) ^ 0x20;

      // copy old values here and calculate crc
      crc_checks_array[2] = crc_checks_array[1];
      crc_checks_array[1] = crc_checks_array[0];
      crc_checks_array[0] = crc16_floating(orig_char, crc_checks_array[1]);

      *(dest++) = orig_char;
    } else if (*src == BSN_MSG_ESC_BYTE) {
      prev_escape = 1;
    } else {
      orig_char = *src;

      // copy old values here and calculate crc
      crc_checks_array[2] = crc_checks_array[1];
      crc_checks_array[1] = crc_checks_array[0];
      crc_checks_array[0] = crc16_floating(orig_char, crc_checks_array[1]);

      *(dest++) = orig_char;
    }
    ++src;
  }

  //>>>>>>>>>>>>>>>>>>>>>>>>
  // START FOUND, END FOUND
  //>>>>>>>>>>>>>>>>>>>>>>>>

  if (dest - dest_start >= 2) {
    // message long enough to be ok -> check CRC
    // calc dest_len
    *dest_len = dest - dest_start;

    // get the CRC from the last 2 bytes of the message buffer
    uint16_t crc = dest_start[*dest_len - 2] << 8 | dest_start[*dest_len - 1];

    // compare it with the previous crc value
    if (crc == crc_checks_array[2]) {
      // remove the two CRC bytes from message
      *dest_len -= 2;
      return ((msg_end - src_start));
    } else {
      crc_errors_since_start++;
    }
  }
  // in case an error dest_len will be used to remove this amount of byte from buffer -> set to handled bytes
  *dest_len = msg_end - src_start;
  return (-2);

}

int32_t bsn_compress_message(const uint8_t *src, uint32_t src_len, uint8_t *dest, uint32_t *dest_len) {
  if(src == NULL || dest == NULL || dest_len == NULL)
    return (-10);
  const uint8_t *src_end = src + src_len;
  const uint8_t *dest_start = dest;
  uint16_t crc = 0;

  // start with the BSN_MSG_START_BYTE
  *(dest++) = BSN_MSG_START_BYTE;

  // reset the escape set flag
  uint_fast8_t prev_escape = 0;

  // iterate to the end and escape the data here
  while (src < src_end) {
    if (prev_escape) {
      prev_escape = 0;
      crc = crc16_floating(*src, crc);
      *(dest++) = *(src) ^ 0x20;
    } else if (*src == BSN_MSG_START_BYTE || *src == BSN_MSG_ESC_BYTE) {
      prev_escape = 1;
      *(dest++) = BSN_MSG_ESC_BYTE;
      continue;
    } else {
      crc = crc16_floating(*src, crc);
      *(dest++) = *src;
    }
    ++src;
  }

  // encode the crc as well
  uint8_t crc_high = ((crc) >> 8) & 0xFF;
  uint8_t crc_low = crc & 0xFF;

  //crc high byte first
  if (crc_high == BSN_MSG_START_BYTE || crc_high == BSN_MSG_ESC_BYTE) {
    *(dest++) = BSN_MSG_ESC_BYTE;
    *(dest++) = crc_high ^ 0x20;
  } else {
    *(dest++) = crc_high;
  }
  // now crc low byte
  if (crc_low == BSN_MSG_START_BYTE || crc_low == BSN_MSG_ESC_BYTE) {
    *(dest++) = BSN_MSG_ESC_BYTE;
    *(dest++) = crc_low ^ 0x20;
  } else {
    *(dest++) = crc_low;
  }

  *(dest++) = BSN_MSG_END_BYTE;

  *dest_len = dest - dest_start;

  return (0);
}

/*
 * PUBLIC FUNCTION OF THE LIBRARY
 */

int bsnmsg_stream_to_message(const uint8_t *buffer, const uint32_t length, messageCallback_t receivedCallback) {

  // leave if there's no buffer initialized
  if (messagebuffer == 0 || incoming_msg == 0) {
    printf("initialize buffer first!\n\r");
    return (-1);
  }

  if (receivedCallback == 0) {
    printf("receivedCallback missing!\n\r");
    return (-2);
  }

  // 1. add incoming bytes to buffer
  if (messagebuffer_length + length < messagebuffer_size) {
    memcpy(&messagebuffer[messagebuffer_length], buffer, length);
    messagebuffer_length += length;
  } else {
    printf("error BUFFER OVERFLOW!!!\n\r");
    messagebuffer_length = 0;
    memset(messagebuffer, 0, messagebuffer_size);
    return (-3);
  }

  // 2. message handling
  while (1) {

    memset(incoming_msg, 0, messagebuffer_size);
    incoming_msg_length = 0;

    // 2.1 find and validate message with bsn_find_message
    int32_t message_size = bsn_find_message(messagebuffer, messagebuffer_length, incoming_msg, &incoming_msg_length);

    if (message_size >= 0) {
      //remove the message from the buffer
      messagebuffer_length -= message_size;

      memmove(messagebuffer, &messagebuffer[message_size], messagebuffer_length);
      //clear the buffer behind the copied parts
      //memset(&messagebuffer[messagebuffer_length],0,messagebuffer_size - messagebuffer_length);
      //just zero temrinate so that string functions will not get problems
      incoming_msg[incoming_msg_length]=0;

      // 2.2 call plugins with incoming_msg buffer
      receivedCallback(incoming_msg, incoming_msg_length);

    } else {

      //check if there's bytes that needs to be removed from the start of the buffer
      if (incoming_msg_length > 0) {

        messagebuffer_length -= incoming_msg_length;

        if (messagebuffer_length) {
          memmove(messagebuffer, &messagebuffer[incoming_msg_length], messagebuffer_length);
        } else {
          memset(messagebuffer, 0, messagebuffer_size);
        }
      }

      if (message_size == -1) {
        //no there will be no other message, so exit loop
        //>>>>>>>>>>>>>>>>>>>>>>>
        //   !!! IMPORTANT !!!
        // always break on error
        //   !!! IMPORTANT !!!
        //>>>>>>>>>>>>>>>>>>>>>>>
        break;
      }
    }
  }

  return (0);

}

int bsnmsg_message_to_stream(const uint8_t *buffer, const uint32_t length, messageCallback_t sendCallback) {
  // leave if there's no buffer initialized
  if (messagebuffer == 0 || outgoing_msg == 0) {
    printf("initialize buffer first!\n\r");
    return (-1);
  }
  if (sendCallback == 0) {
    printf("sendCallback missing!\n\r");
    return (-2);
  }
  // reset outgoing buffer
  memset(outgoing_msg, 0, messagebuffer_size);
  //put frame around the message
  bsn_compress_message(buffer, length, outgoing_msg, &outgoing_msg_length);
  //callback for sending data
  sendCallback(outgoing_msg, outgoing_msg_length);
  return (0);
}

uint32_t bsnmsg_get_crc_errors_since_start(void) {
  return (crc_errors_since_start);
}
