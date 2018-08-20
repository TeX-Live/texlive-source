//========================================================================
//
// SignatureInfo.h
//
// This file is licensed under the GPLv2 or later
//
// Copyright 2015 André Guerreiro <aguerreiro1985@gmail.com>
// Copyright 2015 André Esser <bepandre@hotmail.com>
// Copyright 2015, 2017, 2018 Albert Astals Cid <aacid@kde.org>
// Copyright 2017 Hans-Ulrich Jüttner <huj@froreich-bioscientia.de>
// Copyright 2018 Chinmoy Ranjan Pradhan <chinmoyrp65@protonmail.com>
//
//========================================================================

#ifndef SIGNATUREINFO_H
#define SIGNATUREINFO_H

#include <time.h>

enum SignatureValidationStatus
{
  SIGNATURE_VALID,
  SIGNATURE_INVALID,
  SIGNATURE_DIGEST_MISMATCH,
  SIGNATURE_DECODING_ERROR,
  SIGNATURE_GENERIC_ERROR,
  SIGNATURE_NOT_FOUND,
  SIGNATURE_NOT_VERIFIED
};

enum CertificateValidationStatus
{
  CERTIFICATE_TRUSTED,
  CERTIFICATE_UNTRUSTED_ISSUER,
  CERTIFICATE_UNKNOWN_ISSUER,
  CERTIFICATE_REVOKED,
  CERTIFICATE_EXPIRED,
  CERTIFICATE_GENERIC_ERROR,
  CERTIFICATE_NOT_VERIFIED
};

class SignatureInfo {
public:
  SignatureInfo();
  SignatureInfo(SignatureValidationStatus, CertificateValidationStatus);
  ~SignatureInfo();

  /* GETTERS */
  SignatureValidationStatus getSignatureValStatus();
  CertificateValidationStatus getCertificateValStatus();
  const char *getSignerName();
  const char *getSubjectDN();
  const char *getLocation() const;
  const char *getReason() const;
  int getHashAlgorithm(); // Returns a NSS3 HASH_HashType or -1 if compiled without NSS3
  time_t getSigningTime();
  bool isSubfilterSupported() { return sig_subfilter_supported; }

  /* SETTERS */
  void setSignatureValStatus(enum SignatureValidationStatus );
  void setCertificateValStatus(enum CertificateValidationStatus );
  void setSignerName(char *);
  void setSubjectDN(const char *);
  void setLocation(char *);
  void setReason(char *);
  void setHashAlgorithm(int);
  void setSigningTime(time_t);
  void setSubFilterSupport(bool isSupported) { sig_subfilter_supported = isSupported; }

private:
  SignatureInfo(const SignatureInfo &);
  SignatureInfo& operator=(const SignatureInfo &);

  SignatureValidationStatus sig_status;
  CertificateValidationStatus cert_status;
  char *signer_name;
  char *subject_dn;
  char *location;
  char *reason;
  int hash_type;
  time_t signing_time;
  bool sig_subfilter_supported;
};

#endif
