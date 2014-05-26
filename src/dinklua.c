/**
 * Lua Integration

 * Copyright (C) 2013  Alexander Krivács Schrøder

 * This file is part of GNU FreeDink

 * GNU FreeDink is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.

 * GNU FreeDink is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

//#define LUA_DINK_DEBUG

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include <xalloc.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "dinkc.h"
#include "io_util.h"
#include "log.h"
#include "str_util.h"
#include "scripting.h"

#include "dinklua.h"
#include "dinklua_bindings.h"

int dinklua_enabled = 1;
struct script_engine *lua_engine = NULL;

lua_State* luaVM;

int current_lua_script = -1;
int lua_script_stack[MAX_SCRIPTS];
int lua_script_p = -1;

/* DinkLua script data conversion */
#define luainfo(script) ((struct luainfo*)(sinfo[script]->data))

static void lua_report_error(const char* error_intro, const char* error_message)
{
  log_error(error_intro);

  // TODO: Change from truncation to multi-line reporting
  if (strlen(error_message) > 99)
  {
    char cut_error[100];
    strncpy(cut_error, error_message, 96);
    cut_error[96] = '.';
    cut_error[97] = '.';
    cut_error[98] = '.';
    cut_error[99] = 0;
    log_error("%s", cut_error);
  }
  else
  {
    log_error("%s", error_message);
  }
}

#ifdef LUA_DINK_DEBUG
static void lua_dink_hook(lua_State *L, lua_Debug *ar)
{
  if (!lua_getinfo (L, "nSltu", ar))
  {
    log_debug("INVALID GETINFO");
    return;
  }
  
  switch (ar->event)
  {
    case LUA_HOOKCALL:      
      log_debug("[Lua] call");

      /*const char* local;
      int n = 1;
      while ((local = lua_getlocal(L, ar, n)) != NULL)
      {
        switch(lua_type (L, -1))
        {
          case LUA_TNIL:
            log_debug("%s = nil", local);
            break;

          case LUA_TNUMBER:
            log_debug("%s = %f", local, lua_tonumber(L, -1));
            break;

          case LUA_TBOOLEAN:
            log_debug("%s = %d", local, lua_toboolean (L, -1));
            break;

          case LUA_TSTRING:
            log_debug("%s = \"%s\"", local, lua_tostring(L, -1));
            break;

          case LUA_TTABLE:
            log_debug("%s = [table]", local);
            break;

          case LUA_TFUNCTION:
            log_debug("%s = [function]", local);
            break;

          case LUA_TUSERDATA:
            log_debug("%s = [userdata]", local);
            break;

          case LUA_TTHREAD:
            log_debug("%s = [thread]", local);
            break;

          case LUA_TLIGHTUSERDATA:
            log_debug("%s = [lightuserdata]", local);
            break;
        }

        lua_pop(L, 1);
        n++;
      }*/
      
      break;

    case LUA_HOOKRET:
      log_debug("[Lua] return");
      break;

    case LUA_HOOKTAILCALL:
      log_debug("[Lua] tailcall");
      break;

    case LUA_HOOKLINE:
      log_debug("[Lua] line");
      break;
  }

  if (!lua_getinfo (L, "nSltu", ar))
  {
    log_debug("INVALID GETINFO");
    return;
  }
  
  if (ar->name)
    log_debug("    name=%s", ar->name);
  if (ar->namewhat && strlen(ar->namewhat) > 0)
    log_debug("    namewhat=%s", ar->namewhat);
  if (ar->what)
    log_debug("    what=%s", ar->what);

  log_debug("    currentline=%d", ar->currentline);
  log_debug("    linedefined=%d", ar->linedefined);
  log_debug("    lastlinedefined=%d", ar->lastlinedefined);
  log_debug("    nups=%d", ar->nups);
  log_debug("    nparams=%d", ar->nparams);
  log_debug("    isvararg=%d", ar->isvararg);
  log_debug("    istailcall=%d", ar->istailcall);
  
  if (ar->short_src)
    log_debug("    short_src=%s", ar->short_src);
}
#endif

static int lua_allocate_data(void **data)
{
  *data = XZALLOC(struct luainfo);
  if (*data == NULL)
  {
    return 0;
  }
  memset(*data, 0, sizeof(struct luainfo));
  return 1;
}

static int lua_load_script(const char *path, int script)
{
  log_info("Loading Lua script %s.lua", sinfo[script]->name);

  /*
   * load_script(script, path, sprite)
   */
  int top = lua_gettop(luaVM);
  lua_getglobal(luaVM, "load_script");
  lua_pushinteger(luaVM, script);
  lua_pushstring(luaVM, path);
  lua_pushinteger(luaVM, sinfo[script]->sprite);
  int retVal = lua_pcall(luaVM, 3, LUA_MULTRET, 0);

  if (retVal != LUA_OK)
  {
    if (retVal != LUA_ERRRUN)
    {
      // TODO: Report/handle this better
      log_error("Internal Lua error in lua_load_script()");
      return 0;
    }
    
    const char* error_message = lua_tostring(luaVM, -1);
    lua_report_error("Error while executing load_script() in init.lua:",
                     error_message);
    lua_pop(luaVM, 1);
    return 0;
  }
  
  int nresults = lua_gettop(luaVM) - top;

  int success = lua_toboolean(luaVM, -nresults);
  if (!success)
  {
    const char* error_message = lua_tostring(luaVM, -1);
    const char* error_format = "Error while executing lua script %s.lua:"; 
    char error_title[strlen(error_format) + strlen(sinfo[script]->name)];
    sprintf(error_title, error_format, sinfo[script]->name);
    lua_report_error(error_title, error_message);
    lua_pop(luaVM, nresults);
    return 0;
  }

  lua_pop(luaVM, nresults);
  return 1;
}

static int lua_proc_exists(int script, const char *proc)
{
  char *proc_name = strdup(proc);
  strtolower(proc_name);
  
  lua_getglobal(luaVM, "proc_exists");
  lua_pushinteger(luaVM, script);
  lua_pushstring(luaVM, proc_name);
  lua_call(luaVM, 2, 1);

  int exists = lua_toboolean(luaVM, -1);
  lua_pop(luaVM, 1);

  free(proc_name);
  return exists;
}

static int lua_run_script_proc(int script, const char *proc)
{
  char *proc_name = strdup(proc);
  strtolower(proc_name);
  
  lua_script_stack[++lua_script_p] = script;
  current_lua_script = lua_script_stack[lua_script_p];

  int top = lua_gettop(luaVM);
  lua_getglobal(luaVM, "run_script");
  lua_pushinteger(luaVM, script);
  lua_pushstring(luaVM, proc_name);
  lua_pcall(luaVM, 2, LUA_MULTRET, 0);
  int nresults = lua_gettop(luaVM) - top;

  if (--lua_script_p < 0)
    current_lua_script = -1;
  else
    current_lua_script = lua_script_stack[lua_script_p];

  int success = lua_toboolean(luaVM, -nresults);
  if (!success)
  {
    const char* error_message = lua_tostring(luaVM, -1);
    const char* error_format = "Error while executing lua function %s() in script %s.lua:"; 
    char error_title[strlen(error_format) + strlen(sinfo[script]->name) + strlen(proc_name)];
    sprintf(error_title, error_format, proc_name, sinfo[script]->name);
    lua_report_error(error_title, error_message);
    lua_pop(luaVM, nresults);
  
    free(proc_name);
    return 0;
  }
  
  lua_pop(luaVM, nresults);
  free(proc_name);
  return 1;
}

static void lua_resume_script(int script)
{
  lua_script_stack[++lua_script_p] = script;
  current_lua_script = lua_script_stack[lua_script_p];

  int top = lua_gettop(luaVM);
  lua_getglobal(luaVM, "resume_script");
  lua_pushinteger(luaVM, script);
  lua_pcall(luaVM, 1, LUA_MULTRET, 0);
  int nresults = lua_gettop(luaVM) - top;

  if (--lua_script_p < 0)
    current_lua_script = -1;
  else
    current_lua_script = lua_script_stack[lua_script_p];

  int success = lua_toboolean(luaVM, -nresults);
  if (!success)
  {
    const char* error_message = lua_tostring(luaVM, -1);
    const char* error_format = "Error while resuming script %s.lua:"; 
    char error_title[strlen(error_format) + strlen(sinfo[script]->name)];
    sprintf(error_title, error_format, sinfo[script]->name);
    lua_report_error(error_title, error_message);
    lua_pop(luaVM, nresults);
    return;
  }
  
  lua_pop(luaVM, nresults);
  return;
}

static void lua_kill_script(int script)
{
  /* This is equivalent to doing this in Lua:
   * script[script] = nil
   * routine[script] = nil
   */
  lua_getglobal(luaVM, "script");
  lua_pushinteger(luaVM, script);
  lua_pushnil(luaVM);
  lua_settable (luaVM, -3);
  lua_pop(luaVM, 1);

  lua_getglobal(luaVM, "routine");
  lua_pushinteger(luaVM, script);
  lua_pushnil(luaVM);
  lua_settable (luaVM, -3);
  lua_pop(luaVM, 1);
}

void dinklua_initialize(struct script_engine *script_engine)
{
  lua_engine = script_engine;
  
  script_engine->active = 1;
  script_engine->name = strdup("Lua");
  script_engine->extensions = XNMALLOC(2, char*);
  script_engine->extensions[0] = strdup("lua");
  script_engine->extensions[1] = NULL;
  
  script_engine->allocate_data = lua_allocate_data;
  script_engine->free_data = free;

  script_engine->load_script = lua_load_script;
  script_engine->kill_script = lua_kill_script;
  script_engine->script_proc_exists = lua_proc_exists;
  script_engine->run_script_proc = lua_run_script_proc;
  script_engine->resume_script = lua_resume_script;
  
  luaVM = luaL_newstate();

  luaL_openlibs(luaVM);
  dinklua_bind_init();

#ifdef LUA_DINK_DEBUG
  lua_sethook(luaVM, lua_dink_hook, LUA_MASKCALL /*| LUA_MASKRET | LUA_MASKLINE*/, 0);
#endif

  char *path = find_resource_as_path("init.lua");
  if (path == NULL)
  {
    log_error("Could not load init.lua script. File not found.");
    return;
  }

  int retVal = luaL_dofile(luaVM, path);
  if (retVal)
  {
    const char* error_message = lua_tostring(luaVM, -1);
    lua_report_error("Could not load Lua init script. Lua error:",
                     error_message);
    lua_pop(luaVM, 1);
    return;
  }
  
  log_info("Loaded Lua init script.");
}

void dinklua_quit()
{
  lua_close(luaVM);
  lua_engine = NULL;
  log_info("Closed Lua.");
}

int dinklua_get_current_line(lua_State *l)
{
  // Tries to find the lowest stack (which is the script itself) and get its
  // line number. This number will vary due to various wrappers in init.lua. I
  // think it's safe to assume no function will be wrapped more than nine times.
  lua_Debug ar;
  int found = 0;
  for (int i = 10; i > 0; i--)
  {
    if (lua_getstack(l, i, &ar))
    {
      found = 1;
      break;
    }
  }

  if (!found)
    return -9999;
  
  if (lua_getinfo(l, "l", &ar))
    return ar.currentline;
  else
    return -8888;
}
