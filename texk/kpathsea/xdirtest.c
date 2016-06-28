/* xdirtest.c: standalone program to test xdirname() and xbasename().

   Copyright 1999 Karl Berry.
   Copyright 2005 Olaf Weber.
   Copyright 2011 Peter Breitenlohner.
   Copyright 2016 Akira Kakuto.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library; if not, see <http://www.gnu.org/licenses/>.  */

#include <kpathsea/config.h>

static const char *tab[] = {
/* UNC names */
#if defined (WIN32)
    "\\\\neuromancer\\fptex\\bin\\win32\\kpsewhich.exe",
    "\\\\neuromancer\\fptex\\win32\\kpsewhich.exe",
    "\\\\neuromancer\\fptex\\kpsewhich.exe",
    "\\\\neuromancer\\kpsewhich.exe",
    "//server.host/share/file",
    "//server.host/share/file/",
    "//server.host/share/",
    "//server.host//share",	/* malformed */
    "//server.host",		/* malformed */
    "//.host/share",		/* not UNC name */
#endif
    "//neuromancer/kpsewhich.exe",
/* names with device */
#if defined (WIN32)
    "p:\\bin\\win32\\kpsewhich.exe",
    "p:\\bin\\win32\\\\",
    "p:\\win32\\kpsewhich.exe",
    "p:\\win32\\",
    "p:\\kpsewhich.exe",
    "p:\\",
    "p:bin\\win32\\kpsewhich.exe",
    "p:win32\\kpsewhich.exe",
#endif
    "p:win32//kpsewhich.exe",
    "p:win32/",
    "p:kpsewhich.exe",
    "p:///kpsewhich.exe",
/* 'normal' names */
    "/usr/bin/win32/kpsewhich.exe",
    "/usr/bin/win32//",
    "/usr/bin/kpsewhich.exe",
    "/usr/bin/",
    "///usr/kpsewhich.exe",
    "/usr/kpsewhich.exe",
    "///kpsewhich.exe",
    "/kpsewhich.exe",
    "",
    NULL
};

#if defined (WIN32)
static const char *ktab[] = {
/* UNC names */
    "\\\\serverK\\\\shareK\\\\file",
    "\\\\serverKK\\shareKK\\file",
    "\\\\serverK\\\\shareK\\",
    "\\\\serverKK\\shareKK",
/* 'normal' names */
    "\\abcK\\deKKfK\\\\hijK\\",
    "\\abcKKdeKKfKK\\hijKK",
    "\\abcK\\deKKfK\\",
    "\\abcKKdeKKfKK",
    NULL
};

static char *
to_kanji (const char *str) {
    char *p, *ret = xstrdup(str);

    for (p = ret; *p; p++)
        if (*p == 'K')
            *p = 0x81;
    
    return ret;
}

static char *
from_kanji (char *str) {
    char *p;

    for (p = str; *p; p++)
        if ((unsigned char) *p == 0x81)
            *p = 'K';
    
    return str;
}

static void
do_kanji (kpathsea kpse) {
    const char **p;

    printf("\nAssuming CP %s 932\n", kpse->Is_cp932_system ? "is" : "is not");

    for (p = ktab; *p; p++) {
        char *q = to_kanji(*p);
        char *r = xdirname(q);

        printf("%s -> %s + %s\n", *p, from_kanji(r), *p + (xbasename(q)-q));

        free (r);
        free (q);
    }
}

static void
kanji_test(kpathsea kpse) {
    int save_cp932 = kpse->Is_cp932_system;

    printf("\nTesting 2-Byte Kanji (CP 932, SJIS) codes with 'K' representing 0x81\n");

    kpse->Is_cp932_system = 932;	/* pretend CP is 932 */
    do_kanji(kpse);

    kpse->Is_cp932_system = 0;	/* pretend CP is not 932 */
    do_kanji(kpse);

    kpse->Is_cp932_system = save_cp932;
}
#endif

int main(int argc, char **argv)
{
    const char **p;
    kpathsea kpse = kpathsea_new();

    kpathsea_set_program_name (kpse, argv[0], NULL);

    printf("\n%s: name -> xdirname(name) + xbasename(name)\n\n",
           kpse->invocation_short_name);

    for (p = tab; *p; p++) {
        char *q = xdirname(*p);

        printf("%s -> %s + %s\n", *p, q, xbasename(*p));
        free (q);
    }

#if defined (WIN32)
    kanji_test(kpse);
#endif

    return 0;
}
