%
% routine to process JFM file format
@x
@d read_two(A) { (A)=tfbyte;
  if ( (A)>127 ) goto BAD_TFM;
  tfget; (A)=(A)*0400+tfbyte;
}
@y
@d read_two(A) { (A)=tfbyte;
  tfget; (A)=(A)*0400+tfbyte;
}
@z
