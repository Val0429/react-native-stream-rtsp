
#include <jni.h>
#include <android/log.h>
#include <string>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <stdint.h>
#include <assert.h>
#include <openssl/blowfish.h>

#ifdef __cplusplus
extern "C" {
#endif


#include "openssl/aes.h"
#include "openssl/rand.h"
#include "openssl/hmac.h"
#include "openssl/buffer.h"

#define  LOG_TAG "DLinkCrypto"
#define  LOGI(...)   __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)   __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)   __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)

#define JNIREG_CLASS "com/isap/crypto/DLinkCrypto"

static BF_KEY bf_key;
static const std::string Default_Key = "7tGYX@t3pXty!z8BsMWf3TnweTeuzME#EG(kp2x5xpU5nHUW-Kb@FTE9Z6hbNdu7";
static const unsigned char ivec[] = "24741158";
static const size_t ivec_size = sizeof(ivec);
static const int ci_bf_ivec_pos = 0;

unsigned char* encodebase64(const unsigned char *input, int length, size_t &size)
{
    BIO *bmem, *b64;
    BUF_MEM *bptr;

    b64 = BIO_new(BIO_f_base64());
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(b64, input, length);
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bptr);
    size = bptr->length;
    /// char *buff = (char *)malloc(size + 1);
    char *buff = new char[size + 1];
    memcpy(buff, bptr->data, size);
    buff[size] = 0;
    BIO_free_all(b64);
    return (unsigned char*)buff;
}

size_t calcDecodeLength(const char* b64input)
{   //Calculates the length of a decoded string
    size_t len = strlen(b64input),
            padding = 0;

    if (b64input[len - 1] == '=' && b64input[len - 2] == '=') //last two chars are =
        padding = 2;
    else if (b64input[len - 1] == '=') //last char is =
        padding = 1;

    return (len * 3) / 4 - padding;
}

unsigned char* decodebase64(unsigned char *input, int length, size_t &out_length)
{
    BIO *b64, *bmem;
    out_length = calcDecodeLength((char *)input);
    unsigned int base64_size = out_length + 1;
    char *buffer = new char[base64_size];
    memset(buffer, 0, base64_size);

    b64 = BIO_new(BIO_f_base64());
    bmem = BIO_new_mem_buf(input, length);
    bmem = BIO_push(b64, bmem);
    BIO_set_flags(bmem, BIO_FLAGS_BASE64_NO_NL);
    BIO_read(bmem, buffer, length);
    BIO_free_all(bmem);

    return (unsigned char*)buffer;
}

void encode(const unsigned char* In_strOriginal, unsigned char*& InOut_strEncode, const size_t In_StringSize, size_t &Out_OutputSize, bool bUseBase64)
{
    BF_set_key(&bf_key, strlen(Default_Key.c_str()), (const unsigned char*)Default_Key.c_str());

    if (InOut_strEncode != NULL)
        delete[] InOut_strEncode;

    int bf_ivec_pos = ci_bf_ivec_pos;
    unsigned char bf_ivec[ivec_size];
    memcpy(bf_ivec, ivec, ivec_size);

    if (bUseBase64)
    {
        unsigned char *bf_crypt = new unsigned char[In_StringSize];
        BF_cfb64_encrypt((const unsigned char*)In_strOriginal, bf_crypt, In_StringSize, &bf_key, bf_ivec, &bf_ivec_pos, BF_ENCRYPT);
        InOut_strEncode = encodebase64(bf_crypt, In_StringSize, Out_OutputSize);
        delete[] bf_crypt;
    }
    else
    {
        Out_OutputSize = In_StringSize;
        InOut_strEncode = new unsigned char[Out_OutputSize];
        BF_cfb64_encrypt((const unsigned char*)In_strOriginal, InOut_strEncode, Out_OutputSize, &bf_key, bf_ivec, &bf_ivec_pos, BF_ENCRYPT);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#define MAX_INPUT_LENGTH 64
#define MAX_KEY_LENGTH 32
#define MAX_BLOCK_LENGTH 16

#define BYTE unsigned char

static void AES_SubBytes(BYTE state[], BYTE sbox[]);

static void AES_AddRoundKey(BYTE state[], BYTE rkey[]);

static void AES_ShiftRows(BYTE state[], BYTE shifttab[]);

static void AES_MixColumns(BYTE state[]);

static void AES_MixColumns_Inv(BYTE state[]);

// AES_Init: initialize the tables needed at runtime.
// Call this function before the (first) key expansion.
static void AES_Init();

// AES_Done: release memory reserved by AES_Init.
// Call this function after the last encryption/decryption operation.
static void AES_Done() {}

/* AES_ExpandKey: expand a cipher key. Depending on the desired encryption
   strength of 128, 192 or 256 bits 'key' has to be a byte array of length
   16, 24 or 32, respectively. The key expansion is done "in place", meaning
   that the array 'key' is modified.
*/
static int AES_ExpandKey(BYTE key[], int keyLen);

// AES_Encrypt: encrypt the 16 byte array 'block' with the previously expanded key 'key'.
static void AES_Encrypt(BYTE block[], BYTE key[], int keyLen);

// AES_Decrypt: decrypt the 16 byte array 'block' with the previously expanded key 'key'.
static void AES_Decrypt(BYTE block[], BYTE key[], int keyLen);


static void print_bytes(char *str,unsigned char *input, int len);

static unsigned int hexstr2array (char *input, BYTE *output);

static unsigned int ary2hexstring (BYTE *input, char *output, unsigned int bytelength);


static unsigned char encrypt(BYTE *key_original, unsigned int key_length, BYTE *input_original, unsigned int input_length, BYTE *output);

static unsigned char decrypt(BYTE *key_original, unsigned int key_length, BYTE *input_original, unsigned int input_length, BYTE *output);

static void generate_random_array (BYTE * ary, unsigned int length);

int Media_AES_Encrypt(uint8_t* key, uint32_t keyLen, uint8_t* input, uint32_t inputSz, uint8_t* outBuff, uint32_t outBuffSZ);

int Media_AES_Decrypt(uint8_t* key, uint32_t keyLen, uint8_t* input, uint32_t inputSz, uint8_t* outBuff, uint32_t outBuffSZ);


int init_ctr(struct ctr_state *state, const unsigned char iv[16]);

void fencrypt(char* read, char* write, const unsigned char* enc_key);

void fdecrypt(char* read, char* write, const unsigned char* enc_key);

int AESCTR256_Encrypt(char* enc, int enclen, char* denc_buf, int denc_bufsz, const unsigned char* enc_key, const unsigned char* ivector);

int AESCTR256_Decrypt(char* ciph, int ciphlen, char* pain_buf, int pain_bufsz, const unsigned char* enc_key, const unsigned char* ivector);




// The following lookup tables and functions are for internal use only!
static BYTE AES_Sbox[] = {99,124,119,123,242,107,111,197,48,1,103,43,254,215,171,
                          118,202,130,201,125,250,89,71,240,173,212,162,175,156,164,114,192,183,253,
                          147,38,54,63,247,204,52,165,229,241,113,216,49,21,4,199,35,195,24,150,5,154,
                          7,18,128,226,235,39,178,117,9,131,44,26,27,110,90,160,82,59,214,179,41,227,
                          47,132,83,209,0,237,32,252,177,91,106,203,190,57,74,76,88,207,208,239,170,
                          251,67,77,51,133,69,249,2,127,80,60,159,168,81,163,64,143,146,157,56,245,
                          188,182,218,33,16,255,243,210,205,12,19,236,95,151,68,23,196,167,126,61,
                          100,93,25,115,96,129,79,220,34,42,144,136,70,238,184,20,222,94,11,219,224,
                          50,58,10,73,6,36,92,194,211,172,98,145,149,228,121,231,200,55,109,141,213,
                          78,169,108,86,244,234,101,122,174,8,186,120,37,46,28,166,180,198,232,221,
                          116,31,75,189,139,138,112,62,181,102,72,3,246,14,97,53,87,185,134,193,29,
                          158,225,248,152,17,105,217,142,148,155,30,135,233,206,85,40,223,140,161,
                          137,13,191,230,66,104,65,153,45,15,176,84,187,22};

static BYTE AES_ShiftRowTab[] = {0,5,10,15,4,9,14,3,8,13,2,7,12,1,6,11};

static BYTE AES_Sbox_Inv[256];
static BYTE AES_ShiftRowTab_Inv[16];
static BYTE AES_xtime[256];

static void AES_SubBytes(BYTE state[], BYTE sbox[]) {
    int i;
    for(i = 0; i < 16; i++)
        state[i] = sbox[state[i]];
}

static void AES_AddRoundKey(BYTE state[], BYTE rkey[]) {
    int i;
    for(i = 0; i < 16; i++)
        state[i] ^= rkey[i];
}

static void AES_ShiftRows(BYTE state[], BYTE shifttab[]) {
    BYTE h[16];
    memcpy(h, state, 16);
    int i;
    for(i = 0; i < 16; i++)
        state[i] = h[shifttab[i]];
}

static void AES_MixColumns(BYTE state[]) {
    int i;
    for(i = 0; i < 16; i += 4) {
        BYTE s0 = state[i + 0], s1 = state[i + 1];
        BYTE s2 = state[i + 2], s3 = state[i + 3];
        BYTE h = s0 ^ s1 ^ s2 ^ s3;
        state[i + 0] ^= h ^ AES_xtime[s0 ^ s1];
        state[i + 1] ^= h ^ AES_xtime[s1 ^ s2];
        state[i + 2] ^= h ^ AES_xtime[s2 ^ s3];
        state[i + 3] ^= h ^ AES_xtime[s3 ^ s0];
    }
}

static void AES_MixColumns_Inv(BYTE state[]) {
    int i;
    for(i = 0; i < 16; i += 4) {
        BYTE s0 = state[i + 0], s1 = state[i + 1];
        BYTE s2 = state[i + 2], s3 = state[i + 3];
        BYTE h = s0 ^ s1 ^ s2 ^ s3;
        BYTE xh = AES_xtime[h];
        BYTE h1 = AES_xtime[AES_xtime[xh ^ s0 ^ s2]] ^ h;
        BYTE h2 = AES_xtime[AES_xtime[xh ^ s1 ^ s3]] ^ h;
        state[i + 0] ^= h1 ^ AES_xtime[s0 ^ s1];
        state[i + 1] ^= h2 ^ AES_xtime[s1 ^ s2];
        state[i + 2] ^= h1 ^ AES_xtime[s2 ^ s3];
        state[i + 3] ^= h2 ^ AES_xtime[s3 ^ s0];
    }
}

// AES_Init: initialize the tables needed at runtime.
// Call this function before the (first) key expansion.
static void AES_Init() {
    int i;
    for(i = 0; i < 256; i++)
        AES_Sbox_Inv[AES_Sbox[i]] = i;

    for(i = 0; i < 16; i++)
        AES_ShiftRowTab_Inv[AES_ShiftRowTab[i]] = i;

    for(i = 0; i < 128; i++) {
        AES_xtime[i] = i << 1;
        AES_xtime[128 + i] = (i << 1) ^ 0x1b;
    }
}

// AES_Done: release memory reserved by AES_Init.
// Call this function after the last encryption/decryption operation.
//static void AES_Done() {}

/* AES_ExpandKey: expand a cipher key. Depending on the desired encryption
   strength of 128, 192 or 256 bits 'key' has to be a byte array of length
   16, 24 or 32, respectively. The key expansion is done "in place", meaning
   that the array 'key' is modified.
*/
static int AES_ExpandKey(BYTE key[], int keyLen) {
    int kl = keyLen, ks, Rcon = 1, i, j;
    BYTE temp[4], temp2[4];
    switch (kl) {
        case 16: ks = 16 * (10 + 1); break;
        case 24: ks = 16 * (12 + 1); break;
        case 32: ks = 16 * (14 + 1); break;
        default:
            printf("AES_ExpandKey: Only key lengths of 16, 24 or 32 bytes allowed!");
    }
    for(i = kl; i < ks; i += 4) {
        memcpy(temp, &key[i-4], 4);
        if (i % kl == 0) {
            temp2[0] = AES_Sbox[temp[1]] ^ Rcon;
            temp2[1] = AES_Sbox[temp[2]];
            temp2[2] = AES_Sbox[temp[3]];
            temp2[3] = AES_Sbox[temp[0]];
            memcpy(temp, temp2, 4);
            if ((Rcon <<= 1) >= 256)
                Rcon ^= 0x11b;
        }
        else if ((kl > 24) && (i % kl == 16)) {
            temp2[0] = AES_Sbox[temp[0]];
            temp2[1] = AES_Sbox[temp[1]];
            temp2[2] = AES_Sbox[temp[2]];
            temp2[3] = AES_Sbox[temp[3]];
            memcpy(temp, temp2, 4);
        }
        for(j = 0; j < 4; j++)
            key[i + j] = key[i + j - kl] ^ temp[j];
    }
    return ks;
}

// AES_Encrypt: encrypt the 16 byte array 'block' with the previously expanded key 'key'.
static void AES_Encrypt(BYTE block[], BYTE key[], int keyLen) {
    int l = keyLen, i;
//  printBytes(block, 16);
    AES_AddRoundKey(block, &key[0]);
    for(i = 16; i < l - 16; i += 16) {
        AES_SubBytes(block, AES_Sbox);
        AES_ShiftRows(block, AES_ShiftRowTab);
        AES_MixColumns(block);
        AES_AddRoundKey(block, &key[i]);
    }
    AES_SubBytes(block, AES_Sbox);
    AES_ShiftRows(block, AES_ShiftRowTab);
    AES_AddRoundKey(block, &key[i]);
}

// AES_Decrypt: decrypt the 16 byte array 'block' with the previously expanded key 'key'.
static void AES_Decrypt(BYTE block[], BYTE key[], int keyLen) {
    int l = keyLen, i;
    AES_AddRoundKey(block, &key[l - 16]);
    AES_ShiftRows(block, AES_ShiftRowTab_Inv);
    AES_SubBytes(block, AES_Sbox_Inv);
    for(i = l - 32; i >= 16; i -= 16) {
        AES_AddRoundKey(block, &key[i]);
        AES_MixColumns_Inv(block);
        AES_ShiftRows(block, AES_ShiftRowTab_Inv);
        AES_SubBytes(block, AES_Sbox_Inv);
    }
    AES_AddRoundKey(block, &key[0]);
}


static void print_bytes(char *str,unsigned char *input, int len)
{
    int i=0;
    printf ("%s\n",str);
    for (i=0;i<len;i++)
    {
//		if (i%2 == 0) printf (" ");
        printf ("%02x",input[i]);
    }
    printf ("\n");
    return ;
}

static unsigned int hexstr2array (char *input, int input_size, BYTE *output)
{
    unsigned int length = input_size;
    unsigned int i=0;
    char tmp[3];

    if ((length % 2) != 0)
    {
        fprintf (stderr, "[ERROR] %s:%u: input length error:%u\n", __FILE__,__LINE__,length);
        return 0;
    }

    for (i=0;i<length;i++)
    {
        char c = input[i];
        if (c < '0' ||
            ((c > '9') && (c < 'A')) ||
            ((c > 'F') && (c < 'a')) ||
            (c > 'f'))
        {
            fprintf (stderr, "[ERROR] %s:%u: input string error: %s\n", __FILE__,__LINE__, input);
            return 0;
        }
    }

    for (i=0;i<length;i+=2)
    {
        tmp[0] = input[i];
        tmp[1] = input[i+1];
        tmp[2] = '\0';
        output[i/2] = strtol (tmp, NULL, 16);
    }

    return length/2;
}

static unsigned int ary2hexstring (BYTE *input, char *output, unsigned int bytelength)
{
    unsigned int i=0;
    char tmp[3];
    output[0] = '\0';
    static char hex_list[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
    for (i=0;i<bytelength;i++)
    {
        //sprintf (tmp, "%02x", input[i]); /* need to increase speed */
        unsigned char c = input[i];
        tmp[0] = hex_list[(c >>4)];
        tmp[1] = hex_list[(c & 0xf)];
        tmp[2] = '\0';

        strcat(output,tmp);
    }
    return 0;
}


static unsigned char encrypt(BYTE *key_original, unsigned int key_length, BYTE *input_original, unsigned int input_length, BYTE *output)
{
    BYTE block[MAX_INPUT_LENGTH] = {0};
    BYTE key[MAX_KEY_LENGTH] = {0};

    int i;
    memcpy((void *)key, (void *)key_original, key_length);
    memcpy((void *)block, (void *)input_original, input_length);
    AES_Init();

    for (i=0;i<MAX_INPUT_LENGTH/MAX_BLOCK_LENGTH;i++)
    {
        AES_Encrypt(block+i*MAX_BLOCK_LENGTH, key, MAX_KEY_LENGTH);
    }

    memcpy(output, block, MAX_INPUT_LENGTH);

    AES_Done();
    return 1;
}

static unsigned char decrypt(BYTE *key_original, unsigned int key_length, BYTE *input_original, unsigned int input_length, BYTE *output)
{
    BYTE block[MAX_INPUT_LENGTH] = {0};
    BYTE key[MAX_KEY_LENGTH] = {0};

    int i;

    memcpy((void *)key, (void *)key_original, key_length);
    memcpy((void *)block, (void *)input_original, input_length);
    AES_Init();

    for (i=0;i<MAX_INPUT_LENGTH/MAX_BLOCK_LENGTH;i++)
    {
        AES_Decrypt(block+i*MAX_BLOCK_LENGTH, key, MAX_KEY_LENGTH);
    }

    memcpy(output, block, MAX_INPUT_LENGTH);

    AES_Done();
    return 1;
}

static void generate_random_array (BYTE * ary, unsigned int length)
{
    unsigned int i = 0;

    for (i=0;i<length;i++)
    {
        ary[i] = rand() & 0xff;
    }
    return ;
}

int Media_AES_Encrypt(uint8_t* key, uint32_t keyLen, uint8_t* input, uint32_t inputSz, uint8_t* outBuff, uint32_t outBuffSZ)
{
    uint8_t oHex[MAX_INPUT_LENGTH*2+1] = {0};

    if (outBuffSZ < MAX_INPUT_LENGTH*2+1)
        return -1;

    encrypt(key, keyLen, input, inputSz, outBuff);

    ary2hexstring(outBuff, (char*)oHex, MAX_INPUT_LENGTH);
    strncpy((char*)outBuff, (char*)oHex, (MAX_INPUT_LENGTH*2+1));

    return 0;
}

int Media_AES_Decrypt(uint8_t* key, uint32_t keyLen, uint8_t* input, uint32_t inputSz, uint8_t* outBuff, uint32_t outBuffSZ)
{
    if (outBuffSZ < MAX_INPUT_LENGTH*2+1)
        return -1;

    decrypt(key, keyLen, input, inputSz, outBuff);

    return 0;
}


/***************************************/
/****** Openssl base AES 256 ******/
/***************************************/
/***************************************/

// Code example uses partail code from: http://stackoverflow.com/questions/3141860/aes-ctr-256-encryption-mode-of-operation-on-openssl
// Mostly in the ctr_ state, and init_ctr functions.

struct ctr_state
{
    unsigned char ivec[AES_BLOCK_SIZE];
    unsigned int num;
    unsigned char ecount[AES_BLOCK_SIZE];
};

FILE *readFile;
FILE *writeFile;
AES_KEY key;

int bytes_read, bytes_written;
unsigned char indata[AES_BLOCK_SIZE];
unsigned char outdata[AES_BLOCK_SIZE];
unsigned char iv[AES_BLOCK_SIZE];
struct ctr_state state;

int init_ctr(struct ctr_state *state, const unsigned char iv[16])
{
    /* aes_ctr128_encrypt requires 'num' and 'ecount' set to zero on the
     * first call. */
    state->num = 0;
    memset(state->ecount, 0, AES_BLOCK_SIZE);

    memcpy(state->ivec, iv, 16);
}

void fencrypt(char* read, char* write, const unsigned char* enc_key)
{
    if(!RAND_bytes(iv, AES_BLOCK_SIZE))
    {
        fprintf(stderr, "Could not create random bytes.");
        exit(1);
    }

    readFile = fopen(read,"rb"); // The b is required in windows.
    writeFile = fopen(write,"wb");

    if(readFile==NULL)
    {
        fprintf(stderr, "Read file is null.");
        exit(1);
    }

    if(writeFile==NULL)
    {
        fprintf(stderr, "Write file is null.");
        exit(1);
    }

    fwrite(iv, 1, 8, writeFile); // IV bytes 1 - 8
    fwrite("\0\0\0\0\0\0\0\0", 1, 8, writeFile); // Fill the last 4 with null bytes 9 - 16

    //Initializing the encryption KEY
//[]	if (AES_set_encrypt_key(enc_key, 128, &key) < 0)
    if (AES_set_encrypt_key(enc_key, 256, &key) < 0)
    {
        fprintf(stderr, "Could not set encryption key.");
        exit(1);
    }

    init_ctr(&state, iv); //Counter call
    //Encrypting Blocks of 16 bytes and writing the output.txt with ciphertext

    while(1)
    {
        bytes_read = fread(indata, 1, AES_BLOCK_SIZE, readFile);
        AES_ctr128_encrypt(indata, outdata, bytes_read, &key, state.ivec, state.ecount, &state.num);

        bytes_written = fwrite(outdata, 1, bytes_read, writeFile);
        if (bytes_read < AES_BLOCK_SIZE)
        {
            break;
        }
    }

    fclose(writeFile);
    fclose(readFile);
}

void fdecrypt(char* read, char* write, const unsigned char* enc_key)
{

    readFile=fopen(read,"rb"); // The b is required in windows.
    writeFile=fopen(write,"wb");

    if(readFile==NULL)
    {
        fprintf(stderr,"Read file is null.");
        exit(1);
    }

    if(writeFile==NULL)
    {
        fprintf(stderr, "Write file is null.");
        exit(1);
    }

    fread(iv, 1, AES_BLOCK_SIZE, readFile);

    //Initializing the encryption KEY
    if (AES_set_encrypt_key(enc_key, 256, &key) < 0)
    {
        fprintf(stderr, "Could not set decryption key.");
        exit(1);
    }

    init_ctr(&state, iv);//Counter call
    //Encrypting Blocks of 16 bytes and writing the output.txt with ciphertext
    while(1)
    {
        bytes_read = fread(indata, 1, AES_BLOCK_SIZE, readFile);
        //printf("%i\n", state.num);
        AES_ctr128_encrypt(indata, outdata, bytes_read, &key, state.ivec, state.ecount, &state.num);

        bytes_written = fwrite(outdata, 1, bytes_read, writeFile);
        if (bytes_read < AES_BLOCK_SIZE)
        {
            break;
        }
    }
    fclose(writeFile);
    fclose(readFile);
}

#define ZERO_IVEC	(1)

int AESCTR256_Encrypt(char* enc, int enclen, char* denc_buf, int denc_bufsz, const unsigned char* enc_key, const unsigned char* ivector)
{
    unsigned char ivec[AES_BLOCK_SIZE];
    unsigned char src_data[AES_BLOCK_SIZE];
    unsigned char des_data[AES_BLOCK_SIZE];
    AES_KEY akey;
    struct ctr_state counter_state;
    int wr_offset = 0;
    int rd_offset = 0;
    int rbytes = 0;
    int eps;

    if (enc == NULL)
    {
        fprintf(stderr, "Encrypt source is null.\n");
        goto AESCTR256_ENCRYPT_ERROR;
    }

    if (denc_buf == NULL)
    {
        fprintf(stderr, "Decrypt buffer is null.\n");
        goto AESCTR256_ENCRYPT_ERROR;
    }

    if (denc_bufsz < 16)
    {
        fprintf(stderr, "Decrypt buffer is not enough. [%d]\n", denc_bufsz);
        goto AESCTR256_ENCRYPT_ERROR;
    }

    memcpy(ivec, ivector, AES_BLOCK_SIZE);

    //Initializing the encryption KEY
    if (AES_set_encrypt_key(enc_key, 256, &akey) < 0)
    {
        fprintf(stderr, "Could not set encryption key.");
        goto AESCTR256_ENCRYPT_ERROR;
    }

    init_ctr(&counter_state, ivec); //Counter call

    //Encrypting Blocks of 16 bytes
    while(rd_offset < enclen)
    {
        eps = ((enclen-rd_offset) < AES_BLOCK_SIZE)? (enclen-rd_offset) : (AES_BLOCK_SIZE);

        memcpy(src_data, enc+rd_offset, eps);
        rd_offset += eps;

        AES_ctr128_encrypt(src_data, des_data, eps, &akey, counter_state.ivec, counter_state.ecount, &counter_state.num);

        if ((denc_bufsz-wr_offset) < eps)
        {
            fprintf(stderr, "Decrypt buffer is not enough. [%d]\n", denc_bufsz);
            goto AESCTR256_ENCRYPT_ERROR;
        }

        memcpy(denc_buf+wr_offset, des_data, eps);
        wr_offset += eps;
    }

    return wr_offset;

    AESCTR256_ENCRYPT_ERROR:

    return -1;

}

int AESCTR256_Decrypt(char* ciph, int ciphlen, char* pain_buf, int pain_bufsz, const unsigned char* enc_key, const unsigned char* ivector)
{
    unsigned char ivec[AES_BLOCK_SIZE];
    unsigned char src_data[AES_BLOCK_SIZE];
    unsigned char des_data[AES_BLOCK_SIZE];
    AES_KEY akey;
    struct ctr_state counter_state;
    int wr_offset = 0;
    int rd_offset = 0;
    int rbytes = 0;
    int dps;

    if (ciph == NULL)
    {
        fprintf(stderr, "Cipher input is null.\n");
        goto AESCTR256_DECRYPT_ERROR;
    }

    if (pain_buf == NULL)
    {
        fprintf(stderr, "Paintext buffer is null.\n");
        goto AESCTR256_DECRYPT_ERROR;
    }

    if (ciphlen < 16)
    {
        fprintf(stderr, "Cipher input is too small.\n");
        goto AESCTR256_DECRYPT_ERROR;
    }

    memcpy(ivec, ivector, AES_BLOCK_SIZE);

    //Initializing the encryption KEY
    if (AES_set_encrypt_key(enc_key, 256, &akey) < 0)
    {
        fprintf(stderr, "Could not set decryption key.");
        goto AESCTR256_DECRYPT_ERROR;
    }

    init_ctr(&counter_state, ivec);//Counter call
    //Encrypting Blocks of 16 bytes
    while(rd_offset < ciphlen)
    {
        dps = ((ciphlen-rd_offset) < AES_BLOCK_SIZE)? (ciphlen-rd_offset) : (AES_BLOCK_SIZE);

        memcpy(src_data, ciph+rd_offset, dps);
        rd_offset += dps;

        //[]  printf("%i\n", counter_state.num);
        AES_ctr128_encrypt(src_data, des_data, dps, &akey, counter_state.ivec, counter_state.ecount, &counter_state.num);

        if ((pain_bufsz-wr_offset) < dps)
        {
            fprintf(stderr, "Paintext buffer is not enough. [%d]\n", pain_bufsz);
            goto AESCTR256_DECRYPT_ERROR;
        }

        memcpy(pain_buf+wr_offset, des_data, dps);
        wr_offset += dps;
    }

    return wr_offset;

    AESCTR256_DECRYPT_ERROR:

    return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT int JNICALL native_GetOrbwebSID( JNIEnv* env, jclass clazz, jbyteArray token, jint token_size, jbyteArray sid)
{
    jbyte* tokenArr = env->GetByteArrayElements(token, NULL);
    jbyte* sidArr = env->GetByteArrayElements(sid, NULL);

    int sid_size = 0;

    unsigned char *InOut_strEncode = NULL;
    size_t Out_OutputSize = 0;
    const unsigned char *In_strOriginal = (unsigned char *) tokenArr;

    encode(In_strOriginal, InOut_strEncode, token_size, Out_OutputSize, true);
    sid_size = Out_OutputSize;

    memcpy(sidArr, InOut_strEncode, sid_size);
    delete[] InOut_strEncode;

    env->ReleaseByteArrayElements(sid, sidArr, 0);
    env->ReleaseByteArrayElements(token, tokenArr, 0);

    return sid_size;
}

JNIEXPORT int JNICALL native_EncryptNVRPwd( JNIEnv* env,
                                            jclass clazz,
                                            jbyteArray primaryKey,
                                            jint  pk_size,
                                            jbyteArray input,
                                            jint input_size,
                                            jbyteArray output)
{
    jbyte* keyArr = env->GetByteArrayElements(primaryKey, NULL);
    jbyte* inputArr = env->GetByteArrayElements(input, NULL);
    jbyte* outputArr = env->GetByteArrayElements(output, NULL);

    unsigned int key_size = pk_size/2;
    BYTE key_byte[key_size];
    hexstr2array((char *) keyArr, pk_size, key_byte);

    LOGE("%02x %02x %02x %02x %02x", key_byte[0], key_byte[1], key_byte[2], key_byte[3], key_byte[4]);

    BYTE *input_byte = (BYTE *) inputArr;

    BYTE output_byte[128];
    memset(output_byte, 0, 128);

    encrypt(key_byte, key_size, input_byte, input_size, output_byte);

    char output_str[128+1];
    memset(output_str, 0, 128);
    ary2hexstring(output_byte, output_str, 128);

    memcpy(outputArr, output_str, 128);

    env->ReleaseByteArrayElements(output, outputArr, 0);
    env->ReleaseByteArrayElements(input, inputArr, 0);
    env->ReleaseByteArrayElements(primaryKey, keyArr, 0);

    return 128;
}

static JNINativeMethod gMethods[] = {
        {	"GetOrbwebSID",     "([BI[B)I",	        (void*)native_GetOrbwebSID	},
        {	"EncryptNVRPwd",    "([BI[BI[B)I",      (void*)native_EncryptNVRPwd	},
};

static int registerNativeMethods(JNIEnv* env, const char* className,
                                 JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz;
    clazz = env->FindClass(className);
    if (clazz == NULL) {
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

static int registerNatives(JNIEnv* env)
{
    if (!registerNativeMethods(env, JNIREG_CLASS, gMethods,
                               sizeof(gMethods) / sizeof(gMethods[0])))
        return JNI_FALSE;

    return JNI_TRUE;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env = NULL;
    jint result = -1;

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    assert(env != NULL);

    if (!registerNatives(env)) {
        return -1;
    }

    /* success -- return valid version number */
    result = JNI_VERSION_1_6;

    return result;
}

#ifdef __cplusplus
}
#endif
