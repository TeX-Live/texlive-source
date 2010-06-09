/******
** The following parameters should be modified as necessary
**	MAXINCLIST - maximum number of files allowed in an \includeonly list
**
**	DEFAULTINPUTS - this should be the same as the default TEXINPUTS
**	CHPATHSEP - the path separator character in TEXINPUTS
**	MAXINPUTPATHS - (arbitrary) number of separate paths in TEXINPUTS
**
**	DEFAULTENV - list of LaTeX environments ignored
**	CHENVSEP - the list separator character in the ignore envronment list
**	MAXENVS - maximum number of environments listed in the ignore list
**	CCHMAXENV - maximum count of characters in an environment name (LaTex)
******/

#define	MAXINCLIST	40

#ifdef OS2
#define	DEFAULTINPUTS	".;/emtex/texinput"
#define	CHPATHSEP	';'
#else
#define	DEFAULTINPUTS	".:/usr/local/tex/inputs"
#define	CHPATHSEP	':'
#endif
#define	MAXINPUTPATHS	10

#define	DEFAULTENV	"array,eqnarray,equation,figure,mathmatica,picture,table,verbatim"
#define	CHENVSEP	','
#define	MAXENVS		10
#define	CCHMAXENV	100

/******
** These probably should not be changed
******/

#define	CHOPT		'-'
#define	CHCITEOPT	'c'
#define	CHENVOPT	'e'
#define	CHLATEXOPT	'l'
#define	CHNOFOLLOWOPT	'n'
#define	CHSPACEOPT	's'
#define	CHTEXOPT	't'
#define	CHWORDOPT	'w'

#define	ERROR		-1
