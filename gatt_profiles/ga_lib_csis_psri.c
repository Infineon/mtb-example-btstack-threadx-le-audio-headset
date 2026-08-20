/*
 * $ Copyright Cypress Semiconductor $
 */
/** @file
 *
 * Coordinated set identification Service implementation
 */

#include "ga_lib_csis.h"

#ifdef CSIS_DEBUG
#define CSIS_TRACE(...) WICED_BT_TRACE(__VA_ARGS__) /**< Enable this to get CSIS library traces */
#else
#define CSIS_TRACE(...)
#endif

#define CSIS_KEY_LEN 16
#define CSIS_R_LEN 3
#define CSIS_HASH_LEN 3
#define CSIS_PRAND_LEN 3
#define CSIS_PADDING_LEN 13
#define CSIS_HASH_VAL 16777216        /*2^24 */
#define CSIS_PRAND_MAX_RANDOM 4194304 /*2^22*/

// need wiced apis
extern wiced_bool_t AES_CMAC(BT_OCTET16 key, uint8_t *input, uint16_t length, uint16_t tlen, uint8_t *p_signature);

#define N_ROW 4
#define N_COL 4
#define N_BLOCK (N_ROW * N_COL)
#define N_MAX_ROUNDS 14

typedef struct
{
    uint8_t ksch[(N_MAX_ROUNDS + 1) * N_BLOCK];
    uint8_t rnd;
}aes_context_t;

extern wiced_result_t smp_aes_set_key(const unsigned char key[], uint8_t keylen, aes_context_t *p_ctx);
extern wiced_result_t smp_aes_encrypt(const unsigned char in[], unsigned char out[], aes_context_t *p_ctx);

#ifndef AES_ENC_128_OTFK
#define AES_ENC_128_OTFK 1
#endif
uint8_t hard_coded_r[] = { 0x69,0xf5,0x63 };

uint8_t ga_lib_csis_get_rand(void)
{
	// coverity[dont_call]
	return (rand() % 0xFF);
}

void ga_lib_csis_generate_prand(uint8_t *prand)
{
    prand[0] = ga_lib_csis_get_rand();
    prand[1] = ga_lib_csis_get_rand();
    prand[2] = ga_lib_csis_get_rand();
    prand[1] |= 1;    //At least one bit of the random part of prand shall be 1
    prand[2] &= ~(1); //At least one bit of the random part of prand shall be 0

    prand[0] |= (1<<6);        // Second Most Significant bit of prand should be 1
    prand[0] &= ~(1<<7);       // Most Significant bit of prand should be 0

    for (int i = 0; i < 3; i++)
        CSIS_TRACE("[%s] prand %x \n", __FUNCTION__, prand[i]);
}

void ga_lib_csis_generate_psri(const ga_lib_csis_sirk_t *sirk,  ga_lib_csis_psri_t *psri )
{
    uint32_t hash;
    uint8_t* p = NULL;
    uint8_t* p_hash = NULL;
    uint8_t prand[3];

    ga_lib_csis_generate_prand(prand);
    p = (uint8_t*)psri;
    p_hash = (uint8_t*)&hash;

    ga_lib_csis_calc_hash((uint8_t*)sirk, (uint8_t*)&prand, p_hash);
    REVERSE_ARRAY_TO_STREAM(p, p_hash, CSIS_HASH_LEN);
    REVERSE_ARRAY_TO_STREAM(p, prand, CSIS_PRAND_LEN);

    CSIS_TRACE("[%s] hash %x \n", __FUNCTION__, hash);
    CSIS_TRACE("[%s] prand %x \n", __FUNCTION__, prand);
    p = (uint8_t*)psri;
    for (int i=0 ;i <6 ;i++)
        CSIS_TRACE("[%s] psri %x \n", __FUNCTION__, p[i]);

    return ;
}

/*private_set_random_identifier_hash_function
* k is 128 bits
* r is 24 bits
* padding is 104 bits, all set to 0.
*/
void ga_lib_csis_calc_hash(const uint8_t* k, const uint8_t* r, uint8_t* h)
{
    uint8_t msg[CSIS_R_LEN + CSIS_PADDING_LEN];
    uint8_t*p = NULL;
    uint8_t cmac[16];
    uint8_t key[16];
    uint32_t hash;
    uint8_t* p_hash;

    memcpy(key, k, 16);

    /* calculating r' */
    p = msg;
    memset(p, 0, CSIS_PADDING_LEN);
    p = p + CSIS_PADDING_LEN;
    ARRAY_TO_STREAM(p, r, CSIS_R_LEN);

    aes_context_t ctx;
    smp_aes_set_key(key, 16, &ctx);
    smp_aes_encrypt(msg, cmac, &ctx);

    p_hash = (uint8_t*)&hash;
    *p_hash++ = cmac[13];
    *p_hash++ = cmac[14];
    *p_hash++ = cmac[15];

    hash = hash % CSIS_HASH_VAL;

    memcpy(h, &hash, 3);
    p_hash = (uint8_t*)&hash;

    for(int i=0; i<3; i++)
        CSIS_TRACE("[%s] hash: %x \n",__FUNCTION__, p_hash[i]);
}

//--------------------------- Security Toolbox --------------------------

static void invert(uint8_t *in, uint8_t *out, uint16_t len)
{
    uint16_t i;
    for (i = 0; i < len; i++) {
        out[i] = in[len - 1 - i];
    }
}

static void AES_CMAC_BE(unsigned char *key, unsigned char *input, int length, unsigned char *mac)
{
    uint8_t key_le[16];
    uint8_t input_le[150]; //max length is 145 - length of ConfirmationInputs on calcualtion of ConfirmationSalt
    uint8_t mac_le[16];
    invert(key, key_le, 16);
    invert(input, input_le, length);
    AES_CMAC(key_le, input_le, length, 16, mac_le);
    invert(mac_le, mac, 16);
}

/**
* SALT Generation Function.
* s1(M) = AES-CMAC_ZERO (M)
*
* Parameters:
*   m:          non-zero length ASCII encoded string
*   m_len:      Length of the m (>0)
*   out:        Buffer for result with length GA_LIB_CSIS_SET_IDENTITY_RESOLVING_KEY_LEN.
*
* Return:   None
*
*/
void ga_lib_csis_s1(char* m, uint32_t m_len, uint8_t* out)
{
    uint8_t zero[GA_LIB_CSIS_SET_IDENTITY_RESOLVING_KEY_LEN];
    memset(zero,0,sizeof(zero));
    AES_CMAC_BE(zero, (uint8_t*)m, m_len, out);

    int i = 0;
    for (i = 0; i < GA_LIB_CSIS_SET_IDENTITY_RESOLVING_KEY_LEN; i++)
        CSIS_TRACE("[%s] ____ encrypted res: after s1 %x \n", __FUNCTION__, out[i]);
}


/**
* same as ga_lib_csis_k1 but with possible shorter salt to be padded mwith 0.
*
* Parameters:
*   n:          Data for first AES_CMAC_BE (>=0 bytes).
*   n_len:      Length of the n (>=0)
*   salt:       Key for first AES_CMAC_BE (<= 16 bytes).
*   salt_len:   Length of the salt. Can be <= 16 bytes
*   p:          Data for second AES_CMAC_BE.
*   p_len:      Length of the p (>=0)
*   out:        Buffer for result with length GA_LIB_CSIS_SET_IDENTITY_RESOLVING_KEY_LEN.
*
* Return:   None
*
*/


void ga_lib_csis_k1(void* n, uint32_t n_len, uint8_t* salt, uint32_t salt_len, uint8_t* p, uint32_t p_len, uint8_t* out)
{

    uint8_t salt_zero_padded[GA_LIB_CSIS_SET_IDENTITY_RESOLVING_KEY_LEN];
    uint8_t t[GA_LIB_CSIS_SET_IDENTITY_RESOLVING_KEY_LEN];
    uint8_t ltk_rev[GA_LIB_CSIS_SET_IDENTITY_RESOLVING_KEY_LEN];
    int i = 0;



    if (salt_len < GA_LIB_CSIS_SET_IDENTITY_RESOLVING_KEY_LEN)
    {
        memcpy(salt_zero_padded, salt, salt_len);
        memset(&salt_zero_padded[salt_len], 0, GA_LIB_CSIS_SET_IDENTITY_RESOLVING_KEY_LEN - salt_len);
        salt = salt_zero_padded;
    }

    memset(t, 0, GA_LIB_CSIS_SET_IDENTITY_RESOLVING_KEY_LEN);
    memset(out, 0, GA_LIB_CSIS_SET_IDENTITY_RESOLVING_KEY_LEN);


    invert(n, ltk_rev, 16);
    CSIS_TRACE("LLTK [%A]", (uint8_t *)ltk_rev, 16);

    AES_CMAC_BE(salt, (uint8_t*)ltk_rev, n_len, t);

    for (i = 0; i < GA_LIB_CSIS_SET_IDENTITY_RESOLVING_KEY_LEN; i++)
        CSIS_TRACE("[%s] ____ encrypted res: after K1_1 %x \n", __FUNCTION__, t[i]);


    AES_CMAC_BE(t, (uint8_t*)p, p_len, out);
    for (i = 0; i < GA_LIB_CSIS_SET_IDENTITY_RESOLVING_KEY_LEN; i++)
        CSIS_TRACE("[%s] ____ encrypted res: after K1_2 %x \n", __FUNCTION__, out[i]);
}


void ga_lib_csis_sirk_encryption_func(ga_lib_csis_sirk_t* sirk_plain_text, wiced_bt_device_link_keys_t* link_keys, ga_lib_csis_sirk_t* sirk_encr)
{
    int i = 0;
    uint8_t* p_plain_sirk;
    uint8_t* p_encr_sirk;
    uint8_t s[GA_LIB_CSIS_SET_IDENTITY_RESOLVING_KEY_LEN];
    p_plain_sirk = (uint8_t *)sirk_plain_text;
    p_encr_sirk = (uint8_t *)sirk_encr;

    //sef(K, ga_lib_csis_sirk_t) = k1(K, s1(“SIRKenc”), “csis”) ^ ga_lib_csis_sirk_t

    //s1(“SIRKenc”)

    ga_lib_csis_s1("SIRKenc", 7, s);
    CSIS_TRACE("salt [%A]", s, GA_LIB_CSIS_SET_IDENTITY_RESOLVING_KEY_LEN);


    ga_lib_csis_k1(link_keys->key_data.le_keys.lltk,
               GA_LIB_CSIS_SET_IDENTITY_RESOLVING_KEY_LEN,
               s,
               GA_LIB_CSIS_SET_IDENTITY_RESOLVING_KEY_LEN,
               (uint8_t *)"csis",
               4,
               p_encr_sirk);
    CSIS_TRACE(" encrypted sirk before XOR [%A]", p_encr_sirk, 16);

    for (i = 0; i < GA_LIB_CSIS_SET_IDENTITY_RESOLVING_KEY_LEN; i++)
        p_encr_sirk[i] = p_encr_sirk[i] ^ p_plain_sirk[i];

    CSIS_TRACE(" encrypted sirk [%A]", p_encr_sirk, 16);
}

void ga_lib_csis_sirk_decryption_func(ga_lib_csis_sirk_t* sirk_enc, wiced_bt_device_link_keys_t* link_keys, ga_lib_csis_sirk_t* sirk_plain_text)
{
    CSIS_TRACE("[%s] \n", __FUNCTION__);
    uint8_t* p_plain_sirk;
    uint8_t* p_encr_sirk;
    uint8_t s[GA_LIB_CSIS_SET_IDENTITY_RESOLVING_KEY_LEN];
    uint8_t p[6];
    int i;

    p_plain_sirk = (uint8_t*)sirk_plain_text;
    p_encr_sirk = (uint8_t*)sirk_enc;

    ga_lib_csis_s1("SIRKenc", 7, s);
    WICED_MEMCPY(p, "csis", 4);


    CSIS_TRACE("LLTK [%A]", link_keys->key_data.le_keys.lltk, 16);
    ga_lib_csis_k1(link_keys->key_data.le_keys.lltk, GA_LIB_CSIS_SET_IDENTITY_RESOLVING_KEY_LEN, s, GA_LIB_CSIS_SET_IDENTITY_RESOLVING_KEY_LEN, p, 4, p_plain_sirk);

    CSIS_TRACE(" plain sirk before XOR [%A]", p_plain_sirk, GA_LIB_CSIS_SET_IDENTITY_RESOLVING_KEY_LEN);

    // PlainSIRK = K1 ^ EncrSIRK
    for (i = 0; i < GA_LIB_CSIS_SET_IDENTITY_RESOLVING_KEY_LEN; i++)
        p_plain_sirk[i] = p_plain_sirk[i] ^ p_encr_sirk[i];

    CSIS_TRACE(" plain sirk [%A]", p_plain_sirk, GA_LIB_CSIS_SET_IDENTITY_RESOLVING_KEY_LEN);
}

wiced_bool_t ga_lib_csis_check_if_belongs_to_coordinated_set(const wiced_ble_ext_scan_results_t *p_adv_report,
                                                          uint16_t adv_len,
                                                          const uint8_t *p_adv_data,
                                                          const ga_lib_csis_sirk_t *sirk)
{
    if (!p_adv_report)
        return WICED_FALSE;

    uint16_t length;
    uint32_t rec_prand;
    uint32_t rec_hash;
    uint32_t local_hash = 0;
    uint8_t adv_data;
    uint8_t curr_len = 0;

    length = adv_len;
    uint8_t *p_received_prand = (uint8_t *)&rec_prand;
    uint8_t *p_received_hash = (uint8_t *)&rec_hash;
    uint8_t *p_local_hash = (uint8_t *)&local_hash;

    CSIS_TRACE("my sirk %A", sirk, 16);
    while (length > 2)
    {
        curr_len = *p_adv_data++;
        adv_data = *p_adv_data++;
        switch (adv_data)
        {
        case BTM_BLE_ADVERT_TYPE_PSRI:
        {
            REVERSE_STREAM_TO_ARRAY(p_received_hash, p_adv_data, CSIS_HASH_LEN);
            REVERSE_STREAM_TO_ARRAY(p_received_prand, p_adv_data, CSIS_PRAND_LEN);
            ga_lib_csis_calc_hash((uint8_t *)sirk, p_received_prand, p_local_hash);
            CSIS_TRACE("received prand", p_received_prand, CSIS_PRAND_LEN);
            CSIS_TRACE("my hash", p_local_hash, CSIS_HASH_LEN);
            CSIS_TRACE("received hash", p_received_hash, CSIS_HASH_LEN);

            if (WICED_MEMCMP(p_local_hash, p_received_hash, CSIS_HASH_LEN) == 0)
                return WICED_TRUE;
            else
                return WICED_FALSE;
        }
        break;
        default:
        {
            p_adv_data += curr_len - 1;
            length -= curr_len + 1;
        }
        }
    }
    return WICED_FALSE;
}
