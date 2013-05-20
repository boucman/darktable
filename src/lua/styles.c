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

#include "lua/styles.h"
#include "lua/glist.h"
#include "lua/image.h"
#include "lua/types.h"
#include "common/styles.h"



// can't use glist functions we need a list of int and glist can only produce a list of int*
static GList * style_item_table_to_id_list(lua_State*L, int index);
/////////////////////////
// dt_style_t
/////////////////////////
typedef enum
{
  DUPLICATE,
  DELETE,
  NAME,
  DESCRIPTION,
  LAST_STYLE_FIELD
} style_fields;
static const char *style_fields_name[] =
{
  "duplicate",
  "delete",
  "name",
  "description",
  NULL
};
static int style_gc(lua_State*L)
{
  dt_style_t * style =luaL_checkudata(L,-1,"dt_style_t");
  free(style->name);
  free(style->description);
  return 0;
}

static int style_tostring(lua_State*L)
{
  dt_style_t * style =luaL_checkudata(L,-1,"dt_style_t");
  lua_pushstring(L,style->name);
  return 1;
}


static int style_delete(lua_State*L)
{
  dt_style_t * style =luaL_checkudata(L,1,"dt_style_t");
  dt_styles_delete_by_name(style->name);
  return 0;
}


static int style_duplicate(lua_State*L)
{
  dt_style_t * style =luaL_checkudata(L,1,"dt_style_t");
  const char * newname =luaL_checkstring(L,2);
  const char * description =lua_isnoneornil(L,3)?style->description:luaL_checkstring(L,3);
  GList* filter= style_item_table_to_id_list(L, 4);
  dt_styles_create_from_style(style->name,newname,description,filter,-1,NULL);
  g_list_free(filter);
  return 0;
}

static int style_index(lua_State*L)
{
  dt_style_t * style =luaL_checkudata(L,-2,"dt_style_t");
  switch(luaL_checkoption(L,-1,NULL,style_fields_name))
  {
    case DUPLICATE:
      lua_pushcfunction(L,style_duplicate);
      return 1;
    case DELETE:
      lua_pushcfunction(L,style_delete);
      return 1;
    case NAME:
      lua_pushstring(L,style->name);
      return 1;
    case DESCRIPTION:
      lua_pushstring(L,style->description);
      return 1;
  }
  return luaL_error(L,"darktable bug in lua/styles.c, this should never happen");
}

static int style_getnumber(lua_State* L)
{
  int index = luaL_checknumber(L,-1);
  if ( index <= 0 )
  {
    return luaL_error(L,"incorrect index for style");
  }
  dt_style_t * style =luaL_checkudata(L,-2,"dt_style_t");
  GList * items = dt_styles_get_item_list(style->name,true,-1);
  GList * item  = g_list_nth(items,index-1);
  if(!item)
  {
    return luaL_error(L,"incorrect index for style");
  }
  luaA_push(L,dt_style_item_t,item->data);
  g_list_free_full(items,free);
  return 1;
}


static int style_length(lua_State* L)
{

  dt_style_t * style =luaL_checkudata(L,-1,"dt_style_t");
  GList * items = dt_styles_get_item_list(style->name,true,-1);
  lua_pushnumber(L,g_list_length(items));
  g_list_free_full(items,free);
  return 1;
}


static int style_newindex(lua_State*L)
{
  dt_style_t * style =luaL_checkudata(L,-3,"dt_style_t");
  const char * newval = NULL;
  switch(luaL_checkoption(L,-2,NULL,style_fields_name))
  {
    case NAME:
      newval = luaL_checkstring(L,-1);
      dt_styles_update(style->name,newval,style->description,NULL,-1,NULL);
      return 0;
    case DESCRIPTION:
      newval = luaL_checkstring(L,-1);
      dt_styles_update(style->name,style->name,newval,NULL,-1,NULL);
      lua_pushstring(L,style->description);
      return 0;
    case DELETE:
    case DUPLICATE:
    default:
      return luaL_error(L,"unknown index for style : ",lua_tostring(L,-2));
  }
}

/////////////////////////
// dt_style_item_t
/////////////////////////

static int style_item_tostring(lua_State*L)
{
  dt_style_item_t * item =luaL_checkudata(L,-1,"dt_style_item_t");
  lua_pushfstring(L,"%d : %s",item->num,item->name);
  return 1;
}

static int style_item_gc(lua_State*L)
{
  dt_style_item_t * item =luaL_checkudata(L,-1,"dt_style_item_t");
  free(item->name);
  free(item->params);
  free(item->blendop_params);
  return 0;
}

static GList * style_item_table_to_id_list(lua_State*L, int index)
{
  if(lua_isnoneornil(L,index)) return NULL;
  luaL_checktype(L,index,LUA_TTABLE);
  lua_pushnil(L);  /* first key */
  GList * result=NULL;
  while (lua_next(L, index) != 0)
  {
    /* uses 'key' (at index -2) and 'value' (at index -1) */
    dt_style_item_t * item =luaL_checkudata(L,-1,"dt_style_item_t");
    result =g_list_prepend(result,(gpointer)(long unsigned int)item->num);
    lua_pop(L,1);
  }
  result = g_list_reverse(result);
  return result;
}

/////////////////////////
// toplevel and common
/////////////////////////
static int style_table(lua_State*L)
{
  GList *style_list = dt_styles_get_list ("");
  lua_newtable(L);
  while(style_list)
  {
    dt_style_t *data =style_list->data;
    luaA_push(L,dt_style_t,data);
    lua_setfield(L,-2,data->name);
    style_list = g_list_delete_link(style_list,style_list);
  }
  return 1;
}


int dt_lua_style_create_from_image(lua_State*L)
{
  dt_lua_image_t imgid;
  luaA_to(L,dt_lua_image_t,&imgid,-3);
  const char * newname =luaL_checkstring(L,-2);
  const char * description =lua_isnoneornil(L,-1)?"":luaL_checkstring(L,-1);
  dt_styles_create_from_image(newname,description,imgid,NULL);
  GList * style_list = dt_styles_get_list(newname);
  while(style_list)
  {
    dt_style_t *data =style_list->data;
    if(!strcmp(data->name, newname))
    {
      luaA_push(L,dt_style_t,data);
    }
    style_list = g_list_delete_link(style_list,style_list);
  }
  return 1;
}


int dt_lua_init_styles(lua_State * L)
{
  // dt_style
  dt_lua_init_type(L,dt_style_t);
  dt_lua_register_type_callback_list(L,dt_style_t,style_index,style_newindex,style_fields_name);
  dt_lua_register_type_callback_number(L,dt_style_t,style_getnumber,NULL,style_length);
  luaL_getmetatable(L,"dt_style_t");
  lua_pushcfunction(L,style_gc);
  lua_setfield(L,-2,"__gc");
  lua_pushcfunction(L,style_tostring);
  lua_setfield(L,-2,"__tostring");
  lua_pop(L,1);

  //dt_style_item_t
  dt_lua_init_type(L,dt_style_item_t);
  luaL_getmetatable(L,"dt_style_item_t");
  luaA_struct(L,dt_style_item_t);
  luaA_struct_member(L,dt_style_item_t,num,const int);
  luaA_struct_member(L,dt_style_item_t,name,const_string);
  dt_lua_register_type_callback_type(L,dt_style_item_t,NULL,NULL,dt_style_item_t);
  lua_pushcfunction(L,style_item_gc);
  lua_setfield(L,-2,"__gc");
  lua_pushcfunction(L,style_item_tostring);
  lua_setfield(L,-2,"__tostring");
  lua_pop(L,1);



  /* darktable.styles.members() */
  dt_lua_push_darktable_lib(L);
  lua_pushcfunction(L,style_table);
  lua_setfield(L,-2,"styles");
  return 0;
}


// modelines: These editor modelines have been set for all relevant files by tools/update_modelines.sh
// vim: shiftwidth=2 expandtab tabstop=2 cindent
// kate: tab-indents: off; indent-width 2; replace-tabs on; indent-mode cstyle; remove-trailing-space on;
