/************************************************************************/
/*                                                                      */
/*	CalPSK.c This implements a conversion from                          */
/*	SSID and PassPhrase to a WPA PSK key                                */
/*                                                                      */
/*  This is a modification of the MRF24 Universal Driver PSK key gen    */
/*  software. Modified by Digilent to be non-blocking, iterative and    */
/*  stand-a-lone.                                                       */
/*                                                                      */
/************************************************************************/
/*	Author: 	Keith Vogel                                             */
/*	Copyright 2018, Digilent Inc.                                       */
/************************************************************************/
// Required retaining of the original Microchip License:

/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*	3/13/2018(KeithV): Created                                          */
/*                                                                      */
/************************************************************************/
#include <./libraries/DEIPcK/DEIPcK.h>

#define LITTLE_ENDIAN
#define SHA1_MAC_LEN 20

#ifdef LITTLE_ENDIAN
#define CONV_TO_BIG_ENDIAN(a)  ((((a) >> 24) & 0x000000FF) | (((a) >> 8) & 0x0000FF00) | (((a) << 8) & 0x00FF0000) | (((a) << 24) & 0xFF000000))
#else
#define CONV_TO_BIG_ENDIAN(a) (a)
#endif

//==============================================================================
//                                  DEFINES/CONSTANTS
//==============================================================================
#define WF_MAC_ADDRESS_LENGTH               (6)         // MAC addresses are always 6 bytes
#define WF_BSSID_LENGTH                     (6)         // BSSID is always 6 bytes
#define WF_MAX_SSID_LENGTH                  (32)        // does not include string terminator
#define WF_MAX_PASSPHRASE_LENGTH            (64)        // must include string terminator
#define WF_MAX_CHANNEL_LIST_LENGTH          (14)        // max channel list (for Japan) is 1-14
#define WF_WPS_PIN_LENGTH                   (8)         // 7 digits + checksum byte
#define WF_WPA_KEY_LENGTH                   (32)        // WPA binary key always 32 bytes
#define WF_WEP40_KEY_LENGTH                 (20)        // 4 keys of 5 bytes each
#define WF_WEP104_KEY_LENGTH                (52)        // 4 keys of 13 bytes each
#define WF_MAX_WEP_KEY_LENGTH               (WF_WEP104_KEY_LENGTH)
#define WF_NO_EVENT_DATA                    ((uint32_t)0xffffffff)
#define WF_RETRY_FOREVER                    (0xff)
#define WF_MAX_NUM_RATES                    (8)

/*
 * This is some code external to this module
 * provided by DEIPcK.h, this is here for reference.
#define ASSERT(a) {if(!(a)) while(1);}
#define DEWF_PSK_LENGTH WF_WPA_KEY_LENGTH
typedef struct
{
    // these fields must be provided for a connect.
    // you can create these from a passphrase and ssid with convertSSIDandPassPhraseToPSK()
    uint8_t     rgbKey[DEWF_PSK_LENGTH];            // must be filled in for connect
    
    // these are state variable used to do a cal PSK key
    // these are internal private variables that should not be tampered with while
    // the conversion is taking place.
    uint16_t        ssid_len;
	size_t          passphrase_len;
	size_t          left;
    uint32_t        count;      
    uint32_t        bigEndianCount;      
    int             indexIteration;
	unsigned char   digest[SHA1_MAC_LEN];
    unsigned char   curDigest[SHA1_MAC_LEN];
} WPA2KEY;
*/

typedef struct {
	uint32_t state[5];
	uint32_t count[2];
	unsigned char buffer[64];
} SHA1_CTX;

static void SHA1Init(SHA1_CTX *context);
static void SHA1Update(SHA1_CTX *context, const void *data, uint32_t len);
static void SHA1Final(unsigned char digest[20], SHA1_CTX* context);
static void SHA1Transform(uint32_t state[5], const unsigned char buffer[64]);

#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

/* blk0() and blk() perform the initial expand. */
/* I got the idea of expanding during the round function from SSLeay */
#if defined(LITTLE_ENDIAN)
#define blk0(i) (block->l[i] = (rol(block->l[i], 24) & 0xFF00FF00) | \
	(rol(block->l[i], 8) & 0x00FF00FF))
#else
#define blk0(i) block->l[i]
#endif
#define blk(i) (block->l[i & 15] = rol(block->l[(i + 13) & 15] ^ \
	block->l[(i + 8) & 15] ^ block->l[(i + 2) & 15] ^ block->l[i & 15], 1))

/* (R0+R1), R2, R3, R4 are the different operations used in SHA1 */
#define R0(v,w,x,y,z,i) \
	z += ((w & (x ^ y)) ^ y) + blk0(i) + 0x5A827999 + rol(v, 5); \
	w = rol(w, 30);
#define R1(v,w,x,y,z,i) \
	z += ((w & (x ^ y)) ^ y) + blk(i) + 0x5A827999 + rol(v, 5); \
	w = rol(w, 30);
#define R2(v,w,x,y,z,i) \
	z += (w ^ x ^ y) + blk(i) + 0x6ED9EBA1 + rol(v, 5); w = rol(w, 30);
#define R3(v,w,x,y,z,i) \
	z += (((w | x) & y) | (w & x)) + blk(i) + 0x8F1BBCDC + rol(v, 5); \
	w = rol(w, 30);
#define R4(v,w,x,y,z,i) \
	z += (w ^ x ^ y) + blk(i) + 0xCA62C1D6 + rol(v, 5); \
	w=rol(w, 30);

/* Hash a single 512-bit block. This is the core of the algorithm. */
static void SHA1Transform(uint32_t state[5], const unsigned char buffer[64])
{
	uint32_t a, b, c, d, e;
	typedef union {
		unsigned char c[64];
		uint32_t l[16];
	} CHAR64LONG16;
	CHAR64LONG16* block;

	uint32_t workspace[16];
	block = (CHAR64LONG16 *) workspace;
	memcpy((uint8_t*)block, (uint8_t*)buffer, 64);
	/* Copy context->state[] to working vars */
	a = state[0];
	b = state[1];
	c = state[2];
	d = state[3];
	e = state[4];
	/* 4 rounds of 20 operations each. Loop unrolled. */
	R0(a,b,c,d,e, 0); R0(e,a,b,c,d, 1); R0(d,e,a,b,c, 2); R0(c,d,e,a,b, 3);
	R0(b,c,d,e,a, 4); R0(a,b,c,d,e, 5); R0(e,a,b,c,d, 6); R0(d,e,a,b,c, 7);
	R0(c,d,e,a,b, 8); R0(b,c,d,e,a, 9); R0(a,b,c,d,e,10); R0(e,a,b,c,d,11);
	R0(d,e,a,b,c,12); R0(c,d,e,a,b,13); R0(b,c,d,e,a,14); R0(a,b,c,d,e,15);
	R1(e,a,b,c,d,16); R1(d,e,a,b,c,17); R1(c,d,e,a,b,18); R1(b,c,d,e,a,19);
	R2(a,b,c,d,e,20); R2(e,a,b,c,d,21); R2(d,e,a,b,c,22); R2(c,d,e,a,b,23);
	R2(b,c,d,e,a,24); R2(a,b,c,d,e,25); R2(e,a,b,c,d,26); R2(d,e,a,b,c,27);
	R2(c,d,e,a,b,28); R2(b,c,d,e,a,29); R2(a,b,c,d,e,30); R2(e,a,b,c,d,31);
	R2(d,e,a,b,c,32); R2(c,d,e,a,b,33); R2(b,c,d,e,a,34); R2(a,b,c,d,e,35);
	R2(e,a,b,c,d,36); R2(d,e,a,b,c,37); R2(c,d,e,a,b,38); R2(b,c,d,e,a,39);
	R3(a,b,c,d,e,40); R3(e,a,b,c,d,41); R3(d,e,a,b,c,42); R3(c,d,e,a,b,43);
	R3(b,c,d,e,a,44); R3(a,b,c,d,e,45); R3(e,a,b,c,d,46); R3(d,e,a,b,c,47);
	R3(c,d,e,a,b,48); R3(b,c,d,e,a,49); R3(a,b,c,d,e,50); R3(e,a,b,c,d,51);
	R3(d,e,a,b,c,52); R3(c,d,e,a,b,53); R3(b,c,d,e,a,54); R3(a,b,c,d,e,55);
	R3(e,a,b,c,d,56); R3(d,e,a,b,c,57); R3(c,d,e,a,b,58); R3(b,c,d,e,a,59);
	R4(a,b,c,d,e,60); R4(e,a,b,c,d,61); R4(d,e,a,b,c,62); R4(c,d,e,a,b,63);
	R4(b,c,d,e,a,64); R4(a,b,c,d,e,65); R4(e,a,b,c,d,66); R4(d,e,a,b,c,67);
	R4(c,d,e,a,b,68); R4(b,c,d,e,a,69); R4(a,b,c,d,e,70); R4(e,a,b,c,d,71);
	R4(d,e,a,b,c,72); R4(c,d,e,a,b,73); R4(b,c,d,e,a,74); R4(a,b,c,d,e,75);
	R4(e,a,b,c,d,76); R4(d,e,a,b,c,77); R4(c,d,e,a,b,78); R4(b,c,d,e,a,79);
	/* Add the working vars back into context.state[] */
	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	state[4] += e;
	/* Wipe variables */
	a = b = c = d = e = 0;

	memset((uint8_t*)block, 0, 64);
}

/* SHA1Init - Initialize new context */
static void SHA1Init(SHA1_CTX* context)
{
	/* SHA1 initialization constants */
	context->state[0] = 0x67452301;
	context->state[1] = 0xEFCDAB89;
	context->state[2] = 0x98BADCFE;
	context->state[3] = 0x10325476;
	context->state[4] = 0xC3D2E1F0;
	context->count[0] = context->count[1] = 0;
}

/* Run your data through this. */
static void SHA1Update(SHA1_CTX* context, const void *_data, uint32_t len)
{
	uint32_t i, j;
	const unsigned char *data = _data;

	j = (context->count[0] >> 3) & 63;
	if ((context->count[0] += len << 3) < (len << 3))
		context->count[1]++;
	context->count[1] += (len >> 29);
	if ((j + len) > 63) {
		memcpy((uint8_t*)(&context->buffer[j]), (uint8_t*)data, (i = 64-j));
		SHA1Transform(context->state, context->buffer);
		for ( ; i + 63 < len; i += 64) {
			SHA1Transform(context->state, &data[i]);
		}
		j = 0;
	}
	else i = 0;
	memcpy((uint8_t*)(&context->buffer[j]), (uint8_t*)(&data[i]), len - i);
}

/* Add padding and return the message digest. */
static void SHA1Final(unsigned char digest[20], SHA1_CTX* context)
{
	uint32_t i;
	unsigned char finalcount[8];

	for (i = 0; i < 8; i++) {
		finalcount[i] = (unsigned char)
			((context->count[(i >= 4 ? 0 : 1)] >>
			  ((3-(i & 3)) * 8) ) & 255);  /* Endian independent */
	}
	SHA1Update(context, (unsigned char *) "\200", 1);
	while ((context->count[0] & 504) != 448) {
		SHA1Update(context, (unsigned char *) "\0", 1);
	}
	SHA1Update(context, finalcount, 8);  /* Should cause a SHA1Transform()
					      */
	for (i = 0; i < 20; i++) {
		digest[i] = (unsigned char)
			((context->state[i >> 2] >> ((3 - (i & 3)) * 8)) &
			 255);
	}
	/* Wipe variables */
	i = 0;
	memset(context->buffer, 0, 64);
	memset((uint8_t*)(context->state), 0, 20);
	memset((uint8_t*)(context->count), 0, 8);
	memset(finalcount, 0, 8);
}

/**
 * sha1_vector - SHA-1 hash for data vector
 * @num_elem: Number of elements in the data vector
 * @addr: Pointers to the data areas
 * @len: Lengths of the data blocks
 * @mac: Buffer for the hash
 */
void sha1_vector(size_t num_elem, const uint8_t *addr[], const size_t *len,
		 uint8_t *mac)
{
	SHA1_CTX ctx;
	unsigned int i;

	SHA1Init(&ctx);
	for (i = 0; i < num_elem; i++)
		SHA1Update(&ctx, addr[i], len[i]);
	SHA1Final(mac, &ctx);
}

/**
 * hmac_sha1_vector - HMAC-SHA1 over data vector (RFC 2104)
 * @key: Key for HMAC operations
 * @key_len: Length of the key in bytes
 * @num_elem: Number of elements in the data vector
 * @addr: Pointers to the data areas
 * @len: Lengths of the data blocks
 * @mac: Buffer for the hash (20 bytes)
 */
void hmac_sha1_vector(const uint8_t *key, size_t key_len, size_t num_elem,
		      const uint8_t *addr[], const size_t *len, uint8_t *mac)
{
	unsigned char k_pad[64]; /* padding - key XORd with ipad/opad */
	unsigned char tk[20];
	unsigned int i;
	const uint8_t *_addr[6];
	size_t _len[6];

	if (num_elem > 5) {
		/*
		 * Fixed limit on the number of fragments to avoid having to
		 * allocate memory (which could fail).
		 */
		return;
	}

        /* if key is longer than 64 bytes reset it to key = SHA1(key) */
        if (key_len > 64) {
		sha1_vector(1, &key, &key_len, tk);
		key = tk;
		key_len = 20;
        }

	/* the HMAC_SHA1 transform looks like:
	 *
	 * SHA1(K XOR opad, SHA1(K XOR ipad, text))
	 *
	 * where K is an n byte key
	 * ipad is the byte 0x36 repeated 64 times
	 * opad is the byte 0x5c repeated 64 times
	 * and text is the data being protected */

	/* start out by storing key in ipad */
	memset(k_pad, 0, sizeof(k_pad));
	memcpy((uint8_t*)k_pad, (uint8_t*)key, key_len);
	/* XOR key with ipad values */
	for (i = 0; i < 64; i++)
		k_pad[i] ^= 0x36;

	/* perform inner SHA1 */
	_addr[0] = k_pad;
	_len[0] = 64;
	for (i = 0; i < num_elem; i++) {
		_addr[i + 1] = addr[i];
		_len[i + 1] = len[i];
	}
	sha1_vector(1 + num_elem, _addr, _len, mac);

	memset(k_pad, 0, sizeof(k_pad));
	memcpy((uint8_t*)k_pad, (uint8_t*)key, key_len);
	/* XOR key with opad values */
	for (i = 0; i < 64; i++)
		k_pad[i] ^= 0x5c;

	/* perform outer SHA1 */
	_addr[0] = k_pad;
	_len[0] = 64;
	_addr[1] = mac;
	_len[1] = SHA1_MAC_LEN;
	sha1_vector(2, _addr, _len, mac);
}

/**
 * hmac_sha1 - HMAC-SHA1 over data buffer (RFC 2104)
 * @key: Key for HMAC operations
 * @key_len: Length of the key in bytes
 * @data: Pointers to the data area
 * @data_len: Length of the data area
 * @mac: Buffer for the hash (20 bytes)
 */
void hmac_sha1(const uint8_t *key, size_t key_len, const uint8_t *data, size_t data_len,
	       uint8_t *mac)
{
	hmac_sha1_vector(key, key_len, 1, &data, &data_len, mac);
}

/**
 * szSSID - zero terminated ssid string
 * szPassPhase - zero terminated passphrase
 * pWpaKey, a WPA2KEY structure, this must have pWpaKey->count = 0 initialized before the first call
 * This is an iterative implemenation and WPA2KEY has internal state that must be maintained
 * so WPA2KEY must be maintained by the caller between calls. 
 * @passIterations: This is the only user changeable value. it must be an even number and will determine how 
 *                  many iterations will be executed on each call.
 */
bool wpa_conv_passphrase_to_key(char const * const szSSID, char const * const szPassPhrase, WPA2KEY * pWpaKey)
{
    const int       maxIterations = 4096;
    const int       passIterations = 4;  // must be an even number >= 2 <= maxIterations 4 => ~700usec/pass at 200MHz CPU
    const int       cbKey = 32;
    const uint8_t * addr[2] = {(uint8_t *) szSSID, (const uint8_t *) &pWpaKey->bigEndianCount};
    size_t          len[2] = {pWpaKey->ssid_len, 4};
    unsigned char   tmpA[SHA1_MAC_LEN];
    unsigned char * pCurDigest = pWpaKey->curDigest;
    unsigned char * pNextDigest = tmpA;

    if(pWpaKey->count == 0)
    {
        pWpaKey->ssid_len = strlen(szSSID);
        len[0] = pWpaKey->ssid_len;
        pWpaKey->passphrase_len = strlen(szPassPhrase);
        pWpaKey->left = cbKey;
        
        pWpaKey->count = 1;     
        pWpaKey->bigEndianCount = CONV_TO_BIG_ENDIAN(pWpaKey->count);

        hmac_sha1_vector((uint8_t *) szPassPhrase, pWpaKey->passphrase_len, 2, addr, len, pCurDigest);
        memcpy(pWpaKey->digest, pCurDigest, SHA1_MAC_LEN);
        
        pWpaKey->indexIteration = 1;
    }
                
	if(pWpaKey->left > 0) 
    {    
        int thisIteration = min(pWpaKey->indexIteration + passIterations, maxIterations);
        
        for (; pWpaKey->indexIteration < thisIteration; pWpaKey->indexIteration++) 
        {
            uint8_t* pTemp = pCurDigest;
            int     j;

            hmac_sha1(  (uint8_t *) szPassPhrase, pWpaKey->passphrase_len, pCurDigest, SHA1_MAC_LEN, pNextDigest); 

            for (j = 0; j < SHA1_MAC_LEN; j++)
                pWpaKey->digest[j] ^= pNextDigest[j];
            
            pCurDigest = pNextDigest;
            pNextDigest = pTemp;
        }

        // done with this iteration pass
        if(pWpaKey->indexIteration == maxIterations)
        {
            int iKey = (cbKey - pWpaKey->left);
            int cbKeyThisPass = pWpaKey->left > SHA1_MAC_LEN ? SHA1_MAC_LEN : pWpaKey->left;
            
            memcpy(pWpaKey->rgbKey + iKey, pWpaKey->digest, cbKeyThisPass);
            pWpaKey->left -= cbKeyThisPass;

            // only prepare for another hash if we have more to do
            if(pWpaKey->left > 0)
            {
                pWpaKey->count++; 
                pWpaKey->bigEndianCount = CONV_TO_BIG_ENDIAN(pWpaKey->count);

                pCurDigest = pWpaKey->curDigest;
                hmac_sha1_vector((uint8_t *) szPassPhrase, pWpaKey->passphrase_len, 2, addr, len, pCurDigest);
                memcpy(pWpaKey->digest, pCurDigest, SHA1_MAC_LEN);
                pWpaKey->indexIteration = 1;
            }

            // else we are done.
            else
            {
                pWpaKey->count = 0;
                return(true);
            }
        }
        
        // have to make sure the next value is in our persistent data structure
        else
        {
            ASSERT(pCurDigest == pWpaKey->curDigest);
        }
	}

    return(false);
}
