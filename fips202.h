#ifndef FIPS202_H
#define FIPS202_H

#include <stdint.h>


#define SHAKE128_RATE 168
#define SHAKE256_RATE 136

#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    #define _BIG_ENDIAN_
    #define HTOLE_64(i) BSWAP64((i))
    #define LETOH_64(i) BSWAP64((i))
#else
    #define _LITTLE_ENDIAN_
    #define HTOLE_64(i) (i)
    #define LETOH_64(i) (i)
#endif

void shake128_absorb(uint64_t *s, const unsigned char *input, unsigned int inputByteLen);
void shake128_squeezeblocks(unsigned char *output, unsigned long long nblocks, uint64_t *s);
void shake128(unsigned char *output, unsigned long long outlen, const unsigned char *input,  unsigned long long inlen);

void shake256_absorb(uint64_t *s, const unsigned char *input, unsigned int inputByteLen);
void shake256_squeezeblocks(unsigned char *output, unsigned long long nblocks, uint64_t *s);
void shake256(unsigned char *output, unsigned long long outlen, const unsigned char *input,  unsigned long long inlen);


#endif
