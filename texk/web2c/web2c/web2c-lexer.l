%option never-interactive

%top{
/* web2c-lexer.l -- lexical analysis for Tangle output.  Public domain. */

#include "web2c.h"
#include "web2c-parser.h"
}

%{
/* Hack to make it possible to compile the generated code with C++
   Required if you use flex. */
#ifdef __cplusplus
#define webinput yyinput
#else
#define webinput input
#endif

/* For some reason flex wants to do a system call, so we must lose our
   definition of the Pascal read.  */
#undef read

char conditional[20], negbuf[2], temp[20];
extern boolean doing_statements;


/* We only read one input file.  This is the default definition, but
   giving it ourselves avoids the need to find -lfl or -ll at link time.
   This is a good thing, since libfl.a is often installed somewhere that
   the linker doesn't search by default.  */
static int
yywrap (void)
{
  return 1;
}
#define YY_SKIP_YYWRAP /* not that it matters */
%}
DIGIT		[0-9]
ALPHA		[a-zA-Z]
ALPHANUM	({DIGIT}|{ALPHA}|"_")
IDENTIFIER	({ALPHA}{ALPHANUM}*)
NUMBER		({DIGIT}+)
SIGN		("+"|"-")
SIGNED		({SIGN}?{NUMBER})
WHITE		[ \n\t]+
REAL		({NUMBER}"."{NUMBER}("e"{SIGNED})?)|({NUMBER}"e"{SIGNED})
COMMENT		(("{"[^}]*"}")|("(*"([^*]|"*"[^)])*"*)"))
W		({WHITE}|"packed ")+
WW		({WHITE}|{COMMENT}|"packed ")*
HHB0		("hh"{WW}"."{WW}"b0")
HHB1		("hh"{WW}"."{WW}"b1")

%%
{W}				;
"{"		{ while (webinput() != '}'); }

"#"		{
		    register int c;
		    putc('#', out);
		    while ((c = webinput()) && c != ';')
			putc(c, out);
		    putc('\n', out);
		}

"ifdef("	{register int c;
		 register char *cp=conditional;
		 new_line();
		 while ((c = webinput()) != '\'')
		    ;
		 while ((c = webinput()) != '\'')
		    *cp++ = c;
		 *cp = '\0';
		 (void) webinput();
		 if (doing_statements) fputs("\t;\n", out);
		 fprintf(out, "#ifdef %s\n", conditional);
		}

"endif("	{register int c;
		 new_line();
		 fputs("#endif /* ", out);
		 while ((c = webinput()) != '\'')
		    ;
		 while ((c = webinput()) != '\'')
		    (void) putc(c, out);
		 (void) webinput();
		 conditional[0] = '\0';
		 fputs(" */\n", out);
		}

"ifndef("	{register int c;
		 register char *cp=conditional;
		 new_line();
		 while ((c = webinput()) != '\'')
		    ;
		 while ((c = webinput()) != '\'')
		    *cp++ = c;
		 *cp = '\0';
		 (void) webinput();
		 if (doing_statements) fputs("\t;\n", out);
		 fprintf(out, "#ifndef %s\n", conditional);
		}

"endifn("	{register int c;
		 new_line();
		 fputs("#endif /* not ", out);
		 while ((c = webinput()) != '\'')
		    ;
		 while ((c = webinput()) != '\'')
		    putc(c, out);
		 (void) webinput();
		 conditional[0] = '\0';
		 fputs(" */\n", out);
		}


"procedure "[a-z_]+";"[ \n\t]*"forward;"	;

"function "[(),:a-z_]+";"[ \n\t]*"forward;"	;

"@define"	return last_tok=define_tok;
"@field"	return last_tok=field_tok;
"and"		return last_tok=and_tok;
"array"		return last_tok=array_tok;
"begin"		return last_tok=begin_tok;
"case"		return last_tok=case_tok;
"const"		return last_tok=const_tok;
"div"		return last_tok=div_tok;
"break"		return last_tok=break_tok;
"do"		return last_tok=do_tok;
"downto"	return last_tok=downto_tok;
"else"		return last_tok=else_tok;
"end"		return last_tok=end_tok;
"file"		return last_tok=file_tok;
"for"		return last_tok=for_tok;
"function"	return last_tok=function_tok;
"goto"		return last_tok=goto_tok;
"if"		return last_tok=if_tok;
"label"		return last_tok=label_tok;
"mod"		return last_tok=mod_tok;
"noreturn"	return last_tok=noreturn_tok;
"not"		return last_tok=not_tok;
"of"		return last_tok=of_tok;
"or"		return last_tok=or_tok;
"procedure"	return last_tok=procedure_tok;
"program"	return last_tok=program_tok;
"record"	return last_tok=record_tok;
"repeat"	return last_tok=repeat_tok;
{HHB0}		return last_tok=hhb0_tok;
{HHB1}		return last_tok=hhb1_tok;
"then"		return last_tok=then_tok;
"to"		return last_tok=to_tok;
"type"		return last_tok=type_tok;
"until"		return last_tok=until_tok;
"var"		return last_tok=var_tok;
"while"		return last_tok=while_tok;
"others"	return last_tok=others_tok;

{REAL}		{
		  sprintf (temp, "%s%s", negbuf, yytext);
		  negbuf[0] = '\0';
		  return last_tok=r_num_tok;
		}

{NUMBER}	{
		  sprintf (temp, "%s%s", negbuf, yytext);
		  negbuf[0] = '\0';
		  return last_tok=i_num_tok;
		}

("'"([^']|"''")"'")		return last_tok=single_char_tok;

("'"([^']|"''")*"'")		return last_tok=string_literal_tok;

"+"		{ if ((last_tok>=undef_id_tok &&
		      last_tok<=field_id_tok) ||
		      last_tok==i_num_tok ||
		      last_tok==r_num_tok ||
		      last_tok==')' ||
		      last_tok==']')
		   return last_tok='+';
		else return last_tok=unary_plus_tok; }

"-"		{ if ((last_tok>=undef_id_tok &&
		      last_tok<=field_id_tok) ||
		      last_tok==i_num_tok ||
		      last_tok==r_num_tok ||
		      last_tok==')' ||
		      last_tok==']')
		   return last_tok='-';
		else {
		  int c;
		  while ((c = webinput()) == ' ' || c == '\t')
                    ;
		  unput(c);
		  if (c < '0' || c > '9') {
			return last_tok = unary_minus_tok;
		  }
		  negbuf[0] = '-';
		}}

"*"		return last_tok='*';
"/"		return last_tok='/';
"="		return last_tok='=';
"<>"		return last_tok=not_eq_tok;
"<"		return last_tok='<';
">"		return last_tok='>';
"<="		return last_tok=less_eq_tok;
">="		return last_tok=great_eq_tok;
"("		return last_tok='(';
")"		return last_tok=')';
"["		return last_tok='[';
"]"		return last_tok=']';
":="		return last_tok=assign_tok;
".."		return last_tok=two_dots_tok;
"."		return last_tok='.';
","		return last_tok=',';
";"		return last_tok=';';
":"		return last_tok=':';
"^"		return last_tok='^';

{IDENTIFIER}	{ strcpy (last_id, yytext);
		  l_s = search_table (last_id);
		  return
                    last_tok = (l_s == -1 ? undef_id_tok : sym_table[l_s].typ);
		}


.		{ /* Any bizarre token will do.  */
		  return last_tok = two_dots_tok; }
%%
/* Some helper routines.  Defining these here means we don't have references
   to yytext outside of this file.  Which means we can omit one of the more
   troublesome autoconf tests. */
void
get_string_literal (char *s)
{
    int i, j;
    j = 1;
    s[0] = '"';
    for (i=1; yytext[i-1] != 0; i++) {
        if (yytext[i] == '\\' || yytext[i] == '"')
            s[j++] = '\\';
        else if (yytext[i] == '\'')
            i++;
        s[j++] = yytext[i];
    }
    s[j-1] = '"';
    s[j] = 0;
}

void
get_single_char (char *s)
{
    s[0]='\'';
    if (yytext[1] == '\\' || yytext[1] == '\'') {
        s[1] = '\\';
        s[2] = yytext[1];
        s[3] = '\'';
        s[4] = 0;
    } else {
        s[1] = yytext[1];
        s[2] = '\'';
        s[3] = 0;
    }
}

void
get_result_type (char *s)
{
    strcpy(s, yytext);
}


/* Since a syntax error can never be recovered from, we exit here with
   bad status.  */

int
yyerror (const_string s)
{
  /* This is so the convert script can delete the output file on error.  */
  puts ("@error@");
  fflush (stdout);
  fputs (s, stderr);
  fprintf (stderr, ": Last token = %d (%c), ", last_tok, last_tok);
  fprintf (stderr, "error buffer = `%s',\n\t", yytext);
  fprintf (stderr, "last id = `%s' (", last_id);
  ii = search_table (last_id);
  if (ii == -1)
    fputs ("not in symbol table", stderr);
  else
    switch (sym_table[ii].typ)
      {
      case undef_id_tok:
	fputs ("undefined", stderr);
	break;
      case var_id_tok:
	fputs ("variable", stderr);
	break;
      case const_id_tok:
	fputs ("constant", stderr);
	break;
      case type_id_tok:
	fputs ("type", stderr);
	break;
      case proc_id_tok:
	fputs ("parameterless procedure", stderr);
	break;
      case proc_param_tok:
	fputs ("procedure with parameters", stderr);
	break;
      case fun_id_tok:
	fputs ("parameterless function", stderr);
	break;
      case fun_param_tok:
	fputs ("function with parameters", stderr);
	break;
      default:
	fputs ("unknown!", stderr);
	break;
      }
  fputs (").\n", stderr);
  exit (1);

  /* Avoid silly warnings.  */
  return 0;
}
