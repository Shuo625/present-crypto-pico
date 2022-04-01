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
 * @brief Copy bit to ith bit of byte
 * 
 * @param byte a byte
 * @param i ith
 * @param bit bit value to be copied
 * 
 */
#define CPYBIT(byte, i, bit) byte |= bit << i

/**
 * @brief XOR the pt with roundkey.
 * 
 * @param pt state of previous round
 * @param roundkey key of this round
 */
static void add_round_key(uint8_t pt[CRYPTO_IN_SIZE], uint8_t roundkey[CRYPTO_IN_SIZE])
{
	for (uint8_t i = 0; i < CRYPTO_IN_SIZE; i++) {
		pt[i] ^= roundkey[i];
	}
}

static const uint8_t sbox[16] = {
	0xC, 0x5, 0x6, 0xB, 0x9, 0x0, 0xA, 0xD, 0x3, 0xE, 0xF, 0x8, 0x4, 0x7, 0x1, 0x2,
};

/**
 * @brief Replace each byte of state with value in sbox
 * 
 * @param s state
 */
static void sbox_layer(uint8_t s[CRYPTO_IN_SIZE])
{
	// Up 4 bits and low 4 bits.
	uint8_t un, ln;

	for (uint8_t i = 0; i < CRYPTO_IN_SIZE; i++) {
		un = s[i] >> 4;
		ln = s[i] & 0x0F;

		// Replace old byte with new byte
		s[i] = sbox[ln] | (sbox[un] << 4);
	}
}

/**
 * @brief Permutate each bit of 64-bit state
 * 
 * For ith bit, the new index is (i / 4) + (i % 4) * 16
 * 
 * @param s state
 */
static void pbox_layer(uint8_t s[CRYPTO_IN_SIZE])
{
	// Create a tmp storing permutated state
	uint8_t tmp_s[8] = {0u};

	for (uint8_t i = 0; i < 64; i++) {
		uint8_t tmp = GETBIT(s[i / 8], i % 8);
		uint8_t new_i = PBOX(i);
		CPYBIT(tmp_s[new_i / 8], new_i % 8, tmp);
	}

	memcpy(s, tmp_s, CRYPTO_IN_SIZE);
}

static void update_round_key(uint8_t key[CRYPTO_KEY_SIZE], const uint8_t r)
{
	//
	// There is no need to edit this code
	//
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
	key[8] = tmp0 >> 3   | tmp1 << 5;
	key[9] = tmp1 >> 3   | tmp2 << 5;
	
	// perform sbox lookup on MSbits
	tmp = sbox[key[9] >> 4];
	key[9] &= 0x0F;
	key[9] |= tmp << 4;
	
	// XOR round counter k19 ... k15
	key[1] ^= r << 7;
	key[2] ^= r >> 1;
}

void crypto_func(uint8_t pt[CRYPTO_IN_SIZE], uint8_t key[CRYPTO_KEY_SIZE])
{
	//
	// There is no need to edit this code
	//
	
	uint8_t i = 0;
	
	for(i = 1; i <= 31; i++)
	{
		add_round_key(pt, key + 2);
		sbox_layer(pt);
		pbox_layer(pt);
		update_round_key(key, i);
	}
	
	add_round_key(pt, key + 2);
}
