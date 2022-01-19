#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define VERSION 1
#define REVISION 0
#define PATCHLEVEL 0

#if 0
#include "rendernative.h" /* rendernative needs gcache_s */
#else
extern void nativeInit(void);
extern void nativeClear(void);
extern void nativeSetDark(int dark);
#endif
#include "error.h"
#include "basetypes.h"
#include "format.h"
#include "get.h"
#include "hint.h"
#include "hrender.h"

FILE* hlog;
jmp_buf error_exit;

/* Error Handling */
int herror(const char *title, const char *message)
{ fprintf(stderr,"ERROR %s: %s\n",title,message);
  return 0;
}

int hmessage(char *title, char *format, ...)
{ va_list vargs;
  va_start(vargs,format);
  vfprintf(stderr, format, vargs);
  return 0;
}

void error_callback(int error, const char* description)
{ herror("OpenGL",description);
  longjmp(error_exit,1);
}

GLFWwindow* window;
int px_h=1024, px_v=768; // size in pixel
int page_h, page_v; // size in scaled points
double x_dpi, y_dpi;
#define SCALE_MIN 0.2
#define SCALE_NORMAL 1.0
#define SCALE_MAX 5.0
double scale=SCALE_NORMAL;
uint64_t pos; /* position of current page */

void framebuffer_size_callback(GLFWwindow* window, int w, int h)
{ px_h=w; px_v=h;
  glViewport(0, 0, w, h);
  //LOG("w=%d, h=%d\n",w,h);
  hint_resize(px_h,px_v,scale*x_dpi,scale*y_dpi);
  hint_page();
}

void getDPI(void)
{ int widthMM, heightMM;
  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  glfwGetMonitorPhysicalSize(monitor, &widthMM, &heightMM);
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);
  x_dpi = mode->width / (widthMM / 25.4);
  y_dpi = mode->height / (heightMM / 25.4);
  //LOG("xdpi=%f, ydpi=%f\n",x_dpi,y_dpi);
}

void hint_unmap(void)
{ hget_unmap();
}

bool hint_map(void)
{ return hget_map();
}

int open_file(int home)
{ hint_clear_fonts(true);
  if (!hint_begin()) return 0;
  if (home)
    hint_page_home();
  else
    hint_page_top(0);
    //  strncpy(title_name,hin_name,MAX_PATH);
    //	SetNavigationTree();
   //SetWindowText(hMainWnd,title_name);
return 1;
} 

static void reload_file(void)
{ double fpos=hint_get_fpos();
  hint_begin();
  pos=hint_set_fpos(fpos);
  //LOG("reloading...\n");
}

static int new_file_time(void)
{ struct stat st;
  if (hin_name!=NULL &&
      stat(hin_name,&st)==0 &&
      st.st_size>0)
  {
    //LOG("file %s %lu\n",hin_name,st.st_mtime);
    if (st.st_mtime>hin_time)
    return 1;
  else
    return 0;
  }
  return 0;
}

int dark = 0, loading=0, autoreload=0, home=0;

int usage(void)
{    return herror("Usage:", "hintview [options] file\n"
		  "Call 'hintview --help' for details.\n");
}

int help(void)
{ fprintf(stdout,"%s",
   "Usage: hintview [options] file\n"
   "Options:\n"
   "  -a         Start in autoreload mode.\n"	  
   "  -h         Start with the documents home page.\n"	  
   "  -n         Start in night mode.\n"	  
#if 0
   "  -o         Show the documents outline.\n"
#endif
   "  -z         Start with zoom level 100%.\n"	  
   "  --help     Display this message.\n"	  
   "  --version  Display the version.\n"	  
   "\n"
   "See the hintview man page for more details.\n"
  );
  return 0;
}

int command_line(int argc, char *argv[])
{ size_t sl;
  int i;
  for (i=1; argv[i]!=NULL && argv[i][0]=='-'; i++)
    { switch (argv[i][1])
      { case '-':
	    if (strcmp(argv[i]+2,"help")==0) return help();
	    else if (strcmp(argv[i]+2,"version")==0)
	      { fprintf(stdout,"hintview version %d.%d.%d\n", VERSION, REVISION, PATCHLEVEL);
	        return 0;
	      }
	    else
	      return usage();
	case 'a': autoreload=1; break;
        case 'n': dark=1; break;
        case 'o': break;
        case 'z': scale=SCALE_NORMAL; break;
        case 'h': home=1; break;  
        default: return usage();
      }
    }
  if (argv[i]==NULL)
    return usage();
  if (hin_name!=NULL) free(hin_name);
  hin_name=malloc(strlen(argv[i])+1);
  if (hin_name==NULL)
    return herror("Out of memory for file name", argv[i]);
  strcpy(hin_name,argv[i]);
  sl=strlen(hin_name);
  if (sl>4 && strncmp(hin_name+sl-4,".hnt",4)!=0)
    return herror("Unknown File Type,I dont know how to open this file", argv[1]);
return 1;	
}

GLFWcursor *drag_cursor, *hand_cursor, *arrow_cursor;
int mouse_down=0, drag=0, on_link=0;

void clear_cursor(void)
{ if (on_link||drag) 
  { glfwSetCursor(window, arrow_cursor); on_link=drag=0;}
}

/* combining the key with the shift, control, alt or super flag */
#define KEY_MOD(K,M) (((K)<<4)|((M)&0xF))
#define KEY(K) KEY_MOD(K,0)
#define CTRL(K) KEY_MOD(K,GLFW_MOD_CONTROL)

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{ if (action!=GLFW_PRESS) return;
  switch (KEY_MOD(key,mods)) {
  case CTRL(GLFW_KEY_Q):
    glfwSetWindowShouldClose(window,1);
    break;
  case CTRL(GLFW_KEY_N):
    dark=!dark;
    nativeSetDark(dark);
    break;
  case KEY(GLFW_KEY_HOME):
  case CTRL(GLFW_KEY_H):
    hint_page_home();
    clear_cursor();
    break;
  case CTRL(GLFW_KEY_Z): 
  case CTRL(GLFW_KEY_Y): /* US keyboard */
    scale=SCALE_NORMAL;
    hint_resize(px_h,px_v,scale*x_dpi,scale*y_dpi);
    hint_page();
    hint_clear_fonts(true);
    clear_cursor();
    break;
  case KEY(GLFW_KEY_PAGE_DOWN):
    pos=hint_page_next();
    hint_page();
    clear_cursor();
    break;
  case KEY(GLFW_KEY_PAGE_UP):
    pos=hint_page_prev();
    hint_page();
    clear_cursor();
    break;
  case CTRL(GLFW_KEY_A): /* auto reload */
    autoreload=!autoreload;
    if (!autoreload)
      break; /* else fall through to reload */
  case CTRL(GLFW_KEY_R): /* reload */
    if (!loading)
    { loading=1;
      HINT_TRY reload_file();
      loading=0;
    }
    clear_cursor();
    break;
    break;
  case CTRL(GLFW_KEY_F): /* find */
    break;
  case CTRL(GLFW_KEY_O): /* outlines */
    break;
  default:
    // LOG("key %d, scan %d, action %d, mosd %d\n",key, scancode, action, mods);
    break;
  }
}

/* convert pixel to scaled points (including scale) */

#define PX2SP(X,DPI) floor((X)*ONE*72.27/((DPI)*scale))

void mouse_click(void)
{ double x,y;
  int link;
  glfwGetCursorPos(window, &x, &y);
  //LOG("xy= %fx%f\n", x,y);
  HINT_TRY {
    link=hint_find_link(PX2SP(x,x_dpi),PX2SP(y,y_dpi),2*ONE);
    if (link>=0)
    { hint_link_page(link);
      clear_cursor();
    }
  }
}

double x_start, y_start, xy_start, scale_start, time_start;
#define DELTA_T 0.4
#define DELTA_XY 16.0

static void cursor_position_callback(GLFWwindow* window, double x, double y)
{ //LOG("xy= %fx%f\n", x-x_start,y-y_start);
  double f;
  if (!mouse_down)
  { int link;
    link=hint_find_link(PX2SP(x,x_dpi),PX2SP(y,y_dpi),2*ONE);
    if (link>=0 && !on_link)
    { glfwSetCursor(window, hand_cursor); on_link=1; }
    else if (link<0 && on_link)
    { glfwSetCursor(window, arrow_cursor); on_link=0; }
    return;
  }
  if (!drag) /* we dont know whether this is a click or a drag */  
  { double d = (x-x_start)*(x-x_start)+(y-y_start)*(y-y_start);
    if (d<= DELTA_XY)
    { double t=glfwGetTime();
      if (t<=time_start+DELTA_T)
	return;
    }
    /* start dragging */
    drag=1;
    glfwSetCursor(window, drag_cursor);
    xy_start=x_start*x_start+y_start*y_start;
  }
    /* dragging */
  f=(x_start*x+y_start*y)/xy_start; /* projection of (x,y) on (x_start,y_start) */
  scale=scale_start*f;
  if (scale < SCALE_MIN) scale=SCALE_MIN;
  if (scale > SCALE_MAX) scale=SCALE_MAX;
  hint_resize(px_h,px_v,scale*x_dpi,scale*y_dpi);
  hint_page();
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{ if (button == GLFW_MOUSE_BUTTON_LEFT)
  { if (action == GLFW_PRESS)
    { glfwGetCursorPos(window, &x_start, &y_start);
      time_start=glfwGetTime();
      scale_start=scale;
      drag=0;
      mouse_down =1;
    }
    else if (action == GLFW_RELEASE)
    { mouse_down=0;
      if (drag) /* end dragging */
      {	hint_clear_fonts(true);
        clear_cursor();
      }
      else
        mouse_click();
    }
  }      
}

void cursor_enter_callback(GLFWwindow* window, int entered)
{ if (entered && autoreload && new_file_time())
    reload_file();
  //LOG("entered=%d auto=%d\n",entered, autoreload);
}

int create_window(void)
{ if( !glfwInit() )
  {
    herror("GLFW", "Failed to initialize GLFW\n" );
    getchar();
    return 0;
  }
  glfwSetErrorCallback(error_callback);
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  /* Open a window and create its OpenGL context */
  window = glfwCreateWindow( px_h, px_v, "HintView", NULL, NULL);
  if( window == NULL ){
    herror("GLFW","Failed to open GLFW window.\n"
	   "If you have an Intel GPU, they are not 3.3 compatible.\n"
	   "Try the 2.1 version of the tutorials.\n" );
    glfwTerminate();
    return 0;
  }
  glfwMakeContextCurrent(window);
      
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  
  drag_cursor = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
  hand_cursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
  arrow_cursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);

  getDPI();
 
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetKeyCallback(window, key_callback);
  glfwSetCursorEnterCallback(window,cursor_enter_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetCursorPosCallback(window, cursor_position_callback);

  glfwSetCursorPos(window, px_h/2, px_v/2);
 
  nativeInit();
  return 1;
}

int main(int argc, char *argv[])
{ hlog=stderr;
  if (setjmp(error_exit)!=0) return 1;
  if (!command_line(argc,argv))
    return 1;
   if (!create_window())
    return 1;
  nativeSetDark(dark);
  hint_resize(px_h,px_v,scale*x_dpi,scale*y_dpi);
  if (!open_file(home))
    return 1;
  hint_page_home();
  if (setjmp(error_exit)==0)
    do
    { hint_render();
      glfwSwapBuffers(window);
      glfwWaitEvents();
    } // Check if the ESC key was pressed or the window was closed
    while(glfwWindowShouldClose(window) == 0 );
  nativeClear();
  glfwDestroyWindow(window); // part of glfwTerminate
  glfwTerminate();
  return 0;
}

