//========================================================================
//
// SignatureInfo.cc
//
// This file is licensed under the GPLv2 or later
//
// Copyright 2015 André Guerreiro <aguerreiro1985@gmail.com>
// Copyright 2015 André Esser <bepandre@hotmail.com>
// Copyright 2017 Hans-Ulrich Jüttner <huj@froreich-bioscientia.de>
// Copyright 2017, 2018 Albert Astals Cid <aacid@kde.org>
// Copyright 2018 Chinmoy Ranjan Pradhan <chinmoyrp65@protonmail.com>
//
//========================================================================

#include <config.h>

#include "SignatureInfo.h"
#include "goo/gmem.h"
#include <stdlib.h>
#include <string.h>

#ifdef ENABLE_NSS3
    #include <hasht.h>
#else
    static const int HASH_AlgNULL = -1;
#endif

/* Constructor & Destructor */

SignatureInfo::SignatureInfo()
{
  sig_status = SIGNATURE_NOT_VERIFIED;
  cert_status = CERTIFICATE_NOT_VERIFIED;
  signer_name = nullptr;
  subject_dn = nullptr;
  location = nullptr;
  reason = nullptr;
  hash_type = HASH_AlgNULL;
  signing_time = 0;
  sig_subfilter_supported = false;
}

SignatureInfo::SignatureInfo(SignatureValidationStatus sig_val_status, CertificateValidationStatus cert_val_status)
{
  sig_status = sig_val_status;
  cert_status = cert_val_status;
  signer_name = nullptr;
  subject_dn = nullptr;
  location = nullptr;
  reason = nullptr;
  hash_type = HASH_AlgNULL;
  signing_time = 0;
  sig_subfilter_supported = false;
}

SignatureInfo::~SignatureInfo()
{
  free(location);
  free(reason);
  free(signer_name);
  free(subject_dn);
}

/* GETTERS */

SignatureValidationStatus SignatureInfo::getSignatureValStatus()
{
  return sig_status;
}

CertificateValidationStatus SignatureInfo::getCertificateValStatus()
{
  return cert_status;
}

const char *SignatureInfo::getSignerName()
{
  return signer_name;
}

const char *SignatureInfo::getSubjectDN()
{
  return subject_dn;
}

const char *SignatureInfo::getLocation() const
{
  return location;
}

const char *SignatureInfo::getReason() const
{
  return reason;
}

int SignatureInfo::getHashAlgorithm()
{
  return hash_type;
}

time_t SignatureInfo::getSigningTime()
{
  return signing_time;
}

/* SETTERS */

void SignatureInfo::setSignatureValStatus(enum SignatureValidationStatus sig_val_status)
{
  sig_status = sig_val_status;
}

void SignatureInfo::setCertificateValStatus(enum CertificateValidationStatus cert_val_status)
{
  cert_status = cert_val_status;
}

void SignatureInfo::setSignerName(char *signerName)
{
  free(signer_name);
  signer_name = signerName;
}

void SignatureInfo::setSubjectDN(const char *subjectDN)
{
  free(subject_dn);
  subject_dn = strdup(subjectDN);
}

void SignatureInfo::setLocation(char *loc)
{
  free(location);
  location = loc;
}

void SignatureInfo::setReason(char *signingReason)
{
  free(reason);
  reason = signingReason;
}

void SignatureInfo::setHashAlgorithm(int type)
{
  hash_type = type;
}

void SignatureInfo::setSigningTime(time_t signingTime)
{
  signing_time = signingTime;
}
