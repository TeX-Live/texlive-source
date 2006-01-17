unsigned short int r;
unsigned short int c1 = 52845;
unsigned short int c2 = 22719;

#include "t1crypt.h"

unsigned char t1_encrypt(unsigned char plain)
{
  unsigned char cipher;
  cipher = (plain ^ (r >> 8));
  r = (cipher+r)*c1 + c2;
  return cipher;
}

void t1_crypt_init (unsigned short int key)
{
  r = key;
}

unsigned char t1_decrypt(unsigned char cipher)
{
  unsigned char plain;
  plain = (cipher ^ (r>>8));
  r = (cipher+r)*c1 + c2;
  return plain;
}
