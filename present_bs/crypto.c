#include "crypto.h"

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

/**
 * Bring normal buffer into bitsliced form
 * @param pt Input: state_bs in normal form
 * @param state_bs Output: Bitsliced state
 */
static void enslice(const uint8_t pt[CRYPTO_IN_SIZE * BITSLICE_WIDTH], bs_reg_t state_bs[CRYPTO_IN_SIZE_BIT])
{
	// Loop for 32 texts
	for (uint8_t i = 0; i < BITSLICE_WIDTH; i++)
	{
		// Loop for 64 bits for each text
		for (uint8_t j = 0; j < CRYPTO_IN_SIZE_BIT; j++)
		{
			// Get jth bit of ith text
			bs_reg_t tmp = (pt[i * CRYPTO_IN_SIZE /* which text */ + j / 8 /* which byte */] >> (j % 8 /* which bit */)) & 0x1;
			// jth bit of ith text should be assigned to ith bit of jth state_bs
			state_bs[j] |= tmp << i;
		}
	}
}

/**
 * Bring bitsliced buffer into normal form
 * @param state_bs Input: Bitsliced state
 * @param pt Output: state_bs in normal form
 */
static void unslice(const bs_reg_t state_bs[CRYPTO_IN_SIZE_BIT], uint8_t pt[CRYPTO_IN_SIZE * BITSLICE_WIDTH])
{
	// Clear pt to zero
	for (uint16_t i = 0; i < CRYPTO_IN_SIZE * BITSLICE_WIDTH; i++)
	{
		pt[i] = 0u;
	}

	// Loop for 32 texts
	for (uint8_t i = 0; i < BITSLICE_WIDTH; i++)
	{
		// Loop for 64 bits of each text
		for (uint8_t j = 0; j < CRYPTO_IN_SIZE_BIT; j++)
		{
			// Get jth bit of ith text
			uint8_t tmp = (state_bs[j] >> i) & 0x1;
			pt[i * CRYPTO_IN_SIZE + j / 8] |= tmp << (j % 8);
		}
	}
}

static void add_round_key(bs_reg_t state_bs[CRYPTO_IN_SIZE_BIT], uint8_t round_key[CRYPTO_IN_SIZE])
{
	for (uint8_t i = 0; i < CRYPTO_IN_SIZE_BIT; i++)
	{
		uint8_t bit = GETBIT(round_key[i / 8], i % 8);
		if (bit == 0u)
		{
			state_bs[i] ^= 0;
		}
		else
		{
			state_bs[i] ^= 0xffffffffu;
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
 * Then simple these 4 ANFs
 *
 * y0 = x0 + x2 + x1 * x2 + x3
 * y1 = x1 + x0 * x1 * x2 + x3 + x1 * x3 + x0 * x1 * x3 + x2 * x3 + x0 * x2 * x3
 *    = x1 + x0 * x1 * x2 + x3 + x1 * x3 * (x0 + 1) + x2 * x3 * (x0 + 1)
 *    = x1 + x0 * x1 * x2 + x3 + (x0 + 1) * x3 * (x1 + x2)
 * y2 = 1 + x0 * x1 + x2 + x3 + x0 * x3 + x1 * x3 + x0 * x1 * x3 + x0 * x2 * x3
 *    = x0 * (x1 + x3) + x2 + x3 + x1 * x3 + x0 * x3 * (x1 + x2)
 * y3 = 1 + x0 + x1 + x1 * x2 + x0 * x1 * x2 + x3 + x0 * x1 * x3 + x0 * x2 * x3
 *
 *
 * @param state_bs
 */
static void sbox_layer(bs_reg_t state_bs[CRYPTO_IN_SIZE_BIT])
{
	bs_reg_t tmp0, tmp1, tmp2, tmp3;

	for (uint8_t i = 0; i < 16; i++)
	{
		uint8_t i_times_4 = i * 4;
		tmp0 = state_bs[i_times_4] ^
			   state_bs[i_times_4 + 2] ^
			   (state_bs[i_times_4 + 1] & state_bs[i_times_4 + 2]) ^
			   state_bs[i_times_4 + 3];
		tmp1 = state_bs[i_times_4 + 1] ^
			   (state_bs[i_times_4] & state_bs[i_times_4 + 1] & state_bs[i_times_4 + 2]) ^
			   state_bs[i_times_4 + 3] ^
			   (state_bs[i_times_4 + 1] & state_bs[i_times_4 + 3]) ^
			   (state_bs[i_times_4] & state_bs[i_times_4 + 1] & state_bs[i_times_4 + 3]) ^
			   (state_bs[i_times_4 + 2] & state_bs[i_times_4 + 3]) ^
			   (state_bs[i_times_4] & state_bs[i_times_4 + 2] & state_bs[i_times_4 + 3]);
		tmp2 = 0xffffffffu ^
			   (state_bs[i_times_4] & state_bs[i_times_4 + 1]) ^
			   state_bs[i_times_4 + 2] ^
			   state_bs[i_times_4 + 3] ^
			   (state_bs[i_times_4] & state_bs[i_times_4 + 3]) ^
			   (state_bs[i_times_4 + 1] & state_bs[i_times_4 + 3]) ^
			   (state_bs[i_times_4] & state_bs[i_times_4 + 1] & state_bs[i_times_4 + 3]) ^
			   (state_bs[i_times_4] & state_bs[i_times_4 + 2] & state_bs[i_times_4 + 3]);
		tmp3 = 0xffffffffu ^
			   state_bs[i_times_4] ^
			   state_bs[i_times_4 + 1] ^
			   (state_bs[i_times_4 + 1] & state_bs[i_times_4 + 2]) ^
			   (state_bs[i_times_4] & state_bs[i_times_4 + 1] & state_bs[i_times_4 + 2]) ^
			   state_bs[i_times_4 + 3] ^
			   (state_bs[i_times_4] & state_bs[i_times_4 + 1] & state_bs[i_times_4 + 3]) ^
			   (state_bs[i_times_4] & state_bs[i_times_4 + 2] & state_bs[i_times_4 + 3]);

		state_bs[i_times_4] = tmp0;
		state_bs[i_times_4 + 1] = tmp1;
		state_bs[i_times_4 + 2] = tmp2;
		state_bs[i_times_4 + 3] = tmp3;
	}
}

static void pbox_layer(bs_reg_t state_bs[CRYPTO_IN_SIZE_BIT])
{
	bs_reg_t state_tmp[CRYPTO_IN_SIZE_BIT];

	for (uint8_t i = 0; i < CRYPTO_IN_SIZE_BIT; i++)
	{
		uint8_t new_i = PBOX(i);
		state_tmp[new_i] = state_bs[i];
	}

	memcpy(state_bs, state_tmp, 4 * CRYPTO_IN_SIZE_BIT);
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

void crypto_func(uint8_t pt[CRYPTO_IN_SIZE * BITSLICE_WIDTH], uint8_t key[CRYPTO_KEY_SIZE])
{
	// State buffer and additional backbuffer of same size (you can remove the backbuffer if you do not need it)
	bs_reg_t state[CRYPTO_IN_SIZE_BIT] = {0u};

	uint8_t round;
	uint8_t i;

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
}