#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>

#include "sendx.h"

static char *display_name = NULL;
static Display *display;
static Window root;
static Window window = None;
static XKeyEvent event;

static char error_message[256];
static int error_status = 0;

static void 
throw_exception3(const char *msg, const char *s, int i) {
    sprintf(error_message, msg, s, i);
    error_status = 1;
}

static void 
throw_exception3s(const char *msg1, const char *msg2, const char *msg3) {
    sprintf(error_message, msg1, msg2, msg3);
    error_status = 1;
}

static void 
throw_exception(const char *msg) {
    strncpy(error_message,msg,256);
    error_status = 1;
}

/* Added for window managers like swm and tvtwm that follow solbourne's
 * virtual root window concept
 */
static Window
GetRootWindow (Display * disp, int scrn)
{
  Atom __SWM_VROOT = None;
  Window root, rootReturn, parentReturn, *children;
  unsigned int numChildren;
  unsigned i;

  root = RootWindow (disp, scrn);

  /* see if there is a virtual root */
  __SWM_VROOT = XInternAtom (disp, "__SWM_VROOT", False);
  XQueryTree (disp, root, &rootReturn, &parentReturn, &children, &numChildren);
  for (i = 0; i < numChildren; i++)
    {
      Atom actual_type;
      int actual_format;
      unsigned long nitems, bytesafter;
      unsigned char *prop_return = NULL;
      
      if (XGetWindowProperty (disp, children[i], __SWM_VROOT, 0, 1,
			      False, XA_WINDOW, &actual_type, &actual_format,
			      &nitems, &bytesafter, &prop_return) ==
	  Success && prop_return)
	{
	  root = *(Window *)prop_return;
	  break;
	}
    }
  if (children)
    XFree ((char *) children);
  return (root);
}


/*      -       -       -       -       -       -       -       -       -

 * [These functions are from the file "dsimple.c" used with xwininfo.]
 *
 * Written by Mark Lillibridge.   Last updated 7/1/87
 *
 *
 * Window_With_Name: routine to locate a window with a given name on a display.
 *                   If no window with the given name is found, 0 is returned.
 *                   If more than one window has the given name, the first
 *                   one found will be returned.  Only top and its subwindows
 *                   are looked at.  Normally, top should be the Root Window.
 */
static Window
Window_With_Name (Display * dpy, Window top, const char *wname)
{
  Window *children, dummy;
  unsigned int nchildren;
  unsigned i;
  Window w = 0;
  char *window_name;

  if (XFetchName (dpy, top, &window_name) && !strcmp (window_name, wname))
    return (top);

  if (!XQueryTree (dpy, top, &dummy, &dummy, &children, &nchildren))
    return (0);

  for (i = 0; i < nchildren; i++)
    {
      w = Window_With_Name (dpy, children[i], wname);
      if (w)
	break;
    }
  if (children)
    XFree ((char *) children);
  return (w);
}



static int
open_channel (const char *wname)
{
/* display_name = ":0.0"; */
  if ((display = XOpenDisplay (display_name)) == NULL)
    {
      throw_exception ("can't open display");
      return 1;
    }
  if ((root = GetRootWindow (display, DefaultScreen (display))) == 0)
    {
      throw_exception ("Cannot get DefaultScreen");
      return 1;
    }

  if ((wname[0] == '\0') && (window != None))
    {} /* take selected window */
  else if (wname[0] != '\0')
    {
      if ((window = Window_With_Name (display, root, wname)) == None)
	{
	  throw_exception3s ("Display %s: can't open window named \"%s\"",
			     XDisplayName (display_name), wname);
	  return 1;
	}
    }
  else
    {
      throw_exception3 ("bad condition in %s at line %d", __FILE__, __LINE__);
      return 1;
    }

  event.type = KeyPress;
  event.serial = 0;
  event.send_event = False;
  event.display = display;
  event.x = event.y = event.x_root = event.y_root = 0;
  event.time = CurrentTime;
  event.same_screen = True;
  event.subwindow = None;
  event.window = window;
  event.root = root;
  return 0;
}

static void
close_channel (void)
{
/* XFlush(display); */
  XCloseDisplay (display);
}

static void
sendx_channel (KeySym ks, int km)
{
/* 0=regular, 1=shift, 2=lock, 4=control */
  if (ks < 256)
    {
      event.state = isupper ((char) ks);
      switch (ks)
	{
	case 0x08:
	  ks = XK_BackSpace;
	  break;
	case 0x09:
	  ks = XK_Tab;
	  break;
	case 0x0A:
	  ks = XK_Linefeed;
	  break;
	case 0x0B:
	  ks = XK_Clear;
	  break;
	case 0x0D:
	  ks = XK_Return;
	  break;
	case 0x13:
	  ks = XK_Pause;
	  break;
	case 0x14:
	  ks = XK_Scroll_Lock;
	  break;
	case 0x1B:
	  ks = XK_Escape;
	  break;
	}
    }
  else
    event.state = 0;
  event.type = KeyPress;
  event.state = km ; //  Mod1Mask
  event.keycode = XKeysymToKeycode (display, ks);
  if (XSendEvent (display, window, True, 0xfff, (XEvent *) & event) == 0)
    throw_exception ("Error in XSendEvent");
  event.type = KeyRelease;
  if (XSendEvent (display, window, True, 0xfff, (XEvent *) & event) == 0)
    throw_exception ("Error in XSendEvent");
  return;
}

int
sendx_string (const char *string, const char *wname)
{
  const char *p;
  if (open_channel (wname))
    return error_status;
  p = string;
  while (*p)
    sendx_channel (*p++,0);
  close_channel ();
  return error_status;
}

int
sendx_token (const char *string, const char *wname)
{
  if (open_channel (wname))
    return error_status;
  sendx_channel (XStringToKeysym (string),0);
  close_channel ();
  return error_status;
}


int
sendx_alt_token (const char *string, const char *wname)
{
  if (open_channel (wname))
    return error_status;
  sendx_channel (XStringToKeysym (string),Mod1Mask);
  close_channel ();
  return error_status;
}

int
sendx_controlalt_token (const char *string, const char *wname)
{
  if (open_channel (wname))
    return error_status;
  sendx_channel (XStringToKeysym (string), Mod1Mask | ControlMask);
  close_channel ();
  return error_status;
}


int
sendx_control_token (const char *string, const char *wname)
{
  if (open_channel (wname))
    return error_status;
  sendx_channel (XStringToKeysym (string),ControlMask);
  close_channel ();
  return error_status;
}

/*
   void
   PrintKeySyms ()
   {
   int i;
   for (i = 32; i < 127; i++)
   {
   printf ("%s[%c] ", XKeysymToString (i), i);
   }
   for (i = 128 + 32; i < 128 + 127; i++)
   {
   printf ("%s[%c] ", XKeysymToString (i), i);
   }
   printf ("\n");
   }
 */
