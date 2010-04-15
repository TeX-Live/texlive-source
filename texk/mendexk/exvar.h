extern FILE *efp;

extern int lines,idxcount,acc,reject;
extern int prange,fsti,lorder,bcomp,force,fpage,gflg,verb,debug;
extern int warn,scount,pattr[3];

extern struct index *ind;

extern char keyword[2048];
extern char arg_open,arg_close;
extern char range_open,range_close;
extern char level,actual,encap,quote,escape;
extern char preamble[2048],postamble[2048];
extern char setpage_prefix[2048],setpage_suffix[2048];
extern char group_skip[2048];
extern char lethead_prefix[2048],lethead_suffix[2048];
extern int lethead_flag;
extern char item_0[2048],item_1[2048],item_2[2048];
extern char item_01[2048],item_x1[2048],item_12[2048],item_x2[2048];
extern char delim_0[2048],delim_1[2048],delim_2[2048],delim_n[2048],delim_r[2048],delim_t[2048];
extern char suffix_2p[2048],suffix_3p[2048],suffix_mp[2048];
extern char encap_prefix[2048],encap_infix[2048],encap_suffix[2048];
extern int line_max;
extern char indent_space[2048];
extern int indent_length;
extern int priority;
extern char symbol[2048];
extern char symhead_positive[2048],symhead_negative[2048];
extern char numhead_positive[2048],numhead_negative[2048];
extern int symbol_flag;
extern int letter_head;
extern char atama[2048];
extern char page_compositor[2048],page_precedence[2048];
extern char character_order[2048];
