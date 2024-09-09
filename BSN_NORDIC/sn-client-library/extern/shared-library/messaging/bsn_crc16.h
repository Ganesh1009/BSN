#ifndef CRC16_H
#define CRC16_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief caluclates the crc16 of the given buffer
 * @param buf
 * @param len
 * @return
 */
uint16_t crc16_buff(const void *buf, int len);

/**
 * @brief calculates crc16 in a floating manner
 * @param next
 * @param seed
 * @return
 */
uint16_t crc16_floating(uint8_t next, uint16_t seed);

#ifdef __cplusplus
}
#endif
#endif
