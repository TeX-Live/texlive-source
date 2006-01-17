#define U(a) ((unsigned char)(a))
#define twiddle(n) (( \
     (U(n)<=9)? ((n)+161): \
     (U(n)<=32? ((n)+163): \
     (U(n) == 127? 196: (n)))))

#define untwiddle(n) (( \
     ((U(n)>=161 && (U(n))<=170)? ((n)-161): \
     ((U(n)>=173 && (U(n))<=195)? ((n)-163): \
     (U(n) == 196? 127: (n))))))

