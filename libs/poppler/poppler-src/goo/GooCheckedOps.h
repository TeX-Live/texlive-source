//========================================================================
//
// GooCheckedOps.h
//
// This file is licensed under the GPLv2 or later
//
// Copyright (C) 2018 Adam Reichold <adam.reichold@t-online.de>
//
//========================================================================

#ifndef GOO_CHECKED_OPS_H
#define GOO_CHECKED_OPS_H

#include <climits>

inline bool checkedAssign(long long lz, int *z) {
  static_assert(LLONG_MAX > INT_MAX, "Need type larger than int to perform overflow checks.");

  if (lz > INT_MAX || lz < INT_MIN) {
    return true;
  }

  *z = static_cast<int>(lz);
  return false;
}

#ifndef __has_builtin
  #define __has_builtin(x) 0
#endif

inline bool checkedAdd(int x, int y, int *z) {
#if __GNUC__ >= 5 || __has_builtin(__builtin_sadd_overflow)
  return __builtin_sadd_overflow(x, y, z);
#else
  const auto lz = static_cast<long long>(x) + static_cast<long long>(y);
  return checkedAssign(lz, z);
#endif
}

inline bool checkedMultiply(int x, int y, int *z) {
#if __GNUC__ >= 5 || __has_builtin(__builtin_smul_overflow)
  return __builtin_smul_overflow(x, y, z);
#else
  const auto lz = static_cast<long long>(x) * static_cast<long long>(y);
  return checkedAssign(lz, z);
#endif
}

#endif // GOO_CHECKED_OPS_H
