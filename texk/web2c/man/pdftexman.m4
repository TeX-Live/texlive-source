m4_divert(-1)m4_dnl
=======================================================================
$Id: //depot/Build/source.development/TeX/texk/web2c/man/pdftexman.m4#2 $

Common man page for pdftex and pdfetex, to be preprocessed by GNU m4:

$ m4 -P -DOLD pdftexman.m4 > pdftex.man
$ m4 -P pdfetexman.m4 > pdfetex.man

=======================================================================

m4_define(`M_MANDATE', `23 March 2004')
m4_define(`M_WEB2CVERSION', `@VERSION@')

m4_ifdef(`OLD',
  `
  m4_define(`M_LTXPP', `\*(LX')
  m4_define(`M_TEXPP', `\*(TX')
  m4_define(`M_TEXP', `TeX')
  m4_define(`M_TEX', `tex')
  m4_define(`M_PDFTEX', `PDFTEX')
  m4_define(`M_PDFENGINI', `pdfinitex')
  m4_define(`M_PDFENGVIR', `pdfvirtex')
  m4_define(`M_PDFENGPP', `pdf\*(TX')
  m4_define(`M_PDFENGKP', `pdfTeXk')
  m4_define(`M_PDFENGP', `pdfTeX')
  m4_define(`M_PDFENG', `pdftex')
  m4_define(`M_ENGINI', `initex')
  m4_define(`M_ENGVIR', `virtex')
  m4_define(`M_PDFLTX', `pdflatex')
  m4_define(`M_FMT', `fmt')
  ', `
  m4_define(`M_LTXPP', `e-\*(LX')
  m4_define(`M_TEXPP', `e-\*(TX')
  m4_define(`M_TEXP', `e-TeX')
  m4_define(`M_TEX', `etex')
  m4_define(`M_PDFTEX', `PDFETEX')
  m4_define(`M_PDFENGINI', `pdfeinitex')
  m4_define(`M_PDFENGVIR', `pdfevirtex')
  m4_define(`M_PDFENGPP', `pdfe\*(TX')
  m4_define(`M_PDFENGKP', `pdfeTeXk')
  m4_define(`M_PDFENGP', `pdfeTeX')
  m4_define(`M_PDFENG', `pdfetex')
  m4_define(`M_ENGINI', `einitex')
  m4_define(`M_ENGVIR', `evirtex')
  m4_define(`M_PDFLTX', `pdfelatex')
  m4_define(`M_FMT', `efmt')
  '
)
m4_divert`'m4_dnl
.TH M_PDFTEX 1 "M_MANDATE" "Web2C M_WEB2CVERSION"
.\"=====================================================================
.if n .ds MF Metafont
.if t .ds MF M\s-2ETAFONT\s0
.if t .ds TX \fRT\\h'-0.1667m'\\v'0.20v'E\\v'-0.20v'\\h'-0.125m'X\fP
.if n .ds TX TeX
.if n .ds MF Metafont
.if t .ds MF M\s-2ETAFONT\s0
.ie t .ds OX \fIT\v'+0.25m'E\v'-0.25m'X\fP\" for troff
.el .ds OX TeX\" for nroff
.\" the same but obliqued
.\" BX definition must follow TX so BX can use TX
.if t .ds BX \fRB\s-2IB\s0\fP\*(TX
.if n .ds BX BibTeX
.\" LX definition must follow TX so LX can use TX
.if t .ds LX \fRL\\h'-0.36m'\\v'-0.15v'\s-2A\s0\\h'-0.15m'\\v'0.15v'\fP\*(TX
.if n .ds LX LaTeX
.if t .ds AX \fRA\\h'-0.1667m'\\v'0.20v'M\\v'-0.20v'\\h'-0.125m'S\fP\*(TX
.if n .ds AX AmSTeX
.if t .ds AY \fRA\\h'-0.1667m'\\v'0.20v'M\\v'-0.20v'\\h'-0.125m'S\fP\*(LX
.if n .ds AY AmSLaTeX
.\"=====================================================================

.SH NAME
M_PDFENG \- Direct PDF or DVI output from M_TEXP
.\"=====================================================================

.SH SYNTAX
.B M_PDFENG
[\fIoption\fR] ... [\fItexname\fR[\fI.tex\fR]] ... [\fIcommands\fR]
.br
.B M_PDFENG
[\fIoption\fR] ... \e\fIfirst-line\fR
.br
.B M_PDFENG
[\fIoption\fR] ... &\fI`'M_FMT args\fR
.br
.B M_PDFENG \-\-version
.br
.B M_PDFENG \-\-help
.\"=====================================================================

.SH USAGE
Run M_PDFENGPP on
.IR texname ,
usually creating
.I texname.pdf
or
.IR texname.dvi .
Any remaining
.I commands
are processed as M_PDFENGPP input, after
.I texname
is read.
.P
If the first line of
.I texname
is
.IR "%&M_FMT" ,
where
.I M_FMT
is an existing format
.RI ( .M_FMT )
file, and if parsing of the first line is enabled, use this format.
Else use
.IR "name.M_FMT" ,
where
.I name
is the program invocation name, most commonly M_PDFENG.
.P
Alternatively, if the first non-option argument begins with a backslash
.IR \e ,
interpret all non-option arguments as a line of M_PDFENGPP input.
.P
Alternatively, if the first non-option argument begins with an ampersand
.IR & ,
the next word is taken as the format
.I M_FMT
to read, overriding all else.
Any remaining arguments
.I args
are processed as above.
.P
If no arguments or options are specified, prompt for input.
.P
M_PDFENGPP's handling of its command line arguments is similar to
that of the other \*(TX programs in the
.I WEB2C
implementation.
.P
Non-alphanumeric characters on the command line
might need quoting, depending on the shell used.
.\"=====================================================================

.SH DESCRIPTION
This manual page is not meant to be exhaustive.
The complete documentation for this version of a \*(TX engine
can be found in the manual
.IR "The pdfTeX user manual"
and in the info file or manual
.IR "Web2C: A TeX implementation" .
.P
M_PDFENGPP is a version of M_TEXPP that can create
.I PDF
files as well as
.I DVI
files.
In
.I DVI
mode,
M_PDFENGPP can be used as complete replacement for the M_TEXPP engine.
The selection of
.I PDF
or
.I DVI
mode is done by the
.B --output-format
option or by setting the
.BI \epdfoutput
register.
.P
The typical use of M_PDFENGPP is with a pregenerated format
for which PDF output has been enabled.
The
.B M_PDFENG
command uses the equivalent of the plain M_TEXPP format, and the
.B M_PDFLTX
command uses the equivalent of the M_LTXPP format.
.P
M_PDFENGPP can also be used for generating format
.RI ( .M_FMT )
files by call
.BR "M_PDFENG --ini" .
This gives the same functionality as the former
.B M_PDFENGINI
command, which is considered as deprecated.
.\" The
.\" .B M_PDFENGINI
.\" and
.\" .B M_PDFENGVIR
.\" commands are M_PDFENGPP's analogues to the
.\" .B M_ENGINI
.\" and
.\" .B M_ENGVIR
.\" commands.  In this installation, they are symlinks to the
.\" .B M_PDFENG
.\" executable.
.P
In
.I PDF
mode, M_PDFENGPP can natively handle graphics input formats
.IR PNG ,
.IR JPEG ,
and
.IR PDF .
m4_ifdef(`OLD', `m4_dnl
.P
Nowadays M_PDFENGPP usually is just a link to the pdfe\*(TX program.
', `')m4_dnl
.\"=====================================================================

.SH OPTIONS
This version of M_PDFENGPP understands the following command line options,
which can be also given starting with a single dash.
m4_dnl------------------------------------------------------------------
.TP
.BI --default-translate-file \ tcxname
.rb
Use the default translation table from file
.IR tcxname .
This option does not override a setting given in a
.I %&
line.
The
.BI --translate-file
can overwrite this setting.
m4_dnl------------------------------------------------------------------
.TP
m4_ifdef(`OLD', `', `m4_dnl
.BI --M_FMT \ M_FMT
.rb
Use
.I M_FMT
as the name of the format to be used, instead of the name by which
M_PDFENGPP was called or a
.I %&
line.
m4_dnl------------------------------------------------------------------
.TP
')m4_dnl
.B --enc
.rb
Enable enc\*(TX extension by Petr Olsak, see the file
.IR "encdoc-e.pdf" .
m4_dnl------------------------------------------------------------------
.TP
.B --file-line-error-style
.rb
Print error messages in the form
.I file:line:error
which is similar to the way many compilers format them.
m4_dnl------------------------------------------------------------------
.TP
m4_ifdef(`OLD', `m4_dnl
.BI --M_FMT \ M_FMT
.rb
Use
.I M_FMT
as the name of the format to be used, instead of the name by which
M_PDFENGPP was called or a
.I %&
line.
m4_dnl------------------------------------------------------------------
.TP
', `')m4_dnl
.B --help
.rb
Print help message with list of command line options and exit.
m4_dnl------------------------------------------------------------------
.TP
.B --ini
.rb
Start in
.I INI
mode, for dumping a format M_FMT.
The
.I INI
mode can als be used for general typesetting.
As no format is preloaded,
basic initializations like setting catcodes might be required.
\." ; this is implicitly true if the program is called
\." as
\." .BR M_PDFENGINI .
m4_dnl------------------------------------------------------------------
.TP
.BI --interaction \ mode
.rb
Sets the interaction mode.  The mode can be either
.IR batchmode ,
.IR nonstopmode ,
.IR scrollmode ,
and
.IR errorstopmode .
The meaning of these modes is the same as that of the corresponding
\ecommands.
m4_dnl------------------------------------------------------------------
.TP
.B --ipc
.rb
In
.I DVI mode,
send
.I DVI
output to a socket as well as the usual output file.
Whether this option is available is the choice of the installer.
This option is ignored in
.I PDF
mode.
m4_dnl------------------------------------------------------------------
.TP
.B --ipc-start
.rb
In
.DVI
mode, works as
.BR --ipc ,
and starts the server at the other end as well.
Whether this option is available is the choice of the installer.
This option is ignored in
.I PDF
mode.
m4_dnl------------------------------------------------------------------
.TP
.BI --jobname \ name
.rb
Use
.I name
for the job name, instead of deriving it from the name of the input file.
m4_dnl------------------------------------------------------------------
.TP
.BI --kpathsea-debug \ bitmask
.rb
Sets path searching debugging flags according to the bitmask.  See the
.I Kpathsea
manual for details.
m4_dnl------------------------------------------------------------------
.TP
.BI --mktex \ fmt
.rb
Enable
.RI mktex fmt ,
where
.I fmt
must be either
.I tex
or
.IR tfm .
m4_dnl------------------------------------------------------------------
.TP
.BI --mltex
.rb
Enable MLTeX extensions such as
.IR \echarsubdef .
m4_dnl------------------------------------------------------------------
.TP
.BI --no-mktex \ fmt
.rb
Disable
.RI mktex fmt ,
where
.I fmt
must be either
.I tex
or
.IR tfm .
m4_dnl------------------------------------------------------------------
.TP
.BI --output-comment \ string
.rb
In
.I DVI
mode, use
.I string
for the DVI file comment instead of the date.
This option is ignored in
.I PDF
mode.
m4_dnl------------------------------------------------------------------
.TP
.BI --output-format \ format
.rb
Set output format mode,
where
.I format
must be either
.I pdf
or
.IR dvi .
This option can be overridden by setting the
.BI \epdfoutput
register.
The output format mode also influences the set of graphics formats
understood by M_PDFENGPP.
m4_dnl------------------------------------------------------------------
.TP
.B --parse-first-line
.rb
If the first line of the main input file begins with
.IR %& ,
parse it to look for a dump name or a
.B --translate-file
option.
m4_dnl------------------------------------------------------------------
.TP
.BI --progname \ name
.rb
Pretend to be program
.IR name .
This affects both the format used and the search paths.
.TP
.B --recorder
.rb
Enable the filename recorder.
This leaves a trace of the files opened for input and output
in a file with extension
.IR .fls .
m4_dnl------------------------------------------------------------------
.TP
.B --shell-escape
.rb
Enable the
.BI \ewrite18{ command }
construct.  The
.I command
can be any shell command.
This construct is often disallowed for security reasons.
m4_dnl------------------------------------------------------------------
.TP
.B --src-specials
.rb
In
.I DVI
mode, insert source specials into the
.I DVI
file.
This option is ignored in
.I PDF
mode.
m4_dnl------------------------------------------------------------------
.TP
.BI --src-specials \ where
.rb
In
.I DVI
mode, insert source specials in certain placed of the
.I DVI
file.
.I where
is a comma-separated value list:
.IR cr ,
.IR display ,
.IR hbox ,
.IR math ,
.IR par ,
.IR parent ,
or
.IR vbox .
This option is ignored in
.I PDF
mode.
m4_dnl------------------------------------------------------------------
.TP
.BI --translate-file \ tcxname
.rb
Use the translation table in file
.I tcxname
for mapping of input characters
and re-mapping of output characters.
This option overrides a setting given in a
.I %&
line.
m4_dnl------------------------------------------------------------------
.TP
.B --version
.rb
Print version and copyright information and exit.
.\"=====================================================================

.SH ENVIRONMENT
See the
.I Kpathsea library
documentation, section ```Path searching'''
for precise details of how the environment variables are used.
The
.B kpsewhich
utility can be used to query the values of the variables.
.P
One caveat:
In most M_PDFENGPP formats, you cannot use ~ in a filename
you give directly to M_PDFENGPP, because ~ is an active character,
and hence is expanded, not taken as part of the filename.
Other programs, such as \*(MF, do not have this problem.
.P
.TP
TEXMFOUTPUT
Normally, M_PDFENGPP puts its output files in the current directory.
If any output file cannot be opened there, it tries to open it in the
directory specified in the environment variable TEXMFOUTPUT.
There is no default value for that variable.  For example, if you say
.I M_PDFENG paper
and the current directory is not writable, if TEXMFOUTPUT has
the value
.IR /tmp ,
M_PDFENGPP attempts to create
.I /tmp/paper.log
(and
.IR /tmp/paper.pdf ,
if any output is produced.)
.TP
TEXINPUTS
Search path for files read in by
.I \einput
and
.I \eopenin
commands,
as well as for graphics files.
This should probably start with ```.''', so
that user files are found before system files.  An empty path
component will be replaced with the paths defined in the
.I texmf.cnf
file.  For example, set TEXINPUTS to ".:/home/usr/tex:" to prepend the
current direcory and ```/home/user/tex''' to the standard search path.
.TP
TEXFORMATS
Search path for format
.RI ( .M_FMT )
files.
.TP
TEXPOOL
Search path for string pool
.RI ( .pool )
files.
.TP
ENCFONTS
Search path for font encoding
.RI ( .enc )
files.
.TP
TEXFONTMAPS
Search path for font mapping
.RI ( .map )
files.
.TP
TFMFONTS
Search path for font metric
.RI ( .tfm )
files.
.TP
VFFONTS
Search path for virtual font
.RI ( .vf )
files.
.TP
T1FONTS
Search path for Type 1 font files
.RI ( .pfa )
and
.RI ( .pfb ")."
.TP
TTFONTS
Search path for True-Type font
.RI ( .ttf )
files.
.TP
PKFONTS
Search path for pixel font
.RI ( .pk )
files.
.TP
TEXEDIT
Command template for switching to editor.  The default, usually
.BR vi ,
is set when M_PDFENGPP is compiled.
.\"=====================================================================

.SH FILES
The location of the files mentioned below varies from system to
system.  Use the
.B kpsewhich
utility to find their locations.
.TP
.I texmf.cnf
Configuration file with default settings for M_PDFENGPP as well
as for other \*(TX and \*(MF related programs and utilities.
.TP
.I *.M_FMT
Predigested M_PDFENGPP format files.
.TP
.I *.pool
String pool files.
.TP
.I *.enc
Font encoding files.
.TP
.I *.map
Text files containing font mapping definitions
between TFM entries and font files.
The default file is
.IR pdftex.map .
.TP
.I *.tfm
Font metric files.
.TP
.I *.vf
Virtual font files.
.TP
.IR *.pfa , \ *.pfb
Type 1 font files (ASCII and binary format).
.TP
.I *.ttf
True-Type font files.
.TP
.I *.pk
Pixel font files.
.TP
.I *.tcx
Character translation table files.
.\"=====================================================================

.SH BUGS
This version of M_PDFENGPP implements a number of optional extensions.
In fact, many of these extensions conflict to a greater or lesser
extent with the definition of M_PDFENGPP.  When such extensions are
enabled, the banner printed when M_PDFENGPP starts is changed to print
.B M_PDFENGKP
instead of
.BR M_PDFENGP .
.P
This version of M_PDFENGPP fails to trap arithmetic overflow when
dimensions are added or subtracted.  Cases where this occurs are rare,
but when it does the generated
.I DVI
file will be invalid.  Whether a generated
.I PDF
file would be usable is unknown.
\." .P
.\"=====================================================================

.SH AVAILABILITY
M_PDFENGPP is available for a large variety of machine architectures
and operation systems.
M_PDFENGPP is part of all major \*(TX distributions.
.P
Information on how to get M_PDFENGPP and related information
is available at the
.BI "http://www.pdftex.org" \ pdf\*(TX
website.
.P
The following M_PDFENGPP related mailing list is available:
.BR "pdftex@tug.org" .
This is a mailman list;
to subscribe, send a message with the subject
.I subscribe
to
.BR pdftex-request@tug.org .
More about the list can be found at the
.B "http://tug.org/mailman/listinfo/pdftex"
.I mailing list
website.
.\"=====================================================================

.SH "SEE ALSO"
.BR tex (1),
m4_ifdef(`OLD',m4_dnl
`.BR mf (1).',m4_dnl
`.BR mf (1),
.BR etex (1).')
.\"=====================================================================

.SH AUTHORS
The authors of M_PDFENGPP are Han The Thanh and the pdf\*(TX team.
