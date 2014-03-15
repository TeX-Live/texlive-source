diff -u LuaJIT-2.0.3.orig/src/lib_package.c LuaJIT-2.0.3/src/lib_package.c
--- LuaJIT-2.0.3.orig/src/lib_package.c	2014-03-13 12:38:39.407900526 +0100
+++ LuaJIT-2.0.3/src/lib_package.c	2014-03-13 13:01:11.283956573 +0100
@@ -68,6 +68,9 @@
 #elif LJ_TARGET_WINDOWS
 
 #define WIN32_LEAN_AND_MEAN
+#ifndef WINVER
+#define WINVER 0x0500
+#endif
 #include <windows.h>
 
 #ifndef GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
@@ -354,6 +357,29 @@
   return 1;  /* library loaded successfully */
 }
 
+#define LUA_POF		"luaopen_"
+#define LUA_OFSEP	"_"
+#define POF		LUA_POF
+
+static const char *mkfuncname (lua_State *L, const char *modname) {
+  const char *funcname;
+  const char *mark = strchr(modname, *LUA_IGMARK);
+  if (mark) modname = mark + 1;
+  funcname = luaL_gsub(L, modname, ".", LUA_OFSEP);
+  funcname = lua_pushfstring(L, POF"%s", funcname);
+  lua_remove(L, -2);  /* remove 'gsub' result */
+  return funcname;
+}
+
+
+int loader_C_luatex (lua_State *L, const char *name, const char *filename) {
+  const char *funcname;
+  funcname = mkfuncname(L, name);
+  if (ll_loadfunc(L, filename, funcname,0) != 0)
+    loaderror(L, filename);
+  return 1;  /* library loaded successfully */
+}
+
 static int lj_cf_package_loader_croot(lua_State *L)
 {
   const char *filename;
@@ -373,6 +399,21 @@
   return 1;
 }
 
+int loader_Call_luatex (lua_State *L, const char *name, const char *filename) {
+  const char *funcname;
+  int stat;
+  if (filename == NULL) return 1;  /* root not found */
+  funcname = mkfuncname(L, name);
+  if ((stat = ll_loadfunc(L, filename, funcname,0)) != 0) {
+    if (stat != PACKAGE_ERR_FUNC) loaderror(L, filename);  /* real error */
+    lua_pushfstring(L, "\n\tno module " LUA_QS " in file " LUA_QS,
+                       name, filename);
+    return 1;  /* function not found */
+  }
+  return 1;  /* library loaded successfully */
+}
+
+
 static int lj_cf_package_loader_preload(lua_State *L)
 {
   const char *name = luaL_checkstring(L, 1);
