#define EXTERN extern

#if defined(JIT)
#include "mfluajitd.h"
#else
#include "mfluad.h"
#endif

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <kpathsea/kpathsea.h>
#if defined(JIT)
#include <luajit.h>
#endif

/* See  shell_cmd_is_allowed below */
int shellenabledp = 1;
int restrictedshell = 0;

/**************************************************************/
/*                                                            */
/* private functions                                          */
/*                                                            */
/**************************************************************/
static lua_State *Luas[1];

/* static  */
/* void stackdump_g(lua_State* L) */
/* { */
/*     int i; */
/*     int top = lua_gettop(L); */
 
/*     printf("total in stack %d\n",top); */
 
/*     for (i = 1; i <= top; i++) */
/*     {  /\* repeat for each level *\/ */
/*         int t = lua_type(L, i); */
/*         printf("[%d][%d] ",i,i-top-1); */
/*         switch (t) { */
/*             case LUA_TSTRING:  /\* strings *\/ */
/*                 printf("string: '%s'\n", lua_tostring(L, i)); */
/*                 break; */
/*             case LUA_TBOOLEAN:  /\* booleans *\/ */
/*                 printf("boolean %s\n",lua_toboolean(L, i) ? "true" : "false"); */
/*                 break; */
/*             case LUA_TNUMBER:  /\* numbers *\/ */
/*                 printf("number: %g\n", lua_tonumber(L, i)); */
/*                 break; */
/*             default:  /\* other values *\/ */
/*                 printf("%s\n", lua_typename(L, t)); */
/*                 break; */
/*         } */
/*         printf("  ");  /\* put a separator *\/ */
/*     } */
/*     printf("\n");  /\* end the listing *\/ */
/* } */


static
void priv_lua_reporterrors(lua_State *L, int status)
{
  if ( status!=0 ) {
    fprintf(stderr,"\n! %s\n",lua_tostring(L, -1));
    lua_pop(L, 1); /* remove error message */
  }
}


/* static */
/* void priv_lua_writemessage(lua_State *L, char *startmsg, char *bodymsg, char *endmsg, int status) */
/* { */
/*   (void) L; */
/*   if ( status!=0 ) { */
/*     fprintf(stdout,"%s%s%s\n",startmsg,bodymsg,endmsg); */
/*   } */
/* } */


#define link_field(p) mem[p].hhfield.rh /* {the |link| field of a memory word} */
static int priv_mfweb_link(lua_State *L)
{
  halfword p,q;
  p = (halfword) lua_tonumber(L,1);
  q = link_field(p);
  lua_pushnumber(L,q);
  return 1;
}

#define info_field(p) mem[p].hhfield.lhfield /* {the |info| field of a memory word} */
static int priv_mfweb_info(lua_State *L)
{
  halfword p,q;
  p = (halfword) lua_tonumber(L,1);
  q = info_field(p);
  lua_pushnumber(L,q);
  return 1;
}


#define x_coord(p) mem[p+1].cint /* {the |x| coordinate of this knot} */
static int priv_mfweb_x_coord(lua_State *L)
{
  halfword p;
  int q;
  p = (halfword) lua_tonumber(L,1);
  q = x_coord(p);
  lua_pushnumber(L,q);
  return 1;
}

#define y_coord(p) mem[p+2].cint /* {the |y| coordinate of this knot} */
static int priv_mfweb_y_coord(lua_State *L)
{
  halfword p,q;
  p = (halfword) lua_tonumber(L,1);
  q = y_coord(p);
  lua_pushnumber(L,q);
  return 1;
}


#define left_type(p) mem[p].hhfield.b0 /* {characterizes the path entering this knot} */
static int priv_mfweb_left_type(lua_State *L)
{
  halfword p,q;
  p = (halfword) lua_tonumber(L,1);
  q = left_type(p);
  lua_pushnumber(L,q);
  return 1;
}

#define right_type(p) mem[p].hhfield.b1 /* {characterizes the path leaving this knot} */
static int priv_mfweb_right_type(lua_State *L)
{
  halfword p,q;
  p = (halfword) lua_tonumber(L,1);
  q = right_type(p);
  lua_pushnumber(L,q);
  return 1;
}


#define left_x(p) mem[p+3].cint /* {the |x| coordinate of previous control point} */
static int priv_mfweb_left_x(lua_State *L)
{
  halfword p;
  integer q ;
  p = (halfword) lua_tonumber(L,1);
  q = left_x(p);
  lua_pushnumber(L,q);
  return 1;
}


#define left_y(p) mem[p+4].cint /* {the |y| coordinate of previous control point} */
static int priv_mfweb_left_y(lua_State *L)
{
  halfword p;
  integer q ;
  p = (halfword) lua_tonumber(L,1);
  q = left_y(p);
  lua_pushnumber(L,q);
  return 1;
}


#define right_x(p) mem[p+5].cint /* {the |x| coordinate of next control point} */
static int priv_mfweb_right_x(lua_State *L)
{
  halfword p;
  integer q ;
  p = (halfword) lua_tonumber(L,1);
  q = right_x(p);
  lua_pushnumber(L,q);
  return 1;
}

#define right_y(p) mem[p+6].cint /* {the |y| coordinate of next control point} */
static int priv_mfweb_right_y(lua_State *L)
{
  halfword p;
  integer q;
  p = (halfword) lua_tonumber(L,1);
  q = right_y(p);
  lua_pushnumber(L,q);
  return 1;
}

/* @ Conversely, the |n_sin_cos| routine takes an |angle| and produces the sine */
/* and cosine of that angle. The results of this routine are */
/* stored in global integer variables |n_sin| and |n_cos|. */

/* @<Glob...@>= */
/* @!n_sin,@!n_cos:fraction; {results computed by |n_sin_cos|} */

/* @ Given an integer |z| that is $2^{20}$ times an angle $\theta$ in degrees, */
/* the purpose of |n_sin_cos(z)| is to set */
/* |x=@t$r\cos\theta$@>| and |y=@t$r\sin\theta$@>| (approximately), */
/* for some rather large number~|r|. The maximum of |x| and |y| */
/* will be between $2^{28}$ and $2^{30}$, so that there will be hardly */
/* any loss of accuracy. Then |x| and~|y| are divided by~|r|. */
static int priv_mfweb_n_sin_cos(lua_State *L)
{
  nsincos(lua_tonumber(L,1));
  return 0;
}

	   
/* @ When a lot of work is being done on a particular edge structure, we plant */
/* a pointer to its main header in the global variable |cur_edges|. */
/* This saves us from having to pass this pointer as a parameter over and */
/* over again between subroutines. */

/* Similarly, |cur_wt| is a global weight that is being used by several */
/* procedures at once. */

/* @<Glob...@>= */
/* @!cur_edges:pointer; {the edge structure of current interest} */
/* @!cur_wt:integer; {the edge weight of current interest} */
static int priv_mfweb_LUAGLOBALGET_cur_edges(lua_State *L)
{
  halfword p = curedges;
  lua_pushnumber(L,p);
  return 1;
}


/* @<Glob...@>= */
/* @!cur_type:small_number; {the type of the expression just found} */
/* @!cur_exp:integer; {the value of the expression just found} */
static int priv_mfweb_LUAGLOBALGET_cur_exp(lua_State *L)
{
  halfword p = curexp;
  lua_pushnumber(L,p);
  return 1;
}






/* @d mem_top==30000 {largest index in the |mem| array dumped by \.{INIMF}; */
/*   must be substantially larger than |mem_min| */
/*   and not greater than |mem_max|} */
static int priv_mfweb_LUAGLOBALGET_mem_top(lua_State *L)
{
  integer p = memtop;
  lua_pushnumber(L,p);
  return 1;
}



/* And there are two global variables that affect the rounding */
/* decisions, as we'll see later; they are called |cur_pen| and |cur_path_type|. */
/* The latter will be |double_path_code| if |make_spec| is being */
/* applied to a double path.*/
/* @!cur_pen:pointer; {an implicit input of |make_spec|, used in autorounding} */
static int priv_mfweb_LUAGLOBALGET_cur_pen(lua_State *L)
{
  halfword p = curpen;
  lua_pushnumber(L,p);
  return 1;
}


/* @ The current octant code appears in a global variable. If, for example, */  
/* we have |octant=third_octant|, it means that a curve traveling in a north to */  
/* north-westerly direction has been rotated for the purposes of internal */  
/* calculations so that the |move| data travels in an east to north-easterly */  
/* direction. We want to unrotate as we update the edge structure.       */  
/*                                                                       */
/* @<Glob...@>=                                                          */ 
/* @!octant:first_octant..sixth_octant; {the current octant of interest} */
static int priv_mfweb_LUAGLOBALGET_octant(lua_State *L)
{
  char p = octant;
  lua_pushnumber(L,p);
  return 1;
}


/* @ The |make_spec| routine has an interesting side effect, namely to set */
/* the global variable |turning_number| to the number of times the tangent */
/* vector of the given cyclic path winds around the origin.                */
/*                                                                         */
/* @<Glob...@>=                                                            */
/* @!turning_number:integer; {another output of |make_spec|}               */
static int priv_mfweb_LUAGLOBALGET_turning_number(lua_State *L)
{
  integer p = turningnumber;
  lua_pushnumber(L,p);
  return 1;
}



/* @ \MF\ also has a bunch of internal parameters that a user might want to      */
/* fuss with. Every such parameter has an identifying code number, defined here. */
/* Warning: these parameters must be the same of the original MF !!              */
#define tracing_titles 1 /*show titles online when they appear} */
#define tracing_equations 2 /*show each variable when it becomes known} */
#define tracing_capsules 3 /*show capsules too} */
#define tracing_choices 4 /*show the control points chosen for paths} */
#define tracing_specs 5 /*show subdivision of paths into octants before digitizing} */
#define tracing_pens 6 /*show details of pens that are made} */
#define tracing_commands 7 /*show commands and operations before they are performed} */
#define tracing_restores 8 /*show when a variable or internal is restored} */
#define tracing_macros 9 /*show macros before they are expanded} */
#define tracing_edges 10 /*show digitized edges as they are computed} */
#define tracing_output 11 /*show digitized edges as they are output} */
#define tracing_stats 12 /*show memory usage at end of job} */
#define tracing_online 13 /*show long diagnostics on terminal and in the log file} */
#define year 14 /*the current year (e.g., 1984)} */
#define month 15 /*the current month (e.g., 3 $\equiv$ March)} */
#define day 16 /*the current day of the month} */
#define time 17 /*the number of minutes past midnight when this job started} */
#define char_code 18 /*the number of the next character to be output} */
#define char_ext 19 /*the extension code of the next character to be output} */
#define char_wd 20 /*the width of the next character to be output} */
#define char_ht 21 /*the height of the next character to be output} */
#define char_dp 22 /*the depth of the next character to be output} */
#define char_ic 23 /*the italic correction of the next character to be output} */
#define char_dx 24 /*the device's $x$ movement for the next character, in pixels} */
#define char_dy 25 /*the device's $y$ movement for the next character, in pixels} */
#define design_size 26 /*the unit of measure used for |char_wd..char_ic|, in points} */
#define hppp 27 /*the number of horizontal pixels per point} */
#define vppp 28 /*the number of vertical pixels per point} */
#define x_offset 29 /*horizontal displacement of shipped-out characters} */
#define y_offset 30 /*vertical displacement of shipped-out characters} */
#define pausing 31 /*positive to display lines on the terminal before they are read} */
#define showstopping 32 /*positive to stop after each \&{show} command} */
#define fontmaking 33 /*positive if font metric output is to be produced} */
#define proofing 34 /*positive for proof mode, negative to suppress output} */
#define smoothing 35 /*positive if moves are to be ``smoothed''} */
#define autorounding 36 /*controls path modification to ``good'' points} */
#define granularity 37 /*autorounding uses this pixel size} */
#define fillin 38 /*extra darkness of diagonal lines} */
#define turning_check 39 /*controls reorientation of clockwise paths} */
#define warning_check 40 /*controls error message when variable value is large} */
#define boundary_char 41 /*the right boundary character for ligatures} */
/* @d max_given_internal=41 */

static int priv_mfweb_LUAGLOBALGET_char_code(lua_State *L)
{
  integer p =  roundunscaled ( internal [char_code]) % 256 ;
  lua_pushnumber(L,p);
  return 1;
}
static int priv_mfweb_LUAGLOBALGET_char_ext(lua_State *L)
{
  integer p =  roundunscaled ( internal [char_ext]) ;
  lua_pushnumber(L,p);
  return 1;
}


static int priv_mfweb_LUAGLOBALGET_char_wd(lua_State *L)
{
  integer p =   internal [char_wd]  ;
  lua_pushnumber(L,p);
  return 1;
}
static int priv_mfweb_LUAGLOBALGET_char_ht(lua_State *L)
{
  integer p =   internal [char_ht]  ;
  lua_pushnumber(L,p);
  return 1;
}
static int priv_mfweb_LUAGLOBALGET_char_dp(lua_State *L)
{
  integer p =   internal [char_dp]  ;
  lua_pushnumber(L,p);
  return 1;
}
static int priv_mfweb_LUAGLOBALGET_char_ic(lua_State *L)
{
  integer p =   internal [char_ic]  ;
  lua_pushnumber(L,p);
  return 1;
}

static int priv_mfweb_LUAGLOBALGET_char_dx(lua_State *L)
{
  integer p =   internal [char_dx]  ;
  lua_pushnumber(L,p);
  return 1;
}

static int priv_mfweb_LUAGLOBALGET_char_dy(lua_State *L)
{
  integer p =   internal [char_dy]  ;
  lua_pushnumber(L,p);
  return 1;
}

static int priv_mfweb_LUAGLOBALGET_designsize(lua_State *L)
{
  integer p =   internal [design_size] ;
  lua_pushnumber(L,p);
  return 1;
}

static int priv_mfweb_LUAGLOBALGET_hppp(lua_State *L)
{
  integer p =   internal [hppp]  ;
  lua_pushnumber(L,p);
  return 1;
}

static int priv_mfweb_LUAGLOBALGET_vppp(lua_State *L)
{
  integer p =   internal [vppp]  ;
  lua_pushnumber(L,p);
  return 1;
}

static int priv_mfweb_LUAGLOBALGET_x_offset(lua_State *L)
{
  integer p =   internal [x_offset]  ;
  lua_pushnumber(L,p);
  return 1;
}

static int priv_mfweb_LUAGLOBALGET_y_offset(lua_State *L)
{
  integer p =   internal [y_offset]  ;
  lua_pushnumber(L,p);
  return 1;
}

static int priv_mfweb_LUAGLOBALGET_granularity(lua_State *L)
{
  integer p =   internal [granularity]  ;
  lua_pushnumber(L,p);
  return 1;
}

static int priv_mfweb_LUAGLOBALGET_fillin(lua_State *L)
{
  integer p =   internal [fillin]  ;
  lua_pushnumber(L,p);
  return 1;
}

static int priv_mfweb_LUAGLOBALGET_turning_check(lua_State *L)
{
  integer p =   internal [turning_check]  ;
  lua_pushnumber(L,p);
  return 1;
}

static int priv_mfweb_LUAGLOBALGET_boundary_char(lua_State *L)
{
  integer p =   internal [boundary_char]  ;
  lua_pushnumber(L,p);
  return 1;
}

static int priv_mflua_version(lua_State *L)
{
  lua_pushstring(L,MFLUA_VERSION);
  return 1;
}

static int priv_mflua_banner(lua_State *L)
{
  lua_pushstring(L,BANNER);
  return 1;
}




/**************************************************************/
/*                                                            */
/* mflua layer                                                */
/*                                                            */
/**************************************************************/
static const struct luaL_Reg MFbuiltin_l[] = {
  {"link", priv_mfweb_link},
  {"info", priv_mfweb_info},
  {"x_coord", priv_mfweb_x_coord},
  {"y_coord", priv_mfweb_y_coord},
  {"left_type", priv_mfweb_left_type},
  {"right_type", priv_mfweb_right_type},
  {"left_x", priv_mfweb_left_x},
  {"left_y", priv_mfweb_left_y},
  {"right_x", priv_mfweb_right_x},
  {"right_y", priv_mfweb_right_y},
  {"n_sin_cos", priv_mfweb_n_sin_cos},
  {"cur_edges", priv_mfweb_LUAGLOBALGET_cur_edges},
  {"cur_exp", priv_mfweb_LUAGLOBALGET_cur_exp},
  {"mem_top", priv_mfweb_LUAGLOBALGET_mem_top},
  {"cur_pen", priv_mfweb_LUAGLOBALGET_cur_pen},
  {"octant", priv_mfweb_LUAGLOBALGET_octant},
  {"char_code", priv_mfweb_LUAGLOBALGET_char_code},
  {"char_ext", priv_mfweb_LUAGLOBALGET_char_ext},
  {"char_wd", priv_mfweb_LUAGLOBALGET_char_wd},
  {"char_ht", priv_mfweb_LUAGLOBALGET_char_ht},
  {"char_dp", priv_mfweb_LUAGLOBALGET_char_dp},
  {"char_ic", priv_mfweb_LUAGLOBALGET_char_ic},
  {"char_dx", priv_mfweb_LUAGLOBALGET_char_dx},
  {"char_dy", priv_mfweb_LUAGLOBALGET_char_dy},
  {"designsize", priv_mfweb_LUAGLOBALGET_designsize},
  {"hppp", priv_mfweb_LUAGLOBALGET_hppp},
  {"vppp", priv_mfweb_LUAGLOBALGET_vppp},
  {"x_offset", priv_mfweb_LUAGLOBALGET_x_offset},
  {"y_offset", priv_mfweb_LUAGLOBALGET_y_offset},
  {"granularity", priv_mfweb_LUAGLOBALGET_granularity},
  {"fillin", priv_mfweb_LUAGLOBALGET_fillin},
  {"turning_check", priv_mfweb_LUAGLOBALGET_turning_check},
  {"boundary_char", priv_mfweb_LUAGLOBALGET_boundary_char},
  {"turning_number", priv_mfweb_LUAGLOBALGET_turning_number},
  {"mflua_version",priv_mflua_version},
  {"mflua_banner",priv_mflua_banner},
  {NULL, NULL}                /* sentinel */
};


#define lua_swap(L) lua_insert(L, -2) 
  
#define GETGLOBALTABLEMFLUA(a)  lua_getglobal(L, "mflua");\
  if (!lua_istable(L, -1)) {                               \
    lua_pushstring(L,#a);	                           \
    lua_pushstring(L,":global table mflua not found");	   \
    lua_concat (L, 2);                                     \
    priv_lua_reporterrors(L, 1);			   \
  }                                                        \



int mfluabeginprogram(void)
{
  lua_State *L ;
  char* luafile ;
  int res ;

  L = luaL_newstate();
  luaL_openlibs(L);
  Luas[0] = L;
  /* register lua functions */
  luaopen_kpse(L);
  /* to be sure of having a clear stack */
  lua_settop(L,0);
  

  lua_getglobal(L, "mflua");
  if (!lua_istable(L, -1)) {
    lua_pop(L,1);
    lua_newtable(L);
    lua_setglobal(L,"mflua");
    /* check it */
    lua_getglobal(L,"mflua");
    if (!lua_istable(L, -1)) {
      printf("mflua table NOT registered!\n");
    } else {
      lua_pushstring(L,"MFbuiltin");
#ifdef MFLuaJIT
      /* 5.1 */ 
      luaJIT_setmode(L, 0, LUAJIT_MODE_ENGINE|LUAJIT_MODE_OFF);
      lua_newtable(L);
      luaL_register (L,NULL,MFbuiltin_l);
#else
      luaL_newlib(L,MFbuiltin_l);
#endif
      lua_settable(L, -3);
    }
    lua_pop(L,1);
  }
  
  
  luafile = kpse_find_file("mflua.lua", kpse_lua_format, 0);
  if (luafile==NULL) {
    res = 1;
    lua_pushstring(L,"mflua.lua not found.");
    priv_lua_reporterrors(L, res);
    goto EXIT;
  }
  res = luaL_loadfile(L, luafile);
  free(luafile);
  if ( res==LUA_OK ) {
    if((res=lua_pcall(L,0,0,0))){
      priv_lua_reporterrors(L, res);
      goto EXIT;
    }
  } else {
    priv_lua_reporterrors(L, res);
    goto EXIT;
  }
  /* Still a chance that mflua is around */
  GETGLOBALTABLEMFLUA(mfluabeginprogram);
  if (lua_istable(L, -1)) {
    lua_getfield(L,-1,"begin_program");
    if((res=lua_pcall(L,0,0,0)))
      priv_lua_reporterrors(L, res);
  }
 EXIT:
  lua_settop(L,0);
  return 0;
}

/* /\* TODO*\/ */
/* int mfluaendprogram(void) */
/* { */
/*   lua_State *L; */
/*   char* luafile; */
/*   int res; */

/*   L = Luas[0]; */
/*   GETGLOBALTABLEMFLUA(mfluaendprogram); */
/*   luafile = kpse_find_file("end_program.lua", kpse_lua_format, 0); */
/*   if (luafile==NULL) { */
/*     res = 1; */
/*     lua_pushstring(L,"end_program.lua not found"); */
/*     priv_lua_reporterrors(L, res); */
/*     lua_settop(L,0); */
/*     return 0; */
/*   } */
/*   priv_lua_writemessage(L,"(",luafile,")",1); */
/*   res = luaL_loadfile(L, luafile); */
/*   free(luafile); */
/*   if ( res==0 ) { */
/*       res = lua_pcall(L, 0, 0, 0); */
/*     } */
/*   /\* stackdump_g(L); *\/ */
/*   priv_lua_reporterrors(L, res); */
/*   lua_settop(L,0); */
/*   return 0; */
/* } */

int mfluaendprogram(void)
{
  lua_State *L;
  int res;

  L = Luas[0];
  GETGLOBALTABLEMFLUA(mfluaendprogram);
  if (lua_istable(L, -1)) {
    lua_getfield(L,-1,"end_program");
    /* do the call (0 arguments, 0 result) */
    if((res=lua_pcall(L, 0, 0, 0))){
      lua_pushstring(L,"error in end_program:");
      lua_swap(L);lua_concat (L, 2);
      priv_lua_reporterrors(L, res);
    }
  }
  lua_settop(L,0);
  return 0;
}



int mfluaPREstartofMF(void)
{
  lua_State *L;
  int res;

  L = Luas[0];
  GETGLOBALTABLEMFLUA(mfluaPREstartofMF);
  if (lua_istable(L, -1)) { 
    lua_getfield(L,-1,"PRE_start_of_MF");
    /* do the call (0 arguments, 0 result) */
    if((res=lua_pcall(L, 0, 0, 0))){
      lua_pushstring(L,"error in PRE_start_of_MF:");
      lua_swap(L);lua_concat (L, 2);
      priv_lua_reporterrors(L, res);
    }
  }
  lua_settop(L,0);
  return 0;
}


int mfluaPREmaincontrol(void)
{
  lua_State *L;
  int res;

  L = Luas[0];
  GETGLOBALTABLEMFLUA(mfluaPREmaincontrol);
  if (lua_istable(L, -1)) {
    lua_getfield(L,-1, "PRE_main_control");
    /* do the call (0 arguments, 0 result) */
    if((res=lua_pcall(L, 0, 0, 0))){
      lua_pushstring(L,"error in PRE_main_control:");
      lua_swap(L);lua_concat (L, 2);
      priv_lua_reporterrors(L, res);
    }
  }
  lua_settop(L,0);
  return 0;
}

int mfluaPOSTmaincontrol(void)
{
  lua_State *L;
  int res;

  L = Luas[0];
  GETGLOBALTABLEMFLUA(mfluaPOSTmaincontrol);
  if (lua_istable(L, -1)) {
    lua_getfield(L,-1, "POST_main_control");
    /* do the call (0 arguments, 0 result) */
    if((res=lua_pcall(L, 0, 0, 0))){
      lua_pushstring(L,"error in POST_main_control:");
      lua_swap(L);lua_concat (L, 2);
      priv_lua_reporterrors(L, res);
    }
  }
  lua_settop(L,0);
  return 0;

}


int mfluainitialize(void)
{
  lua_State *L;
  int res;

  L = Luas[0];
  GETGLOBALTABLEMFLUA(mfluainitialize);
  if (lua_istable(L, -1)) {
    lua_getfield(L,-1, "mflua_initialize");
    /* do the call (0 arguments, 0 result) */
    if((res=lua_pcall(L, 0, 0, 0))){
      lua_pushstring(L,"error in mflua_initialize:");
      lua_swap(L);lua_concat (L, 2);
      priv_lua_reporterrors(L, res);
    }
  }
  lua_settop(L,0);
  return 0;
}
  

int mfluaPOSTfinalcleanup(void)
{
  lua_State *L;
  int res;

  L = Luas[0];
  GETGLOBALTABLEMFLUA(mfluaPOSTfinalcleanup);
  if (lua_istable(L, -1)) {
    lua_getfield(L,-1, "POST_final_cleanup");
    /* do the call (0 arguments, 0 result) */
    if((res=lua_pcall(L, 0, 0, 0))){
      lua_pushstring(L,"error in POST_final_cleanup:");
      lua_swap(L);lua_concat (L, 2);
      priv_lua_reporterrors(L, res);
    }
  }
    lua_settop(L,0);
    return 0;
}


/* Not a good way: */
/* 1) these definitions are taken from mfcoerc.h which is generated at run-time */
/* 2) too much coupling with webc2c translation of mf.web*/
/*                               */
/* #define printdiagnostic(s, t, nuline) zprintdiagnostic((strnumber) (s), (strnumber) (t), (boolean) (nuline)) */
/* #define print(s) zprint((integer) (s)) */
/* #define printarg(q, n, b) zprintarg((halfword) (q), (integer) (n), (halfword) (b) */
/* int mfluaprintpath P3C(halfword, h , strnumber, s , boolean, nuline) */
/* { */
/*   fprintf(stderr,"\n! %d \n",s); */
/*   printdiagnostic ( 517 , s , nuline ) ; */
/*   println () ; */
/*   print ( 518 ) ; */
/*   println () ; */
/*   fprintf(stderr,"\n! %s\n","*end***************"); */

/*   lua_State *L = Luas[0]; */
/*   char* file = kpse_find_file("print_path.lua", kpse_lua_format, 0); */
/*   int res = luaL_loadfile(L, file); */
/*   if (file) free (file); */
/*   if ( res==0 ) { */
/*       res = lua_pcall(L, 0, 0, 0); */
/*     } */
/*   // */
/*   //stackdump_g(L); */

/*   // */
/*   priv_lua_reporterrors(L, res); */
/*   return 0; */

/* } */

int mfluaprintpath(halfword h, strnumber s, boolean nuline)
{
  lua_State *L;
  int res;

  L = Luas[0];
  GETGLOBALTABLEMFLUA(mfluaprintpath);
  if (lua_istable(L, -1)) {
    lua_getfield(L,-1, "printpath");
    lua_pushnumber(L, h);   /* push 1st argument */
    lua_pushnumber(L, s);   /* push 2nd argument */
    lua_pushnumber(L, nuline);   /* push 3nd argument */
    /* do the call (3 arguments, 0 result) */
    if((res=lua_pcall(L, 3, 0, 0))){
      lua_pushstring(L,"error in printpath:");
      lua_swap(L);lua_concat (L, 2);
      priv_lua_reporterrors(L, res);
    }
  }
  lua_settop(L,0);
  return 0;
}

 
int mfluaprintedges(strnumber s, boolean nuline, integer xoff, integer yoff)
{
  lua_State *L;
  int res;

  L = Luas[0];
  GETGLOBALTABLEMFLUA(mfluaprintedges);
  if (lua_istable(L, -1)) {
    lua_getfield(L,-1, "printedges");
    lua_pushnumber(L, s);   /* push 1st argument */
    lua_pushnumber(L, nuline);   /* push 2nd argument */
    lua_pushnumber(L, xoff);   /* push 3nd argument */
    lua_pushnumber(L, yoff);   /* push 4nd argument */
    /* do the call (4 arguments, 0 result) */
    if((res=lua_pcall(L, 4, 0, 0))){
      lua_pushstring(L,"error in printedges:");
      lua_swap(L);lua_concat (L, 2);
      priv_lua_reporterrors(L, res);
    }
  }
  lua_settop(L,0);
  return 0;
}


/*                                     */
/* Sensor before and after offset_prep */
/*                                     */
int mfluaPREoffsetprep(halfword c, halfword h)
{

  lua_State *L;
  int res;

  L = Luas[0];
  GETGLOBALTABLEMFLUA(mfluaPREoffsetprep);
  if (lua_istable(L, -1)) {
    lua_getfield(L,-1, "PRE_offset_prep");
    lua_pushnumber(L, c);   /* push 1st argument */
    lua_pushnumber(L, h);   /* push 2nd argument */
    /* do the call (2 arguments, 0 result) */
    if((res=lua_pcall(L, 2, 0, 0))){ 
      lua_pushstring(L,"error in PRE_offset_prep:");
      lua_swap(L);lua_concat (L, 2);
      priv_lua_reporterrors(L, res);
    }
  }
  lua_settop(L,0);
  return 0;
}


int mfluaPOSToffsetprep(halfword c, halfword h)
{
  lua_State *L;
  int res;

  L = Luas[0];
  GETGLOBALTABLEMFLUA(mfluaPOSToffsetprep);
  if (lua_istable(L, -1)) {
    lua_getfield(L,-1, "POST_offset_prep");
    lua_pushnumber(L, c);   /* push 1st argument */
    lua_pushnumber(L, h);   /* push 2nd argument */
    /* do the call (2 arguments, 0 result) */
    if((res=lua_pcall(L, 2, 0, 0))){ 
      lua_pushstring(L,"error in POST_offset_prep:");
      lua_swap(L);lua_concat (L, 2);
      priv_lua_reporterrors(L, res);
    }
  }
  lua_settop(L,0);
  return 0;
}


int mfluaPREmakespecrhs(halfword rhs)
{
  lua_State *L;
  int res;
  
  L = Luas[0];
  GETGLOBALTABLEMFLUA(mfluaPREmakespecrhs);
  if (lua_istable(L, -1)) {
    lua_getfield(L,-1, "PRE_make_spec_rhs");
    lua_pushnumber(L, rhs);   /* push 1st argument */
    /* do the call (1 arguments, 0result) */
    if((res=lua_pcall(L, 1, 0, 0))){
      lua_pushstring(L,"error in PRE_make_spec_rhs:");
      lua_swap(L);lua_concat (L, 2);
      priv_lua_reporterrors(L, res);
    }
  }
  lua_settop(L,0);
  return 0;
}


int mfluaPOSTmakespecrhs(halfword rhs)
{
  lua_State *L;
  int res;
  
  L = Luas[0];
  GETGLOBALTABLEMFLUA(mfluaPOSTmakespecrhs);
  if (lua_istable(L, -1)) {
    lua_getfield(L,-1, "POST_make_spec_rhs");
    lua_pushnumber(L, rhs);   /* push 1st argument */
    /* do the call (1 arguments, 0result) */
    if((res=lua_pcall(L, 1, 0, 0))){
      lua_pushstring(L,"error in POST_make_spec_rhs:");
      lua_swap(L);lua_concat (L, 2);
      priv_lua_reporterrors(L, res);
    }
  }
  lua_settop(L,0);
  return 0;
}


int mfluaPREmakespeclhs(halfword lhs)
{
  lua_State *L;
  int res;

  L = Luas[0];
  GETGLOBALTABLEMFLUA(mfluaPREmakespeclhs);
  if (lua_istable(L, -1)) {
    lua_getfield(L,-1, "PRE_make_spec_lhs");
    lua_pushnumber(L, lhs);   /* push 1st argument */
  /* do the call (1 arguments, 0 result) */
    if((res=lua_pcall(L, 1, 0, 0))){
      lua_pushstring(L,"error in PRE_make_spec_lhs:");
      lua_swap(L);lua_concat (L, 2);
      priv_lua_reporterrors(L, res);
    }
  }
  lua_settop(L,0);
  return 0;
}

int mfluaPOSTmakespeclhs(halfword lhs)
{
  lua_State *L;
  int res;

  L = Luas[0];
  GETGLOBALTABLEMFLUA(mfluaPOSTmakespeclhs);
  if (lua_istable(L, -1)) {
    lua_getfield(L,-1, "POST_make_spec_lhs");
    lua_pushnumber(L, lhs);   /* push 1st argument */
    /* do the call (1 arguments, 0 result) */
    if((res=lua_pcall(L, 1, 0, 0))){
      lua_pushstring(L,"error in POST_make_spec_lhs:");
      lua_swap(L);lua_concat (L, 2);
      priv_lua_reporterrors(L, res);
    }
  }
  lua_settop(L,0);
  return 0;
}



int mfluaPREfillenveloperhs(halfword rhs)
{
  lua_State *L;
  int res;

  L = Luas[0];
  GETGLOBALTABLEMFLUA(mfluaPREfillenveloperhs);
  if (lua_istable(L, -1)) {
    lua_getfield(L,-1, "PRE_fill_envelope_rhs");
    lua_pushnumber(L, rhs);   /* push 1st argument */
    /* do the call (1 arguments, 0 result) */
    if((res=lua_pcall(L, 1, 0, 0))){ 
      lua_pushstring(L,"error in PRE_fill_envelope_rhs:");
      lua_swap(L);lua_concat (L, 2);
      priv_lua_reporterrors(L, res);
    }
  }
  lua_settop(L,0);
  return 0;
}



int mfluaPOSTfillenveloperhs(halfword rhs)
{
  lua_State *L;
  int res;
  
  L = Luas[0];
  GETGLOBALTABLEMFLUA(mfluaPOSTfillenveloperhs);
  if (lua_istable(L, -1)) {
    lua_getfield(L,-1, "POST_fill_envelope_rhs");
    lua_pushnumber(L, rhs);   /* push 1st argument */
    /* do the call (1 arguments, 0 result) */
    if((res=lua_pcall(L, 1, 0, 0))){ 
      lua_pushstring(L,"error in POST_fill_envelope_rhs:");
      lua_swap(L);lua_concat (L, 2);
      priv_lua_reporterrors(L, res);
    }
  }
  lua_settop(L,0);
  return 0;
}



int mfluaPREfillenvelopelhs(halfword lhs)
{
  lua_State *L;
  int res;

  L = Luas[0];
  GETGLOBALTABLEMFLUA(mfluaPREfillenvelopelhs);
  if (lua_istable(L, -1)) {
    lua_getfield(L,-1, "PRE_fill_envelope_lhs");
    lua_pushnumber(L, lhs);   /* push 1st argument */
    /* do the call (1 arguments, 0 result) */
    if((res=lua_pcall(L, 1, 0, 0))){
      lua_pushstring(L,"error in PRE_fill_envelope_lhs:");
      lua_swap(L);lua_concat (L, 2);
      priv_lua_reporterrors(L, res);
    }
  }
  lua_settop(L,0);
  return 0;
}



int mfluaPOSTfillenvelopelhs(halfword lhs)
{
  lua_State *L;
  int res;

  L = Luas[0];
  GETGLOBALTABLEMFLUA(mfluaPOSTfillenvelopelhs);
  if (lua_istable(L, -1)) {
    lua_getfield(L,-1, "POST_fill_envelope_lhs");
    lua_pushnumber(L, lhs);   /* push 1st argument */
    /* do the call (1 arguments, 0 result) */
    if((res=lua_pcall(L, 1, 0, 0))){
      lua_pushstring(L,"error in POST_fill_envelope_lhs:");
      lua_swap(L);lua_concat (L, 2);
      priv_lua_reporterrors(L, res);
    }
  }
  lua_settop(L,0);
  return 0;
}



int mfluaPREfillspecrhs(halfword rhs)
{
  lua_State *L;
  int res;
  
  L = Luas[0];
  GETGLOBALTABLEMFLUA(mfluaPREfillspecrhs);
  if (lua_istable(L, -1)) {
    lua_getfield(L,-1, "PRE_fill_spec_rhs");
    lua_pushnumber(L, rhs);   /* push 1st argument */
    /* do the call (1 arguments, 0 result) */
    if((res=lua_pcall(L, 1, 0, 0))){ 
      lua_pushstring(L,"error in PRE_fill_spec_rhs:");
      lua_swap(L);lua_concat (L, 2);
      priv_lua_reporterrors(L, res);
    }
  }
  lua_settop(L,0);
  return 0;
}

int mfluaPOSTfillspecrhs(halfword rhs)
{
  lua_State *L;
  int res;
  
  L = Luas[0];
  GETGLOBALTABLEMFLUA(mfluaPOSTfillspecrhs);
  if (lua_istable(L, -1)) {
    lua_getfield(L,-1, "POST_fill_spec_rhs");
    lua_pushnumber(L, rhs);   /* push 1st argument */
    /* do the call (1 arguments, 0 result) */
    if((res=lua_pcall(L, 1, 0, 0))){ 
      lua_pushstring(L,"error in POST_fill_spec_rhs:");
      lua_swap(L);lua_concat (L, 2);
      priv_lua_reporterrors(L, res);
    }
  }
  lua_settop(L,0);
  return 0;
}

int mfluaPREfillspeclhs(halfword lhs)
{
  lua_State *L;
  int res;

  L = Luas[0];
  GETGLOBALTABLEMFLUA(mfluaPREfillspeclhs);
  if (lua_istable(L, -1)) {
    lua_getfield(L,-1, "PRE_fill_spec_lhs");
    L = Luas[0];
    lua_pushnumber(L, lhs);   /* push 1st argument */
    /* do the call (1 arguments, 0 result) */
    if((res=lua_pcall(L, 1, 0, 0))){
      lua_pushstring(L,"error in PRE_fill_spec_lhs:");
      lua_swap(L);lua_concat (L, 2);
      priv_lua_reporterrors(L, res);
    }
  }
  lua_settop(L,0);
  return 0;
}


int mfluaPOSTfillspeclhs(halfword lhs)
{
  lua_State *L;
  int res;

  L = Luas[0];
  GETGLOBALTABLEMFLUA(mfluaPOSTfillspeclhs);
  if (lua_istable(L, -1)) {
    lua_getfield(L,-1, "POST_fill_spec_lhs");
    L = Luas[0];
    lua_pushnumber(L, lhs);   /* push 1st argument */
    /* do the call (1 arguments, 0 result) */
    if((res=lua_pcall(L, 1, 0, 0))){
      lua_pushstring(L,"error in POST_fill_spec_lhs:");
      lua_swap(L);lua_concat (L, 2);
      priv_lua_reporterrors(L, res);
    }
  }
  lua_settop(L,0);
  return 0;
}



int mfluaPREmovetoedges(halfword lhs)
{
  lua_State *L;
  int res;

  L = Luas[0];
  GETGLOBALTABLEMFLUA(mfluaPREmovetoedges);
  if (lua_istable(L, -1)) {
    lua_getfield(L,-1, "PRE_move_to_edges");
    lua_pushnumber(L, lhs);   /* push 1st argument */
    /* do the call (1 arguments, 0 result) */
    if((res=lua_pcall(L, 1, 0, 0))){
      lua_pushstring(L,"error in PRE_move_to_edges:");
      lua_swap(L);lua_concat (L, 2);
      priv_lua_reporterrors(L, res);
    }
  }
  lua_settop(L,0);
  return 0;
}


int mfluaPOSTmovetoedges(halfword lhs)
{
  lua_State *L;
  int res;

  L = Luas[0];
  GETGLOBALTABLEMFLUA(mfluaPOSTmovetoedges);
  if (lua_istable(L, -1)) {
    lua_getfield(L,-1, "POST_move_to_edges");
    lua_pushnumber(L, lhs);   /* push 1st argument */
    /* do the call (1 arguments, 0 result) */
    if((res=lua_pcall(L, 1, 0, 0))){
      lua_pushstring(L,"error in POST_move_to_edges:");
      lua_swap(L);lua_concat (L, 2);
      priv_lua_reporterrors(L, res);
    }
  }
  lua_settop(L,0);
  return 0;
}


int mfluaPREmakechoices(halfword p)
{
  lua_State *L;
  int res;

  L = Luas[0];
  GETGLOBALTABLEMFLUA(mfluaPREmakechoices);
  if (lua_istable(L, -1)) {
    lua_getfield(L,-1,"PRE_make_choices");  /* function to be called */
    lua_pushnumber(L, p);   /* push 1st argument */
    /* do the call (1 arguments, 0 result) */
    if((res=lua_pcall(L, 1, 0, 0))){
      lua_pushstring(L,"error in PRE_make_choices:");
      lua_swap(L);lua_concat (L, 2);
      priv_lua_reporterrors(L, res);
    }
  }
  lua_settop(L,0);
  return 0;
}



int mfluaPOSTmakechoices(halfword p)
{
  lua_State *L;
  int res;

  L = Luas[0];
  GETGLOBALTABLEMFLUA(mfluaPOSTmakechoices);
  if (lua_istable(L, -1)) {
    lua_getfield(L,-1,"POST_make_choices");  /* function to be called */
    lua_pushnumber(L, p);   /* push 1st argument */
    /* do the call (1 arguments, 0 result) */
    if((res=lua_pcall(L, 1, 0, 0))){
      lua_pushstring(L,"error in POST_make_choices:");
      lua_swap(L);lua_concat (L, 2);
      priv_lua_reporterrors(L, res);
    }
  }
  lua_settop(L,0);
  return 0;
}



int mfluaprintretrogradeline(integer x0, integer y0, integer cur_x, integer cur_y)
{
  lua_State *L;
  int res;

  L = Luas[0];
  GETGLOBALTABLEMFLUA(mfluaprintretrogradeline);
  if (lua_istable(L, -1)) {
    lua_getfield(L,-1,"print_retrograde_line");
    lua_pushnumber(L, x0);   /* push 1st argument */
    lua_pushnumber(L, y0);   /* push 2nd argument */
    lua_pushnumber(L, cur_x);   /* push 3th argument */
    lua_pushnumber(L, cur_y);   /* push 4th argument */
    /* do the call (4 arguments, 0 result) */
    if((res=lua_pcall(L, 4, 0, 0))){
      lua_pushstring(L,"error in print_retrograde_line:");
      lua_swap(L);lua_concat (L, 2);
      priv_lua_reporterrors(L, res);
    }
  }
  lua_settop(L,0);
  return 0;
}


int mfluaPREmakeellipse(integer major_axis, integer minor_axis, integer theta_angle , integer tx_val, integer ty_val,integer q)
{
  lua_State *L;
  int res;

  L = Luas[0];
  GETGLOBALTABLEMFLUA(mfluaPREmakeellipse);
  if (lua_istable(L, -1)) {
    lua_getfield(L,-1,"PRE_make_ellipse");
    lua_pushnumber(L, major_axis);   /* push 1st argument */
    lua_pushnumber(L, minor_axis);   /* push 2nd argument */
    lua_pushnumber(L, theta_angle);   /* push 3th argument */
    lua_pushnumber(L, tx_val);   /* push 4th argument */
    lua_pushnumber(L, ty_val);   /* push 5th argument */
    lua_pushnumber(L, q);   /* push 6th argument */
    /* do the call (6 arguments, 0 result) */
    if((res=lua_pcall(L, 6, 1, 0))){
      lua_pushstring(L,"error in PRE_make_ellipse:");
      lua_swap(L);lua_concat (L, 2);
      priv_lua_reporterrors(L, res);
    }
  }
  lua_settop(L,0);
  return 0;
}
      


int mfluaPOSTmakeellipse(integer major_axis, integer minor_axis, integer mfl_theta , integer mfl_tx, integer mfl_ty,integer q)
{
  lua_State *L;
  int res;

  L = Luas[0];
  GETGLOBALTABLEMFLUA(mfluaPOSTmakeellipse);
  if (lua_istable(L, -1)) {
    lua_getfield(L,-1,"POST_make_ellipse");
    lua_pushnumber(L, major_axis);   /* push 1st argument */
    lua_pushnumber(L, minor_axis);   /* push 2nd argument */
    lua_pushnumber(L, mfl_theta);   /* push 3th argument */
    lua_pushnumber(L, mfl_tx);   /* push 4th argument */
    lua_pushnumber(L, mfl_ty);   /* push 5th argument */
    lua_pushnumber(L, q);   /* push 6th argument */
    /* do the call (6 arguments, 0 result) */
    if((res=lua_pcall(L, 6, 1, 0))){
      lua_pushstring(L,"error in POST_make_ellipse:");
      lua_swap(L);lua_concat (L, 2);
      priv_lua_reporterrors(L, res);
    }
  }
  lua_settop(L,0);
  return 0;
}
      

int mfluaprinttransitionlinefrom(integer x, integer y)
{
  lua_State *L;
  int res;

  L = Luas[0];
  GETGLOBALTABLEMFLUA(mfluaprinttransitionlinefrom);
  if (lua_istable(L, -1)) {
    lua_getfield(L,-1,"print_transition_line_from");
    lua_pushnumber(L, x);   /* push 1st argument */
    lua_pushnumber(L, y);   /* push 2nd argument */
    /* do the call (2 arguments, 0 result) */
    if((res=lua_pcall(L, 2, 0, 0))){
      lua_pushstring(L,"error in print_transition_line_from:");
      lua_swap(L);lua_concat (L, 2);
      priv_lua_reporterrors(L, res);
    }
  }
  lua_settop(L,0);
  return 0;
}


int mfluaprinttransitionlineto(integer x, integer y)
{
  lua_State *L;
  int res;

  L = Luas[0];
  GETGLOBALTABLEMFLUA(mfluaprinttransitionlineto);
  if (lua_istable(L, -1)) {
    lua_getfield(L,-1,"print_transition_line_to");
    lua_pushnumber(L, x);   /* push 1st argument */
    lua_pushnumber(L, y);   /* push 2nd argument */
    /* do the call (2 arguments, 0 result) */
    if((res=lua_pcall(L, 2, 0, 0))){
      lua_pushstring(L,"error in print_transition_line_to:");
      lua_swap(L);lua_concat (L, 2);
      priv_lua_reporterrors(L, res);
    }
  }
  lua_settop(L,0);
  return 0;
}



#define priv_append_char(c) do { \
  strpool[poolptr]=c;            \
  poolptr++;                     \
} while(0)  

#define  priv_str_room(l) do   {    \
  if ((poolptr + l) > maxpoolptr) { \
    if ((poolptr + l) > poolsize) { \
      fprintf(stderr,"\n! (Lua) MFLua capacity exceeded, sorry [pool size=%ld]\nIf you really absolutely need more capacity,\nyou can ask a wizard to enlarge me.\n",poolsize-initpoolptr); \
      exit(1);                      \
    }                               \
    maxpoolptr = poolptr + l;       \
  }                                 \
} while (0)

int mfluarunscript(halfword j, halfword first_val, halfword limit) 
{
  int i ;
  lua_State *L ;
  unsigned char last_char;
  int error;
  const char *str;
  size_t len;
  L = Luas[0];
  last_char = strpool[j+limit-first_val] ;
  /* end of a C string */ 
  strpool[j+limit-first_val] = '\0';
  str = (const char *)(strpool+j) ;
  /* do the call (0 arguments, 1 result) */
  error = (luaL_loadstring(L, str) || lua_pcall(L, 0, 1, 0)) ; 
  strpool[j+limit-first_val] = last_char;
  if(error) {
    priv_lua_reporterrors(L,error);
  } else {
    /* retrieve result */
    str = lua_tolstring(L, -1,&len);
    priv_str_room(len); 
    for(i=0;i<len;i++){
      priv_append_char( (unsigned char)(*(str+i))) ; 
    }
    lua_pop(L, 1);  /* pop returned value */
  }
  return 0;
}

/* TODO: check if shell_cmd_is_allowed makes sense */
int shell_cmd_is_allowed(const char *cmd, char **safecmd, char **cmdname){return 1;}
