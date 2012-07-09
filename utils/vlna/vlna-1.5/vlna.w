
\input csmac % Makra pro èe¹tinu
\pageheight=9.5in \fullpageheight=9.8in  \setpage
%\nocon % omit table of contents
\datethis % print date on listing

\def\begitems{\medskip\bgroup\catcode`\*=13 \narrower\narrower}
\def\enditems{\par\egroup\medskip}
{\catcode`\*=13 \gdef*{\par\noindent\llap{$\bullet$\ }\ignorespaces}}


@* PROGRAM VLNA.
Program ète vstupní textový soubor a nahrazuje za specifikovanými
jednopísmennými slovy (napø.~v, k, u) mezery symbolem \uv{\.{\char126}}. To
zabrání pøi následném zpracování \TeX{}em zlomit øádek na nevhodných
místech, která jsou v rozporu s typografickou normou.

Program sestává z tìchto hlavních celkù:
@c
@<Hlavièkové soubory k naètení@>@/
@<Globální deklarace@>@/
@<Pomocné funkce@>@/
@<Vlnkovací funkce |tie|@>@/
@<Hlavní program@>

@ Definujeme |BANNER|, co¾ je text, který se objevi pøi startu
programu a obsahuje èíslo verze programu. 
Zde je názornì vidìt, ¾e míchání dvou jazykù se nevyhneme. Pøi tisku
textù na terminál nesmíme pøedpokládat, ¾e tam budou èeské fonty.
V~této dokumentaci se setkáme se tøemi jazyky: angliètinou (vìt¹inou
v~kódu programu, cestinou v~/* komentáøích */ a èe¹tinou jinde.
Tu cestinu si vynutil fakt, ¾e DOS-ovská varianta \.{tangle} a
\.{weave} se nesná¹í s~akcentovanými písmeny v~/* komentáøích */.
A~nyní u¾ slíbený (vícejazyèný) |BANNER|.
@d BANNER "This is program vlna, version 1.5, (c) 1995, 2002, 2009, 2010 Petr Olsak\n"

@ V programu jsou pou¾ity knihovní funkce, jejích¾ prototypy jsou
definovány ve tøech standardních hlavièkových souborech.
@<Hlavièkové ...@>=
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

@ Definujeme konstanty pro návratový kód. |OK| pro úspì¹ný bìh,
|WARNING| pøi výskytu aspoò jedné varovné zprávy, |IO_ERR| pro chybu
v~pøístupu ke vtupním nebo výstupním souborùm, |BAD_OPTIONS| pro
syntaktickou chybu na pøíkazové øádce a |BAD_PROGRAM| pro pøípad
havárie programu. Ta by nemìla nikdy nastat. Promìnná |status| bude
obsahovat návratový kód a promìnná |prog_name| bude ukazovat na text
nultého parametru pøíkazové øádky.
@d OK 0
@d WARNING 1
@d IO_ERR 2
@d BAD_OPTIONS 3
@d BAD_PROGRAM 4
@<Globální deklarace@>=
char *prog_name;
int status;

@ Základní rozvr¾ení funkce |main|.
@<Hlavní program@>=
int main (int argc, char **argv)
{
  @<Lokální promìnné funkce |main|@>;
  prog_name=argv[0]; status = OK;
  @<Naètení parametrù pøíkazového øádku@>;
  if (!silent) fprintf (stderr, BANNER);
  @<Inicializace datových struktur@>;
  @<Zpracování souborù@>;
  return status;
}

@* Parametry pøíkazového øádku.
Program ète z~pøíkazového øádku postupnì (nepovinné) parametry,
které zaèínají znakem \uv{\.{-}}. Pak následují jména vstupních a výstupních
souborù.
\begitems
* \.{-f} \dots\ program pracuje jako filtr (viz sekce |@<Zpracování
  souborù@>|). Není-li tento parametr pou¾it, program pracuje v tzv.
  standardním re¾imu, kdy jednotlivé soubory jsou vstupní i výstupní.
* \.{-s} \dots\ program nevypí¹e |BANNER|, ani sumarizaci, ani varování,
  pøi nich¾ není program pøedèasnì ukonèen.  V¹echny tyto výpisy
  smìøují do |stderr|, tak¾e pokud program pracuje v re¾imu \uv{filtr},
  není nutné tento parametr pou¾ít.
* \.{-r} \dots\ program ma¾e pracovní soubor (soubory), které vytváøí
  ve standardním re¾imu (tj. není pou¾it \.{-f}). V reøimu filter nemá
  tento parametr vliv.
* \.{-v} \dots\ parametr definuje skupinu písmen, které budou
  interpretovány jako neslabièné pøedlo¾ky.
  Napø. \.{-v KkSsVvZzOoUuAI}. Pokud není parametr uveden, je pou¾ita
  skupina uvedená v tomto pøíkladì.
* \.{-x} \dots\ parametr vymezuje pomocí hexadecimálního zápisu string,
  který program vkládá na vyhledaná místa. Implicitnì vkládá vlnku.
  Napøíklad \.{-x C2A0} zpùsobí, ¾e program bude vkládat místo vlnky dva byty,
  první s kódem \.{C2} a druhý s kódem \.{A0}.
* \.{-m} \dots\ program neprovádí kontrolu math/text módù, tj. vlnkuje i
  uvnitø matematického módu \TeX{}u. (Implicite tam nevlnkuje).
* \.{-n} \dots\ prorgram neprovádí kontrolu verbatim módu, tj. vlnkuje i
  uvnitø verbatim módu definovaném bì¾nými prostøedími. Imlicite ve
  verbatim prostøedí nevlnkuje.
* \.{-l} \dots\ La\TeX{} re¾im. Pøi kontrole text-math-verbatim módù jsou
  brány v úvahu dal¹í sekvence, obvyklé v La\TeX{}ových dokumentech.
* \.{-w} \dots\ WEB re¾im. Ohranièení verbatim módu je doplnìno znaky
  pou¾ívanými v dokumentech WEB (napø. tento dokument). Dùsledek: program
  vlnkuje dokumentaèní èást ka¾dé sekce, ale nikoli kód.
\enditems

Definujeme funkci |printusage|, která tiskne (pøi chybì) struèný pøehled
mo¾ných parametrù. Nepodaøilo se mi zjistit, jak se ve WEBu napí¹e
kulturnì dlouhý string obsahující \.{\char92n} s formátovacími
po¾adavky. Byl jsem nucen to takto nehezky zapsat.
@<Pomocné funkce@>=
static void printusage (void)
{
  fprintf(stderr,
    "usage: vlna [opt] [filenames]\n"
    "  opt -f :  filter mode: file1 file2 ... file1->file2\n"
    "                         file1       ... file1->stdout\n"
    "                                     ... stdin->stdout\n"
    "            nofilter: file1 [file2 file3 ...] all are in/out\n"
    "      -s :  silent: no messages to stderr\n"
    "      -r :  rmbackup: if nofilter, removes temporary files\n"
    "      -v charset :  set of lettres to add tie, default: KkSsVvZzOoUuAI\n" 
    "      -x code : code for tie symbol, default: 7E, example -x C2A0\n"
    "      -m :  nomath: ignores math modes\n"
    "      -n :  noverb: ignores verbatim modes\n"
    "      -l :  LaTeX mode\n"
    "      -w :  web mode\n");
}

@ Promìnné |isfilter|, |silent|, |rmbackup|, |nomath|, |noverb|,
|latex|, resp. |web| øíkají, ¾e je nastaven parametr \.{-f}, \.{-s},
\.{-r}, \.{-m}, \.{-n}, \.{-l}, resp. \.{-w}.  Promìnná |charset|
ukazuje buï na implicitní skupinu znakù |charsetdefault|, nebo (pøi
pou¾ití parametru \.{-v}) na text uvedený v pøíkazovém øádku. 
@<Globální deklarace@>=
int isfilter=0, silent=0, rmbackup=0, nomath=0, noverb=0, web=0, latex=0;
char charsetdefault[]="KkSsVvZzOoUuAI";
char *charset=charsetdefault;

@ String |tiestr| obsahuje string, kterým se má nahradit vyhledané 
místo. Pokud není pou¾it parametr \.{-u}, je tento string jadnoznakový 
a obsahuje vlnku. Jinak obsahuje string konvertovaný z parametru \.{-u}.
String má délku |tiestrlen| bez ohledu na to, zda obsahuje nebo 
neobsahuje nulové znaky (C-èková konvence pro stringy není pou¾ita).
@<Globální deklarace@>=
unsigned char tiestr[MAXLEN];
int tiestrlen;

@ @<Naètení parametrù ...@>=
tiestr[0] = '~';
tiestrlen = 1;
while (argc>1 && argv[1][0] == '-') {
  if (argv[1][2] != 0) printusage (), exit (BAD_OPTIONS);
  switch(argv[1][1]) {
  case 'f': isfilter = 1; break;
  case 's': silent = 1; break;
  case 'r': rmbackup = 1; break;
  case 'v': if (argc<2) printusage (), exit (BAD_OPTIONS);
    argv++; argc--; charset = argv[1]; break;
  case 'x': if (argc<2) printusage (), exit (BAD_OPTIONS);
    argv++; argc--; settiestr(argv[1]); break;
  case 'm': nomath = 1; break;
  case 'n': noverb = 1; break;
  case 'l': latex = 1; break;
  case 'w': web = 1; break;
  default: printusage (), exit (BAD_OPTIONS);
          /* nezn\'am\'y parametr */
  }
  argc--; argv++;
}

@ Vyøe¹íme konverzi kódu zapsaného za parametrem \.{-x} na string |tiestr|.
@<Pomocné funkce@>=
static unsigned char hexnum(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  printusage (), exit (BAD_OPTIONS);  
} 
static void settiestr(char *s) {
  int i, j;
  i = strlen(s);
  if ((i > 2*MAXLEN) || i%2 == 1) printusage (), exit (BAD_OPTIONS);
  tiestrlen = i/2;
  j = 0;
  for (i=0; i<tiestrlen; i++) {
     tiestr[i] = hexnum(s[j++]) << 4;
     tiestr[i] += hexnum(s[j++]); 
  }
}

@* Zpracování souborù.  Parametr |MAXLEN| definuje maximální mo¾nou
délku jména souboru, který vytvoøíme jako pøechodný, nebo zálohový.
Dále deklarujeme promìnné typu \uv{stream}.
@d MAXLEN 120
@<Lokální promìnné funkce...@>=
FILE *infile, *outfile;
char backup[MAXLEN];
int j;

@ Definujeme funkci pro výpis chybového hlá¹ení pøi neúspì¹ném otevøení
souboru.
@<Pomocné funkce@>=
static void ioerr (char *f)
{
   fprintf(stderr, "%s: cannot open file %s\n", prog_name, f);
}

@ Zpùsob zpracování souborù rozli¹íme podle re¾imu daným pøepínaèem \.{-f}.
@<Zpracování souborù@>=
if (isfilter)  @<Zpracování v re¾imu filter@> @/
else   @<Zpracování v¹ech souborù pøíkazové øádky@>

@ V re¾imu |isfilter==1| je dal¹í zpracování závislé na poètu souborù v
pøíkazové øádce:
\begitems
* nula souborù -- vstup je |stdin| a výstup je |stdout|,
* jeden soubor -- je vstupní, výstup je |stdout|,
* dva soubory -- první je vstupní, druhý výstupní,
* více souborù -- program skonèí s chybou.
\enditems
@<Zpracování v re¾imu filter@>=
{
  if (argc > 3) printusage (), exit (BAD_OPTIONS) ;
  infile = stdin; outfile = stdout;
  if (argc >= 2) infile = fopen (argv[1], "r");
  if (infile == NULL)  ioerr (argv[1]), exit (IO_ERR);
  if (argc == 3) outfile = fopen(argv[2], "wb");
  if (outfile == NULL) ioerr (argv[2]), exit (IO_ERR);
  if (argc >= 2) filename = argv[1];
  else filename = NULL;
  tie (infile, outfile);
  if (outfile != stdout) fclose (outfile);
  if (infile != stdin) fclose (infile);
}

@ V~re¾imu |isfilter==0| jsou jednotlivé soubory v~pøíkazovém øádku
interpretovány jako vstupní i výstupní. Více souborù v~pøíkazovém øádku má
stejný efekt, jako opakované volání programu na jednotlivé soubory.
V~\UNIX/u lze tedy napø. napsat \.{\jobname\ *.tex} a program doplní vlnky do
v¹ech souborù s~pøíponou~\.{tex}. Toto neplatí v~DOSu, proto¾e interpretace
masky je v~\UNIX/u starostí shellu a nikoli programu samotného. Ná¹ program
masku nebude interpretovat. Je-li v~tomto re¾imu nulový poèet souborù,
program se ukonèí s~chybou. 
@<Zpracování v¹ech souborù pøíkazové øádky@>=
{
  if (argc==1) printusage (), exit(BAD_OPTIONS);
  while (argc>1) {
     argc--; argv++;
     @<Pøejmenuj vstup |argv[0]| na |backup| a otevøi jej jako |infile|@>;
     if (infile == NULL) {
       ioerr (argv[0]); continue;
     }
     outfile = fopen (argv[0], "wb");
     if (outfile == NULL) {
       ioerr (argv[0]);
       rename (backup, argv[0]); 
       status = WARNING; 
       continue;
     }
     filename = argv[0];
     tie (infile, outfile);
     fclose (outfile), fclose (infile);
     if (rmbackup) remove (backup);
   }
}

@ Pøi |isfilter==0| program pøejmenuje ka¾dý  zpracovávaný soubor tak, ¾e
zmìní poslední písmeno názvu souboru na znak \.{\char126}. Tento
pøejmenovaný soubor bude otevøen jako vstupní a výstupem bude pùvodní
soubor. Vstupní soubor pøi |rmbackup==0| zùstane zachován jako záloha.

Proè vlnku nepøidáváme na konec názvu souboru, ale mìníme ji za poslední
znak souboru? Proto¾e chceme, aby program fungoval i v tak nemo¾ných
systémech, jako je DOS.
@<Pøejmenuj vstup...@>=
infile = NULL;
j = strlen (argv[0]) - 1;
if (j >= MAXLEN || argv[0][j] == '~') {
   if (!silent) fprintf (stderr, "%s: the conflict of file name %s\n",
      prog_name, argv[0]);
}
else {
  strcpy (backup, argv[0]);
  backup[j] = '~';
  remove (backup);
  j = rename (argv[0], backup);
  if (j == 0) infile = fopen (backup, "r");
}

@* Patterny.  Abychom mohli úèelnì definovat chování programu
v~rùzných situacích, zavedeme datovou strukturu |PATTERN|. Zhruba
øeèeno, budeme sledovat vstup znak po znaku a pokud bude èást vstupu
souhlasit s~definovaným patternem, provedeme námi po¾adovanou
akci. Napøíklad nejèastìj¹í aktivitu, pøidání vlnky uvnitø øádku,
spustíme v~okam¾iku, kdy vstupní text odpovídá patternu \uv{\.{\ (v\
p}}, kde \uv{\.{\ }} znamená jedna nebo více mezer a tabelátorù,
\uv{\.{(}} je nula nebo více otevíracích závorek v¹eho druhu,
\uv{\.{v}} znamená jedno písmeno z~mno¾iny pøedlo¾ek (viz |charset|) a
\uv{\.{p}} zde znamená libovolné písmeno. Pøíklad zde není zcela pøesný.
Pøesnì jsou v¹echny patterny pro ná¹ program definovány v~závìreèných
sekcích tohoto povídání.

Pattern bude znamenat koneènou sekvenci tzv. pozic patternu (|PATITEM|).
Cykly uvnitø pozic pro jednoduchost nepøipustíme. Ka¾dá pozice obsahuje
øetìzec znakù, uva¾ovaný pro danou pozici (v~pøíkladu pozice~\uv{\.{\ }} by
obsahovala mezeru a tabelátor, zatímco pozice \.{v} odpovídá |charset|).
Ka¾dá pozice má svùj pøepínaè (|flag|), který obsahuje informaci o~tom,
zda shodu testovaného znaku s~nìkterým prvkem v~mno¾inì znakù
budeme pova¾ovat za úspìch èi neúspìch a zda pozice se ve zkoumaném
øetìzci mù¾e vyskytovat právì jednou nebo opakovanì. Jako druhý pøípad
staèí implementovat \uv{nula nebo více} proto¾e \uv{jedna nebo více} lze
popsat pomocí dvou pozic, první \uv{právì jednou} a následující \uv{nula
nebo více}. Jednotlivé pozice jsou zøetìzeny ukazatelem |next|, poslední
pozice má |next==NULL|. Stejnì tak jednotlivé patterny budeme
sestavovat do seznamù a budou rovnì¾ zøetìzeny ukazatelem |next|.

Pattern kromì øetìzu pozic obsahuje ukazatel na funkci (proceduru) |proc|,
která se má vykonat v~pøípadì, ¾e testovaný øetìzec vyhovuje patternu. 

@d ONE      1        /* flag: prave jeden vyskyt */
@d ANY      2        /* flag: nula nebo vice */
@d ONE_NOT -1        /* flag: prave jednou, znak nesmi byt v mnozine */
@d ANY_NOT -2        /* flag: nula nebo vice, znak nesmi byt v mnozine */

@<Globální deklarace@>=
typedef struct PATITEM {     /* jedna pozice patternu */
   char *str;                /* seznam znaku na teto pozici */
   int flag;                 /* vyznam seznamu znaku */
   struct PATITEM *next ;    /* nasledujici pozice patternu */
} PATITEM;
typedef struct PATTERN {     /* jeden pattern */
   PATITEM *patt;            /* ukazatel na prvni pozici */
   void (*proc)(void);       /* procedura spustena pri souhlasu patternu */
   struct PATTERN *next ;    /* nasledujici v seznamu vsech patternu */
} PATTERN;

@ Deklarujeme nìkteré globální promìnné pro práci s~patterny. |lapi| je pole
obsahující ukazatele na aktuální pozice v~otevøených patternech. Øíkáme,
¾e \uv{pattern je otevøen}, pokud zkoumaný øetìzec s~ním {\it zaèíná\/}
souhlasit. Pattern se uzavøe, pokud nastane jedna ze dvou mo¾ností:
zkoumaný øetìzec s~mím souhlasí a¾ do konce (v~takovém pøípadì se provede
procedura |proc|), nebo pøi vy¹etøování dal¹ích znakù ze zkoumaného
øetìzce pøestane øetìzec s~patternem souhlasit.

V~dané chvíli mù¾e být pattern otevøen nìkolikrát. Napø. pattern \.{abac}
je pøi stringu \.{aba} pøi výskytu druhého \.{a} otevøen podruhé. Proto
pole obsahuje ukazatele na právì aktuální pozici patternu a nikoli na
pattern jako takový.

V~poli |lapi| budou na poèátku samá |NULL| (to se pøi pøekladu inicializuje
samo) a pøemazání ukazatele na pozici konstantou |NULL| budeme pova¾ovat
za zavøení patternu. Vedle pole |lapi| soumìrnì udr¾ujeme pole |lapt|,
do nìho¾ budeme ukládat ukazatele na odpovídající otevøený pattern. Tuto
informaci pou¾ijeme v~pøípadì, ¾e potøebujeme napø, znát |proc|
patternu.

|listpatt| bude ukazovat na zaèátek aktuálního seznamu patternù. Seznamy
budeme mít dva. Jeden se pou¾ije, nacházíme-li se mimo komentáø a druhý
v~pøípadì, ¾e se nacházíme v~prostoru \TeX{}ovského komentáøe (tj. za
procentem). Starty tìchto seznamù patternù jsou |normallist| a
|commentlist| a aktivní |listpatt| má v¾dy jednu z~tìchto dvou hodnot.

Promìnné |lastpt| a |lastpi| pou¾ijeme pro budování øetìzové struktury
patternù.

Promìnná |c| obsahuje právì testovaný znak ze vstupu (který se rovnì¾
pøepí¹e do bufferu |buff|). Z~bufferu obèas ukládáme data do výstupního
proudu. Dìláme to ale v¾dy jen v~okam¾iku, kdy není otevøen ¾ádný
pattern. Tehdy toti¾ \uv{nehrozí} situace, ¾e by nìjaká procedura vyvolaná
souhlasem patternu po¾adovala v~tomto bufferu nìjaké zmìny se zpìtnou
platností. O~vyprázdnìní bufferu se zaèneme zajímat a¾ v~okam¾iku, kdy je
zaplnìn aspoò na hodnotu |BUFI|, abychom proceduru pøepisu bufferu do
výstupního proudu neaktivovali zbyteènì èasto.
@d MAXPATT 200       /* maximalni pocet patternu */
@d MAXBUFF 500      /* velikost bufferu pro operace */
@d BUFI 300         /* velikost stredniho zaplneni */
@<Globální deklarace@>=
PATITEM *lapi[MAXPATT];      /* pole ukazatelu na aktualni pozice */
PATTERN *lapt[MAXPATT];      /* pole odpovidajicich ukazatelu na patterny */
PATTERN *listpatt, *normallist, *commentlist, *pt, *lastpt=NULL;
PATITEM *lastpi=NULL;
char c;             /* zrovna nacetny znak */
char buff[MAXBUFF]; /* prechodny buffer */
int ind;            /* aktualni pozice prechodneho bufferu */

@ Dne 30. 4. 2009 jsem pøidal mo¾nost ètení vstupu, který obsahuje nulové byty.
Takové nuly se pøepisují do výstupu, ale program si jich nev¹ímá pøi
procházení patternù. Tím je mo¾no program pou¾ít na soubory kódované
v UTF16, aèkoli patterny obsahují jen jednobytové ASCII znaky.
Buffer |buff| mù¾e obsahovat i nulové byty, které je tøeba pøepsat do výstupu.
Na druhé stranì buffer |buffnz| obsahuje jen nenulové byty, na které se
nìkdy ptáme pøi pohledu dozadu. Nejdel¹í pohled dozadu je o ètyøi byty.
Udìlám tedy |buffnz| osmibytový, zaènu jej plnit od |buffnz|[4]
a kdykoli je buffer zcela zaplnìn, pøesunu horní ètyøi byty na spodní a dále 
pokraèuji v plnìní bufferu od pozice |buffnz|[4].
@<Globální deklarace@>=
char buffnz[8];
int inz;

@ Nyní definujeme pomocné funkce |setpattern|, |setpi| a |normalpattern|.
Tyto funkce alokují pamì» pomocí standardní funkce |malloc|. Abychom mohli
ohlídat pøípadnou chybu pøi alokaci, budeme allokovat pamì» zprostøedkovanì
pomocí funkce |myalloc|.
@<Pomocné funkce@>=
static void *myalloc (int size)
{
  void *p;
  p = malloc (size);
  if (p == NULL)
  {
    fprintf (stderr, "%s, no memory, malloc failed\n", prog_name);
    exit (BAD_PROGRAM) ;
  }
  return p;
}

@ Funkce |setpattern| alokuje pamì»ové místo struktury |PATTERN| a napojí
ji pomocí promìnné |lastpt| na u¾ alokovaný øetìz patternù. 
Vrátí ukazatel na novì alokované místo. Jednotlivé pozice patternu se musí
následovnì alokovat pomocí |setpi|.
@<Pomocné funkce@>=
static PATTERN *setpattern (void (*proc)(void))
{
  PATTERN *pp;
  pp = myalloc (sizeof (PATTERN));
  pp->proc = proc;  
  pp->next = NULL;
  pp->patt = NULL;
  if (lastpt != NULL) lastpt->next = pp;
  lastpt = pp;
  lastpi = NULL;
  return pp;
}

@ Funkce |setpi| alokuje pamì»ové místo pro jednu pozici patternu. Provede
zøetìzení tak, aby první pozice øetìzu pozic byla zaznamenána v polo¾ce
|patt| ve struktuøe |PATTERN| a dal¹í byly provázány polo¾kou |next| ve
struktuøe |PATITEM|. Poslední pozice má |next==NULL|.
@<Pomocné funkce@>=
static void setpi (char *str, int flag)
{
  PATITEM* p;
  p = myalloc (sizeof (PATITEM));
  p->str = str; p->flag = flag;
  p->next = NULL;
  if (lastpi == NULL) lastpt->patt = p;
  else lastpi->next = p;
  lastpi = p;
}

@ Pøipravme si pùdu pro funkci |normalpattern|. Tato funkce alokuje
strukturu pro jeden pattern vèetnì pozic patternu na základì vstupního
stringu. Ka¾dá pozice patternu obsahuje v~mno¾inì znakù jediný znak a má
|flag=ONE|. Znaky ve vstupním stringu odpovídají po øadì jednotlivým
pozicím. Vytvoøí se vlastnì jakýsi absolutní pattern, tj. testovaný øetìzec
se musí pøesnì shodovat s~uvedeným stringem. Výjimku tvoøí znak |"."|,
který se interpretuje jako nula nebo více mezer. Chceme-li teèku
vnutit do patternu, napí¹eme dvì teèky za sebou.

Nejdøíve deklarujeme pole v¹ech mo¾ných jednopísmenných stringù.
@<Globální deklarace@>=
char strings[512];
int i;

@ Inicializujeme toto pole (znak, nula, znak, nula, atd...).
@<Inicializace datových struktur@>=
for (i=0; i<256; i++) {
  strings[2*i] = (char) i; strings[2*i+1] = 0;
}

@ Definujme funkci |normalpattern|.
@<Pomocné funkce@>=
static PATTERN *normalpattern (void (*proc)(void), const char *str)
{
  PATTERN *pp;
  int j=0;
  pp = setpattern (proc);
  while (str[j]) {
    if (str[j]=='.') {
      j++;
      if (str[j]!='.') {
        setpi (blankscr, ANY); 
        continue;
      }
    }  
    setpi (&strings[(unsigned char)str[j]*2], ONE);
    j++;
  }
  return pp;
}

@ Funkce |match|. Definujeme funkci, která na základì hodnoty znaku |c|
(promìnná |c| je definována jako globální), a pozice patternu |p| (parametr
funkce) vrátí informaci o tom, zda znak souhlasí s patternem. Záporná èísla
|FOUND|, resp. |NOFOUND| znamenají, ¾e je tøeba uzavøít pattern s tím, ¾e
vzor odpovídá, resp. neodpovídá patternu. Nezáporné èíslo vrátí v pøípadì,
¾e zkoumaný vstup stále souhlasí s patternem, ale není je¹tì
rozhodnuto. Velikost návratové hodnoty v takovém pøípadì udává, o kolik
pozic je tøeba se posunout v patternu, abychom mìli ukazatel na pozici
patternu v souhlase s novou situací, zpùsobenou znakem |c|.

Pokud je |c| v mno¾inì znakù pro danou pozici |p->str|, bude |m==1|, jinak
je |m==-1|. Pokud tímto èíslem pronásobíme hodnotu |p->flag|, nemusíme
vìtvení podle |p->flag| programovat dvakrát. Hodnoty |flag| jsou toti¾
symetrické podle nuly, napø. |ANY==-ANY_NOT|.
@d FOUND   -1
@d NOFOUND -2
@<Pomocné funkce@>=
static int match (PATITEM *p)
{
  int m;
  if (strchr (p->str, c) != NULL) m = 1;  /* Znak nalezen */
  else m = -1;                            /* Znak nenalezen */
  switch (m * p->flag) {
  case ANY: return 0;                  /* Souhas, neni nutny posun */
  case ONE: if (p->next == NULL) return FOUND;
            return 1;                    /* Souhas, nutny posun o 1 */
  case ONE_NOT: return NOFOUND;          /* Nesouhlas */
  case ANY_NOT: @<Vra» hodnotu podle následující...@>;
  }
  return 0; /* Tady bychom nikdy nemeli byt, return pro potlaceni varovani */
}

@ O kolik pozic je tøeba se posunout a s jakým výsledkem zjistíme
rekurzivním voláním funkce |match|.
@<Vra» hodnotu podle následující pozice patternu@>=
switch (m = match (p->next)) {
case NOFOUND: return NOFOUND;
case FOUND: return FOUND;
default: return 1 + m;
}

@* Vlnkovací funkce.
Nejprve pøipravíme globální deklarace pro \uv{vlnkovací} funkci |tie|.
Funkce |tie| \uv{ovlnkuje} vstupní soubor |infile| a vytvoøí soubor
|outfile|.  Pøi |silent=0| tiskne závìreènou zprávu o zpracování. V této
zprávì se objeví jméno souboru, které se funkce \uv{dozví} prostøednictvím
globální promìnné |filename|. Promìnná |numline| poèítá øádky, promìnná
|numchanges| sèítá zmìny, tj. poèet doplnìných vlnek. 
Promìnná |mode| nabývý nìkteré z hodnot |TEXTMODE|, |MATHMODE|,
|DISPLAYMODE| a |VERBMODE| podle stavu ve èteném textu.
@d TEXTMODE 0
@d MATHMODE 1
@d DISPLAYMODE 2
@d VERBMODE 3
@<Globální deklarace@>=
char *filename;     /* jmeno zpracovavaneho souboru */
long int numline, numchanges;   /* pro zaverecnou statistiku */
int mode;   

@ Nyní definujeme vlnkovací funkci |tie|. Ve¹kerá èinnost se opírá o
strukturu patternù. Výhodné je (z dùvodu rychlosti) \uv{natvrdo} zde
implementovat jen pøepínání mezi stavem ètení z oblasti komentáøe
(|listpatt==commentlist|) a mimo komentáø (|listpatt==normallist|);
@<Vlnkovací funkce |tie|@>=
static void tie (FILE *input, FILE *output)
{
  int ap;  /* ap je pocet otevrenych patternu */
  register int k, m, n;
  int ic;
  PATTERN *pp;
  PATITEM *pi;

  @<Inicializace promìnných pøi startu funkce |tie|@>;

  while (!feof(input)) {
    if (ap == 0  && ind > BUFI && c !='\\') @<Vyprázdni buffer@>;
    @<Otevøi nové patterny@>;  /* 1. 2. 2010: prohozene poradi */
    if (ind >= MAXBUFF) {
      fprintf (stderr, "Operating buffer overflow, is anything wrong?\n");
      exit (BAD_PROGRAM);
    }
    if ((ic = getc(input)) == EOF)  /* opravil Cejka Rudolf */
      break;
    buff[ind++] = c = ic;
    if (c == 0) continue;     /* 30. 4. 2009 */
    if (inz>=8) {
      for (inz=0; inz<4; inz++) buffnz[inz] = buffnz[inz+4];
      inz=4;
    }
    buffnz[inz++] = c;
    if (c == '\n') numline++, listpatt = normallist;
    if (c == '%' && mode!=VERBMODE && buffnz[inz-2] != '\\') listpatt = commentlist;
    @<Projdi otevøené patterny@>;
  }
  @<Vyprázdni buffer@>;
  if (!web) checkmode ();   /* zaverecna kontrola modu */
  if (!silent) @<Tiskni závìreènou zprávu@>;
}

@ @<Inicializace promìnných pøi ...@>=
for (k=0; k<MAXPATT; k++) lapi[k] = NULL;
c = '\n';
buff[0] = 1; mode = ap = 0;  ind = 1;
for(inz=0; inz<4; inz++) buffnz[inz] = 0;
inz = 4;
numline = 1; numchanges = 0;
mode = TEXTMODE;

@ Pøi manipulaci s bufferem byl pou¾it jeden trik. Ve¹keré naètené znaky
zaèínají a¾ od |buff[1]|, zatímco |buff[0]| je rovno nule. Je to proto, ¾e
nìkteré algoritmy se vrací o jeden znak zpìt za svùj pattern, aby zjistily,
zda tam není symbol \uv{\.{\char92}} (napøíklad na výskyt sekvence
\.{\char92\char37} je tøeba reagovat jinak, ne¾ na výskyt obyèejného
procenta). Kdybychm zazaèali od |buff[0]|, v nìkterých situacích
bychom se ptali, zda |buff[-1]=='\\'|, tj. sahali bychom na neo¹etøené
místo v pamìti. Od 30. 4. 2009 tento problém pominul, proto¾e se ptáme dozadu pouze 
v~|buffnz|, ale vlastnost døíve implementovanou v |buff| jsem ponechal beze zmìny.
@<Vyprázdni buffer@>=
{
  fwrite (&buff[1], ind-1, 1, output);
  ind = 1;
}

@ Pøi procházení otevøenými patterny posunujeme v poli |lapi| pozice
jednotlivých patternù podle pokynù funkce |match|, pøípadnì pattern zavøeme
a pøípadnì vyvoláme proceduru patternu. 

Nìkteré patterny v poli |lapi| u¾ mohou být zavøeny, tak¾e je nutno s tímto
polem pracovat jako s jakýmsi dìravým sýrem.
@<Projdi otevøené patterny@>=
n = ap; k = 0;
while (n) {
  while (lapi[k]==NULL) k++;  /* zastav se na prvnim ukazateli na pattern */
  switch (m = match (lapi[k])) {
  case FOUND:   (*lapt[k]->proc)();  /* Pattern nalezen, spustit proceduru */
  case NOFOUND: lapi[k] = NULL;  /* Deaktivace patternu */
                ap--; break;  
  default:  while (m--) lapi[k] = lapi[k]->next;  /* dalsi pozice patternu */
  }
  k++; n--;
}

@ Pøi otevírání nových patternù, které nejsou v tuto chvíli zablokovány,
se hned vypoøádáme s takovými patterny, které nám dávají rovnou odpovìï
typu |FOUND| nebo |NOFOUND|. V takových pøípadech ani nezaná¹íme ukazatel
na pozici do pole |lapi|.
@<Otevøi nové patterny@>=
pp = listpatt;
if (c) while (pp != NULL) {
  switch (m = match (pp->patt)) {
  case FOUND:    (*pp->proc)();   /* spustit proceduru */
  case NOFOUND: break;
  default: @<Vytvoø ukazatel na nový pattern a |break|@>;
  }
  pp=pp->next;
}

@ Není-li hned známa odpovìï, zda pattern vyhovuje èi nikoli,
pøekontrolujeme nejdøíve, zda u¾ není pattern ve stejné pozici otevøený.
Pak najdeme první \uv{díru} v tabulce |lapi| a tam uhnízdíme nový ukazatel
na pozici v patternu.
@<Vytvoø ukazatel na nový pattern...@>=
pi = pp->patt;
while (m--) pi = pi->next;
n = ap;  k = 0;
while (n) {
  if (lapi[k]==pi) break;
  if (lapi[k++] != NULL) n--;
}
if (!n) {
  k = 0;
  while (lapi[k] != NULL) k++;
  if (k >= MAXPATT) {
    fprintf (stderr, "I cannot allocate pp, is anything wrong?\n");
    exit (BAD_PROGRAM);
  }
  lapt[k] = pp;  lapi[k] = pi; ap++;
}

@ Poslední vìcí ve funci |tie| je tisk závìreèné statistiky zpracování.
@<Tiskni závìreènou zprávu@>=
fprintf (stderr, "~~~ file: %s\t  lines: %ld, changes: %ld\n", 
   filename, numline,  numchanges);

@* Inicializace patternù.
Po vytvoøení pøedchozího kódu opírajícího se o~patterny máme nyní v~ruce
pomìrnì silný nástroj na definování rùzných èinností programu prostým
vytvoøením patternu a pøíslu¹né jeho procedury. Pokud budeme chtít
v~budoucnu nìjaký rys programu pøidat, pravdìpodobnì to bude snadné.

Nejprve deklarujeme nìkteré èasto pou¾ívané skupiny znakù v~patternech.

@<Globální deklarace@>=
char tblanks[] = " ~\t";
char blanks[] =  " \t";
char blankscr[] = " \t\n";
char tblankscr[] = " ~\t\n";
char nochar[] = "%~\n";
char cr[] = "\n";
char prefixes[] = "[({~";
char dolar[] = "$";
char backslash[] = "\\";
char openbrace[] = "{";
char letters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
PATTERN *vlnkalist, *mathlist, *parcheck, *verblist ;

@ Zaèneme definicí nejèastìji pou¾ívaného patternu na vlnkování uvnitø
øádku. Pøipomeòme, ¾e opakované volání funkce |setpattern| vytváøí internì
seznam patternù, pøièem¾ o~jejich propojení se nemusíme starat. Vyzvedneme
si z~návratového kódu funkce pouze ukazatel na první polo¾ku seznamu
|normallist|. Stejnì tak opakované volání funkce |setpi| vytváøí seznam
pozic pro naposledy deklarovaný pattern.
@<Inicializace datových struktur@>=
vlnkalist = setpattern (vlnkain);
setpi (tblankscr, ONE);
setpi (tblanks,   ANY);
setpi (prefixes,  ANY);
setpi (charset,   ONE);
setpi (blanks,    ONE);
setpi (blanks,    ANY);
setpi (nochar,    ONE_NOT);

@ @<Inicializace promìnných pøi ...@>=
listpatt = normallist = vlnkalist;

@ Vlo¾ení vlnky znamená vykonat následující práci: Zapamatovat si znak za skupinou mezer
(do promìnné |p|). Pokud pøed tímto znakem pøedchází nulový byte, pou¾ijeme ho pozdìji, proto
si jej ulo¾íme do promìnné |z|. Dále se posuneme v bufferu vlevo pøes v¹echny mezery, tabelátory
(pøesnìji |blanks|) a pøeskakujeme pøitom v¹echny nulové byty. Index |ind| se zastaví 
na pøedlo¾ce. Posuneme jej doprava za pøedlo¾ku (++|ind|) a pokud tam je nulový byte a první znak 
|tiestr| není nulový, posuneme se a¾ za tento nulový byte. Dále vlo¾íme string |tiestr|, 
neboli vlnku. Nakonec pøipojíme zapamatovaný znak |p|, ov¹em pokud pøed ním byla nula, 
vlo¾íme ji je¹tì pøed znak |p|.
@<Pomocné funkce@>=
static void vlnkain(void)
{
  int i;
  char p, z;
  ind--;
  p = buff[ind--];
  z = buff[ind];
  while (!buff[ind] || (strchr(blanks, buff[ind]) !=NULL)) ind--;
  if (!buff[++ind] && tiestr[0]) ind++; 
  for (i=0; i<tiestrlen; i++) buff[ind++] = tiestr[i];
  i = tiestrlen;                     /* nulu pred p vlozime, pokud je z==0 a */
  if (!tiestr[0]) i--;               /* pocet vlozenych znaku z tiestr */
  if (!z && (i%2)) buff[ind++] = 0;  /* je sudy */
  buff[ind++] = p;
  numchanges++;
}

@ Podobnì pro tvorbu vlnky \uv{pøes øádek} vytvoøíme pattern a kód
procedury.
@<Inicializace dat...@>=
setpattern (vlnkacr);
setpi (tblankscr, ONE);
setpi (tblanks,   ANY);
setpi (prefixes,  ANY);
setpi (charset,   ONE);
setpi (blanks,    ANY);
setpi (cr,        ONE);
setpi (blanks,    ANY);
setpi (nochar,    ONE_NOT);

@ V proceduøe k tomuto patternu musíme o¹etøit pøípad typu 
\uv{\.{a\char126v\char92np}},
kdy nelze prostì pøehodit \uv{\.{\char92n}} za \uv{\.{v}}, proto¾e 
bychom roztrhli
mezeru svázanou vlnkou u¾ døíve. Proto musíme vyhledat vhodné místo pro
roztr¾ení øádku, které bude a¾ {\it pøed\/} znakem \uv{\.{a}}. Pøi dùsledném
o¹etøení tohoto fenoménu mù¾eme dokonce narazit na situaci
\uv{\.{\char92n\ v\char126v\char126v\char92np}},  kde nemù¾eme vlo¾it 
\uv{\.{\char92n}} pøed první výskyt \uv{\.{v}}, proto¾e bychom dostali 
\uv{\.{\char92n\char92n}}, tedy prázdný
øádek. Ten je v \TeX{}u interperetován odli¹nì. V této výjimeèné
situaci pouze zru¹íme stávající (v poøadí druhé) \uv{\.{\char92n}} a
nebudeme vytváøet nové. Na výstupu bude soubor o jeden øádek krat¹í.
@<Pomocné funkce@>=
static void vlnkacr(void)
{
  char p, z;
  int i, j;
  ind--;
  p = buff[ind--];
  z = buff[ind];
  while (!buff[ind] || (strchr(blankscr, buff[ind]) !=NULL)) ind--;
  i = ind;  /* misto predlozky, kterou chceme vazat */
  while (i >= 0 && (strchr(blankscr, buff[i]) == NULL)) i--;
  j = i;
  while (i >= 0 && (!buff[ind] || (strchr(blanks, buff[i]) != NULL))) i--;
  if (i >= 0 && buff[i] == '\n') j = -1;
  if (j >= 0)  buff[j] = '\n';
  else numline--;
  if (!buff[++ind] && tiestr[0]) ind++;
  for (i=0; i<tiestrlen; i++) buff[ind++] = tiestr[i];
  i = tiestrlen;
  if (!tiestr[0]) i--;
  if (!z && (i%2)) buff[ind++] = 0;
  buff[ind++] = p;
  numchanges++;
}

@ Nyní vytvoøíme patterny pro pøípady typu \.{\char92uv\char`\{v lese\char`\}}.
@<Inicializace dat...@>=
setpattern (vlnkain);    /* na radku */
setpi (tblankscr, ONE);
setpi (backslash, ONE);
setpi (letters,   ONE);
setpi (letters,   ANY);
setpi (openbrace, ONE);
setpi (prefixes,  ANY);
setpi (charset,   ONE);
setpi (blanks,    ONE);
setpi (blanks,    ANY);
setpi (nochar,    ONE_NOT);

setpattern (vlnkacr);    /* pres radek */
setpi (tblankscr, ONE);
setpi (backslash, ONE);
setpi (letters,   ONE);
setpi (letters,   ANY);
setpi (openbrace, ONE);
setpi (prefixes,  ANY);
setpi (charset,   ONE);
setpi (blanks,    ANY);
setpi (cr,        ONE);
setpi (blanks,    ANY);
setpi (nochar,    ONE_NOT);



@ Vytvoøíme patterny a proceduru pro potlatèení tvorby vlnky u písmen tìsnì
následujících sekvence \.{\char92TeX} a \.{\char92LaTeX}. Tj. nechceme, aby
napø z textu \uv{\.{Vlastnosti~\char92TeX~u~jsou...}} jsme dostali text
s nesprávnì vázaným písmenem
\uv{\.{Vlastnosti~\char92TeX~u\char126jsou...}}.
@<Inicializace dat...@>=
normalpattern (tielock, "\\TeX");
setpi (blankscr, ONE);
normalpattern (tielock, "\\LaTeX");
setpi (blankscr, ONE);

@ Procedura |tielock| obsahuje neèistý trik. Pøi provádìní procedury je
právì naèten znak z |blankscr| a je ulo¾en do |buff|. Testy na otevírání
nových patternù pro tento znak teprve budou následovat a testují se na
hodnotu promìnné |c|. Staèí tedy zmìnit hodnotu |c| a vlnkovací patterny se
neotevøou.
@<Pomocné funkce@>=
static void tielock (void)
{
  c = 1;
}

@ O¹etøíme nyní pøechod do/z matematického re¾imu \TeX{}u. Uvnitø math
módu vlnky nedìláme. Pøi zji¹tìném nesouladu v pøechodech mezi
math-módy spustíme následující proceduru.
@<Pomocné funkce@>=
static void printwarning (void)
{
  if (!silent)
    fprintf (stderr, 
      "~!~ warning: text/math/verb mode mismatch,  file: %s,  line: %ld\n", 
      filename, numline - (c=='\n'?1:0));
  status = WARNING;
}

@ Zaèneme patterny pro pøechod do/z matematického re¾imu, ohranièeného
jedním dolarem, nebo v La\TeX{}u pøíslu¹nými sekvencemi.  Sekvence
La\TeX{}u \.{\char92(} a \.{\char92)} nejsou zahrnuty, proto¾e bývají
èasto pøedefinovány k jiným u¾iteènìj¹ím vìcem.
@<Inicializace datových ...@>=
if (!nomath) {
  mathlist = setpattern (onedollar);
  setpi (dolar, ONE);
  setpi (dolar, ONE_NOT);
  if (latex) {
    normalpattern (mathin, "\\begin.{math}");
    normalpattern (mathout, "\\end.{math}");
  }
}

@ @<Pomocné funkce@>=
static void mathin (void)
{
  if (mode!=TEXTMODE) printwarning ();
  mode = MATHMODE;
  normallist = listpatt = mathlist;
}
static void mathout (void)
{
  if (mode!=MATHMODE) printwarning ();
  mode = TEXTMODE;
  normallist = listpatt = vlnkalist;
}

@ Pøi programování procedury |onedollar| nesmíme zapomenout na výskyt
sekvence \.{\char92\$}. V tom pøípadì akci ignorujeme. Podobnì u sekvence
\.{\$\$} souhlasí ten druhý dolar s na¹ím patternem, ale to u¾ jsme uvnitø
display módu. V takovém pøípadì také nic nedìláme.
@<Pomocné funkce@>=
static void onedollar (void)
{
  if (buffnz[inz-3]=='\\' || (buffnz[inz-3]=='$' && buffnz[inz-4]!='\\')) return;
  if (mode==DISPLAYMODE) printwarning ();
  else {
    if (mode==TEXTMODE) mathin();
    else mathout();
  }
}

@ Pokud najdeme prázdný øádek, pøekontrolujeme, zda náhodou nejsme v
math-módu. Pokud ano, vypí¹eme varování a pøejdeme do textového módu.
@<Inicializace dat...@>=
parcheck = setpattern (checkmode);
setpi (cr, ONE);
setpi (blanks, ANY);
setpi (cr, ONE);

@ @<Pomocné funkce@>=
static void checkmode (void)
{
  if (mode!=TEXTMODE) {
    printwarning ();
    mode = TEXTMODE;
    normallist = listpatt = vlnkalist;
  }
}

@ Nyní o¹etøíme výskyt dvou dolarù, tj. vstup do/z display módu.
Rovnì¾ mysleme na La\TeX{}isty a jejich prostøedí pro display-mód. Proto¾e
je mo¾ná alternativa s hvìzdièkou na konci názvu prostøedí, radìji u¾
uzavírací závorku do patternu nezahrnujeme.

@<Inicializace dat...@>=
if (!nomath) {
  normalpattern (twodollars, "$$");
  if (latex) {
    normalpattern (displayin, "\\begin.{displaymath");
    normalpattern (displayin, "\\begin.{equation");
    normalpattern (displayout, "\\end.{displaymath");
    normalpattern (displayout, "\\end.{equation");
  }
}

@ @<Pomocné funkce@>=
static void displayin (void)
{
  if (mode!=TEXTMODE) printwarning ();
  mode = DISPLAYMODE; normallist = listpatt = parcheck;
}
static void displayout (void)
{
  if (mode!=DISPLAYMODE) printwarning();
  mode = TEXTMODE; normallist =  listpatt = vlnkalist;
}
static void twodollars (void)
{
  if (buffnz[inz-3]=='\\') return;
  if (mode==DISPLAYMODE) displayout ();
  else displayin ();
}

@ Následuje o¹etøení tzv. verbatim módu. Pro plain i La\TeX{} jsou nejèastìj¹í
závorky pro verbatim mod tyto (variantu s \.{\char92begtt} pou¾ívám
s oblibou já).
@<Inicializace dat...@>=
if (!noverb) {
  verblist = normalpattern (verbinchar, "\\verb"); 
  setpi (blankscr, ANY);
  setpi (blankscr, ONE_NOT); 
  normalpattern (verbin, "\\begtt"); 
  if (latex) normalpattern (verbin, "\\begin.{verbatim");
}
if (web) {
  normalpattern (verbin, "@@<");
  normalpattern (verbin, "@@d");
}
if (!noverb) {
  verboutlist[0] = setpattern (verbout);
  setpi (verbchar, ONE);
  verboutlist[1] = normalpattern (verbout, "\\endtt");
  if (latex) verboutlist[2] = normalpattern (verbout, "\\end{verbatim");
}
if (web) {
  verboutlist[3] = normalpattern (verbout, "@@ ");
  normalpattern (verbout, "@@*");
  normalpattern (verbout, "@@>|");
}


@ Procedura |verbinchar| se od \uv{spoleèné} procedury |verbin| li¹í v
tom, ¾e zavede do stringu |verbchar| momentální hodnotu promìnné |c|.
Proto druhý výskyt této hodnoty verbatim re¾im ukonèí.
@<Pomocné funkce@>=
int prevmode;
PATTERN *prevlist, *verboutlist[4];
char verbchar[2];
static void verbinchar (void)
{
  prevmode = mode;
  verbchar[0] = c;
  c = 1;
  listpatt = normallist = verboutlist[0];
  prevlist = listpatt->next;
  listpatt->next = NULL;
  mode = VERBMODE;
}

@ Pøi programování \uv{obecné} funkce |verbin| musíme dbát na to, aby
zùstal aktivní pouze odpovídající \uv{výstupní} pattern k danému
vstupnímu. Také si zapamatujeme mód, ze kterého jsme do verbatim
oblasti vstoupili, abychom se k nìmu mohli vrátit (napø. uvnitø
math. módu mù¾e být
\.{\char92hbox} a v nìm lokálnì verbatim konstrukce).
@<Pomocné funkce@>=
static void verbin (void)
{ 
  int i;
  i = 0;
  prevmode = mode; 
  switch (c) {
  case 't': i = 1; break;
  case 'm': i = 2; break;
  case '<': ;
  case 'd': i = 3; 
       if (buffnz[inz-3]=='@@') return;  /* dvojity @@ ignorovat */ 
       break;
  }
  listpatt = normallist = verboutlist[i]; 
  prevlist = listpatt->next;
  if (c != '<' && c != 'd')  listpatt->next = NULL;
  mode = VERBMODE;
}

@ @<Pomocné funkce@>=
static void verbout (void)
{
  if (mode!=VERBMODE) return;
  if (web && buffnz[inz-2] == '@@' && buffnz[inz-3] == '@@') return;
  mode = prevmode;
  normallist->next = prevlist;
  switch (mode) {
  case DISPLAYMODE: normallist = listpatt = parcheck; break;
  case MATHMODE: normallist = listpatt = mathlist; break ;
  case TEXTMODE:  normallist = listpatt = vlnkalist; break;
  }
}

@ Nyní implementujeme vlastnost døíve pou¾ívaného programu vlnka, tj. ¾e
lze jeho èinnost vypnout a opìt zapnout v komentáøích. Vytváøíme druhý
nezávislý seznam patternù a proto nejprve pronulujeme |lastpt|.
@<Inicializace dat...@>=
lastpt = 0;
commentlist = normalpattern (tieoff, "%.~.-");
normalpattern (tieon, "%.~.+");

@ @<Pomocné funkce@>=
static void tieoff (void)
{
  normallist = NULL;
}
static void tieon (void)
{
  normallist = vlnkalist;
}

@ Dal¹í plánovaná vylep¹ení. Program by mohl èíst definici svého chování
nejen z~pøíkazové øádky, ale v~mnohem kompletnìj¹í podobì, vèetnì
u¾ivatelsky definovaných patternù, z komentáøové oblasti ve èteném souboru.
Parametry zde uvedené by mohly mít vy¹¹í prioritu, ne¾ parametry 
z~pøíkazové øádky a mohl by se tøeba roz¹iøovat seznam sekvencí, za nimi¾
písmena nemají být vázana vlnkou (zatím je implemenováno na pevno jen
\.{\char92TeX} a \.{\char92LaTeX}). 

@* Rejstøík.


