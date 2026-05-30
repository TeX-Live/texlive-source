# UTFpatgen
Reimplementation of the classic PatGen pattern generation program in CWEB.
UTFpatgen is a part of the TeX Live suite of programs (link 
[here](https://ctan.org/pkg/utfpatgen)).

## Instalation
We strongly recommend downloading **UTFpatgen** included in the TeX Live
distribution. However, if you wish to compile **UTFpatgen** from scratch,
you need the `ctangle` TeX utility to translate the CWEB source file to
standard C. If you have `ctangle` ready, simply run:

```
make build/utfpatgen.c
```

Once you created the C source file, you can compile it into an executable.
The `gcc` compiler is required, the only outside dependencies are the standard
C libraries.

```
make build-execute
```

## Documentation
Find the commented source code in [utfpatgen.pdf](./utfpatgen.pdf). If you wish
to compile the documentation yourself, you can do so by running:

```
make utfpatgen.pdf
```

The recipe "weaves" the CWEB source to Plain TeX and compiles it into a PDF.
Please note that `cweave` and `pdftex` TeX utilities are needed to successfully
run the recipe.

## Licence
*version 1.0 (May 2026)*

*© 2026 Ondřej Metelka, under [MIT licence](https://mit-license.org/)*

*This is free software: you are free to change and redistribute it. There is*
*NO WARRANTY, to the extent permitted by law.*
