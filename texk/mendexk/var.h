#include "exvar.h"

FILE *efp;

int lines,idxcount,acc=0,reject=0;
int prange=1,fsti=0,lorder=0,bcomp=0,force=0,fpage=0,gflg=0,verb=1,debug=0;
int warn=0,scount=0,pattr[3]={0,0,0};
/* static char roman[]={"ivxlcdm"},Roman[]={"IVXLCDM"}; */

struct index *ind;

char keyword[2048]={"\\indexentry"};
char arg_open='{',arg_close='}';
char range_open='(',range_close=')';
char level='!',actual='@',encap='|',quote='\"',escape='\\';
char preamble[2048]={"\\begin{theindex}\n"},postamble[2048]={"\n\n\\end{theindex}\n"};
char setpage_prefix[2048]={"\n  \\setcounter{page}{"},setpage_suffix[2048]={"}\n"};
char group_skip[2048]={"\n\n  \\indexspace\n"};
char lethead_prefix[2048]={""},lethead_suffix[2048]={""};
int lethead_flag=0;
char item_0[2048]={"\n  \\item "},item_1[2048]={"\n    \\subitem "},item_2[2048]={"\n      \\subsubitem "};
char item_01[2048]={"\n    \\subitem "},item_x1[2048]={"\n    \\subitem "},item_12[2048]={"\n      \\subsubitem "},item_x2[2048]={"\n      \\subsubitem "};
char delim_0[2048]={", "},delim_1[2048]={", "},delim_2[2048]={", "},delim_n[2048]={", "},delim_r[2048]={"--"},delim_t[2048]={""};
char suffix_2p[2048]={""},suffix_3p[2048]={""},suffix_mp[2048]={""};
char encap_prefix[2048]={"\\"},encap_infix[2048]={"{"},encap_suffix[2048]={"}"};
int line_max=72;
char indent_space[2048]={"\t\t"};
int indent_length=16;
int priority=0;
char symbol[2048]={""};
char symhead_positive[2048]={"Symbols"},symhead_negative[2048]={"symbols"};
char numhead_positive[2048]={"Numbers"},numhead_negative[2048]={"numbers"};
int symbol_flag=1;
int letter_head=1;
char atama[2048];
char page_compositor[2048]={"-"},page_precedence[2048]={"rnaRA"};
char character_order[2048]={"SNEJ"};
