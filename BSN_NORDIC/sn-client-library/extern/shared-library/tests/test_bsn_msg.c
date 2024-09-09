/*
 * bsn_msg_test.c
 *
 *  Created on: 26 Sep 2018
 *      Author: musahl
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "bsn_msg.h"

#define BUFSIZE (4096)


int receivemessagecallback (const uint8_t * buffer, const uint32_t length) {
  printf("received message [%d] : \n", length);
  puts(buffer);
  return (0);
}

int receivemessage_binary_callback (const uint8_t * buffer, const uint32_t length) {
  printf("received message [%d] : \n", length);
  //bsn_msg_printhex(buffer,length);
  return (0);
}

int sendmessagecallback (const uint8_t * buffer, const uint32_t length) {
  printf("sending buffer [%d] : \n", length);
  puts(buffer);
  bsnmsg_stream_to_message(buffer, length, receivemessagecallback);
  return (0);
}

int sendmessage_binary_callback (const uint8_t * buffer, const uint32_t length) {
  printf("sending buffer [%d] : \n", length);
  //bsn_msg_printhex(buffer,length);
  bsnmsg_stream_to_message(buffer, length, receivemessage_binary_callback);
  return (0);
}

int sendmessage_binary_bytewise_callback (const uint8_t * buffer, const uint32_t length) {
  printf("sending buffer [%d] : \n", length);
  //bsn_msg_printhex(buffer,length);
  int i=0;
  for(i=0;i<length;i++)
    bsnmsg_stream_to_message(&buffer[i], 1, receivemessage_binary_callback);
  return (0);
}

int sendmessagecallbytewise (const uint8_t * buffer, const uint32_t length) {
  printf("sending buffer [%d] : \n", length);
  puts(buffer);

  int i=0;
  for(i=0;i<length;i++)
    bsnmsg_stream_to_message(&buffer[i], 1, receivemessagecallback);

  return (0);
}

int sendmessagecallChunked (const uint8_t * buffer, const uint32_t length) {
  printf("sending buffer [%d] : \n", length);
  puts(buffer);

  bsnmsg_stream_to_message(buffer, length, receivemessagecallback);
  return (0);
}

int sendmessagecallWithError (const uint8_t * buffer, const uint32_t length) {
  printf("sending buffer [%d] : \n", length);
  puts(buffer);
  uint8_t errorbuffer[15] = {0,1,2,3,4,5,5,46,235,45,34,245,234,54,5};
  if(length > 32)
  {
    bsnmsg_stream_to_message(buffer, 32, receivemessagecallback);
    bsnmsg_stream_to_message(errorbuffer, 15, receivemessagecallback);
    bsnmsg_stream_to_message(&buffer[32], 0, receivemessagecallback);
    bsnmsg_stream_to_message(&buffer[32],  length - 32, receivemessagecallback);
  }
  return (0);
}

int main(int argc, char **argv) {

  uint8_t sendbuffer[BUFSIZE] = {0};

  //call before init
  bsnmsg_stream_to_message(sendbuffer, 10, 0);
  bsnmsg_message_to_stream(sendbuffer, 10, 0);

  bsnmsg_initialize(BUFSIZE);

  time_t dummytime;
  srand ( time(&dummytime) );

  printf("starting with buffer size %d\n", BUFSIZE);

  int i = 0;
  int length = 256;
  for(i=0;i<length;i++)
    sendbuffer[i] = i;

  bsnmsg_stream_to_message(sendbuffer, 0, receivemessagecallback);
  bsnmsg_stream_to_message(sendbuffer, 1, receivemessagecallback);
  bsnmsg_stream_to_message(sendbuffer, 2, receivemessagecallback);
  bsnmsg_stream_to_message(sendbuffer, 3, receivemessagecallback);
  bsnmsg_stream_to_message(sendbuffer, 4, receivemessagecallback);

  printf("\n================\nwrong parameters:\n================\n");
  bsnmsg_stream_to_message(sendbuffer, 0, 0);
  bsnmsg_message_to_stream(sendbuffer, 0, 0);

  printf("\n================\ninvalid messages:\n================\n");
  sendbuffer[3]=0x7D;
  sendbuffer[4]=0x7D;
  bsnmsg_stream_to_message(sendbuffer, 5, receivemessagecallback);

  printf("\nBYTEWISE - all characters :\n");
  for(i=0;i<length;i++)
    sendbuffer[i] = i;
  bsnmsg_message_to_stream(sendbuffer, length, sendmessage_binary_callback);


  printf("\nOVERFLOW :\n");
  for(i=0;i<length;i++)
	  sendbuffer[i] = i;
  for(i=0;i<(BUFSIZE/length)+2;i++)
	  bsnmsg_stream_to_message(sendbuffer, length, receivemessagecallback);


  printf("\nBYTEWISE - random characters :\n");
  length=rand() & 0x3FF;
  for(i=0;i<length;i++)
    sendbuffer[i] = rand() & 0xFF;
  bsnmsg_message_to_stream(sendbuffer, length, sendmessage_binary_bytewise_callback);

  printf("\nBYTEWISE :\n");
  snprintf(sendbuffer,BUFSIZE,"987654321");
  bsnmsg_message_to_stream(sendbuffer, strlen(sendbuffer), sendmessagecallbytewise);

  snprintf(sendbuffer,BUFSIZE,"5555588888AHJKL");
  printf("adding errors here... %s\n", sendbuffer);
  bsnmsg_stream_to_message(sendbuffer, strlen(sendbuffer), receivemessagecallback);

  printf("\nBYTEWISE after error:\n");
  snprintf(sendbuffer,BUFSIZE,"987654321");
  bsnmsg_message_to_stream(sendbuffer, strlen(sendbuffer), sendmessagecallbytewise);

  printf("\nCHUNKS :\n");

  snprintf(sendbuffer,BUFSIZE,"Dies is ein Test 123456789 hin und wieder zurück, das klappt schon");
  bsnmsg_message_to_stream(sendbuffer, strlen(sendbuffer), sendmessagecallChunked);

  snprintf(sendbuffer,BUFSIZE,"5555588888AHJKL");
  printf("adding errors here... %s\n", sendbuffer);
  bsnmsg_stream_to_message(sendbuffer, strlen(sendbuffer), receivemessagecallback);

  printf("\nCHUNKS after error:\n");
  snprintf(sendbuffer,BUFSIZE,"Dies is ein Test 123456789 hin und wieder zurück, das klappt schon\nABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
  bsnmsg_message_to_stream(sendbuffer, strlen(sendbuffer), sendmessagecallChunked);

  snprintf(sendbuffer,BUFSIZE,"5555588888AHJKL");
  printf("adding errors here... %s\n", sendbuffer);
  bsnmsg_stream_to_message(sendbuffer, strlen(sendbuffer), receivemessagecallback);

  printf("\nCHUNKS with error in transmittion:\n");
  snprintf(sendbuffer,BUFSIZE,"Dies is ein Test 123456789 hin und wieder zurück, das klappt schon\nABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
  bsnmsg_message_to_stream(sendbuffer, strlen(sendbuffer), sendmessagecallWithError);

  printf("\nCRC errors since start %d\n", bsnmsg_get_crc_errors_since_start());

  exit(0);
}
