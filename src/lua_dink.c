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

#include "lua_dinkc.h"

lua_State* luaVM;
int current_lua_script = -1;

static char* read_whole_file(FILE* f)
{
  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET);

  char *string = xmalloc(fsize + 1);

  size_t read = fread(string, fsize, 1, f);

  if (read != 1)
  {
    free(string);
    return NULL;
  }
  
  string[fsize] = 0;
  return string;
}

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

void lua_dink_init()
{
  luaVM = luaL_newstate();

  luaL_openlibs(luaVM);
  lua_bind_dinkc();

#ifdef LUA_DINK_DEBUG
  lua_sethook(luaVM, lua_dink_hook, LUA_MASKCALL /*| LUA_MASKRET | LUA_MASKLINE*/, 0);
#endif
  
  FILE *in = find_resource_as_file("init.lua");
  if (in == NULL)
  {
    log_error("Could not load Lua init script. File not found.");
    return;
  }
  
  char* script = read_whole_file(in);
  fclose(in);

  if (script == NULL)
  {
    log_error("Could not load Lua init script. Could not read file.");
    return;
  }

  int retVal = luaL_dostring(luaVM, script);
  //lua_load(luaVM, NULL, NULL, 
  
  free(script);

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

void lua_dink_quit()
{
  lua_close(luaVM);
  log_info("Closed Lua.");
}

int lua_load_script(struct refinfo* rinfo, char* script)
{
  if (rinfo->lua_script_loaded)
    return 1;

  log_info("Loading Lua script %s.L", rinfo->name);

  /*
   * load_script(rinfo->name, rinfo->sprite, rinfo, script)
   */
  int top = lua_gettop(luaVM);
  lua_getglobal(luaVM, "load_script");
  lua_pushstring(luaVM, rinfo->name);
  lua_pushinteger(luaVM, rinfo->sprite);
  lua_pushlightuserdata(luaVM, rinfo);
  lua_pushstring(luaVM, script);
  int retVal = lua_pcall(luaVM, 4, LUA_MULTRET, 0);

  if (retVal != LUA_OK)
  {
    if (retVal != LUA_ERRRUN)
    {
      // TODO: Report/handle this better
      log_error("Internal Lua error");
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
    const char* error_format = "Error while executing lua script %s.L:"; 
    char error_title[strlen(error_format) + strlen(rinfo->name)];
    sprintf(error_title, error_format, rinfo->name);
    lua_report_error(error_title, error_message);
    lua_pop(luaVM, nresults);
    return 0;
  }

  lua_pop(luaVM, nresults);
  rinfo->lua_script_loaded = 1;
  return 1;
}

int lua_script_procedure_exists(struct refinfo* rinfo)
{
  if (!rinfo->luaproc)
  {
    log_error("Tried checking lua script procedure without setting procedure name");
    return 0;
  }

  lua_getglobal(luaVM, "script_procedure_exists");
  lua_pushstring(luaVM, rinfo->name);
  lua_pushstring(luaVM, rinfo->luaproc);
  lua_call(luaVM, 2, 1);

  int exists = lua_toboolean (luaVM, -1);
  lua_pop(luaVM, 1);
  return exists;
}

int script_stack[MAX_SCRIPTS];
int script_p = -1;

int lua_run_script(struct refinfo* rinfo)
{
  if (!rinfo->luaproc)
  {
    log_error("Tried running lua script without setting procedure name");
    return 0;
  }

  script_stack[++script_p] = rinfo->lua_script_index;
  current_lua_script = script_stack[script_p];

  int top = lua_gettop(luaVM);
  lua_getglobal(luaVM, "run_script");
  lua_pushstring(luaVM, rinfo->name);
  lua_pushstring(luaVM, rinfo->luaproc);
  lua_pcall(luaVM, 2, LUA_MULTRET, 0);
  int nresults = lua_gettop(luaVM) - top;

  if (--script_p < 0)
    current_lua_script = -1;
  else
    current_lua_script = script_stack[script_p];

  int success = lua_toboolean(luaVM, -nresults);
  if (!success)
  {
    const char* error_message = lua_tostring(luaVM, -1);
    const char* error_format = "Error while executing lua function %s() in script %s.L:"; 
    char error_title[strlen(error_format) + strlen(rinfo->name) + strlen(rinfo->luaproc)];
    sprintf(error_title, error_format, rinfo->luaproc, rinfo->name);
    lua_report_error(error_title, error_message);
    lua_pop(luaVM, nresults);
    return 0;
  }
  
  lua_pop(luaVM, nresults);
  return 1;
}

void lua_kill_script(struct refinfo* rinfo)
{
  /* This is equivalent to doing this in Lua:
   * script[rinfo->name] = nil
   * routine[rinfo->name] = nil
   * refinfo[rinfo->name] = nil
   */
  lua_getglobal(luaVM, "script");
  lua_pushstring(luaVM, rinfo->name);
  lua_pushnil(luaVM);
  lua_settable (luaVM, -3);
  lua_pop(luaVM, 1);

  lua_getglobal(luaVM, "routine");
  lua_pushstring(luaVM, rinfo->name);
  lua_pushnil(luaVM);
  lua_settable (luaVM, -3);
  lua_pop(luaVM, 1);

  lua_getglobal(luaVM, "refinfo");
  lua_pushstring(luaVM, rinfo->name);
  lua_pushnil(luaVM);
  lua_settable (luaVM, -3);
  lua_pop(luaVM, 1);
}
