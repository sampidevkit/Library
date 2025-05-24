#ifndef ECC_H
#define ECC_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define secp64r1  8
#define secp128r1 16
#define secp192r1 24
#define secp256r1 32
#define secp384r1 48

#ifndef ECC_CURVE
#define ECC_CURVE secp128r1
#endif

#define NUM_ECC_DIGITS ECC_CURVE

typedef struct {
    uint8_t key[NUM_ECC_DIGITS];
} ecc_single_point_t;

typedef struct {
    uint8_t x[NUM_ECC_DIGITS];
    uint8_t y[NUM_ECC_DIGITS];
} ecc_double_point_t;

typedef struct {
    ecc_single_point_t prv; // private
    ecc_double_point_t pub; // 
} ecc_key_t;

#ifndef ecc_rng_function
#include <stdlib.h>

void ecc_rng_function(uint8_t *pD, size_t size);
#endif

int ecc_make_key(ecc_key_t *pCtx);
int ecdh_shared_secret(uint8_t *p_secret, ecc_double_point_t *p_Key, uint8_t *p_privateKey);

#endif