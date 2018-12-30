@x In my work (mainly TAOCP) I use \: (not \.) for the dot accent
'&`&\relax\^&"&\relax\~&=&.&u&v\cr
@y
'&`&\relax\^&"&\relax\~&=&:&u&v\cr
@z
@x
  1,1,1,1,1,0,0,1, /* \.{\\-} and \.{\\.} */
  1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1, /* \.{\\=} */
@y
  1,1,1,1,1,0,1,1, /* \.{\\-} */
  1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1, /* \.{:} and \.{\\=} */
@z
@x and I use |...xref-tag...| for cross-references
case '|': if (web>1) {
     save_skipping=skipping;
     goto skip_C_prime;
   }
@y
case '|': if (web>1) {
     save_skipping=skipping;
     goto skip_C_prime;
   } else discard_to('|');
@z
