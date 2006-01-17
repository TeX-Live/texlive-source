/* key translations */
#include "c-auto.h"
#include "xdvi.h"
#include "version.h"

static const char base_key_translations[] =
/* note: more specific key bindings must always come first
   (i.e. `alt ctrl' before `ctrl' before unmodified)
   `:' at start of description means that case matters
*/
    "Ctrl<Key>]:	pagehistory-delete-forward()\n"
    "Ctrl<Key>[:	pagehistory-delete-backward()\n"
/* it seems the following is needed for compatibility with german, french,
   italian, spanish and other keyboards that have [, ] on right Alt key? */
    "Ctrl Mod2<Key>]:	pagehistory-delete-forward()\n"
    "Ctrl Mod2<Key>[:	pagehistory-delete-backward()\n"
    "Mod2<Key>]:	pagehistory-forward()\n"
    "Mod2<Key>[:	pagehistory-back()\n"
    "<Key>]:		pagehistory-forward()\n"
    "<Key>[:		pagehistory-back()\n"
    "Alt Ctrl<Key>+:	change-density(10)\n" /* case insensitive, otherwise too hard to type */
    ":Alt Ctrl<Key>-:	change-density(-10)\n"
    "Ctrl<Key>+:	set-shrink-factor(+)\n" /* case insensitive, otherwise too hard to type */
    ":Ctrl<Key>-:	set-shrink-factor(-)\n"
    ":<Key>-:		minus()\n"
    ":<Key>?:		help()\n"
    ":<Key>>:		goto-page()\n"
    ":<Key><:		goto-page(1)\n"
    ":<Key>^:		home()\n"
    "<Key>space:	unpause-or-next()\n"
    ":<Key>B:		htex-back()\n"
    ":<Key>b:		back-page()\n"
/* removed, redundant and gets in the way
    ":Ctrl<Key>c:	quit()\n" */
#if COLOR
    ":<Key>C:		set-color()\n"
#endif
    ":<Key>c:		center()\n"
/* removed, redundant and gets in the way
    ":Ctrl<Key>d:	quit()\n" */
    ":<Key>d:		down()\n"
    "Ctrl<Key>f:	find()\n"
    ":<Key>F:		htex-forward()\n"
    ":~Alt<Key>f:	forward-page()\n"
    "Ctrl<Key>g:	find-next()\n"
#ifdef GREY
    ":<Key>G:		set-greyscaling()\n"
#endif
    ":<Key>g:		goto-page()\n"
    "Ctrl<Key>h:	back-page()\n"
    ":~Alt<Key>h:	help()\n"
    ":<Key>i:		htex-anchorinfo()\n"
    "Ctrl<Key>j:	forward-page()\n"
    ":<Key>k:		set-keep-flag()\n"
    "Ctrl<Key>l:	fullscreen()\n"
    ":<Key>l:		left()\n"
    "Ctrl<Key>m:	switch-mode()\n"
    ":<Key>M:		set-margins()\n"
    ":~Alt<Key>m:	toggle-mark()\n"
    "~Alt Ctrl<Key>n:	toggle-mark()forward-page()\n"
    ":~Alt<Key>n:	forward-page()\n"
    "Ctrl<Key>o:	select-dvi-file()\n"
    ":~Alt<Key>o:	ruler-snap-origin()\n"
    /* this used to be show-display-attributes() - use the output of `-debug PK' instead */
    "Ctrl<Key>p:	print()\n"
    ":<Key>P:		declare-page-number()\n"
    ":<Key>p:		back-page()\n"
    ":<Key>q:		quit()\n"
    "Ctrl<Key>r:	forward-page(0)\n" /* redraw page */
    ":<Key>R:		reread-dvi-file()\n"
    ":<Key>r:		right()\n"
    "Ctrl<Key>s:	save()\n"
    ":<Key>S:		set-density()\n"
    ":<Key>s:		set-shrink-factor()\n"
    ":<Key>T:		use-tex-pages(toggle)\n"
    ":<Key>t:		switch-magnifier-units()\n"
    "Ctrl<Key>u:	back-page()toggle-mark()\n"
    ":<Key>u:		up()\n"
    "Ctrl<Key>v:	show-source-specials()\n"
#ifdef PS_GS
    ":<Key>V:		set-gs-alpha()\n"
#endif /* PS_GS */
#ifdef PS
    ":<Key>v:		set-ps()\n"
#endif
    "Ctrl<Key>x:	source-what-special()\n"
    ":<Key>x:		set-expert-mode()\n"
#ifdef VMS
    "~Alt Ctrl<Key>z:	quit()\n"
#endif
    "<Key>Return:	forward-page()\n"
    "<Key>Delete:	up-or-previous()\n"
    "<Key>BackSpace:	back-page()\n"
#ifdef MOTIF
    "<Key>osfDelete:	up-or-previous()\n"
    "<Key>osfBackSpace:	back-page()\n"
    "Ctrl<Key>osfBeginLine:	goto-page(1)\n"
    "Ctrl<Key>osfEndLine:	goto-page()\n"
    "<Key>osfBeginLine:	home-or-top()\n"
    "<Key>osfEndLine:	end-or-bottom()\n"
    "<Key>osfLeft:	left(0.015)\n"
    "<Key>osfUp:	up(0.015)\n"
    "<Key>osfRight:	right(0.015)\n"
    "<Key>osfDown:	down(0.015)\n"
    "<Key>osfPageUp:	back-page()\n"
    "<Key>osfPageDown:	forward-page()\n"
    "<Key>osfCancel:	discard-number()\n"
#else
    "Ctrl<Key>Home:	goto-page(1)\n"
    "Ctrl<Key>End:	goto-page()\n"
    "<Key>Home:		home-or-top()\n"
    "<Key>End:		end-or-bottom()\n"
    "<Key>Left:		left(0.015)\n"
    "<Key>Up:		up(0.015)\n"
    "<Key>Right:	right(0.015)\n"
    "<Key>Down:		down(0.015)\n"
    "<Key>Prior:	back-page()\n"
    "<Key>Next:		forward-page()\n"
    "<Key>Escape:	discard-number()\n"
#endif
#ifdef XK_KP_Left
    "<Key>KP_Home:	home()\n"
    "<Key>KP_Left:	left()\n"
    "<Key>KP_Up:	up()\n"
    "<Key>KP_Right:	right()\n"
    "<Key>KP_Down:	down()\n"
    "<Key>KP_Prior:	back-page()\n"
    "<Key>KP_Next:	forward-page()\n"
    "<Key>KP_Delete:	up-or-previous()\n"
    "<Key>KP_Enter:	forward-page()\n"
#endif
    "";


static const char base_mouse_translations[] =
    "Shift<Btn1Down>:	drag(+)\n"
    "Shift<Btn2Down>:	drag(|)\n"
    "Shift<Btn3Down>:	drag(-)\n"
    "Ctrl<Btn1Down>:	source-special()\n"
    "<Btn1Down>:	do-href()magnifier(*1)\n"
    "<Btn2Down>:	do-href-newwindow()magnifier(*2)\n"
    "<Btn3Down>:	magnifier(*3)\n"
    "<Btn4Down>:	magnifier(*4)\n"
    "<Btn5Down>:	magnifier(*5)\n"
    "";


#if defined(NEW_MENU_CREATION) || defined(MOTIF)
static const char default_menu_config[] =
/*
  BNF-like Syntax of the entries:

  entry ::= entry-line `\n' ( entry )*
  entry-line ::= menu-spec `:' button-type `:' mnemonic `:' action
  menu-spec ::= menu-name ( `>' menu-name )*
  menu-name ::= menu-title `|' accelerator

  menu-title ::= String
  mnemonic ::= <empty> | String
  action ::= <empty> | String
  accelerator ::= `A'|`B'|`C'|...|`a'|`b'|`c'|...
  button-type ::= `PUSH' | `RADIO' | `CHECK' | `SEP'

  Notes:
     - In entry-line, every occurrence of `:' that's not a separator needs to be escaped
       like this: `\:'; similar for `>' in menu-spec and `|' in menu-title.
     - Every `>' in menu-spec creates a submenu for the corresponding menu.
     - mnemonic is the key binding for the action, displayed as mnemonic string in
       the Motif menus (not yet in the Xaw version, since Xaw doesn't provide for
       such mnemonics by default).
     - accelerator is the Motif menu accelerator key.
 */
    "File|F > Open ...|O		:PUSH	:Ctrl-o	:select-dvi-file()\n"
    "File|F > Open Recent |R		:PUSH	:	:recent-files()\n"
    "File|F > Reload|R			:PUSH	:R	:reread-dvi-file()\n"
    "File|F > Find ...|F		:PUSH	:Ctrl-f :find()\n"
    "File|F > Find next ...|N		:PUSH	:Ctrl-g :find-next()\n"
    "File|F > Print ...|P		:PUSH	:Ctrl-p :print()\n"
    "File|F > Save as ...|S		:PUSH	:Ctrl-s	:save()\n"
    "File|F > Quit|Q			:PUSH	:q	:quit()\n"
/* ========== */			
    "Navigate|N > Page-10|		:PUSH	:10 p	:back-page(10)\n"
    "Navigate|N > Page-5|		:PUSH	:5 p	:back-page(5)\n"
    "Navigate|N > Prev|P		:PUSH	:p	:back-page(1)\n"
    "Navigate|N > Next|N		:PUSH	:n	:forward-page(1)\n"
    "Navigate|N > Page+5|		:PUSH	:5 n	:forward-page(5)\n"
    "Navigate|N > Page+10|		:PUSH	:10 n	:forward-page(10)\n"
    "Navigate|N				:SEP	::\n"
    "Navigate|N > First Page|F		:PUSH	:1 g	:goto-page(1)\n"
    "Navigate|N > Last Page|L		:PUSH	:g	:goto-page()\n"
    "Navigate|N				:SEP	::\n"
    "Navigate|N > Page History Back|	:PUSH	:[	:pagehistory-back()\n"
    "Navigate|N > Page History Forward| :PUSH	:]	:pagehistory-forward()\n"
/*     "Navigate|N				:SEP	::\n" */
/*     "Navigate|N	> Clear Page History|	:PUSH	:	:pagehistory-clear()\n" */
    "Navigate|N				:SEP	::\n"
    "Navigate|N > Back Hyperlink|B	:PUSH	:B	:htex-back()\n"
    "Navigate|N > Forward Hyperlink|F	:PUSH	:F	:htex-forward()\n"
/* ========== */
    "Zoom|Z > Shrink by 1|1		:RADIO	:1 s	:set-shrink-factor(1)\n"
    "Zoom|Z > Shrink by 2|2		:RADIO	:2 s	:set-shrink-factor(2)\n"
    "Zoom|Z > Shrink by 3|3		:RADIO	:3 s	:set-shrink-factor(3)\n"
    "Zoom|Z > Shrink by 4|4		:RADIO	:4 s	:set-shrink-factor(4)\n"
    "Zoom|Z > Shrink by 5|5		:RADIO	:5 s	:set-shrink-factor(5)\n"
    "Zoom|Z > Shrink by 6|6		:RADIO	:6 s	:set-shrink-factor(6)\n"
    "Zoom|Z > Shrink by 7|7		:RADIO	:7 s	:set-shrink-factor(7)\n"
    "Zoom|Z > Shrink by 8|8		:RADIO	:8 s	:set-shrink-factor(8)\n"
    "Zoom|Z > Shrink by 9|9		:RADIO	:9 s	:set-shrink-factor(9)\n"
    "Zoom|Z				:SEP	::\n"
    "Zoom|Z > Fit in Window|F		:PUSH	:s	:set-shrink-factor(a)\n"
    "Zoom|Z				:SEP	::\n"
    "Zoom|Z > Zoom In|I			:PUSH	:Ctrl-+	:set-shrink-factor(+)\n"
    "Zoom|Z > Zoom Out|O		:PUSH	:Ctrl--	:set-shrink-factor(-)\n"
/* ========== */
    "Mark|M > Mark All Pages|M		:PUSH	:-1 m	:toggle-mark(-1)\n"
    "Mark|M > Unmark All Pages|U	:PUSH	:0 m	:toggle-mark(0)\n"
    "Mark|M				:SEP	::\n"
    "Mark|M > Toggle Odd Pages|O	:PUSH	:1 m	:toggle-mark(1)\n"
    "Mark|M > Toggle Even Pages|E	:PUSH	:2 m	:toggle-mark(2)\n"
    "Mark|M > Toggle Current Page|C	:PUSH	:m	:toggle-mark()\n"
/* ========== */
    "Options|O > Keep Position|K		:CHECK	:k	:set-keep-flag(toggle)\n"
    "Options|O > Use TeX Page Numbers|T	:CHECK	:T	:use-tex-pages(toggle)\n"
    "Options|O				:SEP	::\n"
/* removed this one, since it only duplicates the toolbar and is too awkward to use */
/*     "Options|O > Fonts|F > Fonts Darker|D		:PUSH	:Alt-Ctrl-+	:change-density(5)\n" */
/*     "Options|O > Fonts|F > Fonts Lighter|L		:PUSH	:Alt-Ctrl--	:change-density(-5)\n" */
/* ---------- */
#if 0
/* TODO: implement setting paper size at runtime */
    "Options|O > Paper|a > US|U		:RADIO	:	:set-papersize(us)\n"
    "Options|O > Paper|a > Legal|L	:RADIO	:	:set-papersize(legal)\n"
    "Options|O > Paper|a > Foolscap|F	:RADIO	:	:set-papersize(foolscap)\n"
    "Options|O > Paper|a > A1|1		:RADIO	:	:set-papersize(a1)\n"
    "Options|O > Paper|a > A2|2		:RADIO	:	:set-papersize(a2)\n"
    "Options|O > Paper|a > A3|3		:RADIO	:	:set-papersize(a3)\n"
    "Options|O > Paper|a > A4|4		:RADIO	:	:set-papersize(a4)\n"
    "Options|O > Paper|a > A5|5		:RADIO	:	:set-papersize(a5)\n"
    "Options|O > Paper|a > A6|5		:RADIO	:	:set-papersize(a6)\n"
    "Options|O > Paper|a > A7|7		:RADIO	:	:set-papersize(a7)\n"
    /* Note: the b* and c* DIN sizes are too rarely used to be exposed here;
       people can still customize the Menu if they really need them. */
    "Options|O > Paper|a		:SEP	::\n"
    "Options|O > Paper|a > Landscape|L	:CHECK	:	:set-paper-landscape(toggle)\n"
#endif /* 0 */
/* ---------- */
# if PS
    "Options|O > Postscript|o > Show PS|S			:RADIO	:1 v	:set-ps(1)\n"
    "Options|O > Postscript|o > Show PS and Bounding Boxes|B	:RADIO	:2 v	:set-ps(2)\n"
    "Options|O > Postscript|o > Show Bounding Boxes Only|O	:RADIO	:0 v	:set-ps(0)\n"
    "Options|O > Postscript|o					:SEP	::\n"
    "Options|O > Postscript|o > Use PS Anti-Aliasing|A		:CHECK	:V	:set-gs-alpha(toggle)\n"
# endif
/* ---------- */
/* removed, since it was redundant to Preferences */
/* # ifdef MOTIF */
/*     "Options|O > Window Configuration|W > Show Statusline|S	:CHECK	:1x	:set-expert-mode(1)\n" */
/*     "Options|O > Window Configuration|W > Show Scrollbars|c	:CHECK	:2x	:set-expert-mode(2)\n" */
/*     "Options|O > Window Configuration|W > Show Pagelist|P	:CHECK	:3x	:set-expert-mode(3)\n" */
/*     "Options|O > Window Configuration|W > Show Toolbar|T	:CHECK	:4x	:set-expert-mode(4)\n" */
/* # endif */
/* ---------- */
    "Options|O > Mouse Mode|M > Magnifier|M			:RADIO	:0 Ctrl-m	:switch-mode(0)\n"
    "Options|O > Mouse Mode|M > Text Selection|T		:RADIO	:1 Ctrl-m	:switch-mode(1)\n"
    "Options|O > Mouse Mode|M > Ruler|R				:RADIO	:2 Ctrl-m	:switch-mode(2)\n"
    "Options|O				:SEP	::\n"
    "Options|O > Preferences ...|P 	:PUSH	:		:prefs-dialog()\n"
/* ========== */
    "Help|H > Introduction ...|I	:PUSH	:		:help(Introduction)\n"
    "Help|H > Page Motion ...|P		:PUSH	:		:help(Page Motion)\n"
    "Help|H > Other Commands ...|O	:PUSH	:		:help(Other Commands)\n"
    "Help|H > Hyperlinks ...|H		:PUSH	:		:help(Hyperlinks)\n"
    "Help|H > Mouse Buttons ...|M	:PUSH	:		:help(Mouse Buttons)\n"
    "Help|H > Mouse Modes ...|d		:PUSH	:		:help(Mouse Modes)\n"
    "Help|H > String Search ...|e	:PUSH	:		:help(String Search)\n"
    "Help|H > Printing and Saving ...|S	:PUSH	:		:help(Printing and Saving)\n"
    "Help|H > Source Specials ...|r	:PUSH	:		:help(Source Specials)\n"
    "Help|H				:SEP	::\n"
    "Help|H > New Features in this Version|F:PUSH::load-url(http\\://xdvi.sourceforge.net/releases.html#" XDVI_VERSION ")\n"
    "Help|H > Submit a Bug Report|B	:PUSH	::load-url(http\\://sourceforge.net/tracker/?func=add&group_id=23164&atid=377580)\n"
    "";
#else /* NEW_MENU_CREATION */
/* TODO:
   - describe this in manpage
   - use in the Motif version as well
*/
static const char default_menu_config[] =
/*  menu:	submenu:		accelerator:	action */
    "File:	Open ...:		Ctrl-o:	select-dvi-file()\n"
    "File:	Open Recent:		:	recent-files()\n"
    "File:	Find ...:		Ctrl-f:	find()\n"
    "File:	Find next ...:		Ctrl-g:	find-next()\n"
    "File:	Reload:			R:	reread-dvi-file()\n"
    "File:	Save as ...:		Ctrl-s:	save()\n"
    "File:	Print ...:		Ctrl-p:	print()\n"
    "File:	Quit:			q:	quit()\n"
    "Navigate:	First:			1 g:	goto-page(1)\n"
    "Navigate:	Page-10:		10 p:	back-page(10)\n"
    "Navigate:	Page-5:			5 p:	back-page(10)\n"
    "Navigate:	Prev:			p:	back-page(1)\n"
    "Navigate:	SEP\n"
    "Navigate:	Next:			n:	forward-page(1)\n"
    "Navigate:	Page+5:			5 n:	forward-page(5)\n"
    "Navigate:	Page+10:		10 n:	forward-page(10)\n"
    "Navigate:	Last:			g:	goto-page()\n"
    "Navigate:	SEP\n"
    "Navigate:	Back Hyperlink:		B:	htex-back()\n"
    "Navigate:	Forward Hyperlink:	F:	htex-forward()\n"
    "Zoom:	Zoom In:		Ctrl-+:	set-shrink-factor(+)\n"
    "Zoom:	Zoom Out:		Ctrl--:	set-shrink-factor(-)\n"
    "Zoom:	SEP\n"
/*      "Zoom:Full size (1 s):set-shrink-factor(1)\n" */
    "Zoom:	Fit in Window:		s:	set-shrink-factor(a)\n"
    "Zoom:	SEP\n"
    "Zoom:	$#:			1 s:	set-shrink-factor(1)\n"
    "Zoom:	$#:			2 s:	set-shrink-factor(2)\n"
    "Zoom:	$#:			3 s:	set-shrink-factor(3)\n"
    "Zoom:	$#:			4 s:	set-shrink-factor(4)\n"
    "Zoom:	$#:			5 s:	set-shrink-factor(5)\n"
    "Zoom:	$#:			6 s:	set-shrink-factor(6)\n"
    "Zoom:	$#:			7 s:	set-shrink-factor(7)\n"
    "Zoom:	$#:			8 s:	set-shrink-factor(8)\n"
    "Zoom:	$#:			9 s:	set-shrink-factor(9)\n"
/*     "Zoom:$%%:shrink-to-dpi(150)\n" */
/*     "Zoom:$%%:shrink-to-dpi(100)\n" */
/*     "Zoom:$%%:shrink-to-dpi(50)\n" */
    "Mark:	Mark All Pages:			-1 m:	toggle-mark(-1)\n"
    "Mark:	Unmark All Pages:		0 m:	toggle-mark(0)\n"
    "Mark:	SEP\n"
    "Mark:	Toggle Odd Pages:		1 m:	toggle-mark(1)\n"
    "Mark:	Toggle Even Pages:		2 m:	toggle-mark(2)\n"
    "Mark:	Toggle Current Page:		m:	toggle-mark()\n"
    "Modes:	Magnifier:			Ctrl-m:	switch-mode(0)\n"
    "Modes:	Text Selection:			Ctrl-m:	switch-mode(1)\n"
    "Modes:	Ruler:				Ctrl-m:	switch-mode(2)\n"
# if PS
    "Options:	Show PS:			1 v:	set-ps(1)\n"
    "Options:	Show PS and Bounding Boxes:	2 v:	set-ps(2)\n"
    "Options:	Show Bounding Boxes Only:	0 v:	set-ps(0)\n"
    "Options:	SEP\n"
    "Options:	Use PS Anti-Aliasing:	V:	set-gs-alpha(toggle)\n"
# endif
    "Options:	SEP\n"
/*     "Options:Keep position:set-keep-flag(1)\n" */
/*     "Options:Don't keep position:set-keep-flag(0)\n" */
    "Options:	Keep Position:			k:	set-keep-flag(toggle)\n"
    "Options:	Use TeX Pages:			T:	use-tex-pages(toggle)\n"
/* hmm, this probably is better kept in a `preferences' window: */
/*     "Options:SEP\n" */
/*     "Options:Show statusline:1 x:set-expert-mode(1)\n" */
    "Options:	SEP\n"
    "Options:	Fonts Darker:		Alt-Ctrl-+:	change-density(5)\n"
    "Options:	Fonts Lighter:		Alt-Ctrl--:	change-density(-5)\n"
    "Help:	Introduction ...:	:		help(Introduction)\n"
    "Help:	Page Motion ...:	:		help(Page Motion)\n"
    "Help:	Other Commands ...:	:		help(Other Commands)\n"
    "Help:	Hyperlinks ...:		:		help(Hyperlinks)\n"
    "Help:	Mouse Buttons ...:	:		help(Mouse Buttons)\n"
    "Help:	Mouse Modes ...:	:		help(Mouse Modes)\n"
    "Help:	String Search ...:	:		help(String Search)\n"
    "Help:	Printing and Saving ...::		help(Printing and Saving)\n"
    "Help:	Source Specials ...:	:		help(Source Specials)\n"
    "Help:SEP\n"
    "Help:	New Features in this Version::	load-url(http\\://xdvi.sourceforge.net/releases.html#" XDVI_VERSION ")\n"
    "Help:	Send a Bug Report:	:		load-url(http\\://sourceforge.net/tracker/?func=add&group_id=23164&atid=377580)\n"
    "";
#endif /* NEW_MENU_CREATION */

static const char default_toolbar_translations[] =
/*  index	statusline-text					tooltip		action */
    "SPACER(5)\n"
    "0:	Open a new file   (Key\\: Ctrl-o):			Open file:	select-dvi-file()\n"
    "SPACER(10)\n"
    "1:	Reload this file   (Key\\: R):				Reload file:	reread-dvi-file()\n"
    "SPACER(10)\n"
    "2:	Go to the first page of this file   (Key\\: 1g):	First page:	goto-page(1)\n"
    "3:	Go to the previous page of this file   (Key\\: p):	Previous page:	back-page(1)\n"
    "4:	Go to the next page of this file   (Key\\: n):	Next page:	forward-page(1)\n"
    "5:	Go to the last page of this file   (Key\\: g):	Last page:	goto-page(e)\n"
    "SPACER(10)\n"
    "8:	Back to previous page in history   (Key\\: [):	Page History back:	pagehistory-back()\n"
    "9:	Forward to next page in history   (Key\\: ]):	Page History forward:	pagehistory-forward()\n"
    "SPACER(10)\n"
    "6:	Zoom in   (Key\\: Ctrl-+):		Zoom in:		set-shrink-factor(+)\n"
    "7:	Zoom out   (Key\\: Ctrl--):		Zoom out:		set-shrink-factor(-)\n"
    "SPACER(10)\n"
    "11:Print this DVI file:Print:print()\n"
    "SPACER(10)\n"
    "16:Find in DVI file   (Key\\: Ctrl-f):	Find in file:	find()\n"
    "SPACER(10)\n"
    "12:Toggle marks of odd pages   (Key\\: 1m):	Toggle odd pages:	toggle-mark(1)\n"
    "13:Toggle marks of even pages   (Key\\: 2m):	Toggle even pages:	toggle-mark(2)\n"
    "14:Toggle mark of current page   (Key\\: 2m):	Toggle current page:	toggle-mark()\n"
    "15:Unmark all pages   (Key\\: 0m):			Unmark all pages:	toggle-mark(0)\n"
    "SPACER(10)\n"
    "19:Display fonts darker   (Key\\: Alt-Ctrl-+):	Fonts darker:		change-density(5)\n"
    "20:Display fonts lighter   (Key\\: Alt-Ctrl--):	Fonts lighter:		change-density(-5)\n"
    "";
