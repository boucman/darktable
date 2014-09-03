/*
   This file is part of darktable,
   copyright (c) 2012 Jeremy Rosen

   darktable is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   darktable is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with darktable.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "lua/iop.h"
#include "lua/modules.h"
#include "lua/types.h"
#include "gui/gtk.h"

static int version_member(lua_State *L)
{
  dt_iop_module_so_t * module = *(dt_iop_module_so_t**)lua_touserdata(L,-2);
      lua_pushinteger(L,module->version());
  return 1;
}
static int id_member(lua_State *L)
{
  dt_iop_module_so_t * module = *(dt_iop_module_so_t**)lua_touserdata(L,-2);
      lua_pushstring(L,module->op);
  return 1;
}
static int name_member(lua_State *L)
{
  dt_iop_module_so_t * module = *(dt_iop_module_so_t**)lua_touserdata(L,-2);
      lua_pushstring(L,module->name());
  return 1;
}
static int in_styles_member(lua_State *L)
{
  dt_iop_module_so_t * module = *(dt_iop_module_so_t**)lua_touserdata(L,-2);
      lua_pushboolean(L,module->flags() & IOP_FLAGS_INCLUDE_IN_STYLES);
  return 1;
}
static int has_blending_member(lua_State *L)
{
  dt_iop_module_so_t * module = *(dt_iop_module_so_t**)lua_touserdata(L,-2);
      lua_pushboolean(L,module->flags() & IOP_FLAGS_SUPPORTS_BLENDING);
  return 1;
}
static int deprecated_member(lua_State *L)
{
  dt_iop_module_so_t * module = *(dt_iop_module_so_t**)lua_touserdata(L,-2);
      lua_pushboolean(L,module->flags() & IOP_FLAGS_DEPRECATED);
  return 1;
}
static int hidden_member(lua_State *L)
{
  dt_iop_module_so_t * module = *(dt_iop_module_so_t**)lua_touserdata(L,-2);
      lua_pushboolean(L,module->flags() & IOP_FLAGS_HIDDEN);
  return 1;
}
static int in_history_member(lua_State *L)
{
  dt_iop_module_so_t * module = *(dt_iop_module_so_t**)lua_touserdata(L,-2);
      lua_pushboolean(L,!(module->flags() & IOP_FLAGS_NO_HISTORY_STACK));
  return 1;
}
static int has_masks_member(lua_State *L)
{
  dt_iop_module_so_t * module = *(dt_iop_module_so_t**)lua_touserdata(L,-2);
      lua_pushboolean(L,!(module->flags() & IOP_FLAGS_NO_MASKS));
  return 1;
}
static int in_group_basic_member(lua_State *L)
{
  dt_iop_module_so_t * module = *(dt_iop_module_so_t**)lua_touserdata(L,-2);
      lua_pushboolean(L,module->groups() & IOP_GROUP_BASIC);
  return 1;
}
static int in_group_color_member(lua_State *L)
{
  dt_iop_module_so_t * module = *(dt_iop_module_so_t**)lua_touserdata(L,-2);
      lua_pushboolean(L,module->groups() & IOP_GROUP_COLOR);
  return 1;
}
static int in_group_correct_member(lua_State *L)
{
  dt_iop_module_so_t * module = *(dt_iop_module_so_t**)lua_touserdata(L,-2);
      lua_pushboolean(L,module->groups() & IOP_GROUP_CORRECT);
  return 1;
}
static int in_group_effect_member(lua_State *L)
{
  dt_iop_module_so_t * module = *(dt_iop_module_so_t**)lua_touserdata(L,-2);
      lua_pushboolean(L,module->groups() & IOP_GROUP_EFFECT);
  return 1;
}
static int in_group_tone_member(lua_State *L)
{
  dt_iop_module_so_t * module = *(dt_iop_module_so_t**)lua_touserdata(L,-2);
      lua_pushboolean(L,module->groups() & IOP_GROUP_TONE);
  return 1;
}

static int iop_tostring(lua_State* L)
{
  dt_iop_module_so_t * module = *(dt_iop_module_so_t**)lua_touserdata(L,-1);
  lua_pushstring(L,module->op);
  return 1;
}



/*static int test_push(lua_State *L) {
  dt_iop_module_aat * module =  *(dt_iop_module_aat**)lua_touserdata(L,-1);
  luaA_Type preset_type = dt_lua_module_get_preset_type(L,"iop",module->op);
  luaA_push_type(L,preset_type,module);
  return 1;
}
static int test_get(lua_State * L){return luaL_error(L,"TBSL");}
*/

void dt_lua_register_iop(lua_State* L,dt_iop_module_so_t* module)
{
  dt_lua_register_module_entry_new(L,"iop",module->op,module);
  int my_type = dt_lua_module_get_entry_type(L,"iop",module->op);
  dt_lua_type_register_parent_type(L,my_type,luaA_type_find(L,"dt_iop_module_so_t"));
  luaL_getmetatable(L,luaA_typename(L,my_type));
  lua_pushcfunction(L,iop_tostring);
  lua_setfield(L,-2,"__tostring");
  lua_pop(L,1);

};

int dt_lua_init_iop(lua_State *L)
{

  dt_lua_init_type(L,dt_iop_module_so_t);
  lua_pushcfunction(L,version_member);
  dt_lua_type_register_const(L,dt_iop_module_so_t,"version");
  lua_pushcfunction(L,id_member);
  dt_lua_type_register_const(L,dt_iop_module_so_t,"id");
  lua_pushcfunction(L,name_member);
  dt_lua_type_register_const(L,dt_iop_module_so_t,"name");
  lua_pushcfunction(L,in_styles_member);
  dt_lua_type_register_const(L,dt_iop_module_so_t,"in_styles");
  lua_pushcfunction(L,has_blending_member);
  dt_lua_type_register_const(L,dt_iop_module_so_t,"has_blending");
  lua_pushcfunction(L,deprecated_member);
  dt_lua_type_register_const(L,dt_iop_module_so_t,"deprecated");
  lua_pushcfunction(L,hidden_member);
  dt_lua_type_register_const(L,dt_iop_module_so_t,"hidden");
  lua_pushcfunction(L,in_history_member);
  dt_lua_type_register_const(L,dt_iop_module_so_t,"in_history");
  lua_pushcfunction(L,has_masks_member);
  dt_lua_type_register_const(L,dt_iop_module_so_t,"has_masks");
  lua_pushcfunction(L,in_group_basic_member);
  dt_lua_type_register_const(L,dt_iop_module_so_t,"in_group_basic");
  lua_pushcfunction(L,in_group_color_member);
  dt_lua_type_register_const(L,dt_iop_module_so_t,"in_group_color");
  lua_pushcfunction(L,in_group_correct_member);
  dt_lua_type_register_const(L,dt_iop_module_so_t,"in_group_correct");
  lua_pushcfunction(L,in_group_effect_member);
  dt_lua_type_register_const(L,dt_iop_module_so_t,"in_group_effect");
  lua_pushcfunction(L,in_group_tone_member);
  dt_lua_type_register_const(L,dt_iop_module_so_t,"in_group_tone");
  dt_lua_init_module_type(L,"iop");
  return 0;
}
// modelines: These editor modelines have been set for all relevant files by tools/update_modelines.sh
// vim: shiftwidth=2 expandtab tabstop=2 cindent
// kate: tab-indents: off; indent-width 2; replace-tabs on; indent-mode cstyle; remove-trailing-space on;
