#ifndef _DES_H
#define _DES_H

#ifndef uint8
#define uint8  unsigned char
#endif

#ifndef uint32
#define uint32 unsigned long int
#endif

typedef struct
{
    uint32 esk[32];     /* DES encryption subkeys */
    uint32 dsk[32];     /* DES decryption subkeys */
}
des_context;

typedef struct
{
    uint32 esk[96];     /* Triple-DES encryption subkeys */
    uint32 dsk[96];     /* Triple-DES decryption subkeys */
}
des3_context;

int  des_set_key( des_context *ctx, uint8 key[8] );
void des_encrypt( des_context *ctx, uint8 input[8], uint8 output[8] );
void des_decrypt( des_context *ctx, uint8 input[8], uint8 output[8] );

int  des3_set_2keys( des3_context *ctx, uint8 key1[8], uint8 key2[8] );
int  des3_set_3keys( des3_context *ctx, uint8 key1[8], uint8 key2[8],
                                        uint8 key3[8] );

void des3_encrypt( des3_context *ctx, uint8 input[8], uint8 output[8] );
void des3_decrypt( des3_context *ctx, uint8 input[8], uint8 output[8] );

#endif /* des.h */
