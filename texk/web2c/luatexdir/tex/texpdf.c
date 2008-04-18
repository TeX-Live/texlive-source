/* $Id: texpdf.c 1158 2008-04-14 08:13:06Z oneiros $ */

#include "luatex-api.h"
#include <ptexlib.h>
#include <ctype.h>

#define number_chars 1114112
#define string_offset 2097152
#define str_start_macro(a) str_start[(a) - string_offset]
#define str_length(a) (str_start_macro((a)+1)-str_start_macro(a))       /* the number of characters  in string number (a) */

#define is_hex_char isxdigit

/* output a byte to PDF buffer without checking of overflow */

#define pdf_quick_out(a)  pdf_buf[pdf_ptr++] = a

/* make sure that there are at least |n| bytes free in PDF buffer */

#define pdf_room(a) do {                                        \
    if ((pdf_os_mode) && ((a) + pdf_ptr > pdf_buf_size))        \
      pdf_os_get_os_buf(a);                                     \
    else if ((!pdf_os_mode) && ((a) > pdf_buf_size) )           \
      overflow(maketexstring("PDF output buffer"), pdf_op_buf_size);            \
    else if ((!pdf_os_mode) && ((a) + pdf_ptr > pdf_buf_size))  \
      pdf_flush();                                              \
  } while (0)

/* do the same as |pdf_quick_out| and flush the PDF buffer if necessary  */

#define pdf_out(a) do {   \
    pdf_room(1);          \
    pdf_quick_out(a);     \
  } while (0)


/* print out a character to PDF buffer; the character will be printed in octal
 * form in the following cases: chars <= 32, backslash (92), left parenthesis
 * (40) and  right parenthesis (41) 
 */

#define pdf_print_escaped(c)                                            \
  if ((c)<=32||(c)=='\\'||(c)=='('||(c)==')'||(c)>127) {                \
    pdf_room(4);                                                        \
    pdf_quick_out('\\');                                                \
    pdf_quick_out('0' + (((c)>>6) & 0x3));                              \
    pdf_quick_out('0' + (((c)>>3) & 0x7));                              \
    pdf_quick_out('0' + ( (c)     & 0x7));                              \
  } else {                                                              \
    pdf_out((c));                                                       \
  }

void pdf_print_char(internal_font_number f, integer cc)
{
    register int c;
    pdf_mark_char(f, cc);
    if (font_encodingbytes(f) == 2) {
        register int chari;
        chari = char_index(f, cc);
        c = chari >> 8;
        pdf_print_escaped(c);
        c = chari & 0xFF;
    } else {
        if (cc > 255)
            return;
        c = cc;
    }
    pdf_print_escaped(c);
}

/* print out a string to PDF buffer */

void pdf_print(str_number s)
{
    if (s < number_chars) {
        assert(s < 256);
        pdf_out(s);
    } else {
        register pool_pointer j = str_start_macro(s);
        while (j < str_start_macro(s + 1)) {
            pdf_out(str_pool[j++]);
        }
    }
}

/* print out a integer to PDF buffer */

void pdf_print_int(integer n)
{
    register integer k = 0;     /*  current digit; we assume that $|n|<10^{23}$ */
    if (n < 0) {
        pdf_out('-');
        if (n < -0x7FFFFFFF) {  /* need to negate |n| more carefully */
            register integer m;
            k++;
            m = -1 - n;
            n = m / 10;
            m = (m % 10) + 1;
            if (m < 10) {
                dig[0] = m;
            } else {
                dig[0] = 0;
                incr(n);
            }
        } else {
            n = -n;
        }
    }
    do {
        dig[k++] = n % 10;
        n = n / 10;
    } while (n != 0);
    pdf_room(k);
    while (k-- > 0) {
        pdf_quick_out('0' + dig[k]);
    }
}


/* print $m/10^d$ as real */
void pdf_print_real(integer m, integer d)
{
    if (m < 0) {
        pdf_out('-');
        m = -m;
    };
    pdf_print_int(m / ten_pow[d]);
    m = m % ten_pow[d];
    if (m > 0) {
        pdf_out('.');
        d--;
        while (m < ten_pow[d]) {
            pdf_out('0');
            d--;
        }
        while (m % 10 == 0)
            m = m / 10;
        pdf_print_int(m);
    }
}

/* print out |s| as string in PDF output */

void pdf_print_str(str_number s)
{
    pool_pointer i, j;
    i = str_start_macro(s);
    j = str_start_macro(s + 1) - 1;
    if (i > j) {
        pdf_room(2);
        pdf_quick_out('(');
        pdf_quick_out(')');
        return;
    }
    /* the next is not really safe, the string could be "(a)xx(b)" */
    if ((str_pool[i] == '(') && (str_pool[j] == ')')) {
        pdf_print(s);
        return;
    }
    if ((str_pool[i] != '<') || (str_pool[j] != '>') || odd(str_length(s))) {
        pdf_out('(');
        pdf_print(s);
        pdf_out(')');
        return;
    }
    i++;
    j--;
    while (i < j) {
        if (!is_hex_char(str_pool[i++])) {
            pdf_out('(');
            pdf_print(s);
            pdf_out(')');
            return;
        }
    }
    pdf_print(s);               /* it was a hex string after all  */
}
