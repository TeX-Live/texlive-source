#ifndef T1CRYPT_H
#define T1CRYPT_H

#define EEKEY 55665u
#define CHARKEY 4330u

unsigned char t1_encrypt(unsigned char plain);
void t1_crypt_init (unsigned short int key);
unsigned char t1_decrypt(unsigned char cipher);

#endif
