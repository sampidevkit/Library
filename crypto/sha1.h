#ifndef SHA1_H
#define SHA1_H

#include <stdbool.h>
#include <stdint.h>

int self_check(void);
void sha1_compress(uint32_t *state, uint32_t *block);
bool sha1_hash(uint8_t *message, uint32_t len, uint32_t *hash);

#endif 
