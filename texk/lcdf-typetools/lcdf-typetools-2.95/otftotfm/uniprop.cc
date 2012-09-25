/* uniprop.{cc,hh} -- code for Unicode character properties
 *
 * Copyright (c) 2004-2012 Eddie Kohler
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version. This program is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 */

#include <config.h>
#include "uniprop.hh"
#include <lcdf/string.hh>

const unsigned char UnicodeProperty::property_pages[] = {
    0, P_Cn,
    0, P_Cc, 32, P_Zs, 33, P_Po, 36, P_Sc, 37, P_Po, 40, P_Ps, 41, P_Pe, 42, P_Po, 43, P_Sm, 44, P_Po, 45, P_Pd, 46, P_Po, 48, P_Nd, 58, P_Po, 60, P_Sm, 63, P_Po, 65, P_Lu, 91, P_Ps, 92, P_Po, 93, P_Pe, 94, P_Sk, 95, P_Pc, 96, P_Sk, 97, P_Ll, 123, P_Ps, 124, P_Sm, 125, P_Pe, 126, P_Sm, 127, P_Cc, 160, P_Zs, 161, P_Po, 162, P_Sc, 166, P_So, 168, P_Sk, 169, P_So, 170, P_Ll, 171, P_Pi, 172, P_Sm, 173, P_Cf, 174, P_So, 175, P_Sk, 176, P_So, 177, P_Sm, 178, P_No, 180, P_Sk, 181, P_Ll, 182, P_So, 183, P_Po, 184, P_Sk, 185, P_No, 186, P_Ll, 187, P_Pf, 188, P_No, 191, P_Po, 192, P_Lu, 215, P_Sm, 216, P_Lu, 223, P_Ll, 247, P_Sm, 248, P_Ll,
    0, P_Lul, 56, P_Ll, 57, P_Lul, 73, P_Ll, 74, P_Lul, 120, P_Lu, 122, P_Ll, 123, P_Lul, 127, P_Ll, 129, P_Lu, 131, P_Ll, 132, P_Lul, 134, P_Lu, 136, P_Ll, 137, P_Lu, 140, P_Ll, 142, P_Lu, 146, P_Ll, 147, P_Lu, 149, P_Ll, 150, P_Lu, 153, P_Ll, 156, P_Lu, 158, P_Ll, 159, P_Lu, 161, P_Ll, 162, P_Lul, 166, P_Lu, 168, P_Ll, 169, P_Lul, 171, P_Ll, 172, P_Lul, 174, P_Lu, 176, P_Ll, 177, P_Lu, 180, P_Ll, 181, P_Lul, 183, P_Lu, 185, P_Ll, 187, P_Lo, 188, P_Lul, 190, P_Ll, 192, P_Lo, 196, P_Lu, 197, P_Lt, 198, P_Ll, 199, P_Lu, 200, P_Lt, 201, P_Ll, 202, P_Lu, 203, P_Lt, 204, P_Ll, 205, P_Lul, 221, P_Ll, 222, P_Lul, 240, P_Ll, 241, P_Lu, 242, P_Lt, 243, P_Ll, 244, P_Lul, 246, P_Lu, 249, P_Ll, 250, P_Lul,
    0, P_Lul, 52, P_Ll, 58, P_Lu, 60, P_Ll, 61, P_Lu, 63, P_Ll, 65, P_Lul, 67, P_Lu, 71, P_Ll, 72, P_Lul, 80, P_Ll, 148, P_Lo, 149, P_Ll, 176, P_Lm, 194, P_Sk, 198, P_Lm, 210, P_Sk, 224, P_Lm, 229, P_Sk, 238, P_Lm, 239, P_Sk,
    0, P_Mn, 112, P_Cn, 116, P_Sk, 118, P_Cn, 122, P_Lm, 123, P_Ll, 126, P_Po, 127, P_Cn, 132, P_Sk, 134, P_Lu, 135, P_Po, 136, P_Lu, 139, P_Cn, 140, P_Lu, 141, P_Cn, 142, P_Lu, 144, P_Ll, 145, P_Lu, 162, P_Cn, 163, P_Lu, 172, P_Ll, 207, P_Cn, 208, P_Ll, 210, P_Lu, 213, P_Ll, 216, P_Lul, 240, P_Ll, 244, P_Lul, 246, P_Sm, 247, P_Lul, 249, P_Lu, 251, P_Ll, 253, P_Lu,
    0, P_Lu, 48, P_Ll, 96, P_Lul, 130, P_So, 131, P_Mn, 135, P_Cn, 136, P_Me, 138, P_Lul, 192, P_Lu, 194, P_Ll, 195, P_Lul, 207, P_Ll, 208, P_Lul,
    0, P_Lul, 20, P_Cn, 49, P_Lu, 87, P_Cn, 89, P_Lm, 90, P_Po, 96, P_Cn, 97, P_Ll, 136, P_Cn, 137, P_Po, 138, P_Pd, 139, P_Cn, 145, P_Mn, 190, P_Po, 191, P_Mn, 192, P_Po, 193, P_Mn, 195, P_Po, 196, P_Mn, 198, P_Po, 199, P_Mn, 200, P_Cn, 208, P_Lo, 235, P_Cn, 240, P_Lo, 243, P_Po, 245, P_Cn,
    0, P_Cf, 4, P_Cn, 11, P_Sc, 12, P_Po, 14, P_So, 16, P_Mn, 22, P_Cn, 27, P_Po, 28, P_Cn, 30, P_Po, 32, P_Cn, 33, P_Lo, 59, P_Cn, 64, P_Lm, 65, P_Lo, 75, P_Mn, 95, P_Cn, 96, P_Nd, 106, P_Po, 110, P_Lo, 112, P_Mn, 113, P_Lo, 212, P_Po, 213, P_Lo, 214, P_Mn, 221, P_Cf, 222, P_Me, 223, P_Mn, 229, P_Lm, 231, P_Mn, 233, P_So, 234, P_Mn, 238, P_Lo, 240, P_Nd, 250, P_Lo, 253, P_So, 255, P_Lo,
    0, P_Po, 14, P_Cn, 15, P_Cf, 16, P_Lo, 17, P_Mn, 18, P_Lo, 48, P_Mn, 75, P_Cn, 77, P_Lo, 110, P_Cn, 128, P_Lo, 166, P_Mn, 177, P_Lo, 178, P_Cn, 192, P_Nd, 202, P_Lo, 235, P_Mn, 244, P_Lm, 246, P_So, 247, P_Po, 250, P_Lm, 251, P_Cn,
    0, P_Cn, 1, P_Mn, 3, P_Mc, 4, P_Lo, 58, P_Cn, 60, P_Mn, 61, P_Lo, 62, P_Mc, 65, P_Mn, 73, P_Mc, 77, P_Mn, 78, P_Cn, 80, P_Lo, 81, P_Mn, 85, P_Cn, 88, P_Lo, 98, P_Mn, 100, P_Po, 102, P_Nd, 112, P_Po, 113, P_Cn, 123, P_Lo, 128, P_Cn, 129, P_Mn, 130, P_Mc, 132, P_Cn, 133, P_Lo, 141, P_Cn, 143, P_Lo, 145, P_Cn, 147, P_Lo, 169, P_Cn, 170, P_Lo, 177, P_Cn, 178, P_Lo, 179, P_Cn, 182, P_Lo, 186, P_Cn, 188, P_Mn, 189, P_Lo, 190, P_Mc, 193, P_Mn, 197, P_Cn, 199, P_Mc, 201, P_Cn, 203, P_Mc, 205, P_Mn, 206, P_Lo, 207, P_Cn, 215, P_Mc, 216, P_Cn, 220, P_Lo, 222, P_Cn, 223, P_Lo, 226, P_Mn, 228, P_Cn, 230, P_Nd, 240, P_Lo, 242, P_Sc, 244, P_No, 250, P_So, 251, P_Cn,
    0, P_Cn, 1, P_Mn, 3, P_Mc, 4, P_Cn, 5, P_Lo, 11, P_Cn, 15, P_Lo, 17, P_Cn, 19, P_Lo, 41, P_Cn, 42, P_Lo, 49, P_Cn, 50, P_Lo, 52, P_Cn, 53, P_Lo, 55, P_Cn, 56, P_Lo, 58, P_Cn, 60, P_Mn, 61, P_Cn, 62, P_Mc, 65, P_Mn, 67, P_Cn, 71, P_Mn, 73, P_Cn, 75, P_Mn, 78, P_Cn, 89, P_Lo, 93, P_Cn, 94, P_Lo, 95, P_Cn, 102, P_Nd, 112, P_Mn, 114, P_Lo, 117, P_Cn, 129, P_Mn, 131, P_Mc, 132, P_Cn, 133, P_Lo, 142, P_Cn, 143, P_Lo, 146, P_Cn, 147, P_Lo, 169, P_Cn, 170, P_Lo, 177, P_Cn, 178, P_Lo, 180, P_Cn, 181, P_Lo, 186, P_Cn, 188, P_Mn, 189, P_Lo, 190, P_Mc, 193, P_Mn, 198, P_Cn, 199, P_Mn, 201, P_Mc, 202, P_Cn, 203, P_Mc, 205, P_Mn, 206, P_Cn, 208, P_Lo, 209, P_Cn, 224, P_Lo, 226, P_Mn, 228, P_Cn, 230, P_Nd, 240, P_Cn, 241, P_Sc, 242, P_Cn,
    0, P_Cn, 1, P_Mn, 2, P_Mc, 4, P_Cn, 5, P_Lo, 13, P_Cn, 15, P_Lo, 17, P_Cn, 19, P_Lo, 41, P_Cn, 42, P_Lo, 49, P_Cn, 50, P_Lo, 52, P_Cn, 53, P_Lo, 58, P_Cn, 60, P_Mn, 61, P_Lo, 62, P_Mc, 63, P_Mn, 64, P_Mc, 65, P_Mn, 68, P_Cn, 71, P_Mc, 73, P_Cn, 75, P_Mc, 77, P_Mn, 78, P_Cn, 86, P_Mn, 87, P_Mc, 88, P_Cn, 92, P_Lo, 94, P_Cn, 95, P_Lo, 98, P_Cn, 102, P_Nd, 112, P_So, 113, P_Lo, 114, P_Cn, 130, P_Mn, 131, P_Lo, 132, P_Cn, 133, P_Lo, 139, P_Cn, 142, P_Lo, 145, P_Cn, 146, P_Lo, 150, P_Cn, 153, P_Lo, 155, P_Cn, 156, P_Lo, 157, P_Cn, 158, P_Lo, 160, P_Cn, 163, P_Lo, 165, P_Cn, 168, P_Lo, 171, P_Cn, 174, P_Lo, 186, P_Cn, 190, P_Mc, 192, P_Mn, 193, P_Mc, 195, P_Cn, 198, P_Mc, 201, P_Cn, 202, P_Mc, 205, P_Mn, 206, P_Cn, 215, P_Mc, 216, P_Cn, 230, P_Nd, 240, P_No, 243, P_So, 249, P_Sc, 250, P_So, 251, P_Cn,
    0, P_Cn, 1, P_Mc, 4, P_Cn, 5, P_Lo, 13, P_Cn, 14, P_Lo, 17, P_Cn, 18, P_Lo, 41, P_Cn, 42, P_Lo, 52, P_Cn, 53, P_Lo, 58, P_Cn, 62, P_Mn, 65, P_Mc, 69, P_Cn, 70, P_Mn, 73, P_Cn, 74, P_Mn, 78, P_Cn, 85, P_Mn, 87, P_Cn, 96, P_Lo, 98, P_Cn, 102, P_Nd, 112, P_Cn, 130, P_Mc, 132, P_Cn, 133, P_Lo, 141, P_Cn, 142, P_Lo, 145, P_Cn, 146, P_Lo, 169, P_Cn, 170, P_Lo, 180, P_Cn, 181, P_Lo, 186, P_Cn, 188, P_Mn, 189, P_Lo, 190, P_Mc, 191, P_Mn, 192, P_Mc, 197, P_Cn, 198, P_Mn, 199, P_Mc, 201, P_Cn, 202, P_Mc, 204, P_Mn, 206, P_Cn, 213, P_Mc, 215, P_Cn, 222, P_Lo, 223, P_Cn, 224, P_Lo, 226, P_Mn, 228, P_Cn, 230, P_Nd, 240, P_Cn, 241, P_So, 243, P_Cn,
    0, P_Cn, 2, P_Mc, 4, P_Cn, 5, P_Lo, 13, P_Cn, 14, P_Lo, 17, P_Cn, 18, P_Lo, 41, P_Cn, 42, P_Lo, 58, P_Cn, 62, P_Mc, 65, P_Mn, 68, P_Cn, 70, P_Mc, 73, P_Cn, 74, P_Mc, 77, P_Mn, 78, P_Cn, 87, P_Mc, 88, P_Cn, 96, P_Lo, 98, P_Cn, 102, P_Nd, 112, P_Cn, 130, P_Mc, 132, P_Cn, 133, P_Lo, 151, P_Cn, 154, P_Lo, 178, P_Cn, 179, P_Lo, 188, P_Cn, 189, P_Lo, 190, P_Cn, 192, P_Lo, 199, P_Cn, 202, P_Mn, 203, P_Cn, 207, P_Mc, 210, P_Mn, 213, P_Cn, 214, P_Mn, 215, P_Cn, 216, P_Mc, 224, P_Cn, 242, P_Mc, 244, P_Po, 245, P_Cn,
    0, P_Cn, 1, P_Lo, 49, P_Mn, 50, P_Lo, 52, P_Mn, 59, P_Cn, 63, P_Sc, 64, P_Lo, 70, P_Lm, 71, P_Mn, 79, P_Po, 80, P_Nd, 90, P_Po, 92, P_Cn, 129, P_Lo, 131, P_Cn, 132, P_Lo, 133, P_Cn, 135, P_Lo, 137, P_Cn, 138, P_Lo, 139, P_Cn, 141, P_Lo, 142, P_Cn, 148, P_Lo, 152, P_Cn, 153, P_Lo, 160, P_Cn, 161, P_Lo, 164, P_Cn, 165, P_Lo, 166, P_Cn, 167, P_Lo, 168, P_Cn, 170, P_Lo, 172, P_Cn, 173, P_Lo, 177, P_Mn, 178, P_Lo, 180, P_Mn, 186, P_Cn, 187, P_Mn, 189, P_Lo, 190, P_Cn, 192, P_Lo, 197, P_Cn, 198, P_Lm, 199, P_Cn, 200, P_Mn, 206, P_Cn, 208, P_Nd, 218, P_Cn, 220, P_Lo, 222, P_Cn,
    0, P_Lo, 1, P_So, 4, P_Po, 19, P_So, 24, P_Mn, 26, P_So, 32, P_Nd, 42, P_No, 52, P_So, 53, P_Mn, 54, P_So, 55, P_Mn, 56, P_So, 57, P_Mn, 58, P_Ps, 59, P_Pe, 60, P_Ps, 61, P_Pe, 62, P_Mc, 64, P_Lo, 72, P_Cn, 73, P_Lo, 107, P_Cn, 113, P_Mn, 127, P_Mc, 128, P_Mn, 133, P_Po, 134, P_Mn, 136, P_Lo, 140, P_Cn, 144, P_Mn, 152, P_Cn, 153, P_Mn, 189, P_Cn, 190, P_So, 198, P_Mn, 199, P_So, 205, P_Cn, 207, P_So, 208, P_Po, 210, P_Cn,
    0, P_Lo, 34, P_Cn, 35, P_Lo, 40, P_Cn, 41, P_Lo, 43, P_Cn, 44, P_Mc, 45, P_Mn, 49, P_Mc, 50, P_Mn, 51, P_Cn, 54, P_Mn, 56, P_Mc, 57, P_Mn, 58, P_Cn, 64, P_Nd, 74, P_Po, 80, P_Lo, 86, P_Mc, 88, P_Mn, 90, P_Cn, 160, P_Lu, 198, P_Cn, 208, P_Lo, 251, P_Po, 252, P_Lm, 253, P_Cn,
    0, P_Lo, 90, P_Cn, 95, P_Lo, 163, P_Cn, 168, P_Lo, 250, P_Cn,
    0, P_Lo, 73, P_Cn, 74, P_Lo, 78, P_Cn, 80, P_Lo, 87, P_Cn, 88, P_Lo, 89, P_Cn, 90, P_Lo, 94, P_Cn, 96, P_Lo, 137, P_Cn, 138, P_Lo, 142, P_Cn, 144, P_Lo, 177, P_Cn, 178, P_Lo, 182, P_Cn, 184, P_Lo, 191, P_Cn, 192, P_Lo, 193, P_Cn, 194, P_Lo, 198, P_Cn, 200, P_Lo, 215, P_Cn, 216, P_Lo,
    0, P_Lo, 17, P_Cn, 18, P_Lo, 22, P_Cn, 24, P_Lo, 91, P_Cn, 95, P_Mn, 96, P_So, 97, P_Po, 105, P_No, 125, P_Cn, 128, P_Lo, 144, P_So, 154, P_Cn, 160, P_Lo, 245, P_Cn,
    0, P_Cn, 1, P_Lo,
    0, P_Lo,
    0, P_Lo, 109, P_Po, 111, P_Lo, 119, P_Cn, 128, P_Zs, 129, P_Lo, 155, P_Ps, 156, P_Pe, 157, P_Cn, 160, P_Lo, 235, P_Po, 238, P_Nl, 241, P_Cn,
    0, P_Lo, 13, P_Cn, 14, P_Lo, 18, P_Mn, 21, P_Cn, 32, P_Lo, 50, P_Mn, 53, P_Po, 55, P_Cn, 64, P_Lo, 82, P_Mn, 84, P_Cn, 96, P_Lo, 109, P_Cn, 110, P_Lo, 113, P_Cn, 114, P_Mn, 116, P_Cn, 128, P_Lo, 180, P_Cf, 182, P_Mc, 183, P_Mn, 190, P_Mc, 198, P_Mn, 199, P_Mc, 201, P_Mn, 212, P_Po, 215, P_Lm, 216, P_Po, 219, P_Sc, 220, P_Lo, 221, P_Mn, 222, P_Cn, 224, P_Nd, 234, P_Cn, 240, P_No, 250, P_Cn,
    0, P_Po, 6, P_Pd, 7, P_Po, 11, P_Mn, 14, P_Zs, 15, P_Cn, 16, P_Nd, 26, P_Cn, 32, P_Lo, 67, P_Lm, 68, P_Lo, 120, P_Cn, 128, P_Lo, 169, P_Mn, 170, P_Cn,
    0, P_Lo, 29, P_Cn, 32, P_Mn, 35, P_Mc, 39, P_Mn, 41, P_Mc, 44, P_Cn, 48, P_Mc, 50, P_Mn, 51, P_Mc, 57, P_Mn, 60, P_Cn, 64, P_So, 65, P_Cn, 68, P_Po, 70, P_Nd, 80, P_Lo, 110, P_Cn, 112, P_Lo, 117, P_Cn, 128, P_Lo, 170, P_Cn, 176, P_Mc, 193, P_Lo, 200, P_Mc, 202, P_Cn, 208, P_Nd, 218, P_Cn, 222, P_Po, 224, P_So,
    0, P_Lo, 23, P_Mn, 25, P_Mc, 28, P_Cn, 30, P_Po, 32, P_Cn,
    0, P_Mn, 4, P_Mc, 5, P_Lo, 52, P_Mn, 53, P_Mc, 54, P_Mn, 59, P_Mc, 60, P_Mn, 61, P_Mc, 66, P_Mn, 67, P_Mc, 69, P_Lo, 76, P_Cn, 80, P_Nd, 90, P_Po, 97, P_So, 107, P_Mn, 116, P_So, 125, P_Cn,
    0, P_Ll, 44, P_Lm, 98, P_Ll, 120, P_Lm, 121, P_Ll, 155, P_Lm, 192, P_Mn, 203, P_Cn, 254, P_Mn,
    0, P_Lul, 150, P_Ll, 156, P_Cn, 160, P_Lul, 250, P_Cn,
    0, P_Ll, 8, P_Lu, 16, P_Ll, 22, P_Cn, 24, P_Lu, 30, P_Cn, 32, P_Ll, 40, P_Lu, 48, P_Ll, 56, P_Lu, 64, P_Ll, 70, P_Cn, 72, P_Lu, 78, P_Cn, 80, P_Ll, 88, P_Cn, 89, P_Lu, 90, P_Cn, 91, P_Lu, 92, P_Cn, 93, P_Lu, 94, P_Cn, 95, P_Lul, 97, P_Ll, 104, P_Lu, 112, P_Ll, 126, P_Cn, 128, P_Ll, 136, P_Lt, 144, P_Ll, 152, P_Lt, 160, P_Ll, 168, P_Lt, 176, P_Ll, 181, P_Cn, 182, P_Ll, 184, P_Lu, 188, P_Lt, 189, P_Sk, 190, P_Ll, 191, P_Sk, 194, P_Ll, 197, P_Cn, 198, P_Ll, 200, P_Lu, 204, P_Lt, 205, P_Sk, 208, P_Ll, 212, P_Cn, 214, P_Ll, 216, P_Lu, 220, P_Cn, 221, P_Sk, 224, P_Ll, 232, P_Lu, 237, P_Sk, 240, P_Cn, 242, P_Ll, 245, P_Cn, 246, P_Ll, 248, P_Lu, 252, P_Lt, 253, P_Sk, 255, P_Cn,
    0, P_Zs, 11, P_Cf, 16, P_Pd, 22, P_Po, 24, P_Pi, 25, P_Pf, 26, P_Ps, 27, P_Pi, 29, P_Pf, 30, P_Ps, 31, P_Pi, 32, P_Po, 40, P_Zl, 41, P_Zp, 42, P_Cf, 47, P_Zs, 48, P_Po, 57, P_Pi, 58, P_Pf, 59, P_Po, 63, P_Pc, 65, P_Po, 68, P_Sm, 69, P_Ps, 70, P_Pe, 71, P_Po, 82, P_Sm, 83, P_Po, 84, P_Pc, 85, P_Po, 95, P_Zs, 96, P_Cf, 100, P_Cn, 106, P_Cf, 112, P_No, 113, P_Ll, 114, P_Cn, 116, P_No, 122, P_Sm, 125, P_Ps, 126, P_Pe, 127, P_Ll, 128, P_No, 138, P_Sm, 141, P_Ps, 142, P_Pe, 143, P_Cn, 144, P_Lm, 149, P_Cn, 160, P_Sc, 182, P_Cn, 208, P_Mn, 221, P_Me, 225, P_Mn, 226, P_Me, 229, P_Mn, 240, P_Cn,
    0, P_So, 2, P_Lu, 3, P_So, 7, P_Lu, 8, P_So, 10, P_Ll, 11, P_Lu, 14, P_Ll, 16, P_Lu, 19, P_Ll, 20, P_So, 21, P_Lu, 22, P_So, 25, P_Lu, 30, P_So, 36, P_Lu, 37, P_So, 38, P_Lu, 39, P_So, 40, P_Lu, 41, P_So, 42, P_Lu, 46, P_So, 47, P_Ll, 48, P_Lu, 52, P_Ll, 53, P_Lo, 57, P_Ll, 58, P_So, 60, P_Ll, 62, P_Lu, 64, P_Sm, 69, P_Lul, 71, P_Ll, 74, P_So, 75, P_Sm, 76, P_So, 78, P_Ll, 79, P_Cn, 83, P_No, 96, P_Nl, 131, P_Lul, 133, P_Cn, 144, P_Sm, 149, P_So, 154, P_Sm, 156, P_So, 160, P_Sm, 161, P_So, 163, P_Sm, 164, P_So, 166, P_Sm, 167, P_So, 174, P_Sm, 175, P_So, 206, P_Sm, 208, P_So, 210, P_Sm, 211, P_So, 212, P_Sm, 213, P_So, 244, P_Sm,
    0, P_Sm,
    0, P_So, 8, P_Sm, 12, P_So, 32, P_Sm, 34, P_So, 41, P_Ps, 42, P_Pe, 43, P_So, 124, P_Sm, 125, P_So, 155, P_Sm, 180, P_So, 220, P_Sm, 226, P_So, 232, P_Cn,
    0, P_So, 39, P_Cn, 64, P_So, 75, P_Cn, 96, P_No, 156, P_So, 234, P_No,
    0, P_So, 183, P_Sm, 184, P_So, 193, P_Sm, 194, P_So, 248, P_Sm,
    0, P_So, 111, P_Sm, 112, P_So, 157, P_Cn, 160, P_So, 179, P_Cn,
    0, P_Cn, 1, P_So, 5, P_Cn, 6, P_So, 10, P_Cn, 12, P_So, 40, P_Cn, 41, P_So, 76, P_Cn, 77, P_So, 78, P_Cn, 79, P_So, 83, P_Cn, 86, P_So, 87, P_Cn, 88, P_So, 95, P_Cn, 97, P_So, 104, P_Ps, 105, P_Pe, 106, P_Ps, 107, P_Pe, 108, P_Ps, 109, P_Pe, 110, P_Ps, 111, P_Pe, 112, P_Ps, 113, P_Pe, 114, P_Ps, 115, P_Pe, 116, P_Ps, 117, P_Pe, 118, P_No, 148, P_So, 149, P_Cn, 152, P_So, 176, P_Cn, 177, P_So, 191, P_Cn, 192, P_Sm, 197, P_Ps, 198, P_Pe, 199, P_Sm, 203, P_Cn, 208, P_Sm, 230, P_Ps, 231, P_Pe, 232, P_Ps, 233, P_Pe, 234, P_Ps, 235, P_Pe, 236, P_Cn, 240, P_Sm,
    0, P_So,
    0, P_Sm, 131, P_Ps, 132, P_Pe, 133, P_Ps, 134, P_Pe, 135, P_Ps, 136, P_Pe, 137, P_Ps, 138, P_Pe, 139, P_Ps, 140, P_Pe, 141, P_Ps, 142, P_Pe, 143, P_Ps, 144, P_Pe, 145, P_Ps, 146, P_Pe, 147, P_Ps, 148, P_Pe, 149, P_Ps, 150, P_Pe, 151, P_Ps, 152, P_Pe, 153, P_Sm, 216, P_Ps, 217, P_Pe, 218, P_Ps, 219, P_Pe, 220, P_Sm, 252, P_Ps, 253, P_Pe, 254, P_Sm,
    0, P_So, 27, P_Cn, 32, P_So, 36, P_Cn,
    0, P_Lu, 47, P_Cn, 48, P_Ll, 95, P_Cn, 96, P_Lul, 98, P_Lu, 101, P_Ll, 103, P_Lul, 109, P_Cn, 116, P_Ll, 117, P_Lul, 119, P_Ll, 120, P_Cn, 128, P_Lul, 228, P_Ll, 229, P_So, 235, P_Cn, 249, P_Po, 253, P_No, 254, P_Po,
    0, P_Ll, 38, P_Cn, 48, P_Lo, 102, P_Cn, 111, P_Lm, 112, P_Cn, 128, P_Lo, 151, P_Cn, 160, P_Lo, 167, P_Cn, 168, P_Lo, 175, P_Cn, 176, P_Lo, 183, P_Cn, 184, P_Lo, 191, P_Cn, 192, P_Lo, 199, P_Cn, 200, P_Lo, 207, P_Cn, 208, P_Lo, 215, P_Cn, 216, P_Lo, 223, P_Cn,
    0, P_Po, 2, P_Pi, 3, P_Pf, 4, P_Pi, 5, P_Pf, 6, P_Po, 9, P_Pi, 10, P_Pf, 11, P_Po, 12, P_Pi, 13, P_Pf, 14, P_Po, 23, P_Pd, 24, P_Cn, 28, P_Pi, 29, P_Pf, 30, P_Cn, 128, P_So, 154, P_Cn, 155, P_So, 244, P_Cn,
    0, P_So, 214, P_Cn, 240, P_So, 252, P_Cn,
    0, P_Zs, 1, P_Po, 4, P_So, 5, P_Lm, 6, P_Lo, 7, P_Nl, 8, P_Ps, 9, P_Pe, 10, P_Ps, 11, P_Pe, 12, P_Ps, 13, P_Pe, 14, P_Ps, 15, P_Pe, 16, P_Ps, 17, P_Pe, 18, P_So, 20, P_Ps, 21, P_Pe, 22, P_Ps, 23, P_Pe, 24, P_Ps, 25, P_Pe, 26, P_Ps, 27, P_Pe, 28, P_Pd, 29, P_Ps, 30, P_Pe, 32, P_So, 33, P_Nl, 42, P_Mn, 48, P_Pd, 49, P_Lm, 54, P_So, 56, P_Nl, 59, P_Lm, 60, P_Lo, 61, P_Po, 62, P_So, 64, P_Cn, 65, P_Lo, 151, P_Cn, 153, P_Mn, 155, P_Sk, 157, P_Lm, 159, P_Lo, 160, P_Pd, 161, P_Lo, 251, P_Po, 252, P_Lm, 255, P_Lo,
    0, P_Cn, 5, P_Lo, 45, P_Cn, 49, P_Lo, 143, P_Cn, 144, P_So, 146, P_No, 150, P_So, 160, P_Lo, 184, P_Cn, 192, P_So, 208, P_Cn, 240, P_Lo,
    0, P_So, 31, P_Cn, 32, P_No, 42, P_So, 68, P_Cn, 80, P_So, 81, P_No, 96, P_So, 128, P_No, 138, P_So, 177, P_No, 192, P_So, 255, P_Cn,
    0, P_Lo, 1, P_Cn,
    0, P_Cn, 181, P_Lo, 182, P_Cn, 192, P_So,
    0, P_Cn, 187, P_Lo, 188, P_Cn,
    0, P_Lo, 21, P_Lm, 22, P_Lo,
    0, P_Lo, 141, P_Cn, 144, P_So, 199, P_Cn,
    0, P_Sk, 23, P_Lm, 27, P_Cn, 32, P_Sk, 34, P_Cn,
    0, P_Lo, 2, P_Mc, 3, P_Lo, 6, P_Mn, 7, P_Lo, 11, P_Mn, 12, P_Lo, 35, P_Mc, 37, P_Mn, 39, P_Mc, 40, P_So, 44, P_Cn, 64, P_Lo, 116, P_Po, 120, P_Cn,
    0, P_Cn, 163, P_Lo, 164, P_Cn,
    0, P_Cs, 1, P_Cn,
    0, P_Cn, 127, P_Cs, 129, P_Cn, 255, P_Cs,
    0, P_Cn, 255, P_Cs,
    0, P_Co, 1, P_Cn,
    0, P_Cn, 255, P_Co,
    0, P_Lo, 46, P_Cn, 48, P_Lo, 107, P_Cn, 112, P_Lo, 218, P_Cn,
    0, P_Ll, 7, P_Cn, 19, P_Ll, 24, P_Cn, 29, P_Lo, 30, P_Mn, 31, P_Lo, 41, P_Sm, 42, P_Lo, 55, P_Cn, 56, P_Lo, 61, P_Cn, 62, P_Lo, 63, P_Cn, 64, P_Lo, 66, P_Cn, 67, P_Lo, 69, P_Cn, 70, P_Lo, 178, P_Cn, 211, P_Lo,
    0, P_Lo, 62, P_Ps, 63, P_Pe, 64, P_Cn, 80, P_Lo, 144, P_Cn, 146, P_Lo, 200, P_Cn, 240, P_Lo, 252, P_Sc, 253, P_So, 254, P_Cn,
    0, P_Mn, 16, P_Po, 23, P_Ps, 24, P_Pe, 25, P_Po, 26, P_Cn, 32, P_Mn, 36, P_Cn, 48, P_Po, 49, P_Pd, 51, P_Pc, 53, P_Ps, 54, P_Pe, 55, P_Ps, 56, P_Pe, 57, P_Ps, 58, P_Pe, 59, P_Ps, 60, P_Pe, 61, P_Ps, 62, P_Pe, 63, P_Ps, 64, P_Pe, 65, P_Ps, 66, P_Pe, 67, P_Ps, 68, P_Pe, 69, P_Po, 71, P_Ps, 72, P_Pe, 73, P_Po, 77, P_Pc, 80, P_Po, 83, P_Cn, 84, P_Po, 88, P_Pd, 89, P_Ps, 90, P_Pe, 91, P_Ps, 92, P_Pe, 93, P_Ps, 94, P_Pe, 95, P_Po, 98, P_Sm, 99, P_Pd, 100, P_Sm, 103, P_Cn, 104, P_Po, 105, P_Sc, 106, P_Po, 108, P_Cn, 112, P_Lo, 117, P_Cn, 118, P_Lo, 253, P_Cn, 255, P_Cf,
    0, P_Cn, 1, P_Po, 4, P_Sc, 5, P_Po, 8, P_Ps, 9, P_Pe, 10, P_Po, 11, P_Sm, 12, P_Po, 13, P_Pd, 14, P_Po, 16, P_Nd, 26, P_Po, 28, P_Sm, 31, P_Po, 33, P_Lu, 59, P_Ps, 60, P_Po, 61, P_Pe, 62, P_Sk, 63, P_Pc, 64, P_Sk, 65, P_Ll, 91, P_Ps, 92, P_Sm, 93, P_Pe, 94, P_Sm, 95, P_Ps, 96, P_Pe, 97, P_Po, 98, P_Ps, 99, P_Pe, 100, P_Po, 102, P_Lo, 112, P_Lm, 113, P_Lo, 158, P_Lm, 160, P_Lo, 191, P_Cn, 194, P_Lo, 200, P_Cn, 202, P_Lo, 208, P_Cn, 210, P_Lo, 216, P_Cn, 218, P_Lo, 221, P_Cn, 224, P_Sc, 226, P_Sm, 227, P_Sk, 228, P_So, 229, P_Sc, 231, P_Cn, 232, P_So, 233, P_Sm, 237, P_So, 239, P_Cn, 249, P_Cf, 252, P_So, 254, P_Cn,
    0, P_Lo, 12, P_Cn, 13, P_Lo, 39, P_Cn, 40, P_Lo, 59, P_Cn, 60, P_Lo, 62, P_Cn, 63, P_Lo, 78, P_Cn, 80, P_Lo, 94, P_Cn, 128, P_Lo, 251, P_Cn,
    0, P_Po, 2, P_So, 3, P_Cn, 7, P_No, 52, P_Cn, 55, P_So, 64, P_Nl, 117, P_No, 121, P_So, 138, P_No, 139, P_Cn,
    0, P_Lo, 31, P_Cn, 32, P_No, 36, P_Cn, 48, P_Lo, 65, P_Nl, 66, P_Lo, 74, P_Nl, 75, P_Cn, 128, P_Lo, 158, P_Cn, 159, P_Po, 160, P_Lo, 196, P_Cn, 200, P_Lo, 208, P_Po, 209, P_Nl, 214, P_Cn,
    0, P_Lu, 40, P_Ll, 80, P_Lo, 158, P_Cn, 160, P_Nd, 170, P_Cn,
    0, P_Lo, 6, P_Cn, 8, P_Lo, 9, P_Cn, 10, P_Lo, 54, P_Cn, 55, P_Lo, 57, P_Cn, 60, P_Lo, 61, P_Cn, 63, P_Lo, 64, P_Cn,
    0, P_Lo, 22, P_No, 26, P_Cn, 31, P_Po, 32, P_Cn,
    0, P_Lo, 1, P_Mn, 4, P_Cn, 5, P_Mn, 7, P_Cn, 12, P_Mn, 16, P_Lo, 20, P_Cn, 21, P_Lo, 24, P_Cn, 25, P_Lo, 52, P_Cn, 56, P_Mn, 59, P_Cn, 63, P_Mn, 64, P_No, 72, P_Cn, 80, P_Po, 89, P_Cn,
    0, P_Lo, 111, P_Cn,
    0, P_Nl, 99, P_Cn, 112, P_Po, 116, P_Cn,
    0, P_So, 246, P_Cn,
    0, P_So, 39, P_Cn, 42, P_So, 101, P_Mc, 103, P_Mn, 106, P_So, 109, P_Mc, 115, P_Cf, 123, P_Mn, 131, P_So, 133, P_Mn, 140, P_So, 170, P_Mn, 174, P_So, 222, P_Cn,
    0, P_So, 66, P_Mn, 69, P_So, 70, P_Cn,
    0, P_So, 87, P_Cn, 96, P_No, 114, P_Cn,
    0, P_Lu, 26, P_Ll, 52, P_Lu, 78, P_Ll, 85, P_Cn, 86, P_Ll, 104, P_Lu, 130, P_Ll, 156, P_Lu, 157, P_Cn, 158, P_Lu, 160, P_Cn, 162, P_Lu, 163, P_Cn, 165, P_Lu, 167, P_Cn, 169, P_Lu, 173, P_Cn, 174, P_Lu, 182, P_Ll, 186, P_Cn, 187, P_Ll, 188, P_Cn, 189, P_Ll, 196, P_Cn, 197, P_Ll, 208, P_Lu, 234, P_Ll,
    0, P_Ll, 4, P_Lu, 6, P_Cn, 7, P_Lu, 11, P_Cn, 13, P_Lu, 21, P_Cn, 22, P_Lu, 29, P_Cn, 30, P_Ll, 56, P_Lu, 58, P_Cn, 59, P_Lu, 63, P_Cn, 64, P_Lu, 69, P_Cn, 70, P_Lu, 71, P_Cn, 74, P_Lu, 81, P_Cn, 82, P_Ll, 108, P_Lu, 134, P_Ll, 160, P_Lu, 186, P_Ll, 212, P_Lu, 238, P_Ll,
    0, P_Ll, 8, P_Lu, 34, P_Ll, 60, P_Lu, 86, P_Ll, 112, P_Lu, 138, P_Ll, 166, P_Cn, 168, P_Lu, 193, P_Sm, 194, P_Ll, 219, P_Sm, 220, P_Ll, 226, P_Lu, 251, P_Sm, 252, P_Ll,
    0, P_Ll, 21, P_Sm, 22, P_Ll, 28, P_Lu, 53, P_Sm, 54, P_Ll, 79, P_Sm, 80, P_Ll, 86, P_Lu, 111, P_Sm, 112, P_Ll, 137, P_Sm, 138, P_Ll, 144, P_Lu, 169, P_Sm, 170, P_Ll, 195, P_Sm, 196, P_Ll, 202, P_Lul, 204, P_Cn, 206, P_Nd,
    0, P_Cn, 214, P_Lo, 215, P_Cn,
    0, P_Lo, 30, P_Cn,
    0, P_Cn, 1, P_Cf, 2, P_Cn, 32, P_Cf, 128, P_Cn,
    0, P_Mn, 240, P_Cn,
    0, P_Cn, 253, P_Co, 254, P_Cn,
};
const unsigned int UnicodeProperty::property_offsets[] = {
    0x0, 2, 122,
    0x100, 122, 248,
    0x200, 248, 290,
    0x300, 290, 356,
    0x400, 356, 382,
    0x500, 382, 436,
    0x600, 436, 510,
    0x700, 510, 554,
    0x800, 0, 2,
    0x900, 554, 678,
    0xA00, 678, 818,
    0xB00, 818, 972,
    0xC00, 972, 1094,
    0xD00, 1094, 1192,
    0xE00, 1192, 1300,
    0xF00, 1300, 1382,
    0x1000, 1382, 1436,
    0x1100, 1436, 1448,
    0x1200, 1448, 1502,
    0x1300, 1502, 1534,
    0x1400, 1534, 1538,
    0x1500, 1538, 1540,
    0x1600, 1540, 1566,
    0x1700, 1566, 1640,
    0x1800, 1640, 1670,
    0x1900, 1670, 1730,
    0x1A00, 1730, 1742,
    0x1B00, 1742, 1780,
    0x1C00, 0, 2,
    0x1D00, 1780, 1798,
    0x1E00, 1798, 1808,
    0x1F00, 1808, 1936,
    0x2000, 1936, 2050,
    0x2100, 2050, 2174,
    0x2200, 2174, 2176,
    0x2300, 2176, 2206,
    0x2400, 2206, 2220,
    0x2500, 2220, 2232,
    0x2600, 2232, 2244,
    0x2700, 2244, 2350,
    0x2800, 2350, 2352,
    0x2900, 2352, 2416,
    0x2A00, 2174, 2176,
    0x2B00, 2416, 2424,
    0x2C00, 2424, 2464,
    0x2D00, 2464, 2512,
    0x2E00, 2512, 2554,
    0x2F00, 2554, 2562,
    0x3000, 2562, 2664,
    0x3100, 2664, 2690,
    0x3200, 2690, 2716,
    0x3300, 2350, 2352,
    0x3400, 2716, 2720,
    0x3500, 0, 2,
    0x4D00, 2720, 2728,
    0x4E00, 2716, 2720,
    0x4F00, 0, 2,
    0x9F00, 2728, 2734,
    0xA000, 2734, 2740,
    0xA100, 1538, 1540,
    0xA400, 2740, 2748,
    0xA500, 0, 2,
    0xA700, 2748, 2758,
    0xA800, 2758, 2788,
    0xA900, 0, 2,
    0xAC00, 2716, 2720,
    0xAD00, 0, 2,
    0xD700, 2788, 2794,
    0xD800, 2794, 2798,
    0xD900, 0, 2,
    0xDB00, 2798, 2806,
    0xDC00, 2794, 2798,
    0xDD00, 0, 2,
    0xDF00, 2806, 2810,
    0xE000, 2810, 2814,
    0xE100, 0, 2,
    0xF800, 2814, 2818,
    0xF900, 1538, 1540,
    0xFA00, 2818, 2830,
    0xFB00, 2830, 2872,
    0xFC00, 1538, 1540,
    0xFD00, 2872, 2896,
    0xFE00, 2896, 3008,
    0xFF00, 3008, 3128,
    0x10000, 3128, 3156,
    0x10100, 3156, 3178,
    0x10200, 0, 2,
    0x10300, 3178, 3214,
    0x10400, 3214, 3226,
    0x10500, 0, 2,
    0x10800, 3226, 3250,
    0x10900, 3250, 3260,
    0x10A00, 3260, 3298,
    0x10B00, 0, 2,
    0x12000, 1538, 1540,
    0x12300, 3298, 3302,
    0x12400, 3302, 3310,
    0x12500, 0, 2,
    0x1D000, 3310, 3314,
    0x1D100, 3314, 3344,
    0x1D200, 3344, 3352,
    0x1D300, 3352, 3360,
    0x1D400, 3360, 3416,
    0x1D500, 3416, 3470,
    0x1D600, 3470, 3502,
    0x1D700, 3502, 3544,
    0x1D800, 0, 2,
    0x20000, 2716, 2720,
    0x20100, 0, 2,
    0x2A600, 3544, 3550,
    0x2A700, 0, 2,
    0x2F800, 1538, 1540,
    0x2FA00, 3550, 3554,
    0x2FB00, 0, 2,
    0xE0000, 3554, 3564,
    0xE0100, 3564, 3568,
    0xE0200, 0, 2,
    0xF0000, 2810, 2814,
    0xF0100, 0, 2,
    0xFFF00, 3568, 3574,
    0x100000, 2810, 2814,
    0x100100, 0, 2,
    0x10FF00, 3568, 3574,
    0x110000, 0, 2,
};
const int UnicodeProperty::nproperty_offsets = (sizeof(UnicodeProperty::property_offsets) / (3*sizeof(unsigned int)));

inline const unsigned int *
UnicodeProperty::find_offset(uint32_t u)
{
    // Up to U+1A00 each page has its own definition.
    if (u < 0x1A00)
	return &property_offsets[3*(u >> 8)];
    // At or after U+1A00, binary search.
    int l = 0x1A, r = nproperty_offsets - 2;
    while (l <= r) {
	int m = (l + r) / 2;
	const unsigned int *ptr = &property_offsets[3*m];
	if (u < ptr[0])
	    r = m - 1;
	else if (u >= ptr[3])
	    l = m + 1;
	else
	    return ptr;
    }
    // If search fails, return last record, which will be all-unassigned.
    return &property_offsets[3*(nproperty_offsets - 1)];
}

int
UnicodeProperty::property(uint32_t u)
{
    const unsigned int *offsets = find_offset(u);

    // Now we only care about the last byte.
    u &= 255;

    // Binary search within record.
    int l = offsets[1], r = offsets[2] - 4;
    const unsigned char *the_ptr;
    while (l <= r) {
	int m = ((l + r) / 2) & ~1;
	const unsigned char *ptr = &property_pages[m];
	if (u < ptr[0])
	    r = m - 2;
	else if (u >= ptr[2])
	    l = m + 2;
	else {
	    the_ptr = ptr;
	    goto found_ptr;
	}
    }
    the_ptr = &property_pages[l];

  found_ptr:
    // Found right block.
    if (the_ptr[1] == P_Lul)
	return ((u - the_ptr[0]) % 2 ? P_Ll : P_Lu);
    else
	return the_ptr[1];
}

static const char property_names[] =
    "Cn\0Co\0Cs\0Cf\0Cc\0\0\0\0\0\0\0\0\0\0"
    "Zs\0Zl\0Zp\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "Mn\0Mc\0Me\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "Lo\0Lu\0Ll\0Lt\0Lm\0\0\0\0\0\0\0\0\0\0"
    "No\0Nd\0Nl\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "Po\0Pc\0Pd\0Ps\0Pe\0Pi\0Pf\0\0\0\0"
    "So\0Sm\0Sc\0Sk";

static const char * const property_long_names[] = {
    "Unassigned", "PrivateUse", "Surrogate", "Format", "Control", 0, 0, 0,
    "SpaceSeparator", "LineSeparator", "ParagraphSeparator", 0, 0, 0, 0, 0,
    "NonspacingMark", "SpacingMark", "EnclosingMark", 0, 0, 0, 0, 0,
    "OtherLetter", "UppercaseLetter", "LowercaseLetter", "TitlecaseLetter", "ModifierLetter", 0, 0, 0,
    "OtherNumber", "DecimalNumber", "LetterNumber", 0, 0, 0, 0, 0,
    "OtherPunctuation", "ConnectorPunctuation", "DashPunctuation", "OpenPunctuation", "ClosePunctuation", "InitialPunctuation", "FinalPunctuation", 0,
    "OtherSymbol", "MathSymbol", "CurrencySymbol", "ModifierSymbol"
};

static const char * const property_category_long_names[] = {
    "Other", "Separator", "Mark", "Letter", "Number", "Punctuation", "Symbol"
};


const char *
UnicodeProperty::property_name(int p)
{
    if (p >= 0 && p <= P_Sk && property_names[p*3])
	return &property_names[p*3];
    else
	return "?";
}

bool
UnicodeProperty::parse_property(const String &s, int &prop, int &prop_mask)
{
    if (s.length() == 0)
	return false;
    else if (s.length() <= 2) {
	for (int i = 0; i <= P_S; i += 010)
	    if (property_names[3*i] == s[0]) {
		if (s.length() == 1) {
		    prop = i;
		    prop_mask = P_TMASK;
		    return true;
		}
		for (; property_names[3*i]; i++)
		    if (property_names[3*i+1] == s[1]) {
			prop = i;
			prop_mask = 0377;
			return true;
		    }
		break;
	    }
	return false;
    } else {
	const char * const *dict = property_category_long_names;
	for (int i = 0; i <= P_S; i += 010, dict++)
	    if (s == *dict) {
		prop = i;
		prop_mask = P_TMASK;
		return true;
	    }
        dict = property_long_names;
	for (int i = 0; i <= P_Sk; i++, dict++)
	    if (*dict && s == *dict) {
		prop = i;
		prop_mask = 0377;
		return true;
	    }
	return false;
    }
}

