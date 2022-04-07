#include "crypto.h"

#include "pico/multicore.h"

#define OPTIMIZATION_SBOX
#define OPTIMIZATION_MULTICORE
#define OPTIMIZATION_UNFOLD_LOOP

/**
 * @brief Get ith bit from a byte
 *
 * @param byte a byte
 * @param i ith
 *
 * @return ith bit
 */
#define GETBIT(byte, i) ((byte >> i) & 0x01)

/**
 * @brief Calculate the new index of bit in pbox_layer
 *
 * @param i index of a bit
 *
 * @return new index
 *
 */
#define PBOX(i) ((i / 4) + (i % 4) * 16)

#define MULTICORE_CORE_NUM 2
#define CORE1 1
#define CORE0 0

#define MULTICORE_FOR_START(x, core_id) (core_id * x / MULTICORE_CORE_NUM)
#define MULTICORE_FOR_END(x, core_id) ((core_id + 1) * x / MULTICORE_CORE_NUM)
#define MULTICORE_FOR(i, x, core_id) for (i = MULTICORE_FOR_START(x, core_id); i < MULTICORE_FOR_END(x, core_id); i++)

#define MULTICORE_BARRIER()          \
    multicore_fifo_push_blocking(0); \
    multicore_fifo_pop_blocking()

/**
 * Bring normal buffer into bitsliced form
 * @param pt Input: state_bs in normal form
 * @param state_bs Output: Bitsliced state
 */
static void enslice(const uint8_t pt[CRYPTO_IN_SIZE * BITSLICE_WIDTH], bs_reg_t state_bs[CRYPTO_IN_SIZE_BIT]
#ifdef OPTIMIZATION_MULTICORE
                   ,uint8_t core_id
#endif
)
{
    uint8_t i;

#ifdef OPTIMIZATION_MULTICORE
    MULTICORE_FOR(i, CRYPTO_IN_SIZE_BIT, core_id)
#else
    for (i = 0; i < CRYPTO_IN_SIZE_BIT; i++)
#endif
    {
#ifdef OPTIMIZATION_UNFOLD_LOOP
        bs_reg_t tmp;

        tmp = (pt[0 * CRYPTO_IN_SIZE /* which text */ + i / 8 /* which byte */] >> (i % 8 /* which bit */)) & 0x1;
        state_bs[i] |= tmp << 0;
        tmp = (pt[1 * CRYPTO_IN_SIZE /* which text */ + i / 8 /* which byte */] >> (i % 8 /* which bit */)) & 0x1;
        state_bs[i] |= tmp << 1;
        tmp = (pt[2 * CRYPTO_IN_SIZE /* which text */ + i / 8 /* which byte */] >> (i % 8 /* which bit */)) & 0x1;
        state_bs[i] |= tmp << 2;
        tmp = (pt[3 * CRYPTO_IN_SIZE /* which text */ + i / 8 /* which byte */] >> (i % 8 /* which bit */)) & 0x1;
        state_bs[i] |= tmp << 3;
        tmp = (pt[4 * CRYPTO_IN_SIZE /* which text */ + i / 8 /* which byte */] >> (i % 8 /* which bit */)) & 0x1;
        state_bs[i] |= tmp << 4;
        tmp = (pt[5 * CRYPTO_IN_SIZE /* which text */ + i / 8 /* which byte */] >> (i % 8 /* which bit */)) & 0x1;
        state_bs[i] |= tmp << 5;
        tmp = (pt[6 * CRYPTO_IN_SIZE /* which text */ + i / 8 /* which byte */] >> (i % 8 /* which bit */)) & 0x1;
        state_bs[i] |= tmp << 6;
        tmp = (pt[7 * CRYPTO_IN_SIZE /* which text */ + i / 8 /* which byte */] >> (i % 8 /* which bit */)) & 0x1;
        state_bs[i] |= tmp << 7;
        tmp = (pt[8 * CRYPTO_IN_SIZE /* which text */ + i / 8 /* which byte */] >> (i % 8 /* which bit */)) & 0x1;
        state_bs[i] |= tmp << 8;
        tmp = (pt[9 * CRYPTO_IN_SIZE /* which text */ + i / 8 /* which byte */] >> (i % 8 /* which bit */)) & 0x1;
        state_bs[i] |= tmp << 9;
        tmp = (pt[10 * CRYPTO_IN_SIZE /* which text */ + i / 8 /* which byte */] >> (i % 8 /* which bit */)) & 0x1;
        state_bs[i] |= tmp << 10;
        tmp = (pt[11 * CRYPTO_IN_SIZE /* which text */ + i / 8 /* which byte */] >> (i % 8 /* which bit */)) & 0x1;
        state_bs[i] |= tmp << 11;
        tmp = (pt[12 * CRYPTO_IN_SIZE /* which text */ + i / 8 /* which byte */] >> (i % 8 /* which bit */)) & 0x1;
        state_bs[i] |= tmp << 12;
        tmp = (pt[13 * CRYPTO_IN_SIZE /* which text */ + i / 8 /* which byte */] >> (i % 8 /* which bit */)) & 0x1;
        state_bs[i] |= tmp << 13;
        tmp = (pt[14 * CRYPTO_IN_SIZE /* which text */ + i / 8 /* which byte */] >> (i % 8 /* which bit */)) & 0x1;
        state_bs[i] |= tmp << 14;
        tmp = (pt[15 * CRYPTO_IN_SIZE /* which text */ + i / 8 /* which byte */] >> (i % 8 /* which bit */)) & 0x1;
        state_bs[i] |= tmp << 15;
        tmp = (pt[16 * CRYPTO_IN_SIZE /* which text */ + i / 8 /* which byte */] >> (i % 8 /* which bit */)) & 0x1;
        state_bs[i] |= tmp << 16;
        tmp = (pt[17 * CRYPTO_IN_SIZE /* which text */ + i / 8 /* which byte */] >> (i % 8 /* which bit */)) & 0x1;
        state_bs[i] |= tmp << 17;
        tmp = (pt[18 * CRYPTO_IN_SIZE /* which text */ + i / 8 /* which byte */] >> (i % 8 /* which bit */)) & 0x1;
        state_bs[i] |= tmp << 18;
        tmp = (pt[19 * CRYPTO_IN_SIZE /* which text */ + i / 8 /* which byte */] >> (i % 8 /* which bit */)) & 0x1;
        state_bs[i] |= tmp << 19;
        tmp = (pt[20 * CRYPTO_IN_SIZE /* which text */ + i / 8 /* which byte */] >> (i % 8 /* which bit */)) & 0x1;
        state_bs[i] |= tmp << 20;
        tmp = (pt[21 * CRYPTO_IN_SIZE /* which text */ + i / 8 /* which byte */] >> (i % 8 /* which bit */)) & 0x1;
        state_bs[i] |= tmp << 21;
        tmp = (pt[22 * CRYPTO_IN_SIZE /* which text */ + i / 8 /* which byte */] >> (i % 8 /* which bit */)) & 0x1;
        state_bs[i] |= tmp << 22;
        tmp = (pt[23 * CRYPTO_IN_SIZE /* which text */ + i / 8 /* which byte */] >> (i % 8 /* which bit */)) & 0x1;
        state_bs[i] |= tmp << 23;
        tmp = (pt[24 * CRYPTO_IN_SIZE /* which text */ + i / 8 /* which byte */] >> (i % 8 /* which bit */)) & 0x1;
        state_bs[i] |= tmp << 24;
        tmp = (pt[25 * CRYPTO_IN_SIZE /* which text */ + i / 8 /* which byte */] >> (i % 8 /* which bit */)) & 0x1;
        state_bs[i] |= tmp << 25;
        tmp = (pt[26 * CRYPTO_IN_SIZE /* which text */ + i / 8 /* which byte */] >> (i % 8 /* which bit */)) & 0x1;
        state_bs[i] |= tmp << 26;
        tmp = (pt[27 * CRYPTO_IN_SIZE /* which text */ + i / 8 /* which byte */] >> (i % 8 /* which bit */)) & 0x1;
        state_bs[i] |= tmp << 27;
        tmp = (pt[28 * CRYPTO_IN_SIZE /* which text */ + i / 8 /* which byte */] >> (i % 8 /* which bit */)) & 0x1;
        state_bs[i] |= tmp << 28;
        tmp = (pt[29 * CRYPTO_IN_SIZE /* which text */ + i / 8 /* which byte */] >> (i % 8 /* which bit */)) & 0x1;
        state_bs[i] |= tmp << 29;
        tmp = (pt[30 * CRYPTO_IN_SIZE /* which text */ + i / 8 /* which byte */] >> (i % 8 /* which bit */)) & 0x1;
        state_bs[i] |= tmp << 30;
        tmp = (pt[31 * CRYPTO_IN_SIZE /* which text */ + i / 8 /* which byte */] >> (i % 8 /* which bit */)) & 0x1;
        state_bs[i] |= tmp << 31;
#else
        // Loop for 64 bits for each text
        for (uint8_t j = 0; j < BITSLICE_WIDTH; j++)
        {
            // Get jth bit of ith text
            bs_reg_t tmp = (pt[j * CRYPTO_IN_SIZE /* which text */ + i / 8 /* which byte */] >> (i % 8 /* which bit */)) & 0x1;
            // jth bit of ith text should be assigned to ith bit of jth state_bs
            state_bs[i] |= tmp << j;
        }
#endif
    }
}

/**
 * Bring bitsliced buffer into normal form
 * @param state_bs Input: Bitsliced state
 * @param pt Output: state_bs in normal form
 */
static void unslice(const bs_reg_t state_bs[CRYPTO_IN_SIZE_BIT], uint8_t pt[CRYPTO_IN_SIZE * BITSLICE_WIDTH]
#ifdef OPTIMIZATION_MULTICORE
                   ,uint8_t core_id
#endif
)
{
    uint8_t i;

#ifdef OPTIMIZATION_MULTICORE
    MULTICORE_FOR(i, CRYPTO_IN_SIZE_BIT, core_id)
#else
    // Clear pt to zero
    memset(pt, 0u, CRYPTO_IN_SIZE * BITSLICE_WIDTH);

    // Loop for 32 texts
    for (uint8_t i = 0; i < CRYPTO_IN_SIZE_BIT; i++)
#endif
    {
#ifdef OPTIMIZATION_UNFOLD_LOOP
        uint8_t tmp;

        tmp = (state_bs[i] >> 0) & 0x1;
        pt[0 * CRYPTO_IN_SIZE + i / 8] |= tmp << (i % 8);
        tmp = (state_bs[i] >> 1) & 0x1;
        pt[1 * CRYPTO_IN_SIZE + i / 8] |= tmp << (i % 8);
        tmp = (state_bs[i] >> 2) & 0x1;
        pt[2 * CRYPTO_IN_SIZE + i / 8] |= tmp << (i % 8);
        tmp = (state_bs[i] >> 3) & 0x1;
        pt[3 * CRYPTO_IN_SIZE + i / 8] |= tmp << (i % 8);
        tmp = (state_bs[i] >> 4) & 0x1;
        pt[4 * CRYPTO_IN_SIZE + i / 8] |= tmp << (i % 8);
        tmp = (state_bs[i] >> 5) & 0x1;
        pt[5 * CRYPTO_IN_SIZE + i / 8] |= tmp << (i % 8);
        tmp = (state_bs[i] >> 6) & 0x1;
        pt[6 * CRYPTO_IN_SIZE + i / 8] |= tmp << (i % 8);
        tmp = (state_bs[i] >> 7) & 0x1;
        pt[7 * CRYPTO_IN_SIZE + i / 8] |= tmp << (i % 8);
        tmp = (state_bs[i] >> 8) & 0x1;
        pt[8 * CRYPTO_IN_SIZE + i / 8] |= tmp << (i % 8);
        tmp = (state_bs[i] >> 9) & 0x1;
        pt[9 * CRYPTO_IN_SIZE + i / 8] |= tmp << (i % 8);
        tmp = (state_bs[i] >> 10) & 0x1;
        pt[10 * CRYPTO_IN_SIZE + i / 8] |= tmp << (i % 8);
        tmp = (state_bs[i] >> 11) & 0x1;
        pt[11 * CRYPTO_IN_SIZE + i / 8] |= tmp << (i % 8);
        tmp = (state_bs[i] >> 12) & 0x1;
        pt[12 * CRYPTO_IN_SIZE + i / 8] |= tmp << (i % 8);
        tmp = (state_bs[i] >> 13) & 0x1;
        pt[13 * CRYPTO_IN_SIZE + i / 8] |= tmp << (i % 8);
        tmp = (state_bs[i] >> 14) & 0x1;
        pt[14 * CRYPTO_IN_SIZE + i / 8] |= tmp << (i % 8);
        tmp = (state_bs[i] >> 15) & 0x1;
        pt[15 * CRYPTO_IN_SIZE + i / 8] |= tmp << (i % 8);
        tmp = (state_bs[i] >> 16) & 0x1;
        pt[16 * CRYPTO_IN_SIZE + i / 8] |= tmp << (i % 8);
        tmp = (state_bs[i] >> 17) & 0x1;
        pt[17 * CRYPTO_IN_SIZE + i / 8] |= tmp << (i % 8);
        tmp = (state_bs[i] >> 18) & 0x1;
        pt[18 * CRYPTO_IN_SIZE + i / 8] |= tmp << (i % 8);
        tmp = (state_bs[i] >> 19) & 0x1;
        pt[19 * CRYPTO_IN_SIZE + i / 8] |= tmp << (i % 8);
        tmp = (state_bs[i] >> 20) & 0x1;
        pt[20 * CRYPTO_IN_SIZE + i / 8] |= tmp << (i % 8);
        tmp = (state_bs[i] >> 21) & 0x1;
        pt[21 * CRYPTO_IN_SIZE + i / 8] |= tmp << (i % 8);
        tmp = (state_bs[i] >> 22) & 0x1;
        pt[22 * CRYPTO_IN_SIZE + i / 8] |= tmp << (i % 8);
        tmp = (state_bs[i] >> 23) & 0x1;
        pt[23 * CRYPTO_IN_SIZE + i / 8] |= tmp << (i % 8);
        tmp = (state_bs[i] >> 24) & 0x1;
        pt[24 * CRYPTO_IN_SIZE + i / 8] |= tmp << (i % 8);
        tmp = (state_bs[i] >> 25) & 0x1;
        pt[25 * CRYPTO_IN_SIZE + i / 8] |= tmp << (i % 8);
        tmp = (state_bs[i] >> 26) & 0x1;
        pt[26 * CRYPTO_IN_SIZE + i / 8] |= tmp << (i % 8);
        tmp = (state_bs[i] >> 27) & 0x1;
        pt[27 * CRYPTO_IN_SIZE + i / 8] |= tmp << (i % 8);
        tmp = (state_bs[i] >> 28) & 0x1;
        pt[28 * CRYPTO_IN_SIZE + i / 8] |= tmp << (i % 8);
        tmp = (state_bs[i] >> 29) & 0x1;
        pt[29 * CRYPTO_IN_SIZE + i / 8] |= tmp << (i % 8);
        tmp = (state_bs[i] >> 30) & 0x1;
        pt[30 * CRYPTO_IN_SIZE + i / 8] |= tmp << (i % 8);
        tmp = (state_bs[i] >> 31) & 0x1;
        pt[31 * CRYPTO_IN_SIZE + i / 8] |= tmp << (i % 8);
#else
        // Loop for 64 bits of each text
        for (uint8_t j = 0; j < BITSLICE_WIDTH; j++)
        {
            // Get jth bit of ith text
            uint8_t tmp = (state_bs[i] >> j) & 0x1;
            pt[j * CRYPTO_IN_SIZE + i / 8] |= tmp << (i % 8);
        }
#endif
    }
}

static void add_round_key(bs_reg_t state_bs[CRYPTO_IN_SIZE_BIT], uint8_t round_key[CRYPTO_IN_SIZE]
#ifdef OPTIMIZATION_MULTICORE
                         ,uint8_t core_id
#endif
)
{
    uint8_t i;

#ifdef OPTIMIZATION_MULTICORE
    MULTICORE_FOR(i, CRYPTO_IN_SIZE_BIT, core_id)
#else
    for (i = 0; i < CRYPTO_IN_SIZE_BIT; i++)
#endif
    {
        uint8_t bit = GETBIT(round_key[i / 8], i % 8);
        if (bit != 0u)
        {
            state_bs[i] = ~state_bs[i];
        }
    }
}

/**
 * @brief Using Butterfly algorithm to calculate each ANF of 4 bits
 *
 * uint8_t sbox[16] = "0xC, 0x5, 0x6, 0xB, 0x9, 0x0, 0xA, 0xD, 0x3, 0xE, 0xF, 0x8, 0x4, 0x7, 0x1, 0x2"
 *
 * y0 ANF
 * x3 x2 x1 x0 y0 S1 S2 S3 S4
 * 0  0  0  0  0  0  0  0  0
 * 0  0  0  1  1  1  1  1  1
 * 0  0  1  0  0  0  0  0  0
 * 0  0  1  1  1  1  0  0  0
 * 0  1  0  0  1  1  1  1  1
 * 0  1  0  1  0  1  1  0  0
 * 0  1  1  0  0  0  1  1  1
 * 0  1  1  1  1  1  0  0  0
 * 1  0  0  0  1  1  1  1  1
 * 1  0  0  1  0  1  1  1  0
 * 1  0  1  0  1  1  0  0  0
 * 1  0  1  1  0  1  0  0  0
 * 1  1  0  0  0  0  0  1  0
 * 1  1  0  1  1  1  1  0  0
 * 1  1  1  0  1  1  1  1  0
 * 1  1  1  1  0  1  0  0  0
 *
 * y0 = x0 + x2 + x1 * x2 + x3
 *
 * y1 ANF
 * x3 x2 x1 x0 y1 S1 S2 S3 S4
 * 0  0  0  0  0  0  0  0  0
 * 0  0  0  1  0  0  0  0  0
 * 0  0  1  0  1  1  1  1  1
 * 0  0  1  1  1  0  0  0  0
 * 0  1  0  0  0  0  0  0  0
 * 0  1  0  1  0  0  0  0  0
 * 0  1  1  0  1  1  1  0  0
 * 0  1  1  1  0  1  1  1  1
 * 1  0  0  0  1  1  1  1  1
 * 1  0  0  1  1  0  0  0  0
 * 1  0  1  0  1  1  0  0  1
 * 1  0  1  1  0  1  1  1  1
 * 1  1  0  0  0  0  0  1  1
 * 1  1  0  1  1  1  1  1  1
 * 1  1  1  0  0  0  0  0  0
 * 1  1  1  1  1  1  0  1  0
 *
 * y1 = x1 + x0 * x1 * x2 + x3 + x1 * x3 + x0 * x1 * x3 + x2 * x3 + x0 * x2 * x3
 *
 * y2 ANF
 * x3 x2 x1 x0 y2 S1 S2 S3 S4
 * 0  0  0  0  1  1  1  1  1
 * 0  0  0  1  1  0  0  0  0
 * 0  0  1  0  1  1  0  0  0
 * 0  0  1  1  0  1  1  1  1
 * 0  1  0  0  0  0  0  1  1
 * 0  1  0  1  0  0  0  0  0
 * 0  1  1  0  0  0  0  0  0
 * 0  1  1  1  1  1  1  0  0
 * 1  0  0  0  0  0  0  0  1
 * 1  0  0  1  1  1  1  1  1
 * 1  0  1  0  1  1  1  1  1
 * 1  0  1  1  0  1  0  0  1
 * 1  1  0  0  1  1  1  1  0
 * 1  1  0  1  1  0  0  1  1
 * 1  1  1  0  0  0  1  0  0
 * 1  1  1  1  0  0  0  0  0
 *
 * y2 = 1 + x0 * x1 + x2 + x3 + x0 * x3 + x1 * x3 + x0 * x1 * x3 + x0 * x2 * x3
 *
 * y3 ANF
 * x3 x2 x1 x0 y3 S1 S2 S3 S4
 * 0  0  0  0  1  1  1  1  1
 * 0  0  0  1  0  1  1  1  1
 * 0  0  1  0  0  0  1  1  1
 * 0  0  1  1  1  1  0  0  0
 * 0  1  0  0  1  1  1  0  0
 * 0  1  0  1  0  1  1  0  0
 * 0  1  1  0  1  1  0  1  1
 * 0  1  1  1  1  0  1  1  1
 * 1  0  0  0  0  0  0  0  1
 * 1  0  0  1  1  1  1  1  0
 * 1  0  1  0  1  1  1  1  0
 * 1  0  1  1  1  0  1  1  1
 * 1  1  0  0  0  0  0  0  0
 * 1  1  0  1  0  0  0  1  1
 * 1  1  1  0  0  0  0  1  0
 * 1  1  1  1  0  0  0  1  0
 *
 * y3 = 1 + x0 + x1 + x1 * x2 + x0 * x1 * x2 + x3 + x0 * x1 * x3 + x0 * x2 * x3
 *
 * y0 = x0 + x2 + x1 * x2 + x3
 * y1 = x1 + x0 * x1 * x2 + x3 + x1 * x3 + x0 * x1 * x3 + x2 * x3 + x0 * x2 * x3
 * y2 = 1 + x0 * x1 + x2 + x3 + x0 * x3 + x1 * x3 + x0 * x1 * x3 + x0 * x2 * x3
 * y3 = 1 + x0 + x1 + x1 * x2 + x0 * x1 * x2 + x3 + x0 * x1 * x3 + x0 * x2 * x3
 *
 * y0 = x0 + x2 + x1 * x2 + x3
 * 	  = x0 + ~x1 * x2 + x3
 *
 * y1 = x1 + x0 * x1 * x2 + x3 + x1 * x3 + x0 * x1 * x3 + x2 * x3 + x0 * x2 * x3
 *    = x1 + x0 * x1 * x2 + x3 + x1 * x3 * (x0 + 1) + x2 * x3 * (x0 + 1)
 *    = x1 + x0 * x1 * x2 + x3 + (x0 + 1) * x3 * (x1 + x2)
 *    = x1 + x0 * x1 * x2 + x3 + ~x0 * x3 * (x1 + x2)
 *
 * y2 = 1 + x0 * x1 + x2 + x3 + x0 * x3 + x1 * x3 + x0 * x1 * x3 + x0 * x2 * x3
 *    = 1 + x0 * (x1 + x3) + x2 + x3 + x1 * x3 + x0 * x3 * (x1 + x2)
 *    = ~x2 + x0 * (x1 + x3) + ~x1 * x3 + x0 * x3 * (x1 + x2)
 *
 * y3 = 1 + x0 + x1 + x1 * x2 + x0 * x1 * x2 + x3 + x0 * x1 * x3 + x0 * x2 * x3
 *    = 1 + x0 + x1 + (x0 + 1) * x1 * x2 + x3 + x0 * x3 * (x1 + x2)
 *    = (x0 + 1) * (x1 * x2 + 1) + x1 + x3 + x0 * x3 * (x1 + x2)
 *    = ~x0 * ~(x1 * x2) + x1 + x3 + x0 * x3 * (x1 + x2)
 *
 * @param state_bs
 */
static void sbox_layer(bs_reg_t state_bs[CRYPTO_IN_SIZE_BIT]
#ifdef OPTIMIZATION_MULTICORE
                      ,uint8_t core_id
#endif
)
{
    uint8_t i;

#ifdef OPTIMIZATION_MULTICORE
    MULTICORE_FOR(i, 16, core_id)
#else
    for (i = 0; i < 16; i++)
#endif
    {
#ifdef OPTIMIZATION_SBOX
        bs_reg_t x0, x1, x2, x3;
        bs_reg_t y0, y1, y2, y3;
        bs_reg_t x3_and_x1_xor_x2, x1_xor_x3, x1_and_x2;

        x0 = state_bs[i * 4];
        x1 = state_bs[i * 4 + 1];
        x2 = state_bs[i * 4 + 2];
        x3 = state_bs[i * 4 + 3];

        x3_and_x1_xor_x2 = x3 & (x1 ^ x2);
        x1_xor_x3 = x1 ^ x3;
        x1_and_x2 = x1 & x2;

        y0 = x0 ^ (~x1 & x2) ^ x3;
        y1 = x1_xor_x3 ^ (x0 & x1_and_x2) ^ (~x0 & x3_and_x1_xor_x2);
        y2 = ~x2 ^ (x0 & x1_xor_x3) ^ (~x1 & x3) ^ (x0 & x3_and_x1_xor_x2);
        y3 = (~x0 & ~x1_and_x2) ^ x1_xor_x3 ^ (x0 & x3_and_x1_xor_x2);

        state_bs[i * 4] = y0;
        state_bs[i * 4 + 1] = y1;
        state_bs[i * 4 + 2] = y2;
        state_bs[i * 4 + 3] = y3;
#else
        bs_reg_t x0, x1, x2, x3;
        bs_reg_t y0, y1, y2, y3;

        x0 = state_bs[i * 4];
        x1 = state_bs[i * 4 + 1];
        x2 = state_bs[i * 4 + 2];
        x3 = state_bs[i * 4 + 3];

        y0 = x0 ^ x2 ^ (x1 & x2) ^ x3;
        y1 = x1 ^ (x0 & x1 & x2) ^ x3 ^ (x1 & x3) ^ (x0 & x1 & x3) ^ (x2 & x3) ^ (x0 & x2 & x3);
        y2 = 0xffffffffu ^ (x0 & x1) ^ x2 ^ x3 ^ (x0 & x3) ^ (x1 & x3) ^ (x0 & x1 & x3) ^ (x0 & x2 & x3);
        y3 = 0xffffffffu ^ x0 ^ x1 ^ (x1 & x2) ^ (x0 & x1 & x2) ^ x3 ^ (x0 & x1 & x3) ^ (x0 & x2 & x3);

        state_bs[i * 4] = y0;
        state_bs[i * 4 + 1] = y1;
        state_bs[i * 4 + 2] = y2;
        state_bs[i * 4 + 3] = y3;
#endif
    }
}

static void pbox_layer(bs_reg_t state_bs[CRYPTO_IN_SIZE_BIT]
#ifdef OPTIMIZATION_MULTICORE
                      ,bs_reg_t state_tmp[CRYPTO_IN_SIZE_BIT]
                      ,uint8_t core_id
#endif
)
{
    uint8_t i;

#ifdef OPTIMIZATION_MULTICORE
    MULTICORE_FOR(i, CRYPTO_IN_SIZE_BIT, core_id)
#else
    bs_reg_t state_tmp[CRYPTO_IN_SIZE_BIT];

    for (uint8_t i = 0; i < CRYPTO_IN_SIZE_BIT; i++)
#endif
    {
        state_tmp[PBOX(i)] = state_bs[i];
    }

#ifndef OPTIMIZATION_MULTICORE
    memcpy(state_bs, state_tmp, 4 * CRYPTO_IN_SIZE_BIT);
#endif
}

/**
 * Perform next key schedule step
 * @param key Key register to be updated
 * @param r Round counter
 * @warning For correct function, has to be called with incremented r each time
 * @note You are free to change or optimize this function
 */
static void update_round_key(uint8_t key[CRYPTO_KEY_SIZE], const uint8_t r)
{
    //
    // There is no need to edit this code - but you can do so if you want to
    // optimise further
    //

    const uint8_t sbox[16] = {
        0xC,
        0x5,
        0x6,
        0xB,
        0x9,
        0x0,
        0xA,
        0xD,
        0x3,
        0xE,
        0xF,
        0x8,
        0x4,
        0x7,
        0x1,
        0x2,
    };

    uint8_t tmp = 0;
    const uint8_t tmp2 = key[2];
    const uint8_t tmp1 = key[1];
    const uint8_t tmp0 = key[0];

    // rotate right by 19 bit
    key[0] = key[2] >> 3 | key[3] << 5;
    key[1] = key[3] >> 3 | key[4] << 5;
    key[2] = key[4] >> 3 | key[5] << 5;
    key[3] = key[5] >> 3 | key[6] << 5;
    key[4] = key[6] >> 3 | key[7] << 5;
    key[5] = key[7] >> 3 | key[8] << 5;
    key[6] = key[8] >> 3 | key[9] << 5;
    key[7] = key[9] >> 3 | tmp0 << 5;
    key[8] = tmp0 >> 3 | tmp1 << 5;
    key[9] = tmp1 >> 3 | tmp2 << 5;

    // perform sbox lookup on MSbits
    tmp = sbox[key[9] >> 4];
    key[9] &= 0x0F;
    key[9] |= tmp << 4;

    // XOR round counter k19 ... k15
    key[1] ^= r << 7;
    key[2] ^= r >> 1;
}

#ifdef OPTIMIZATION_MULTICORE
/**
 * @brief
 *
 */
static void encrypt_core1()
{
    uint8_t *pt = (uint8_t *)multicore_fifo_pop_blocking();
    bs_reg_t *state_bs = (bs_reg_t *)multicore_fifo_pop_blocking();
    uint8_t *key = (uint8_t *)multicore_fifo_pop_blocking();
    bs_reg_t *state_tmp = (bs_reg_t *)multicore_fifo_pop_blocking();

    enslice(pt, state_bs, CORE1);

    MULTICORE_BARRIER();

    for (uint8_t i = 1; i <= 31; i++)
    {
        add_round_key(state_bs, key + 2, CORE1);
        sbox_layer(state_bs, CORE1);
        pbox_layer(state_bs, state_tmp, CORE1);

        MULTICORE_BARRIER();

        // Wait for that core0 finishes update_round_key and copy state_tmp to state_bs

        MULTICORE_BARRIER();
    }

    add_round_key(state_bs, key + 2, CORE1);

    MULTICORE_BARRIER();

    unslice(state_bs, pt, CORE1);

    MULTICORE_BARRIER();
}

/**
 * @brief
 *
 */
static void encrypt(uint8_t pt[CRYPTO_IN_SIZE * BITSLICE_WIDTH], bs_reg_t state_bs[CRYPTO_IN_SIZE_BIT], uint8_t key[CRYPTO_KEY_SIZE])
{
    bs_reg_t state_tmp[CRYPTO_IN_SIZE_BIT];

    multicore_reset_core1();
    multicore_launch_core1(encrypt_core1);

    multicore_fifo_push_blocking(pt);
    multicore_fifo_push_blocking(state_bs);
    multicore_fifo_push_blocking(key);
    multicore_fifo_push_blocking(state_tmp);

    enslice(pt, state_bs, CORE0);

    MULTICORE_BARRIER();

    for (uint8_t i = 1; i <= 31; i++)
    {
        add_round_key(state_bs, key + 2, CORE0);
        sbox_layer(state_bs, CORE0);
        pbox_layer(state_bs, state_tmp, CORE0);

        MULTICORE_BARRIER();

        memcpy(state_bs, state_tmp, 4 * CRYPTO_IN_SIZE_BIT);
        update_round_key(key, i);

        MULTICORE_BARRIER();
    }

    add_round_key(state_bs, key + 2, CORE0);
    memset(pt, 0u, CRYPTO_IN_SIZE * BITSLICE_WIDTH);

    MULTICORE_BARRIER();

    unslice(state_bs, pt, CORE0);

    MULTICORE_BARRIER();
}
#endif

void crypto_func(uint8_t pt[CRYPTO_IN_SIZE * BITSLICE_WIDTH], uint8_t key[CRYPTO_KEY_SIZE])
{
    // State buffer and additional backbuffer of same size (you can remove the backbuffer if you do not need it)
    bs_reg_t state[CRYPTO_IN_SIZE_BIT] = {0u};

#ifdef OPTIMIZATION_MULTICORE
    encrypt(pt, state, key);
#else
    // Bring into bitslicing form
    enslice(pt, state);

    // Encrypt
    for (uint8_t i = 1; i <= 31; i++)
    {
        add_round_key(state, key + 2);
        sbox_layer(state);
        pbox_layer(state);
        update_round_key(key, i);
    }

    add_round_key(state, key + 2);

    // Convert back to normal form
    unslice(state, pt);
#endif
}