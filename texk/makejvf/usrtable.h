#define BUF_SIZE 1024
#define MAX_TABLE 1024
#define MAX_CHAR_TABLE 32768
extern int usertable_replace_max,usertable_move_max,usertable_charset_max;

struct USERTABLE_REPLACE {
	int codepoint;
	int newcodepoint;
};
extern struct USERTABLE_REPLACE usertable_replace[];
struct USERTABLE_MOVE {
	int codepoint;
	double moveright;
	double movedown;
};
extern struct USERTABLE_MOVE usertable_move[];
struct USERTABLE_CHARSET {
	long min, max;
};
extern struct USERTABLE_CHARSET usertable_charset[];

/* usrtable.c */
void get_usertable(char *name);
