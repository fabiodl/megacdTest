#ifndef _CRC32_H_
#define _CRC32_H_

#include <stdint.h>

void initCrcTable();
void crc32(const void *data, uint32_t n_bytes, uint32_t* crc);


#endif
