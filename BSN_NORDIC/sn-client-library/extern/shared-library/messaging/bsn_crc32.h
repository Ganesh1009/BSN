/*
 * crc32.h
 *
 *  Created on: 15 Dec 2017
 *      Author: musahl
 */

#ifndef MESSAGING_CRC32_H_
#define MESSAGING_CRC32_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/**
 * @brief calculates the CRC32 of a buffer
 * @param crc   : crc to add incomfin data to
 * @param p     : pointer to a buffer that should be added
 * @param size  : size of buffer
 * @return crc32 of the given input crc3 and the additional buffer
 */
uint32_t crc32 ( uint32_t crc, const uint8_t *p, size_t size );

#ifdef __cplusplus
}
#endif
#endif /* MESSAGING_CRC32_H_ */
