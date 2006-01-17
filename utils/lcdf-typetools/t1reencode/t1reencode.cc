/* t1reencode.cc -- driver for reencoding Type 1 fonts
 *
 * Copyright (c) 2005 Eddie Kohler
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version. This program is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <efont/t1font.hh>
#include <efont/t1item.hh>
#include <efont/t1rw.hh>
#include <efont/psres.hh>
#include <lcdf/error.hh>
#include <lcdf/clp.h>
#include <ctype.h>
#include <errno.h>
#include "util.hh"
#include "md5.h"
#ifdef HAVE_CTIME
# include <time.h>
#endif
#if defined(_MSDOS) || defined(_WIN32)
# include <fcntl.h>
# include <io.h>
#endif
#ifdef WIN32
/* According to Fabrice Popineau MSVCC doesn't handle std::min correctly. */
# define std /* */
#endif

using namespace Efont;

#define VERSION_OPT		301
#define HELP_OPT		302
#define OUTPUT_OPT		303
#define ENCODING_OPT		304
#define ENCODING_TEXT_OPT	305
#define PFA_OPT			306
#define PFB_OPT			307
#define FONTNAME_OPT		308
#define FULLNAME_OPT		309

Clp_Option options[] = {
    { "help", 'h', HELP_OPT, 0, 0 },
    { "output", 'o', OUTPUT_OPT, Clp_ArgString, 0 },
    { "pfa", 'a', PFA_OPT, 0, 0 },
    { "pfb", 'b', PFA_OPT, 0, 0 },
    { "name", 'n', FONTNAME_OPT, Clp_ArgString, 0 },
    { "fullname", 'N', FULLNAME_OPT, Clp_ArgString, 0 },
    { "full-name", 'N', FULLNAME_OPT, Clp_ArgString, 0 },
    { "encoding", 'e', ENCODING_OPT, Clp_ArgString, 0 },
    { "encoding-text", 'E', ENCODING_TEXT_OPT, Clp_ArgString, 0 },
    { "version", 0, VERSION_OPT, 0, 0 },
};


static const char *program_name;
static PermString::Initializer initializer;
static String::Initializer initializer2;
static HashMap<PermString, int> glyph_order(-1);
static String encoding_name;

static const char ISOLatin1Encoding[] = "/ISOLatin1Encoding [\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /space /exclam /quotedbl /numbersign /dollar /percent /ampersand /quoteright\n\
  /parenleft /parenright /asterisk /plus /comma /minus /period /slash\n\
  /zero /one /two /three /four /five /six /seven\n\
  /eight /nine /colon /semicolon /less /equal /greater /question\n\
  /at /A /B /C /D /E /F /G\n\
  /H /I /J /K /L /M /N /O\n\
  /P /Q /R /S /T /U /V /W\n\
  /X /Y /Z /bracketleft /backslash /bracketright /asciicircum /underscore\n\
  /quoteleft /a /b /c /d /e /f /g\n\
  /h /i /j /k /l /m /n /o\n\
  /p /q /r /s /t /u /v /w\n\
  /x /y /z /braceleft /bar /braceright /asciitilde /.notdef\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /dotlessi /grave /acute /circumflex /tilde /.notdef /breve /dotaccent\n\
  /.notdef /.notdef /ring /.notdef /.notdef /hungarumlaut /ogonek /caron\n\
  /.notdef /exclamdown /cent /sterling /currency /yen /brokenbar /section\n\
  /dieresis /copyright /ordfeminine /guillemotleft /logicalnot /hyphen /registered /macron\n\
  /degree /plusminus /twosuperior /threesuperior /.notdef /mu /paragraph /periodcentered\n\
  /cedilla /onesuperior /ordmasculine /guillemotright /onequarter /onehalf /threequarters /questiondown\n\
  /Agrave /Aacute /Acircumflex /Atilde /Adieresis /Aring /AE /Ccedilla\n\
  /Egrave /Eacute /Ecircumflex /Edieresis /Igrave /Iacute /Icircumflex /Idieresis\n\
  /Eth /Ntilde /Ograve /Oacute /Ocircumflex /.notdef /Odieresis /multiply\n\
  /Oslash /.notdef /.notdef /.notdef /.notdef /Yacute /Thorn /germandbls\n\
  /agrave /aacute /acircumflex /atilde /.notdef /aring /ae /ccedilla\n\
  /egrave /eacute /ecircumflex /edieresis /igrave /iacute /icircumflex /idieresis\n\
  /eth /ntilde /.notdef /oacute /ocircumflex /otilde /odieresis /.notdef\n\
  /oslash /ugrave /uacute /ucircumflex /udieresis /yacute /thorn\n\
] def\n";

static const char ExpertEncoding[] = "/ExpertEncoding [\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /space /exclamsmall /Hungarumlautsmall /.notdef /dollaroldstyle /dollarsuperior /ampersandsmall /Acutesmall\n\
  /parenleftsuperior /parenrightsuperior /twodotenleader /onedotenleader /comma /hyphen /period /fraction\n\
  /zerooldstyle /oneoldstyle /twooldstyle /threeoldstyle /fouroldstyle /fiveoldstyle /sixoldstyle /sevenoldstyle\n\
  /eightoldstyle /nineoldstyle /colon /semicolon /commasuperior /threequartersemdash /periodsuperior /questionsmall\n\
  /.notdef /asuperior /bsuperior /centsuperior /dsuperior /esuperior /.notdef /.notdef\n\
  /.notdef /isuperior /.notdef /.notdef /lsuperior /msuperior /nsuperior /osuperior\n\
  /.notdef /.notdef /rsuperior /ssuperior /tsuperior /.notdef /ff /fi\n\
  /fl /ffi /ffl /parenleftinferior /.notdef /parenrightinferior /Circumflexsmall /hyphensuperior\n\
  /Gravesmall /Asmall /Bsmall /Csmall /Dsmall /Esmall /Fsmall /Gsmall\n\
  /Hsmall /Ismall /Jsmall /Ksmall /Lsmall /Msmall /Nsmall /Osmall\n\
  /Psmall /Qsmall /Rsmall /Ssmall /Tsmall /Usmall /Vsmall /Wsmall\n\
  /Xsmall /Ysmall /Zsmall /colonmonetary /onefitted /rupiah /Tildesmall /.notdef\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /.notdef /exclamdownsmall /centoldstyle /Lslashsmall /.notdef /.notdef /Scaronsmall /Zcaronsmall\n\
  /Dieresissmall /Brevesmall /Caronsmall /.notdef /Dotaccentsmall /.notdef /.notdef /Macronsmall\n\
  /.notdef /.notdef /figuredash /hypheninferior /.notdef /.notdef /Ogoneksmall /Ringsmall\n\
  /Cedillasmall /.notdef /.notdef /.notdef /onequarter /onehalf /threequarters /questiondownsmall\n\
  /oneeighth /threeeighths /fiveeighths /seveneighths /onethird /twothirds /.notdef /.notdef\n\
  /zerosuperior /onesuperior /twosuperior /threesuperior /foursuperior /fivesuperior /sixsuperior /sevensuperior\n\
  /eightsuperior /ninesuperior /zeroinferior /oneinferior /twoinferior /threeinferior /fourinferior /fiveinferior\n\
  /sixinferior /seveninferior /eightinferior /nineinferior /centinferior /dollarinferior /periodinferior /commainferior\n\
  /Agravesmall /Aacutesmall /Acircumflexsmall /Atildesmall /Adieresissmall /Aringsmall /AEsmall /Ccedillasmall\n\
  /Egravesmall /Eacutesmall /Ecircumflexsmall /Edieresissmall /Igravesmall /Iacutesmall /Icircumflexsmall /Idieresissmall\n\
  /Ethsmall /Ntildesmall /Ogravesmall /Oacutesmall /Ocircumflexsmall /Otildesmall /Odieresissmall /OEsmall\n\
  /Oslashsmall /Ugravesmall /Uacutesmall /Ucircumflexsmall /Udieresissmall /Yacutesmall /Thornsmall\n\
] def\n";

static const char ExpertSubsetEncoding[] = "/ExpertSubsetEncoding [\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /space /.notdef /.notdef /.notdef /dollaroldstyle /dollarsuperior /.notdef /.notdef\n\
  /parenleftsuperior /parenrightsuperior /twodotenleader /onedotenleader /comma /hyphen /period /fraction\n\
  /zerooldstyle /oneoldstyle /twooldstyle /threeoldstyle /fouroldstyle /fiveoldstyle /sixoldstyle /sevenoldstyle\n\
  /eightoldstyle /nineoldstyle /colon /semicolon /commasuperior /threequartersemdash /periodsuperior /.notdef\n\
  /.notdef /asuperior /bsuperior /centsuperior /dsuperior /esuperior /.notdef /.notdef\n\
  /.notdef /isuperior /.notdef /.notdef /lsuperior /msuperior /nsuperior /osuperior\n\
  /.notdef /.notdef /rsuperior /ssuperior /tsuperior /.notdef /ff /fi\n\
  /fl /ffi /ffl /parenleftinferior /.notdef /parenrightinferior /.notdef /hyphensuperior\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /.notdef /.notdef /.notdef /colonmonetary /onefitted /rupiah /.notdef /.notdef\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /.notdef /.notdef /centoldstyle /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /.notdef /.notdef /figuredash /hypheninferior /.notdef /.notdef /.notdef /.notdef\n\
  /.notdef /.notdef /.notdef /.notdef /onequarter /onehalf /threequarters /.notdef\n\
  /oneeighth /threeeighths /fiveeighths /seveneighths /onethird /twothirds /.notdef /.notdef\n\
  /zerosuperior /onesuperior /twosuperior /threesuperior /foursuperior /fivesuperior /sixsuperior /sevensuperior\n\
  /eightsuperior /ninesuperior /zeroinferior /oneinferior /twoinferior /threeinferior /fourinferior /fiveinferior\n\
  /sixinferior /seveninferior /eightinferior /nineinferior /centinferior /dollarinferior /periodinferior /commainferior\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
] def\n";

static const char SymbolEncoding[] = "/SymbolEncoding [\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /space /exclam /universal /numbersign /existential /percent /ampersand /suchthat\n\
  /parenleft /parenright /asteriskmath /plus /comma /minus /period /slash\n\
  /zero /one /two /three /four /five /six /seven\n\
  /eight /nine /colon /semicolon /less /equal /greater /question\n\
  /congruent /Alpha /Beta /Chi /Delta /Epsilon /Phi /Gamma\n\
  /Eta /Iota /theta1 /Kappa /Lambda /Mu /Nu /Omicron\n\
  /Pi /Theta /Rho /Sigma /Tau /Upsilon /sigma1 /Omega\n\
  /Xi /Psi /Zeta /bracketleft /therefore /bracketright /perpendicular /underscore\n\
  /radicalex /alpha /beta /chi /delta /epsilon /phi /gamma\n\
  /eta /iota /phi1 /kappa /lambda /mu /nu /omicron\n\
  /pi /theta /rho /sigma /tau /upsilon /omega1 /omega\n\
  /xi /psi /zeta /braceleft /bar /braceright /similar /.notdef\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
  /.notdef /Upsilon1 /minute /lessequal /fraction /infinity /florin /club\n\
  /diamond /heart /spade /arrowboth /arrowleft /arrowup /arrowright /arrowdown\n\
  /degree /plusminus /second /greaterequal /multiply /proportional /partialdiff /bullet\n\
  /divide /notequal /equivalence /approxequal /ellipsis /arrowvertex /arrowhorizex /carriagereturn\n\
  /aleph /Ifraktur /Rfraktur /weierstrass /circlemultiply /circleplus /emptyset /intersection\n\
  /union /propersuperset /reflexsuperset /notsubset /propersubset /reflexsubset /element /notelement\n\
  /angle /gradient /registerserif /copyrightserif /trademarkserif /product /radical /dotmath\n\
  /logicalnot /logicaland /logicalor /arrowdblboth /arrowdblleft /arrowdblup /arrowdblright /arrowdbldown\n\
  /lozenge /angleleft /registersans /copyrightsans /trademarksans /summation /parenlefttp /parenleftex\n\
  /parenleftbt /bracketlefttp /bracketleftex /bracketleftbt /bracelefttp /braceleftmid /braceleftbt /braceex\n\
  /.notdef /angleright /integral /integraltp /integralex /integralbt /parenrighttp /parenrightex\n\
  /parenrightbt /bracketrighttp /bracketrightex /bracketrightbt /bracerighttp /bracerightmid /bracerightbt\n\
] def\n";

void
usage_error(ErrorHandler *errh, const char *error_message, ...)
{
    va_list val;
    va_start(val, error_message);
    if (!error_message)
	errh->message("Usage: %s [OPTION]... FONT", program_name);
    else
	errh->verror(ErrorHandler::ERR_ERROR, String(), error_message, val);
    errh->message("Type %s --help for more information.", program_name);
    exit(1);
}

void
usage()
{
    printf("\
'T1reencode' changes a Type 1 font's embedded encoding and writes the\n\
reencoded font to the standard output.\n\
\n\
Usage: %s [OPTION]... [FONTFILE [OUTPUTFILE]]\n\
\n\
Options:\n\
  -e, --encoding=FILE          Read the encoding from FILE (in DVIPS format).\n\
  -E, --encoding-text=ENC      The ENC argument is the encoding text.\n\
  -n, --name=NAME              Set output font's PostScript name.\n\
  -N, --full-name=NAME         Set output font's full name.\n\
  -a, --pfa                    Output PFA font.\n\
  -b, --pfb                    Output PFB font.  This is the default.\n\
  -o, --output=FILE            Write output to FILE instead of standard out.\n\
  -h, --help                   Print this message and exit.\n\
      --version                Print version number and exit.\n\
\n\
Report bugs to <kohler@cs.ucla.edu>.\n", program_name);
}


// FONT MANIPULATION

static void
kill_def(Type1Font* font, Type1Definition *t1d, int whichd)
{
    if (!t1d || font->dict(whichd, t1d->name()) != t1d)
	return;
  
    int icount = font->nitems();
    for (int i = font->first_dict_item(whichd); i < icount; i++)
	if (font->item(i) == t1d) {
	    StringAccum sa;
	    sa << '%';
	    t1d->gen(sa);
	    PermString name = t1d->name();
	    Type1CopyItem *t1ci = new Type1CopyItem(sa.take_string());
	    font->set_item(i, t1ci);
	    font->set_dict(whichd, name, 0);
	    return;
	}
  
    assert(0);
}

static void
adjust_font_definitions(Type1Font* font, Type1Encoding* encoding, String new_name, String new_full_name)
{
    // prepare an MD5 digest over the encoding
    StringAccum etext;
    for (int i = 0; i < 256; i++)
	etext << encoding->elt(i) << ' ';
    MD5_CONTEXT md5;
    md5_init(&md5);
    md5_update(&md5, (const unsigned char*) etext.data(), etext.length() - 1);
    
    // save UniqueID, then kill its definition
    int uniqueid;
    Type1Definition *t1d = font->dict("UniqueID");
    bool have_uniqueid = (t1d && t1d->value_int(uniqueid));
    kill_def(font, t1d, Type1Font::dFont);
    kill_def(font, font->p_dict("UniqueID"), Type1Font::dPrivate);
    
    // prepare XUID
    t1d = font->dict("XUID");
    Vector<double> xuid;
    if (!t1d || !t1d->value_numvec(xuid)) {
	if (have_uniqueid) {
	    t1d = font->ensure(Type1Font::dFont, "XUID");
	    xuid.clear();
	    xuid.push_back(1);
	    xuid.push_back(uniqueid);
	} else if (t1d) {
	    kill_def(font, t1d, Type1Font::dFont);
	    t1d = 0;
	}
    }
    if (t1d) {
	uint8_t digest[MD5_DIGEST_SIZE + 2]; // leave 2 bytes of space
	md5_final((unsigned char *) digest, &md5);
	digest[MD5_DIGEST_SIZE] = digest[MD5_DIGEST_SIZE + 1] = 0;
	
	// append digest to XUID; each element must be less than 2^24
	for (int i = 0; i < MD5_DIGEST_SIZE; i += 3)
	    xuid.push_back((digest[i] << 16) | (digest[i+1] << 8) | digest[i+2]);
	t1d->set_numvec(xuid);
    }

    // prepare new font name
    if (!encoding_name) {
	char text_digest[MD5_TEXT_DIGEST_SIZE + 1];
	md5_final_text(text_digest, &md5);
	encoding_name = "AutoEnc_" + String(text_digest);
    }
    
    t1d = font->dict("FontName");
    PermString name;
    if (t1d && t1d->value_name(name)) {
	if (!new_name)
	    new_name = name + encoding_name;
	t1d->set_name(new_name.c_str());
	font->uncache_defs();	// remove cached font name
    }

    // add a FullName too
    String full_name;
    t1d = font->fi_dict("FullName");
    if (t1d && t1d->value_string(full_name)) {
	if (!new_full_name)
	    new_full_name = full_name + "_" + encoding_name + " Enc";
	t1d->set_string(new_full_name.c_str());
    }

    // add header comments
    {
	StringAccum sa;
#if HAVE_CTIME
	time_t cur_time = time(0);
	char *time_str = ctime(&cur_time);
	sa << "%% Created by t1reencode-" VERSION " on " << time_str;
	sa.pop_back();
#else
	sa << "%% Created by t1reencode-" VERSION "."; 
#endif

	font->add_header_comment(sa.take_string().c_str());
	font->add_header_comment("%% T1reencode is free software.  See <http://www.lcdf.org/type/>.");
    }
}


// ENCODING READER

static String
tokenize(const String &s, int &pos_in, int &line)
{
    const char *data = s.data();
    int len = s.length();
    int pos = pos_in;
    while (1) {
	// skip whitespace
	while (pos < len && isspace(data[pos])) {
	    if (data[pos] == '\n')
		line++;
	    else if (data[pos] == '\r' && (pos + 1 == len || data[pos+1] != '\n'))
		line++;
	    pos++;
	}
	
	if (pos >= len) {
	    pos_in = len;
	    return String();
	} else if (data[pos] == '%') {
	    for (pos++; pos < len && data[pos] != '\n' && data[pos] != '\r'; pos++)
		/* nada */;
	} else if (data[pos] == '[' || data[pos] == ']' || data[pos] == '{' || data[pos] == '}') {
	    pos_in = pos + 1;
	    return s.substring(pos, 1);
	} else if (data[pos] == '(') {
	    int first = pos, nest = 0;
	    for (pos++; pos < len && (data[pos] != ')' || nest); pos++)
		switch (data[pos]) {
		  case '(': nest++; break;
		  case ')': nest--; break;
		  case '\\':
		    if (pos + 1 < len)
			pos++;
		    break;
		  case '\n': line++; break;
		  case '\r':
		    if (pos + 1 == len || data[pos+1] != '\n')
			line++;
		    break;
		}
	    pos_in = (pos < len ? pos + 1 : len);
	    return s.substring(first, pos_in - first);
	} else {
	    int first = pos;
	    while (pos < len && data[pos] == '/')
		pos++;
	    while (pos < len && data[pos] != '/' && !isspace(data[pos]) && data[pos] != '[' && data[pos] != ']' && data[pos] != '%' && data[pos] != '(' && data[pos] != '{' && data[pos] != '}')
		pos++;
	    pos_in = pos;
	    return s.substring(first, pos - first);
	}
    }
}

static String
landmark(const String &filename, int line)
{
    return filename + String::stable_string(":", 1) + String(line);
}

Type1Encoding *
parse_encoding(String s, String filename, ErrorHandler *errh)
{
    filename = printable_filename(filename);
    int pos = 0, line = 1;

    // parse text
    String token = tokenize(s, pos, line);
    if (!token || token[0] != '/') {
	errh->lerror(landmark(filename, line), "parse error, expected name");
	return 0;
    }
    encoding_name = token.substring(1);

    if (tokenize(s, pos, line) != "[") {
	errh->lerror(landmark(filename, line), "parse error, expected [");
	return 0;
    }

    Type1Encoding *t1e = new Type1Encoding;
    int e = 0;
    while ((token = tokenize(s, pos, line)) && token[0] == '/') {
	if (e > 255) {
	    errh->lwarning(landmark(filename, line), "more than 256 characters in encoding");
	    break;
	}
	t1e->put(e, token.substring(1));
	e++;
    }
    return t1e;
}



// MAIN

/*****
 * MAIN PROGRAM
 **/

static Type1Font *
do_file(const char *filename, PsresDatabase *psres, ErrorHandler *errh)
{
    FILE *f;
    if (!filename || strcmp(filename, "-") == 0) {
	f = stdin;
	filename = "<stdin>";
#if defined(_MSDOS) || defined(_WIN32)
	_setmode(_fileno(f), _O_BINARY);
#endif
    } else
	f = fopen(filename, "rb");
    
    if (!f) {
	// check for PostScript name
	Filename fn = psres->filename_value("FontOutline", filename);
	f = fn.open_read();
    }
  
    if (!f)
	errh->fatal("%s: %s", filename, strerror(errno));
  
    Type1Reader *reader;
    int c = getc(f);
    ungetc(c, f);
    if (c == EOF)
	errh->fatal("%s: empty file", filename);
    if (c == 128)
	reader = new Type1PFBReader(f);
    else
	reader = new Type1PFAReader(f);
  
    Type1Font *font = new Type1Font(*reader);
    if (!font->ok())
	errh->fatal("%s: no glyphs in font", filename);

    delete reader;
    return font;
}

int
main(int argc, char *argv[])
{
    PsresDatabase *psres = new PsresDatabase;
    psres->add_psres_path(getenv("PSRESOURCEPATH"), 0, false);
  
    Clp_Parser *clp =
	Clp_NewParser(argc, (const char * const *)argv, sizeof(options) / sizeof(options[0]), options);
    program_name = Clp_ProgramName(clp);
  
    ErrorHandler *errh = ErrorHandler::static_initialize(new FileErrorHandler(stderr));
    const char *input_file = 0;
    const char *output_file = 0;
    const char *encoding_file = 0;
    const char *encoding_text = 0;
    const char *new_font_name = 0;
    const char *new_full_name = 0;
    bool binary = true;
    Vector<String> glyph_patterns;
  
    while (1) {
	int opt = Clp_Next(clp);
	switch (opt) {

	  case ENCODING_OPT:
	    if (encoding_file || encoding_text)
		errh->fatal("encoding already specified");
	    encoding_file = clp->arg;
	    break;

	  case ENCODING_TEXT_OPT:
	    if (encoding_file || encoding_text)
		errh->fatal("encoding already specified");
	    encoding_text = clp->arg;
	    break;

	  case FONTNAME_OPT:
	    if (new_font_name)
		errh->fatal("font name already specified");
	    new_font_name = clp->arg;
	    break;

	  case FULLNAME_OPT:
	    if (new_full_name)
		errh->fatal("full name already specified");
	    new_full_name = clp->arg;
	    break;
	    
	  case OUTPUT_OPT:
	    if (output_file)
		errh->fatal("output file already specified");
	    output_file = clp->arg;
	    break;

	  case PFA_OPT:
	    binary = false;
	    break;

	  case PFB_OPT:
	    binary = true;
	    break;
      
	  case VERSION_OPT:
	    printf("t1reencode (LCDF typetools) %s\n", VERSION);
	    printf("Copyright (C) 1999-2005 Eddie Kohler\n\
This is free software; see the source for copying conditions.\n\
There is NO warranty, not even for merchantability or fitness for a\n\
particular purpose.\n");
	    exit(0);
	    break;
      
	  case HELP_OPT:
	    usage();
	    exit(0);
	    break;
      
	  case Clp_NotOption:
	    if (input_file && output_file)
		errh->fatal("too many arguments");
	    else if (input_file)
		output_file = clp->arg;
	    else
		input_file = clp->arg;
	    break;
      
	  case Clp_Done:
	    goto done;
      
	  case Clp_BadOption:
	    usage_error(errh, 0);
	    break;
      
	  default:
	    break;
      
	}
    }
  
  done:
    // read the font
    Type1Font *font = do_file(input_file, psres, errh);
    if (!input_file || strcmp(input_file, "-") == 0)
	input_file = "<stdin>";

    // read the encoding
    if (!encoding_file && !encoding_text)
	errh->fatal("missing '-e ENCODING' argument");
    Type1Encoding *t1e = 0;
    if (encoding_file == "StandardEncoding") {
	t1e = Type1Encoding::standard_encoding();
	encoding_name = encoding_file;
    } else {
	String text;
	if (encoding_file == "ISOLatin1Encoding")
	    text = String::stable_string(ISOLatin1Encoding);
	else if (encoding_file == "ExpertEncoding")
	    text = String::stable_string(ExpertEncoding);
	else if (encoding_file == "ExpertSubsetEncoding")
	    text = String::stable_string(ExpertSubsetEncoding);
	else if (encoding_file == "SymbolEncoding")
	    text = String::stable_string(SymbolEncoding);
	else if (encoding_text)
	    text = String::stable_string(encoding_text), encoding_file = "<argument>";
	else if ((text = read_file(encoding_file, errh)), errh->nerrors() > 0)
	    exit(1);
	if (!(t1e = parse_encoding(text, encoding_file, errh)))
	    exit(1);
    }

    // set the encoding
    font->add_type1_encoding(t1e);

    // adjust definitions
    adjust_font_definitions(font, t1e, new_font_name, new_full_name);

    // write it to output
    FILE *outf;
    if (!output_file || strcmp(output_file, "-") == 0)
	outf = stdout;
    else {
	outf = fopen(output_file, "w");
	if (!outf)
	    errh->fatal("%s: %s", output_file, strerror(errno));
    }
    if (binary) {
#if defined(_MSDOS) || defined(_WIN32)
	_setmode(_fileno(outf), _O_BINARY);
#endif
	Type1PFBWriter w(outf);
	font->write(w);
    } else {
	Type1PFAWriter w(outf);
	font->write(w);
    }
    
    return (errh->nerrors() == 0 ? 0 : 1);
}
