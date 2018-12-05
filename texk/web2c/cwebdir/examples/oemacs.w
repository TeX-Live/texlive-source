\datethis
@i xview_types.w

@* Introduction.
This program provides an interface between the GNU Emacs editor and the
OpenWindows environment, using the XView toolkit for interactive graphics.
It is based on \.{emacstool}, a SunView interface written by Jeff Peck
of Sun Microsystems in 1986 and adapted by him in 1988 to \.{xvetool},
an early XView interface. The present code, by Don Knuth, is designed to work
with OpenWindows Version~3 as distributed in 1992, using a Sun Type~4
keyboard.

GNU Emacs is a highly portable and versatile program, which does most
of the hard work involved in editing files. Our job is simply twofold:
To get \.{emacs} started in an appropriate window, and to transmit
keyboard and mouse events as sequences of characters that \.{emacs}
can happily consume.  These simple tasks do, however, require us to
get a lot of fussy details right. In fact, this program could not have been
written without a good deal more knowledge about XView than can be found
in the manuals. Fortunately Jeff Peck works for Sun, and his
inside knowledge has provided the necessary clues. (All complaints below
about the XView documentation are based on the reference manuals and
programming manuals available from Sun and from O'Reilly \AM\ Associates, in
the summer of 1992. Let us hope that such problems will not persist;
computer programming should be fun, but some of the code below was
written only after a bitter struggle!)

The command-line arguments to \.{oemacs} are either standard XView
arguments, which specify the font and the size and position of the window,
the icon, the colors, etc.; or they are standard arguments of \.{emacs},
which typically specify the file to be edited and a position in that file.
If I decide later to make certain things in \.{oemacs} less hardwired,
I will look for options in the X resource database instead of on the
command line.

An important note about using \.{xmodmap} to change the behavior of
certain keys appears below. It makes \.{emacs} more powerful, unless you
have other good uses for those keys. (See the entry for \.{xmodmap}
in the index.)

Before using \.{oemacs}, you must compile \.{emacs} with the
\.{sunfns} module (which the \.{Makefile} will do for you if you've
configured it correctly), and you should include the lines
$$\vbox{\halign{\.{#}\hfil\cr
(load-library "sun-mouse")\cr
(load-library "sun-fns")\cr}}$$
in Emacs's file \.{lisp/site-init.el}.

Caution: This program was developed and tested with Peck's improved
versions of sun-mouse and sun-fns; these are available from
\.{peck@@sun.com} if not yet part of the GNU distribution.

@ We follow the traditional structure of XView applications. The |exit|
statement at the end is important, because \.{oemacs} can be invoked by
the \.{system} command in \TEX/ (when the user has typed \.e in response
to an error message); without |exit(0)|, \TEX/ would complain of trouble
executing this program, although we would simply be terminating the program
without returning any particular value.

@c
@<Include header files@>@;
@#
Frame frame; /* the base frame where we will live */
@<Global variables@>@; /* additional globals besides |frame| */
@#
@<Event-handling procedures@>@;
@#
main(argc,argv)
  int argc;@+char *argv[]; /* typical \UNIX/ setup */
{
  @<Special initialization@>;
  @<Install the components and controls of |frame|@>;
  xv_main_loop(frame);
  exit(0);
}

@ Including the header file \.{<xview/xview.h>} also causes other
basic header files like \.{<xview/frame.h>}, \.{<xview/icon.h>}, etc.,
to be loaded. We must call the \CEE/ compiler with the flag
\.{-I\$(OPENWINHOME)/include} so that the \CEE/ preprocessor will
find the OpenWindows header files.

Some \UNIX/ systems define string functions in \.{<string.h>}, some in
\.{<strings.h>}; the Sun documentation calls for \.{<string.h>}. My
experiments indicate that Sun's compiler and loader work perfectly
well with string functions even when no header files are given, so the
programmer doesn't really have to remember the right name. Similarly,
\.{<stdio.h>} isn't really needed with Sun's \CEE/, unless certain macros
are used. I'll include \.{<string.h>} and \.{<stdio.h>} anyway, in the
spirit of being obedient to the stated rules.

@<Include...@>=
#include <string.h>
#include <stdio.h>
#include <xview/xview.h>

@* The icon and frame.
First we attach an icon that will appear if the user closes the \.{oemacs}
window later.

There are two reasons for doing this first. One is that we might as well
begin with an easy task, in order to get warmed up. The other is that
if we specify the icon {\it after\/} creating the frame, we will unconditionally
override an icon that the user may have specified with the \.{-WI}
option on the command line. (This is one of the little things a
programmer has to turn by trial and error, since the XView documentation
leaves much unsaid.)

The colors used in the icon will be inherited from the frame, so they will
be reversed if the user asks for reverse video. I~prefer to have the icon
always in black on a yellow background, so I'm making the color explicit.
(I hope this will work on monochrome displays; it works fine on my
grayscale monitor.)

@<Create a frame with the gnu icon@>=
{@+Server_image icon_image=(Server_image)xv_create(NULL,SERVER_IMAGE,@|
       XV_WIDTH,64,XV_HEIGHT,64,SERVER_IMAGE_BITS,icon_bits,NULL);
  Server_image mask_image=(Server_image)xv_create(NULL,SERVER_IMAGE,@|
       XV_WIDTH,64,XV_HEIGHT,64,SERVER_IMAGE_BITS,mask_bits,NULL);
  Cms cms=(Cms)xv_create(NULL,CMS,CMS_SIZE,2,@|
        CMS_NAMED_COLORS,"yellow","black",NULL,NULL);
  Icon icon=(Icon)xv_create(NULL,ICON,@|
       ICON_IMAGE,icon_image,ICON_MASK_IMAGE,mask_image,@|
       WIN_CMS,cms,NULL);
  frame=xv_create(NULL,FRAME,FRAME_ICON,icon,NULL);
}

@ @<Include...@>=
#include <xview/cms.h>

@ If the user hasn't specified a label with the \.{-Wl} option, we turn off
the header line at the top of the frame. That gives us a chance to see
two more lines of the file \.{emacs} is editing. However, we add a
label of our own; it will show up in the virtual window manager display.

@<Remove the frame header, unless the user has specifically requested it@>=
if (xv_get(frame,XV_LABEL)==NULL) /* no label specified */
  xv_set(frame,FRAME_SHOW_HEADER,FALSE,XV_LABEL,"OEMACS",NULL);

@ The following icon and mask are derived from the ``what's gnu'' image on the
back cover of the GNU Emacs manual. (This accounts for my black-on-yellow
preference.)

@<Global...@>=
unsigned short icon_bits[]={@|
	0x0000,	0x0000,	0x0000,	0x1E00,
	0x0000,	0x0000,	0x0000,	0x0900,@|
	0x001E,	0x0000,	0x0000,	0x0880,
	0x0064,	0x0000,	0x0000,	0x0440,@|
	0x0088,	0x0000,	0x0000,	0x0420,
	0x0110,	0x0000,	0x0000,	0x0210,@|
	0x0220,	0x0000,	0x0000,	0x0210,
	0x0420,	0x0FCF,	0x01C0,	0x0108,@|
	0x0840,	0x1030,	0x8620,	0x0088,
	0x1080,	0x00C0,	0x5810,	0x0084,@|
	0x1080,	0x1F00,	0x2008,	0x0044,
	0x2100,	0xE200,	0x1004,	0x0044,@|
	0x4103,	0x0400,	0x0002,	0x0042,
	0x4204,	0x080E,	0x0001,	0x0042,@|
	0x8200,	0x7830,	0x0020,	0x8082,
	0x8203,	0x9040,	0x0018,	0x4102,@|
	0x8204,	0x2080,	0x07C6,	0x3E04,
	0x8108,	0x410C,	0x0021,	0x8004,@|
	0x8080,	0x8210,	0x03D0,	0x6008,
	0x4041,	0x0420,	0x0008,	0x1810,@|
	0x403E,	0x0820,	0x0FFC,	0x0620,
	0x2000,	0x1040,	0x0002,	0x01C0,@|
	0x1000,	0x608C,	0x0FFF,	0x0060,
	0x0801,	0x8110,	0x0080,	0x8118,@|
	0x0406,	0x0220,	0x1FFF,	0x66E0,
	0x0238,	0x044F,	0x0000,	0xD800,@|
	0x01C0,	0x0890,	0x8FFF,	0x4000,
	0x0300,	0x10A6,	0x4041,	0x6000,@|
	0x1C00,	0x2026,	0x4FFF,	0x6000,
	0x60CC,	0x4026,	0x4001,	0x6000,@|
	0x1F33,	0x8010,	0x8FFF,	0x4000,
	0x0012,	0x000F,	0x0040,	0xC000,@|
	0x0022,	0x4000,	0x07FF,	0x4000,
	0x0024,	0x4000,	0x0000,	0x2000,@|
	0x0024,	0x4818,	0x8FFF,	0xE000,
	0x0024,	0x4907,	0x0040,	0x2000,@|
	0x0044,	0x4900,	0x1FFF,	0xE000,
	0x0044,	0x4900,	0x0000,	0x2000,@|
	0x0044,	0x4900,	0x07FF,	0xE000,
	0x0044,	0x4880,	0x0020,	0x2000,@|
	0x0044,	0x4880,	0x07FF,	0xE000,
	0x0044,	0x4840,	0x0000,	0x2000,@|
	0x0044,	0x2A20,	0x07FF,	0xE000,
	0x0044,	0x2410,	0x0020,	0x2000,@|
	0x0042,	0x2448,	0x0FFF,	0xE000,
	0x0042,	0x2948,	0x0000,	0x2000,@|
	0x0041,	0x1144,	0x07FF,	0xA000,
	0x0041,	0x1144,	0x2010,	0x1000,@|
	0x0021,	0x1126,	0x20FA,	0x1000,
	0x0024,	0x8925,	0x2600,	0x1000,@|
	0x0014,	0x8924,	0xA138,	0x7000,
	0x0016,	0x88A4,	0x9090,	0x6000,@|
	0x000A,	0x44A4,	0x4880,	0xA000,
	0x0002,	0x44A2,	0x4401,	0x2000,@|
	0x0003,	0x4492,	0x2001,	0x4000,
	0x0001,	0x2451,	0x3002,	0x8000,@|
	0x0000,	0xA251,	0x1E05,	0x0000,
	0x0000,	0x2248,	0xA1F9,	0x8000,@|
	0x0000,	0x1648,	0x9002,	0x8000,
	0x0000,	0x1A28,	0x4C02,	0x8000,@|
	0x0000,	0x1220,	0x43FC,	0x8000,
	0x0000,	0x0120,	0x2000,	0x8000,@|
	0x0000,	0x0120,	0x2003,	0x0000,
	0x0000,	0x0150,	0x1FFC,	0x0000
};
unsigned short mask_bits[]={@|
	0x0000,	0x0000,	0x0000,	0x1E00,
	0x0000,	0x0000,	0x0000,	0x0F00,@|
	0x001E,	0x0000,	0x0000,	0x0F80,
	0x007C,	0x0000,	0x0000,	0x07C0,@|
	0x00F8,	0x0000,	0x0000,	0x07E0,
	0x01F0,	0x0000,	0x0000,	0x03F0,@|
	0x03E0,	0x0000,	0x0000,	0x03F0,
	0x07E0,	0x0FCF,	0x01C0,	0x01F8,@|
	0x0FC0,	0x103F,	0x87F0,	0x00F8,
	0x1F80,	0x00FF,	0xDFF0,	0x00FC,@|
	0x1F80,	0x1FFF,	0xFFF8,	0x007C,
	0x3F00,	0xE3FF,	0xFFFC,	0x007C,@|
	0x7F03,	0x07FF,	0xFFFE,	0x007E,
	0x7E04,	0x0FFF,	0xFFFF,	0x007E,@|
	0xFE00,	0x7FFF,	0xFFFF,	0x80FE,
	0xFE03,	0x9FFF,	0xFFFF,	0xC1FE,@|
	0xFE04,	0x3FFF,	0xFFFF,	0xFFFC,
	0xFF08,	0x7FFF,	0xFFFF,	0xFFFC,@|
	0xFF80,	0xFFFF,	0xFFFF,	0xFFF8,
	0x7FC1,	0xFFFF,	0xFFFF,	0xFFF0,@|
	0x7FFF,	0xFFFF,	0xFFFF,	0xFFE0,
	0x3FFF,	0xFFFF,	0xFFFF,	0xFFC0,@|
	0x1FFF,	0xFFFF,	0xFFFF,	0xFFE0,
	0x0FFF,	0xFFFF,	0xFFFF,	0xFFF8,@|
	0x07FF,	0xFFFF,	0xFFFF,	0xFEE0,
	0x03FF,	0xFFFF,	0xFFFF,	0xF800,@|
	0x01FF,	0xFFFF,	0xFFFF,	0xE000,
	0x03FF,	0xFFFF,	0xFFFF,	0xE000,@|
	0x1FFF,	0xFFFF,	0xFFFF,	0xE000,
	0x7FFF,	0xFFFF,	0xFFFF,	0xE000,@|
	0x1F7F,	0xFFFF,	0xFFFF,	0xC000,
	0x001F,	0xFFFF,	0xFFFF,	0xC000,@|
	0x003F,	0xFFFF,	0xFFFF,	0xC000,
	0x003F,	0xFFFF,	0xFFFF,	0xE000,@|
	0x003F,	0xFFFF,	0xFFFF,	0xE000,
	0x003F,	0xFFFF,	0xFFFF,	0xE000,@|
	0x007F,	0xFFFF,	0xFFFF,	0xE000,
	0x007F,	0xFFFF,	0xFFFF,	0xE000,@|
	0x007F,	0xFFFF,	0xFFFF,	0xE000,
	0x007F,	0xFFFF,	0xFFFF,	0xE000,@|
	0x007F,	0xFFFF,	0xFFFF,	0xE000,
	0x007F,	0xFFFF,	0xFFFF,	0xE000,@|
	0x007F,	0xFFFF,	0xFFFF,	0xE000,
	0x007F,	0xFFFF,	0xFFFF,	0xE000,@|
	0x007F,	0xFFFF,	0xFFFF,	0xE000,
	0x007F,	0xFFFF,	0xFFFF,	0xE000,@|
	0x007F,	0xFFFF,	0xFFFF,	0xE000,
	0x007F,	0xFFFF,	0xFFFF,	0xF000,@|
	0x003F,	0xFFFF,	0xFFFF,	0xF000,
	0x003F,	0xFFFF,	0xFFFF,	0xF000,@|
	0x001F,	0xFFFF,	0xFFFF,	0xF000,
	0x001F,	0xFFFF,	0xFFFF,	0xE000,@|
	0x000B,	0xFFFF,	0xFFFF,	0xE000,
	0x0003,	0xFFFF,	0xFFFF,	0xE000,@|
	0x0003,	0xFFFF,	0xFFFF,	0xC000,
	0x0001,	0xFFFF,	0xFFFF,	0x8000,@|
	0x0000,	0xBFF1,	0xFFFF,	0x0000,
	0x0000,	0x3FF8,	0xFFFF,	0x8000,@|
	0x0000,	0x1FF8,	0xFFFF,	0x8000,
	0x0000,	0x1FF8,	0x7FFF,	0x8000,@|
	0x0000,	0x13E0,	0x7FFF,	0x8000,
	0x0000,	0x01E0,	0x3FFF,	0x8000,@|
	0x0000,	0x01E0,	0x3FFF,	0x0000,
	0x0000,	0x0150,	0x1FFC,	0x0000
};

@* Emulating a terminal.
We will run \.{emacs} in a ``tty subwindow,'' named after the teletype
terminals of ancient yore.

The |argv| array will be a command line that invokes
\.{emacs} with all arguments not removed by |xv_init|, i.e., all arguments
that remain after generic XView arguments have been removed.

We have to say |WIN_IS_CLIENT_PANE|, otherwise fonts specified on the
command line will be ignored. (This cryptic instruction is mentioned briefly
in the XView reference manual, but not in the XView programming manual;
I~would never have discovered it without Jeff Peck's help.)

We also have to set |TTY_QUIT_ON_CHILD_DEATH| to |TRUE|. Otherwise when
\.{emacs} exits (via control-X, control-C) there still will be a terminal
window (into which we can type but not access the shell).

Before starting \.{emacs} we set the environment variable
\.{TERM} equal to \.{sun}.
This will tell \.{emacs} to initialize itself with the
programs in its source file \.{lisp/term/sun.el}, where special
adaptations for Sun-style terminals have been recorded.

@<Put a tty subwindow into the frame@>=
argv[0]="emacs";
putenv("TERM=sun");
tty=(Tty)xv_create(frame,TTY,WIN_IS_CLIENT_PANE,@|
      TTY_QUIT_ON_CHILD_DEATH,TRUE,@|
      TTY_ARGV,argv,NULL);

@ @<Global...@>=
Tty tty;

@ @<Include...@>=
#include <xview/tty.h>

@ The XView manual doesn't tell us that tty subwindows have a view part
and a pseudo-terminal part. (The manual does mention briefly that text
subwindows have views---at the beginning of section 6.3, and indirectly in
one of the examples in the chapter on color.)

The view window of an emulated tty will receive keyboard and
mouse events. We need to know its ``handle,'' because we want to
modify XView's default interpretation of many of those events.

For example, we want to grab the keyboard focus (i.e., to begin receiving
keyboard input) as soon as the user moves the mouse into the \.{emacs}
window. The normal OpenLook default requires a user to click in the window
first, but that is inefficient in a production book-writing environment.
Us \.{emacs} users would rather type than point.

A secret incantation makes the view window accessible. Dear reader,
would you have guessed how to do this, from reading the manuals only,
without looking at Jeff Peck's code? Be honest now.

We don't have to enable the notification of other kinds of events;
tty subwindows already do that.

@<Prepare to be notified when the mouse enters the window@>=
window=(Xv_window)xv_get(tty,OPENWIN_NTH_VIEW,0);
xv_set(window,WIN_CONSUME_EVENT,LOC_WINENTER,NULL);

@ @<Global...@>=
Xv_window window; /* the view window of |tty| */

@ If the user has specified reverse video with the \.{-rv} option,
we will reverse black and white in the mouse cursor. This will make it
more visible against a black background.

Changing the cursor is another undocumented reason why we need to know
about the tty's view window; nothing changes if we try to attach
the new cursor to |frame| or to |tty| instead of to |window|.

@<Change the cursor, to avoid black-on-black@>=
if (rv) {Xv_singlecolor white,black;
  Xv_cursor cursor;
  white.red=white.green=white.blue=255;
  black.red=black.green=black.blue=0;
  cursor=(Xv_cursor)xv_create(NULL,CURSOR,@|
    CURSOR_SRC_CHAR,OLC_BASIC_PTR,CURSOR_MASK_CHAR,OLC_BASIC_MASK_PTR,@|
    CURSOR_FOREGROUND_COLOR,&white,CURSOR_BACKGROUND_COLOR,&black,NULL);
  xv_set(window,WIN_CURSOR,cursor,NULL);
}

@ @<Include...@>=
#include <xview/cursor.h> /* we're using the cursor package */

@ What is the variable |rv| that was tested in the code above? Good question.
We have to scan for \.{-rv} before |xv_init| looks at the command arguments.

@<Scan the command line, setting |rv| nonzero if \.{-rv} is present@>=
rv=0;
{@+int k=argc;
  while (--k>0) if (strcmp(argv[k],"-rv")==0 ||
                     strcmp(argv[k],"-reverse")==0) rv=1;
}

@ @<Global...@>=
int rv;

@ We need to know the height and width of characters in the font, in order
to convert mouse positions into coordinates that \.{emacs} will like.
If the user has specified a font explicitly, the font will presumably have
a fixed width for all characters; we can learn the relevant dimensions
by calling |xv_get|. But if the user has not specified a font, the
situation is trickier; |xv_get| will report the width of the default
{\it non\/}-fixed-width font, and this will probably differ from the width of
the actual fixed-width font the tty subwindow will choose.

Curiously, if we call |xv_find(NULL,FONT,FONT_FAMILY,
FONT_FAMILY_DEFAULT_FIXEDWIDTH,NULL)| {\it before\/} calling |xv_init|,
without even doing anything with the result returned by |xv_find|,
|xv_init| will not install any fonts specified on the command line.
The trick we used for icons---installing the default gnu icon on the first
call to |xv_create|---will therefore fail.

Thus, we have to work around two distinct bugs in XView. The solution
is to discover the effects of the user's command line after |xv_init|
has acted and the frame has been set up; we can determine by brute force what
font will go into the tty window. The program below works correctly
even if the command line specifies \.{-scale} \.{large}, say, instead of
specifying a new font explicitly by something like \.{-font} \.{9x15}.

While we are cataloguing peculiarities of XView, we might as well mention
that the default character dimensions of the default font (Lucida) are
reported to be $8\times13$ in the 12-point (medium) size, $8\times15$ in the
14-point (large) size, and $12\times20$ in the 19-point (extralarge) size.
The actual character dimensions in the window come, however, from the
default fixed-width font (Lucida typewriter), and they are $7\times13$,
$9\times15$, and $11\times23$ in the same three sizes. No logical progression
is evident in either the variable-width or the fixed-width dimensions,
although Lucida is a ``scalable font family.''

@<Compute the height and width of characters in the font@>=
{
  Xv_font font=(Xv_font)xv_get(frame,XV_FONT);
  Xv_font dfont=(Xv_font)xv_find(NULL,FONT,FONT_FAMILY,
     FONT_FAMILY_DEFAULT,NULL);
  if (strcmp((char*)xv_get(font,FONT_NAME),
             (char*)xv_get(dfont,FONT_NAME))==0) {
    /* the user didn't specify a new font by name */
    dfont=(Xv_font)xv_find(NULL,FONT,FONT_FAMILY,
      FONT_FAMILY_DEFAULT_FIXEDWIDTH,NULL);
        /* this one will be used by the tty window */
  } else dfont=font;
  char_width=(int)xv_get(dfont,FONT_DEFAULT_CHAR_WIDTH);
  char_height=(int)xv_get(dfont,FONT_DEFAULT_CHAR_HEIGHT);
}

@ @<Global...@>=
int char_width, char_height; /* character dimensions in the font */

@ @<Include...@>=
#include <xview/font.h> /* header for the font package */

@ OK, we've figured out how to install a tty subwindow with the right
event mask and the right cursor, and how to calculate the sizes of the
characters it will contain. All that remains is for us to do these
operations in the proper order, and to specify a filter routine that
will monitor and edit all communications between the keyboard and the
Emacs processor in the window.

The new ingredient is the filter specification. We tell the XView
notifier to call |filter| when a window event occurs, instead of
letting it call the tty subroutine event handler directly. The parameter
|NOTIFY_SAFE| implies that the window's event handler is
part of XView, not an alien routine.

@<Install the components...@>=
@<Scan the command line, setting |rv| nonzero if \.{-rv} is present@>;
xv_init(XV_INIT_ARGC_PTR_ARGV,&argc,argv,NULL);
      /* start XViewing; strip and remember the OpenWin arguments */
@<Create a frame with the gnu icon@>;
@<Remove the frame header...@>;
@<Put a tty subwindow into the frame@>;
@<Prepare to be notified when the mouse enters the window@>;
@<Change the cursor, to avoid black-on-black@>;
@<Compute the height and width of characters in the font@>;
notify_interpose_event_func(window,filter,NOTIFY_SAFE);

@* Keyboard events.
The job of an interposed filter function is to look at an event and
either process it ourselves or pass it through (possibly modified)
to its normal recipient. In the first case we return the code
value |NOTIFY_DONE|, since we aren't reporting any errors;
in the second case we call the normal event handler and return the value
it returns to us.

An XView event is a data structure that has been partially
interpreted by the XView routines, which add semantic sugar to
the complex union type of X~Window events. We need not look
too far inside an event structure to do the things that concern us.

And what is our job? We are looking for three different kinds of events:

\smallskip
\itemitem{(1)} When the mouse enters the window,
we want to grab the keyboard focus.

\itemitem{(2)} When a mouse button goes up or down, and we have the keyboard
focus, we want to send a coded sequence of characters to \.{emacs}.

\itemitem{(3)} When a key goes down, and we have the keyboard focus, we
want to send an appropriate sequence of characters to \.{emacs}.

\itemitem{(4)} When the status of the Num Lock indicator light changes, we
will send emacs the command {\tt turn-numlock-on} or {\tt turn-numlock-off},
for reasons described below.

\smallskip\noindent Other events, like instructions to repaint or
resize the window, will be passed through without change to the tty window.

@<Event-handling...@>=
Notify_value filter(window,event,arg,type)
  Xv_window window; /* the ``client'' on whom we interposed */
  Event *event; /* the data we are filtering */
  Notify_arg arg; /* optional communication parameter between clients */
  Notify_event_type type; /* |NOTIFY_SAFE| or |NOTIFY_IMMEDIATE| */
{@+register int id=event_id(event);
#ifdef DEBUG
  printf("event %d%s, action %d, shift %x, mouse(%d,%d)\n",
    event_id(event),event_is_up(event)?"UP":"DOWN",event->action,
         event_shiftmask(event),event_x(event),event_y(event));
#endif
  @<Update the Num Lock status@>;
  if (id==LOC_WINENTER) @<Grab the keyboard focus and return@>;
  if (event_is_button(event)) @<Translate a mouse event and return@>;
  if (event_is_up(event)) return NOTIFY_DONE; /* key has been released */
  @<Translate a function key into a special escape sequence@>;
  @<Sanitize a keypress event so that unusual semantic actions are removed@>;
  return notify_next_event_func(window,event,arg,type); /* pass it through */
}

@ It's easy to take charge of the keyboard and mouse, as soon as the mouse
enters our territory.

@<Grab...@>=
{
  win_set_kbd_focus(window,xv_get(window,XV_XID));
  return NOTIFY_DONE;
}

@ If the event is not related to mouse buttons or special function keys,
we want to pass it on unchanged, unless its |event_id| is less than 128.
In that case, it represents a character code, and we want to nuke any
semantic ``keyboard acceleration'' actions it might have been assigned
by OpenWindows.

We also make the Meta key add 128 here. An undocumented macro
called |event_set_id|, found in \.{<xview/win\_input.h>},
clears the |action| code as desired.

@<Sanitize...@>=
if (id<128)
  if (event_meta_is_down(event)) event_set_id(event,id+128);
  else event_set_action(event,ACTION_NULL_EVENT);

@* Function keys.
The Sun Type 4 keyboard has lots of keys, and these can be bound to lots
of useful functions when we are \.{emacs}ing to the max. Besides the
letters and punctuation marks of a normal typewriter, there are ten
``left'' function keys, L1--L10; twelve ``top'' function keys, F1--F12;
fifteen ``right'' function keys, R1--R15; and eight additional keys
labeled Help, Alt, AltGraph, Ins, Del, Enter, $+$, $-$, which we will
pretend have been labeled B1--B8.

The L5 key, also known as Front, is controlled by the Open Look
window manager; it makes a window rise above any that might overlap it,
or shoves the window underneath in case it already was on top.

The L7 key, aka Open, is likewise preempted by the
window manager. It closes a frame to an icon, or opens an icon.

The window manager traps the R2 key and calls it the ``language'' key;
but I have no use for that feature. So I have remapped R2 to the comparatively
useless character $3\over4$, and I will translate it back to R2 below. (The
\.{xmodmap} program allows users to reassign the interpretation of key codes.)
I could have recovered the L5 and L7 keys in the same way, but I like
their functions as they stand. (L5 and L7 still come through if a
Shift, Control, and/or Meta key is down.)

I can never remember the difference between Delete and BackSpace, so I
have mapped them both into control-?, ASCII code 127.

There are two Shift keys, one at the left and one at the right, which
are best kept indistinguishable from each other. Similarly, the left
and right Meta keys (`\.{\char27}') are essentially identical. There's a
Control key too. These three types of modifier keys generate keypress
events, but we ignore them; the only thing that matters to us is whether
they are currently up or down, when other keys are pressed.

\font\ttit=cmitt10
There also are three special keys that do not generate events, so we
never see them. The CapsLock key toggles the Caps Lock light and
changes lowercase letters to uppercase when the light is on. The
NumLock key toggles the Num Lock light and changes the interpretation
of R4--R15 and B4--B8 when that light is on. The Compose key turns the
Compose light on until you type two characters, then it produces a
special symbol if those two characters match a pattern. For example,
when Compose is followed by either \.{a"} or \.{"a} you get the 8-bit
ISO code for {\tt \"a}; when Compose is followed by either \.{th} or
\.{ht} you get the Icelandic thorn; when it's followed by \.{??} you get {\tt
?`}; \.{c/} and \.{L-} give \rlap{\./}\.c and {\ttit\char`\$}
and so on. (A list of all composition patterns
appears in \.{<X11/Suncompose.h>}, but not in any of the manuals
I've seen.) The light goes off after two characters have been
typed, or if your first character is not composable, or if
you hit Compose again prematurely. If no proper two-character pattern
was specified, only ``up'' events (key releases) are transmitted, and
nothing will pass through to \.{emacs}.

One other feature is worth noting: The key between F12 and Delete
produces a backslash `\.\\', or a vertical line `\.{\char125}' when
shifted.  Everybody knows that, but even more is true. If you hold the
AltGraph key down and press this key, it turns out that you get the
broken-bar character `{\tt\hbox to1em{\hss\vbox{\hrule width 1pt height
3pt\vskip1.5pt\hrule height2pt depth1pt}\hss}}'.  This is the only key that the
engineers chose to endow with three different graphic symbols.

A few other anomalies occur; for example, AltGraph-R1 gives ASCII null,
while AltGraph does not affect R4. But I didn't discover any additional
combinations that are either useful or harmful.

Once upon a time the Caps Lock key might have affected the |event_shiftmask|
field of an event, but it has no effect now. The shiftmask is always an
even number, contrary to the implications of \.{<xview/win\_input.h>}.

@ The function keys will be translated into a four-character code.
First comes control-X; then an asterisk; then a letter, \.{a}--\.{o}
for function numbers 1--15, respectively; and then another letter,
identifying left, right, top, or bottom. The final letter is
ordinarily `\.l', `\.r', `\.t', or `\.b', respectively. But it is `\.L', `\.R',
`\.T', or `\.B' if a Shift key is down. Furthermore the Control key
subtracts 64 from the ASCII code, so you get `\.,', `\.2', `\.4', or
`\."' with Control and no Shift. With both Control and Shift you get
\.{\\C-L}, \.{\\C-R}, \.{\\C-T}, \.{\\C-B}. A Meta key adds another 128
to the code.  Thus, each function key leads to eight possibilities.

For example, if F4 is pressed when Control and Shift are down, but not
Meta, the four-letter code is \.{\\C-X*d\\C-T}. The user could type
that sequence of four characters and get the same effect.

Shifted function keys sometimes have a nice mnemonic significance.
For example, key R14, also labeled PgDn, is usually bound to the Emacs
operation `\.{scroll-up}', which moves the window down [sic] by one
screenful; we can then bind Shift-R14 to \.{forward-page}, which advances
down to the next page boundary. In \.{cweb-mode}, the next page boundary
is the next \.{@@*}, beginning a major part of the program. Similarly,
it's convenient to bind B7, the keypad `\.+' key, to \.{forward-paragraph}.
Then in \.{cweb-mode}, Shift-B7 goes to the next module (the next
\.{@@\ } or \.{@@*}).

A Composed character will be preceded by \.{\\C-Q}, the Emacs `quote'
character, to distinguish it from a character that was generated with the
Meta key down. This also applies to the broken-bar character, which
will incidentally be preceded by AltGraph, which is B3; you'll probably
want to bind B3 to a no-op if broken bars are important to you.

@ This program assumes that several key codes have been rebound from
their normal values. Namely, the commands
$$\vbox{\halign{\.{#}\hfil\cr
keysym R2 = threequarters\cr
keysym KP\_Subtract = onehalf\cr
keysym KP\_Add = onequarter\cr
keysym KP\_Enter = threesuperior\cr
keysym KP\_Decimal = twosuperior\cr
keysym KP\_0 = onesuperior\cr}}$$
should be executed by \.{xmodmap}, preferably in the user's \.{.xinitrc} file.
This makes the keys act as $3\over4$, $1\over2$, $1\over4$, $^3$, $^2$, and
$^1$, respectively. The corresponding 8-bit codes are respectively
190, 189, 188, 179, 178, 185. (By the way, can anybody explain why the ISO
LATIN-1 code has $^0$, $^2$, and $^3$ in the positions of meta-0, meta-2,
and meta-3, while $^1$ is in the position of meta-9?)
@.xmodmap@>

We haven't actually bound the keys this way to use them in editing.
We did it to provide linguistically unimportant codes that OpenWindows
wouldn't mess up; its normal conventions make those valuable keys
unusable for editing, except as duplicates for other existing keys.

We send \.{turn-numlock-on/off} commands so that \.{emacs} can keep in
synch with the keyboard state. Namely, it will rebind the function
keys B4--B8 to their numeric-keypad equivalents while the Num Lock light is on.

On the other hand, our remapping does make the Num Lock
feature useless in other (non-Emacs) applications.  If you don't
rebind the keys as stated, you lose the functionality of R2 and B4--B8,
but \.{oemacs} will still work.

The Help key is another special case. We don't want to remap it,
because it gives useful help information with other OpenWindows
applications.  If Help is pressed without the shift or control key,
the |event_id| is zero and the |event_action| is |ACTION_HELP|.
Control-Help is similar, but with |ACTION_TEXT_HELP|. Shift-Help is
more complicated; it invokes `helpopen: starting new Viewer', after
generating an event that has |event_action=WIN_VISIBILITY_NOTIFY|. The
program below considers the Help key B1 to be characterized by any
event with |event_id=0| and either |event_action!=0| or
|event_shiftmask!=CTRLMASK|.

@<Translate a function key into a special escape sequence@>=
{@+register int bank='b'; /* |'l'|, |'r'|, |'t'|, or |'b'| */
  register int n; /* function key serial number, |1<=n<=15| */
  if (id>=KEY_LEFT(1)) @<Translate an ordinary function key@>@;
  else if (id>=256) @<Look for Alt or AltGraph@>@;
  else if (id>=128)
    @<Translate a special function key or composed character@>@;
  else if (id>0 ||
          (event_action(event)==0 && event_shiftmask(event)==CTRLMASK))
    goto non_function;
  else n=1; /* Help key */
emit_function_key:@<Emit the code for a function key and |return|@>;
non_function:;
}

@ I'm assuming here that the event id codes occur in the order left, right,
top, bottom, and that no higher event codes exist.

@<Translate an ordinary function key@>=
{
  if (id<KEY_RIGHT(1)) { /* on the left bank */
    bank='l';@+n=id-KEY_LEFT(0);
  } else if (id<KEY_TOP(1)) { /* on the right bank */
    bank='r';@+n=id-KEY_RIGHT(0);
  } else if (id<KEY_BOTTOM(1)) {
    bank='t';@+n=id-KEY_TOP(0);
  } else n=id-KEY_BOTTOM(0);
  goto emit_function_key;
}

@ The event codes examined here appear in \.{<xview/win\_event.h>}
but not in the XView reference manual.

@<Look for Alt or AltGraph@>=
if (id==SHIFT_ALT) {
  n=2;@+goto emit_function_key;
} else if (id==SHIFT_ALTG) {
  n=3;@+goto emit_function_key;
} else goto non_function;

@ The |ttysw_input| routine sends text to a tty's view window.
The second parameter is a string, not necessarily terminated by
|'\0'| or anything else; the third parameter is the string length.

@<Emit the code for a function key and |return|@>=
{
  if (event_shift_is_down(event)) bank-=32;
  if (event_ctrl_is_down(event)) bank-=64;
  if (event_meta_is_down(event)) bank+=128;
  buf[2]=n+'a'-1;
  buf[3]=bank;
  ttysw_input(window,buf,4);
  return NOTIFY_DONE;
}

@ @<Global...@>=
char buf[]="\030*??\021"; /* |030| and |021| give control-X, control-Q */

@ @<Translate a special function key or composed character@>=
switch (id) {
case 190: bank='r';@+n=2;@+goto emit_function_key;
case 189: n=8;@+goto emit_function_key;
case 188: n=7;@+goto emit_function_key;
case 179: n=6;@+goto emit_function_key;
case 178: n=5;@+goto emit_function_key;
case 185: n=4;@+goto emit_function_key;
default: buf[5]=id; /* composed character or broken-bar */
  ttysw_input(window,buf+4,2);
  return NOTIFY_DONE;
}

@* The NumLock key.
The global variable |num_lock_state| will be 0 if the Num Lock indicator
light is off, 1 if it is on. Whenever an event occurs, we check to see
if |num_lock_state| should change; if so, we change it and send an
appropriate command to \.{emacs}.

To read the state of the keyboard LED indicator lights, we need an I/O
control command called the |KIOCGLED| ioctl, described on the
man page for \.{kb(4m)}.

@<Global...@>=
int num_lock_state;
char turnon[]="\370turn-numlock-on\r", turnoff[]="\370turn-numlock-off\r";
int keyboard; /* file descriptor of \.{/dev/kbd} */

@ @<Include...@>=
#include <sys/file.h> /* definition of |O_RDWR| for |open| */
#include <sundev/kbio.h> /* definition of |KIOCGLED| for |ioctl| */

@ @d LED_NUM_LOCK 0x1 /* the official definition is in \.{<server/sunevq.h>},
    but that header file includes huge gobs of other stuff */

@<Update the Num Lock status@>=
{@+char leds; /* binary encoding of LED lights */
  ioctl(keyboard,KIOCGLED,&leds);
  if ((leds&LED_NUM_LOCK)!=num_lock_state) {
    num_lock_state=leds&LED_NUM_LOCK;
    if (num_lock_state) ttysw_input(window,turnon,17);
    else ttysw_input(window,turnoff,18);
  }
}

@ Any ordinary user can apparently open the keyboard as a file. I would
have tried read-only access if read-write had failed; but read-write access
gives a sense of power even though I won't be writing anything.

@<Special initialization@>=
keyboard=open("/dev/kbd",O_RDWR);
if (keyboard<0) {
  fprintf(stderr,"%s: Can't open /dev/kbd!\n",argv[0]);
  exit(1);
}

@* Mouse events.
When a mouse button is pressed or released, we send \.{emacs} the
codes control-X and ASCII null, followed by a parenthesized list
of four numbers and carriage-return.
For example, as I was typing this paragraph, I
clicked the left mouse button on the screen just for fun; \.{emacs}
received the characters
$$\.{\\030\\0(1 18 28 9999)\\r\\030\\0(129 18 28 141)\\r}$$
as a result (according to `\.{view-lossage}'). I would have received
the same response if I had typed these characters myself, instead
of clicking the mouse.

The first of the four numbers identifies the mouse button itself
as the code number 1, 2, or 4 (for left, middle, right), plus 8 if
a Shift key is down, plus 16 if the Control key is down, plus 32
if a Meta key is down, plus 128 if the mouse key is being released
instead of pressed.

The second number is the row number in the frame, the top row being
considered row~0.

The third number is the column number in the frame, the left column being
considered column~0.

The fourth number is the elapsed time between this mouse event and the
previous one, in milliseconds. If the elapsed time was 10 seconds or
more, 9999 is substituted.

Macros inside \.{emacs} can use the second and third numbers to
position the cursor. The fourth number can be used to determine if the
user is ``double clicking'' or using ``chords.'' Examples of such
macros appear in the Emacs source file \.{lisp/sun-mouse.el}.

Incidentally, the ASCII null character in mouse sequence makes us happy that
the string parameter to |ttysw_input| is not null-terminated.

@<Translate a mouse event...@>=
{@+register int button_code,elapsed_time;
  button_code=(id==MS_LEFT? 1: id==MS_MIDDLE? 2: 4);
  if (event_shift_is_down(event)) button_code += 8;
  if (event_ctrl_is_down(event)) button_code += 16;
  if (event_meta_is_down(event)) button_code += 32;
  if (event_is_up(event)) button_code += 128;
  @<Compute the time elapsed since the previous mouse event@>;
  sprintf(mouse_buf+2,"(%d %d %d %d)\r",button_code,@|
      event_x(event)/char_width, event_y(event)/char_height,@|
      elapsed_time);
  ttysw_input(window,mouse_buf,12+strlen(mouse_buf+12)); /* length is at least 12 */
  return NOTIFY_DONE;
}

@ @<Global...@>=
char mouse_buf[24]="\030";

@ XView's event structure includes |event_time(event)|, which has
type |struct timeval|; this data type is declared in \.{<sys/time.h>},
which is one of the files included automatically as a result of
including \.{<xview/xview.h>}.
A |timeval| structure consists of two |long| integers, |tv_sec| and |tv_usec|,
denoting clock time in seconds and microseconds, respectively.

@<Compute the time...@>=
{@+struct timeval now; /* current time */
  long delta_sec, delta_usec; /* difference between current and
                                 previous time */
  now=event_time(event);
  delta_sec=now.tv_sec-prev_mouse_time.tv_sec;
  delta_usec=now.tv_usec-prev_mouse_time.tv_usec;
  if (delta_usec<0) delta_usec+=1000000,delta_sec--;
  if (delta_sec>=10) elapsed_time=9999; /* infinity (or close enough) */
  else elapsed_time=(delta_sec*1000)+(delta_usec/1000);
  prev_mouse_time=now;
}

@ @<Global...@>=
struct timeval prev_mouse_time;

@* Remaining problems. There's a terribly unfortunate bug in the
present implementation of XView, causing characters of tty subwindows
to be badly painted at crucial times; the rightmost column of pixels
in a character is often clobbered. If I could figure out how to
generate repaint events for the tty subwindow, I might build a mechanism
into \.{oemacs} that does this after the keyboard has been idle for 10
seconds, say.  This would blink the screen; maybe I'll get used to that,
or maybe I'll prefer to refresh the window manually by binding
\.{redraw-display} to the L2 and R1 keys.  In any case a lot of screen
refreshing is necessary at the moment, alas.

(Note added later: I doubt if I'll get used to blinking, and the present
method of manual refresh is tolerable so I won't pursue the 10-second
timer idea. I have meanwhile noticed a procedure |wmgr_refreshwindow(window)|
mentioned in \.{<xview/wmgr.h>}; it will presumably refresh any
given window.

Another bug, much less serious, occurs when the window is resized.
If the window gets smaller, \.{emacs} isn't told to correct its
assumptions; so it puts information in strange places or offscreen.
(Internally, emacs uses the \.{TIOCGWINSZ} or \.{TIOCSWINSZ} ioctl,
described in the man page for \.{termio}.)
You can work around this by first making the window very small, then
making it large.

@* Index.


