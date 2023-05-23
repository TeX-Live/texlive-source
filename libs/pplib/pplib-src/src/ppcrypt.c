
#include "utilmd5.h"
#include "utilsha.h"

#include "pplib.h"

/* crypt struct */

static ppcrypt * ppcrypt_create (ppheap *heap)
{
  ppcrypt *crypt;
  crypt = (ppcrypt *)ppstruct_take(heap, sizeof(ppcrypt));
  memset(crypt, 0, sizeof(ppcrypt));
  return crypt;
}

int ppcrypt_type (ppcrypt *crypt, ppname *cryptname, ppuint *length, int *cryptflags)
{
  ppdict *filterdict;
  ppname *filtertype;
  int cryptmd = 0, default256 = 0;

  if (crypt->map == NULL || (filterdict = ppdict_rget_dict(crypt->map, cryptname->data)) == NULL)
    return 0;
  if ((filtertype = ppdict_get_name(filterdict, "CFM")) == NULL)
    return 0;
  *cryptflags = 0;
  if (ppname_is(filtertype, "V2"))
    *cryptflags |= PPCRYPT_INFO_RC4;
  else if (ppname_is(filtertype, "AESV2"))
    *cryptflags |= PPCRYPT_INFO_AES;
  else if (ppname_is(filtertype, "AESV3"))
    *cryptflags |= PPCRYPT_INFO_AES, default256 = 1;
  else
    return 0;
  /* pdf spec page. 134: /Length is said to be optional bit-length of the key, but it seems to be a mistake, as Acrobat
     produces /Length key with bytes lengths, opposite to /Length key of the main encrypt dict. */
  if (length != NULL)
    if (!ppdict_get_uint(filterdict, "Length", length))
      *length = (*cryptflags & PPCRYPT_INFO_RC4) ? 5 : (default256 ? 32 : 16);
  /* one of metadata flags is set iff there is an explicit EncryptMetadata key */
  if (ppdict_get_bool(filterdict, "EncryptMetadata", &cryptmd))
    *cryptflags |= (cryptmd ? PPCRYPT_INFO_MD : PPCRYPT_INFO_NOMD);
  return 1;
}

/* V1..4 algorithms */

/* V1..4 unicode do PdfDocEncoding */

typedef struct {
  uint32_t unicode;
  uint32_t code;
  uint32_t count;
} map_range_t;

static const map_range_t unicode_to_pdf_doc_encoding_map[] = {
  { 32, 32, 95 },
  { 161, 161, 12 },
  { 174, 174, 82 },
  { 305, 154, 1 },
  { 321, 149, 1 },
  { 322, 155, 1 },
  { 338, 150, 1 },
  { 339, 156, 1 },
  { 352, 151, 1 },
  { 353, 157, 1 },
  { 376, 152, 1 },
  { 381, 153, 1 },
  { 382, 158, 1 },
  { 402, 134, 1 },
  { 710, 26, 1 },
  { 711, 25, 1 },
  { 728, 24, 1 },
  { 729, 27, 1 },
  { 730, 30, 1 },
  { 731, 29, 1 },
  { 732, 31, 1 },
  { 733, 28, 1 },
  { 8211, 133, 1 },
  { 8212, 132, 1 },
  { 8216, 143, 3 },
  { 8220, 141, 2 },
  { 8222, 140, 1 },
  { 8224, 129, 2 },
  { 8226, 128, 1 },
  { 8230, 131, 1 },
  { 8240, 139, 1 },
  { 8249, 136, 2 },
  { 8260, 135, 1 },
  { 8364, 160, 1 },
  { 8482, 146, 1 },
  { 8722, 138, 1 },
  { 64257, 147, 2 }
};

#define unicode_to_pdf_doc_encoding_entries (sizeof(unicode_to_pdf_doc_encoding_map) / sizeof(map_range_t))

static int unicode_to_pdf_doc_encoding (uint32_t unicode, uint8_t *pcode)
{
  const map_range_t *left, *right, *mid;

  left = &unicode_to_pdf_doc_encoding_map[0];
  right = &unicode_to_pdf_doc_encoding_map[unicode_to_pdf_doc_encoding_entries - 1];
  for ( ; left <= right; )
  {
    mid = left + ((right - left) / 2);
    if (unicode > mid->unicode + mid->count - 1)
      left = mid + 1;
    else if (unicode < mid->unicode)
      right = mid - 1;
    else
    {
      *pcode = (uint8_t)(mid->code + (unicode - mid->unicode));
      return 1;
    }
  }
  return 0;
}

#define utf8_unicode2(p) (((p[0]&31)<<6)|(p[1]&63))
#define utf8_unicode3(p) (((p[0]&15)<<12)|((p[1]&63)<<6)|(p[2]&63))
#define utf8_unicode4(p) (((p[0]&7)<<18)|((p[1]&63)<<12)|((p[2]&63)<<6)|(p[3]&63))

#define utf8_get1(p, e, unicode) ((unicode = p[0]), p + 1)
#define utf8_get2(p, e, unicode) (p + 1 < e ? ((unicode = utf8_unicode2(p)), p + 2) : NULL)
#define utf8_get3(p, e, unicode) (p + 2 < e ? ((unicode = utf8_unicode3(p)), p + 3) : NULL)
#define utf8_get4(p, e, unicode) (p + 4 < e ? ((unicode = utf8_unicode3(p)), p + 4) : NULL)

#define utf8_get(p, e, unicode) \
  (p[0] < 0x80 ? utf8_get1(p, e, unicode) : \
   p[0] < 0xC0 ? NULL : \
   p[0] < 0xE0 ? utf8_get2(p, e, unicode) : \
   p[0] < 0xF0 ? utf8_get3(p, e, unicode) : utf8_get4(p, e, unicode))

static int ppcrypt_password_encoding (uint8_t *password, size_t *passwordlength)
{
  uint8_t *p, newpassword[PPCRYPT_MAX_PASSWORD], *n;
  const uint8_t *e;
  uint32_t unicode = 0;

  for (n = &newpassword[0], p = &password[0], e = p + *passwordlength; p < e; ++n)
  {
    p = utf8_get(p, e, unicode);
    if (p == NULL)
      return 0;
    if (unicode_to_pdf_doc_encoding(unicode, n) == 0)
      return 0;
  }
  *passwordlength = n - &newpassword[0];
  memcpy(password, newpassword, *passwordlength);
  return 1;
}

/* setup passwords */

static const uint8_t password_padding[] = {
  0x28, 0xBF, 0x4E, 0x5E, 0x4E, 0x75, 0x8A, 0x41, 0x64, 0x00, 0x4E, 0x56, 0xFF, 0xFA, 0x01, 0x08,
  0x2E, 0x2E, 0x00, 0xB6, 0xD0, 0x68, 0x3E, 0x80, 0x2F, 0x0C, 0xA9, 0xFE, 0x64, 0x53, 0x69, 0x7A
};

static void ppcrypt_set_user_password (ppcrypt *crypt, const void *userpass, size_t userpasslength)
{
  crypt->userpasslength = userpasslength > PPCRYPT_MAX_PASSWORD ? PPCRYPT_MAX_PASSWORD : userpasslength;
  memcpy(crypt->userpass, userpass, crypt->userpasslength);
  if (crypt->algorithm_variant < 5)
  {
    if (ppcrypt_password_encoding(crypt->userpass, &crypt->userpasslength) == 0)
      return;
    if (crypt->userpasslength > 32)
      crypt->userpasslength = 32;
    else if (crypt->userpasslength < 32)
      memcpy(&crypt->userpass[crypt->userpasslength], password_padding, 32 - crypt->userpasslength);
  }
  crypt->flags |= PPCRYPT_USER_PASSWORD;
}

static void ppcrypt_set_owner_password (ppcrypt *crypt, const void *ownerpass, size_t ownerpasslength)
{
  crypt->ownerpasslength = ownerpasslength > PPCRYPT_MAX_PASSWORD ? PPCRYPT_MAX_PASSWORD : ownerpasslength;
  memcpy(crypt->ownerpass, ownerpass, crypt->ownerpasslength);
  if (crypt->algorithm_variant < 5)
  {
    if (ppcrypt_password_encoding(crypt->ownerpass, &crypt->ownerpasslength) == 0)
      return;
    if (crypt->ownerpasslength > 32)
      crypt->ownerpasslength = 32;
    else if (crypt->ownerpasslength < 32)
      memcpy(&crypt->ownerpass[crypt->ownerpasslength], password_padding, 32 - crypt->ownerpasslength);
  }
  crypt->flags |= PPCRYPT_OWNER_PASSWORD;
}

/* V1..4 retrieving user password from owner password and owner key (variant < 5) */

static void ppcrypt_user_password_from_owner_key (ppcrypt *crypt, const void *ownerkey, size_t ownerkeysize)
{
  uint8_t temp[16], rc4key[32], rc4key2[32];
  uint8_t i;
  ppuint k;
  md5_state md5;

  md5_digest_init(&md5);
  md5_digest_add(&md5, crypt->ownerpass, 32);
  md5_digest_get(&md5, rc4key, MD5_BYTES);
  if (crypt->algorithm_revision >= 3)
  {
    for (i = 0; i < 50; ++i)
    {
      md5_digest(rc4key, 16, temp, MD5_BYTES);
      memcpy(rc4key, temp, 16);
    }
  }
  rc4_decode_data(ownerkey, ownerkeysize, crypt->userpass, rc4key, crypt->filekeylength);
  if (crypt->algorithm_revision >= 3)
  {
    for (i = 1; i <= 19; ++i)
    {
      for (k = 0; k < crypt->filekeylength; ++k)
        rc4key2[k] = rc4key[k] ^ i;
      rc4_decode_data(crypt->userpass, 32, crypt->userpass, rc4key2, crypt->filekeylength);
    }
  }
  //crypt->userpasslength = 32;
  for (crypt->userpasslength = 0; crypt->userpasslength < 32; ++crypt->userpasslength)
    if (memcmp(&crypt->userpass[crypt->userpasslength], password_padding, 32 - crypt->userpasslength) == 0)
      break;
  crypt->flags |= PPCRYPT_USER_PASSWORD;
}

/* V1..4 generating file key; pdf spec p. 125 */

static void ppcrypt_compute_file_key (ppcrypt *crypt, const void *ownerkey, size_t ownerkeysize, const void *id, size_t idsize)
{
  uint32_t p;
  uint8_t permissions[4], temp[16];
  int i;
  md5_state md5;

  md5_digest_init(&md5);
  md5_digest_add(&md5, crypt->userpass, 32);
  md5_digest_add(&md5, ownerkey, ownerkeysize);
  p = (uint32_t)crypt->permissions;
  permissions[0] = get_number_byte1(p);
  permissions[1] = get_number_byte2(p);
  permissions[2] = get_number_byte3(p);
  permissions[3] = get_number_byte4(p);
  md5_digest_add(&md5, permissions, 4);
  md5_digest_add(&md5, id, idsize);
  if (crypt->algorithm_revision >= 4 && (crypt->flags & PPCRYPT_NO_METADATA))
    md5_digest_add(&md5, "\xFF\xFF\xFF\xFF", 4);
  md5_digest_get(&md5, crypt->filekey, MD5_BYTES);
  if (crypt->algorithm_revision >= 3)
  {
    for (i = 0; i < 50; ++i)
    {
      md5_digest(crypt->filekey, (size_t)crypt->filekeylength, temp, MD5_BYTES);
      memcpy(crypt->filekey, temp, 16);
    }
  }
}

/* V1..4 generating userkey for comparison with /U; requires a general file key and id; pdf spec page 126-127 */

static void ppcrypt_compute_user_key (ppcrypt *crypt, const void *id, size_t idsize, uint8_t password_hash[32])
{
  uint8_t rc4key2[32];
  uint8_t i;
  ppuint k;

  if (crypt->algorithm_revision <= 2)
  {
    rc4_encode_data(password_padding, 32, password_hash, crypt->filekey, crypt->filekeylength);
  }
  else
  {
    md5_state md5;
    md5_digest_init(&md5);
    md5_digest_add(&md5, password_padding, 32);
    md5_digest_add(&md5, id, idsize);
    md5_digest_get(&md5, password_hash, MD5_BYTES);
    rc4_encode_data(password_hash, 16, password_hash, crypt->filekey, crypt->filekeylength);
    for (i = 1; i <= 19; ++i)
    {
      for (k = 0; k < crypt->filekeylength; ++k)
        rc4key2[k] = crypt->filekey[k] ^ i;
      rc4_encode_data(password_hash, 16, password_hash, rc4key2, crypt->filekeylength);
    }
    for (i = 16; i < 32; ++i)
      password_hash[i] = password_hash[i - 16] ^ i; /* arbitrary 16-bytes padding */
  }
}

static ppcrypt_status ppcrypt_authenticate_legacy (ppcrypt *crypt, ppstring *userkey, ppstring *ownerkey, ppstring *id)
{
  uint8_t password_hash[32];

  if ((crypt->flags & PPCRYPT_USER_PASSWORD) == 0 && (crypt->flags & PPCRYPT_OWNER_PASSWORD) != 0)
    ppcrypt_user_password_from_owner_key(crypt, ownerkey, ownerkey->size);
  ppcrypt_compute_file_key(crypt, ownerkey->data, ownerkey->size, id->data, id->size);
  ppcrypt_compute_user_key(crypt, id->data, id->size, password_hash); /* needs file key */
  return memcmp(userkey->data, password_hash, (crypt->algorithm_revision >= 3 ? 16 : 32)) == 0 ? PPCRYPT_DONE : PPCRYPT_PASS;
}

/* V5 */

static const uint8_t nulliv[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; /* AES-256 initialization vector */

/* V5 R5..6 password hash */

#define PPCRYPT_MAX_MANGLED ((127+64+48)*64) // 127 password, 64 hash, 48 /U key

static void ppcrypt_password_hash_indeed (const uint8_t *password, size_t passwordlength, const uint8_t *userkey, uint8_t hash[64])
{
  size_t hashlength, datalength;
  uint8_t data[PPCRYPT_MAX_MANGLED], *pdata;
  uint8_t *key, *iv;
  uint8_t round, i;
  uint32_t div3;

  hashlength = 32; /* initial hash is sha256 */
  round = 0;
  do
  {
    /* concat password, hash, and /U value 64 times */
    pdata = &data[0];
    memcpy(pdata, password, passwordlength);
    pdata += passwordlength;
    memcpy(pdata, hash, hashlength);
    pdata += hashlength;
    if (userkey != NULL)
    {
      memcpy(pdata, userkey, 48);
      pdata += 48;
    }
    datalength = pdata - &data[0];
    for (i = 1; i < 64; ++i, pdata += datalength)
      memcpy(pdata, &data[0], datalength);
    datalength *= 64;

    /* encrypt the data with aes128 using hash bytes 1..16 as key and bytes 17..32 as initialization vector
       encryption inplace, CBC, no-padding, no change to datalength */
    key = &hash[0]; iv = &hash[16];
    aes_encode_data(data, datalength, data, key, 16, iv, AES_NULL_PADDING);

    /* get modulo 3 of first 16 bytes number of encrypted data (sum of digits modulo 3) */
    for (i = 0, div3 = 0; i < 16; ++i)
      div3 += data[i];

    /* compute new hash using sha256/384/512 */
    switch (div3 % 3)
    {
      case 0:
        sha256_digest(data, datalength, hash, SHA_BYTES);
        hashlength = 32;
        break;
      case 1:
        sha384_digest(data, datalength, hash, SHA_BYTES);
        hashlength = 48;
        break;
      case 2:
        sha512_digest(data, datalength, hash, SHA_BYTES);
        hashlength = 64;
        break;
    }

    /* do 64 times, then keep going until the last byte of data <= round - 32 */
  } while (++round < 64 || round < data[datalength - 1] + 32);

}

static void ppcrypt_password_hash (ppcrypt *crypt, const uint8_t *password, size_t passwordlength, const uint8_t *salt, const uint8_t *userkey, uint8_t password_hash[32])
{
  sha256_state sha;
  uint8_t hash[64]; /* result password_hash is 32 bytes, but we need 64 for R6 procedure */

  /* take sha256 of password, salt and /U */
  sha256_digest_init(&sha);
  sha256_digest_add(&sha, password, passwordlength);
  sha256_digest_add(&sha, salt, 8);
  if (userkey != NULL)
    sha256_digest_add(&sha, userkey, 48);
  sha256_digest_get(&sha, hash, SHA_BYTES);

  /* V5 R5 - password_hash is the digest, V5 R6 - password_hash is mangled */
  if (crypt->algorithm_revision >= 6)
    ppcrypt_password_hash_indeed(password, passwordlength, userkey, hash);

  memcpy(password_hash, hash, 32);
}

/* V5 permissions */

static ppcrypt_status ppcrypt_authenticate_permissions (ppcrypt *crypt, ppstring *perms)
{
  uint8_t permsdata[16];

  aes_decode_data(perms->data, perms->size, permsdata, crypt->filekey, crypt->filekeylength, nulliv, AES_NULL_PADDING);

  if (permsdata[9] != 'a' || permsdata[10] != 'd' || permsdata[11] != 'b')
  { /* if we get here, the password hash is correct, we don't need to fail because of unreadable perms (found such docs) */
    crypt->flags |= PPCRYPT_UNREADABLE_PERMISSIONS;
    return PPCRYPT_DONE;
  }

  /* do not check/update permissions flags here; they might be different inside crypt string */
  if (0)
  {
    int64_t p;
    int i;
    for (p = 0, i = 0; i < 8; ++i)
      p = p + (permsdata[i] << (i << 3)); /* low order bytes first */
    crypt->permissions = (ppint)((int32_t)(p & 0x00000000FFFFFFFFLL)); /* unset bits 33..64, treat as 32-bit signed int */
  }

  if (permsdata[8] == 'T')
    crypt->flags &= ~PPCRYPT_NO_METADATA;
  else if (permsdata[8] == 'F')
    crypt->flags |= PPCRYPT_NO_METADATA;

  return PPCRYPT_DONE;
}

/* V5 authentication */

static ppcrypt_status ppcrypt_authenticate_user (ppcrypt *crypt, ppstring *u, ppstring *ue, ppstring *perms)
{
  uint8_t password_hash[32], *salt;

  salt = (uint8_t *)&u->data[32]; /* validation salt */
  ppcrypt_password_hash(crypt, crypt->userpass, crypt->userpasslength, salt, NULL, password_hash);
  if (memcmp(u->data, password_hash, 32) != 0)
    return PPCRYPT_PASS;

  salt = (uint8_t *)&u->data[40]; /* key salt */
  ppcrypt_password_hash(crypt, crypt->userpass, crypt->userpasslength, salt, NULL, password_hash);
  aes_decode_data(ue->data, 32, crypt->filekey, password_hash, 32, nulliv, AES_NULL_PADDING);

  return ppcrypt_authenticate_permissions(crypt, perms);
}

static ppcrypt_status ppcrypt_authenticate_owner (ppcrypt *crypt, ppstring *u, ppstring *o, ppstring *oe, ppstring *perms)
{
  uint8_t password_hash[32], *salt;

  salt = (uint8_t *)&o->data[32]; /* validation salt */
  ppcrypt_password_hash(crypt, crypt->ownerpass, crypt->ownerpasslength, salt, (uint8_t *)u->data, password_hash);
  if (memcmp(o->data, password_hash, 32) != 0)
    return PPCRYPT_PASS;

  salt = (uint8_t *)&o->data[40]; /* key salt */
  ppcrypt_password_hash(crypt, crypt->ownerpass, crypt->ownerpasslength, salt, (uint8_t *)u->data, password_hash);
  aes_decode_data(oe->data, 32, crypt->filekey, password_hash, 32, nulliv, AES_NULL_PADDING);

  return ppcrypt_authenticate_permissions(crypt, perms);
}


/* authentication */

static ppcrypt_status ppcrypt_authenticate (ppcrypt *crypt, ppstring *u, ppstring *ue, ppstring *o, ppstring *oe, ppstring *id, ppstring *perms)
{
  /* V1..V4 */
  if (crypt->algorithm_variant < 5)
    return ppcrypt_authenticate_legacy(crypt, u, o, id);

  /* V5 */
  if (crypt->flags & PPCRYPT_USER_PASSWORD)
    if (ppcrypt_authenticate_user(crypt, u, ue, perms) == PPCRYPT_DONE)
      return PPCRYPT_DONE;
  if (crypt->flags & PPCRYPT_OWNER_PASSWORD)
    return ppcrypt_authenticate_owner(crypt, u, o, oe, perms);

  return PPCRYPT_PASS;
}

/**/

ppcrypt_status ppdoc_crypt_init (ppdoc *pdf, const void *userpass, size_t userpasslength, const void *ownerpass, size_t ownerpasslength)
{
  ppcrypt *crypt;
  ppdict *trailer, *encrypt;
  ppobj *obj;
  ppname *name, **pkey;
  ppstring *userkey, *ownerkey, *userkey_e = NULL, *ownerkey_e = NULL;
  size_t hashlength;
  pparray *idarray;
  ppstring *id = NULL, *perms = NULL;
  int cryptflags, encryptmd;
  size_t strkeylength, stmkeylength;

  trailer = ppxref_trailer(pdf->xref);
  if ((obj = ppdict_get_obj(trailer, "Encrypt")) == NULL)
    return PPCRYPT_NONE;

  /* this happens early, before loading body, so if /Encrypt is indirect reference, it points nothing */
  obj = ppobj_preloaded(pdf, obj);
  if (obj->type != PPDICT)
    return PPCRYPT_FAIL;
  encrypt = obj->dict;
  for (ppdict_first(encrypt, pkey, obj); *pkey != NULL; ppdict_next(pkey, obj))
    (void)ppobj_preloaded(pdf, obj);

  if ((name = ppdict_get_name(encrypt, "Filter")) != NULL && !ppname_is(name, "Standard"))
    return PPCRYPT_FAIL;

  if ((crypt = pdf->crypt) == NULL)
    crypt = pdf->crypt = ppcrypt_create(&pdf->heap);

  /* get /V /R /P */
  if (!ppdict_get_uint(encrypt, "V", &crypt->algorithm_variant))
    crypt->algorithm_variant = 0;
  if (crypt->algorithm_variant < 1 || crypt->algorithm_variant > 5)
    return PPCRYPT_FAIL;
  if (!ppdict_get_uint(encrypt, "R", &crypt->algorithm_revision))
    return PPCRYPT_FAIL;
  if (!ppdict_get_int(encrypt, "P", &crypt->permissions))
    return PPCRYPT_FAIL;

  /* get /O /U /ID /OE /UE */
  if ((userkey = ppdict_get_string(encrypt, "U")) == NULL || (ownerkey = ppdict_get_string(encrypt, "O")) == NULL)
    return PPCRYPT_FAIL;
  userkey = ppstring_decoded(userkey);
  ownerkey = ppstring_decoded(ownerkey);

  /* for some reason acrobat pads /O and /U to 127 bytes with NULL, so we don't check the exact length but ensure the minimal */
  hashlength = crypt->algorithm_variant < 5 ? 32 : 48;
  if (userkey->size < hashlength || ownerkey->size < hashlength)
    return PPCRYPT_FAIL;
  if (crypt->algorithm_variant < 5)
  { // get first string from /ID (must not be ref)
    if ((idarray = ppdict_get_array(trailer, "ID")) == NULL || (id = pparray_get_string(idarray, 0)) == NULL)
      return PPCRYPT_FAIL;
    id = ppstring_decoded(id);
  }
  else
  {
    if ((userkey_e = ppdict_get_string(encrypt, "UE")) == NULL || (ownerkey_e = ppdict_get_string(encrypt, "OE")) == NULL)
      return PPCRYPT_FAIL;
    userkey_e = ppstring_decoded(userkey_e);
    ownerkey_e = ppstring_decoded(ownerkey_e);
    if (userkey_e->size < 32 || ownerkey_e->size < 32)
      return PPCRYPT_FAIL;
    if ((perms = ppdict_get_string(encrypt, "Perms")) == NULL)
      return PPCRYPT_FAIL;
    perms = ppstring_decoded(perms);
    if (perms->size != 16)
      return PPCRYPT_FAIL;
  }

  /* collect flags and keylength */
  switch (crypt->algorithm_revision)
  {
    case 1:
      crypt->filekeylength = 5;
      crypt->flags |= PPCRYPT_RC4;
      break;
    case 2: case 3:
      if (ppdict_get_uint(encrypt, "Length", &crypt->filekeylength))
        crypt->filekeylength >>= 3; /* 40..256 bits, 5..32 bytes*/
      else
        crypt->filekeylength = 5; /* 40 bits, 5 bytes */
      crypt->flags |= PPCRYPT_RC4;
      break;
    case 4: case 5: case 6:
      if ((crypt->map = ppdict_rget_dict(encrypt, "CF")) == NULL)
        return PPCRYPT_FAIL;
      for (ppdict_first(crypt->map, pkey, obj); *pkey != NULL; ppdict_next(pkey, obj))
        (void)ppobj_preloaded(pdf, obj);
      /* /EncryptMetadata relevant only for version >=4, may be also provided in crypt filter dictionary; which takes a precedence then?
         we assume that if there is an explicit EncryptMetadata key, it overrides main encrypt dict flag or default flag (the default is true,
         meaning that Metadata stream is encrypted as others) */
      if (ppdict_get_bool(encrypt, "EncryptMetadata", &encryptmd) && !encryptmd)
        crypt->flags |= PPCRYPT_NO_METADATA;

      strkeylength = stmkeylength = 0;
      /* streams filter */
      if ((name = ppdict_get_name(encrypt, "StmF")) != NULL && ppcrypt_type(crypt, name, &stmkeylength, &cryptflags))
      {
        if (cryptflags & PPCRYPT_INFO_AES)
          crypt->flags |= PPCRYPT_STREAM_AES;
        else if (cryptflags & PPCRYPT_INFO_RC4)
          crypt->flags |= PPCRYPT_STREAM_RC4;
        if (cryptflags & PPCRYPT_INFO_NOMD)
          crypt->flags |= PPCRYPT_NO_METADATA;
        else if (cryptflags & PPCRYPT_INFO_MD)
          crypt->flags &= ~PPCRYPT_NO_METADATA;
      } /* else identity */
      /* strings filter */
      if ((name = ppdict_get_name(encrypt, "StrF")) != NULL && ppcrypt_type(crypt, name, &strkeylength, &cryptflags))
      {
        if (cryptflags & PPCRYPT_INFO_AES)
          crypt->flags |= PPCRYPT_STRING_AES;
        else if (cryptflags & PPCRYPT_INFO_RC4)
          crypt->flags |= PPCRYPT_STRING_RC4;
      } /* else identity */

      /* /Length of encrypt dict is irrelevant here, theoretically every crypt filter may have own length... It means that we should
         actually keep a different file key for streams and strings. But it leads to nonsense, as /U and /O entries refers to a single
         keylength, without a distinction for strings/streams. So we have to assume /Length is consistent. To expose the limitation: */
      if ((crypt->flags & PPCRYPT_STREAM) && (crypt->flags & PPCRYPT_STRING))
        if (strkeylength != stmkeylength)
          return PPCRYPT_FAIL;
      crypt->filekeylength = stmkeylength ? stmkeylength : strkeylength;
      if ((crypt->flags & PPCRYPT_STREAM) || (crypt->flags & PPCRYPT_STRING))
        if (crypt->filekeylength == 0)
          return PPCRYPT_FAIL;
      break;
    default:
      return PPCRYPT_FAIL;
  }

  /* setup passwords */
  if (userpass != NULL)
    ppcrypt_set_user_password(crypt, userpass, userpasslength);
  if (ownerpass != NULL)
    ppcrypt_set_owner_password(crypt, ownerpass, ownerpasslength);
  if ((crypt->flags & (PPCRYPT_USER_PASSWORD|PPCRYPT_OWNER_PASSWORD)) == 0)
    return PPCRYPT_PASS;

  return ppcrypt_authenticate(crypt, userkey, userkey_e, ownerkey, ownerkey_e, id, perms);
}

/* decrypting strings */

/*
Since strings are generally rare, but might occur in mass (name trees). We generate decryption key when needed.
All strings within the same reference are crypted with the same key. Both RC4 and AES algorithms expands
the crypt key in some way and the result of expansion is the same for the same crypt key. Instead of recreating
the ky for every string, we backup the initial decryption state.
*/

static void ppcrypt_strkey (ppcrypt *crypt, ppref *ref, int aes)
{
  if (crypt->cryptkeylength > 0)
  { /* crypt key already generated, just reinitialize crypt states */
    if (aes)
    { /* aes codecs that works on c-strings do not modify aes_state flags at all, so we actually don't need to revitalize the state,
         we only rewrite an initialization vector, which is modified during crypt procedure */
    }
    else
    { /* rc4 crypt map is modified during crypt procedure, so here we reinitialize rc4 bytes map */
      rc4_map_restore(&crypt->rc4state, &crypt->rc4copy);
    }
    return;
  }

  if (crypt->algorithm_variant < 5)
  {
    crypt->filekey[crypt->filekeylength + 0] = get_number_byte1(ref->number);
    crypt->filekey[crypt->filekeylength + 1] = get_number_byte2(ref->number);
    crypt->filekey[crypt->filekeylength + 2] = get_number_byte3(ref->number);
    crypt->filekey[crypt->filekeylength + 3] = get_number_byte1(ref->version);
    crypt->filekey[crypt->filekeylength + 4] = get_number_byte2(ref->version);

    if (aes)
    {
      crypt->filekey[crypt->filekeylength + 5] = 0x73; // s
      crypt->filekey[crypt->filekeylength + 6] = 0x41; // A
      crypt->filekey[crypt->filekeylength + 7] = 0x6C; // l
      crypt->filekey[crypt->filekeylength + 8] = 0x54; // T
    }

    md5_digest(crypt->filekey, crypt->filekeylength + (aes ? 9 : 5), crypt->cryptkey, MD5_BYTES);
    crypt->cryptkeylength = crypt->filekeylength + 5 >= 16 ? 16 : crypt->filekeylength + 5;
  }
  else
  {
    memcpy(crypt->cryptkey, crypt->filekey, 32);
    crypt->cryptkeylength = 32;
  }

  if (aes)
  {
    aes_decode_initialize(&crypt->aesstate, &crypt->aeskeyblock, crypt->cryptkey, crypt->cryptkeylength, NULL);
    aes_pdf_mode(&crypt->aesstate);
  }
  else
  {
    rc4_state_initialize(&crypt->rc4state, &crypt->rc4map, crypt->cryptkey, crypt->cryptkeylength);
    rc4_map_save(&crypt->rc4state, &crypt->rc4copy);
  }
}

int ppstring_decrypt (ppcrypt *crypt, const void *input, size_t size, void *output, size_t *newsize)
{
  int aes, rc4;
  aes = crypt->flags & PPCRYPT_STRING_AES;
  rc4 = crypt->flags & PPCRYPT_STRING_RC4;
  if (aes || rc4)
  {
    ppcrypt_strkey(crypt, crypt->ref, aes);
    if (aes)
      *newsize = aes_decode_state_data(&crypt->aesstate, input, size, output);
    else // if (rc4)
      *newsize = rc4_decode_state_data(&crypt->rc4state, input, size, output);
    return 1;
  }
  return 0; // identity crypt
}

/* decrypting streams */

/*
Streams are decrypted everytime when accessing the stream data. We need to be able to get or make
the key for decryption as long as the stream is alive. And to get the key we need the reference
number and version, plus document crypt info. First thought was to keep the reference to which
the stream belongs; stream->ref and accessing the crypt info stream->ref->xref->pdf->crypt.
It would be ok as long as absolutelly nothing happens with ref and crypt. At some point pplib
may drift into rewriting support, which would imply ref/xref/crypt/pdf structures modifications.
So I feel better with generating a crypt key for every stream in encrypted document, paying a cost
of md5 for all streams, not necessarily those actually read.

Key generation is the same as for strings, but different for distinct encryption methods (rc4 vs aes).
Since streams and strings might theoretically be encrypted with different filters. No reason to cacche
decryption state here.
*/

ppstring * ppcrypt_stmkey (ppcrypt *crypt, ppref *ref, int aes, ppheap *heap)
{
  ppstring *cryptkeystring;
  //if (crypt->cryptkeylength > 0)
  //  return;

  if (crypt->algorithm_variant < 5)
  {
    crypt->filekey[crypt->filekeylength + 0] = get_number_byte1(ref->number);
    crypt->filekey[crypt->filekeylength + 1] = get_number_byte2(ref->number);
    crypt->filekey[crypt->filekeylength + 2] = get_number_byte3(ref->number);
    crypt->filekey[crypt->filekeylength + 3] = get_number_byte1(ref->version);
    crypt->filekey[crypt->filekeylength + 4] = get_number_byte2(ref->version);

    if (aes)
    {
      crypt->filekey[crypt->filekeylength + 5] = 0x73;
      crypt->filekey[crypt->filekeylength + 6] = 0x41;
      crypt->filekey[crypt->filekeylength + 7] = 0x6C;
      crypt->filekey[crypt->filekeylength + 8] = 0x54;
    }

    md5_digest(crypt->filekey, crypt->filekeylength + (aes ? 9 : 5), crypt->cryptkey, MD5_BYTES);
    crypt->cryptkeylength = crypt->filekeylength + 5 >= 16 ? 16 : crypt->filekeylength + 5; // how about 256bits AES??
  }
  else
  { // we could actually generate this string once, but.. aes itself is way more expensive that we can earn here
    memcpy(crypt->cryptkey, crypt->filekey, 32); // just for the record
    crypt->cryptkeylength = 32;
  }
  cryptkeystring = ppstring_internal(crypt->cryptkey, crypt->cryptkeylength, heap);
  return ppstring_decoded(cryptkeystring);
}
