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
#include "lua/introspection.h"
#include "lua/call.h"
#include "develop/imageop.h"
static int range_float_handler(lua_State *L)
{
  dt_introspection_field_t * field = lua_touserdata(L,lua_upvalueindex(1));
  size_t offset = luaL_checknumber(L,lua_upvalueindex(2));
  dt_introspection_t * type = field->header.so->get_introspection();
  void **param_data = lua_touserdata(L,1);
  float * field_data = (*param_data)+offset;
  if(lua_isnone(L,3)) {
    lua_pushnumber(L,*field_data);
    return 1;
  } else {
    float value = luaL_checknumber(L,3);
    if( value < field->Float.Min || value > field->Float.Max) {
      char msg[512];
      snprintf(msg,sizeof(msg),"Value for %s.%s should be between %f and %f\n",type->type_name,field->header.name,field->Float.Min,field->Float.Max);
      return luaL_argerror(L,3,msg);
    }
    *field_data = value;
    return 0;
  }

}
static int range_int_handler(lua_State *L)
{
  dt_introspection_field_t * field = lua_touserdata(L,lua_upvalueindex(1));
  size_t offset = luaL_checknumber(L,lua_upvalueindex(2));
  dt_introspection_t * type = field->header.so->get_introspection();
  void **param_data = lua_touserdata(L,1);
  int * field_data = (*param_data)+offset;
  if(lua_isnone(L,3)) {
    lua_pushinteger(L,*field_data);
    return 1;
  } else {
    int value = luaL_checknumber(L,3);
    if( value < field->Int.Min || value > field->Int.Max) {
      char msg[512];
      snprintf(msg,sizeof(msg),"Value for %s.%s should be between %d and %d\n",type->type_name,field->header.name,field->Int.Min,field->Int.Max);
      return luaL_argerror(L,3,msg);
    }
    *field_data = value;
    return 0;
  }

}
static int range_uint_handler(lua_State *L)
{
  dt_introspection_field_t * field = lua_touserdata(L,lua_upvalueindex(1));
  size_t offset = luaL_checknumber(L,lua_upvalueindex(2));
  dt_introspection_t * type = field->header.so->get_introspection();
  void **param_data = lua_touserdata(L,1);
  unsigned int * field_data = (*param_data)+offset;
  if(lua_isnone(L,3)) {
    lua_pushunsigned(L,*field_data);
    return 1;
  } else {
    unsigned int value = luaL_checknumber(L,3);
    if( value < field->UInt.Min || value > field->UInt.Max) {
      char msg[512];
      snprintf(msg,sizeof(msg),"Value for %s.%s should be between %d and %d\n",type->type_name,field->header.name,field->UInt.Min,field->UInt.Max);
      return luaL_argerror(L,3,msg);
    }
    *field_data = value;
    return 0;
  }

}
static int range_bool_handler(lua_State *L)
{
  size_t offset = luaL_checknumber(L,lua_upvalueindex(2));
  void **param_data = lua_touserdata(L,1);
  gboolean * field_data = (*param_data)+offset;
  if(lua_isnone(L,3)) {
    lua_pushboolean(L,*field_data);
    return 1;
  } else {
    *field_data = lua_toboolean(L,3);
    return 0;
  }

}
static int range_char_handler(lua_State *L)
{
  dt_introspection_field_t * field = lua_touserdata(L,lua_upvalueindex(1));
  size_t offset = luaL_checknumber(L,lua_upvalueindex(2));
  dt_introspection_t * type = field->header.so->get_introspection();
  void **param_data = lua_touserdata(L,1);
  char * field_data = (*param_data)+offset;
  if(lua_isnone(L,3)) {
    lua_pushinteger(L,*field_data);
    return 1;
  } else {
    lua_Integer value = luaL_checkinteger(L,3);
    if( value < field->Char.Min || value > field->Char.Max) {
      char msg[512];
      snprintf(msg,sizeof(msg),"Value for %s.%s should be between %d and %d\n",type->type_name,field->header.name,field->Char.Min,field->Char.Max);
      return luaL_argerror(L,3,msg);
    }
    *field_data = value;
    return 0;
  }

}

static int range_enum_handler(lua_State *L)
{
  dt_introspection_field_t * field = lua_touserdata(L,lua_upvalueindex(1));
  size_t offset = luaL_checknumber(L,lua_upvalueindex(2));
  dt_introspection_t * type = field->header.so->get_introspection();
  void **param_data = lua_touserdata(L,1);
  char * field_data = (*param_data)+offset;
  if(lua_isnone(L,3)) {
    dt_introspection_type_enum_tuple_t *current = field->Enum.values;
    while(current->name) {
      if(current->value == *field_data) {
        lua_pushstring(L,current->name);
        return 1;
      }
      current++;
    }
    printf("debug, don'tcheck invalid %d\n",*field_data);
    //g_assert(false);
    return luaL_argerror(L,1,"incorrect data in DT iop");
  } else {
    const char *value = luaL_checkstring(L,3);
    dt_introspection_type_enum_tuple_t *current = field->Enum.values;
    while(current->name) {
      if(!strcasecmp(current->name, value)) {
        *field_data = current->value;
        return 0;
      }
      current++;
    }
    char msg[512];
    snprintf(msg,sizeof(msg),"Incorrect value  for %s.%s \n",type->type_name,field->header.name);
    return luaL_argerror(L,3,msg);
  }

}

static int subobject_handler(lua_State *L)
{
  dt_introspection_field_t * field = lua_touserdata(L,lua_upvalueindex(1));
  size_t offset = luaL_checknumber(L,lua_upvalueindex(2));
  dt_introspection_t * type = field->header.so->get_introspection();
  void **param_data = lua_touserdata(L,1);
  char * field_data = (*param_data)+offset;
  if(lua_isnone(L,3)) {
    char tmp_name[1024];
    snprintf(tmp_name,sizeof(tmp_name),"%s.%s",type->type_name,field->header.name);
    luaA_push_type(L,luaA_type_find(L,tmp_name),&field_data);

    // keep a pointer on the original object for GC
    lua_getuservalue(L,1);
    lua_getuservalue(L,-2);
    lua_getfield(L,-2,"data_object");
    lua_setfield(L,-2,"data_object");
    lua_pop(L,2);


    return 1;
  } else {
    printf("TBSL : write to subobject");
    lua_pushstring(L,"TBSL : write to subobject");
    return 0;
  }

}

static int length_handler(lua_State *L)
{
  dt_introspection_field_t * field = lua_touserdata(L,lua_upvalueindex(1));
  lua_pushnumber(L,field->Array.count);
  return 1;
}

static int number_handler(lua_State *L)
{
  gboolean is_write = !lua_isnone(L,3);
  dt_introspection_field_t * field = lua_touserdata(L,lua_upvalueindex(1));
  size_t offset = luaL_checknumber(L,lua_upvalueindex(2));
  dt_introspection_t * type = field->header.so->get_introspection();
  void **param_data = lua_touserdata(L,1);
  char * field_data = (*param_data)+offset;
  int index = luaL_checkinteger(L,2);
  void * target_address = dt_introspection_access_array(field,field_data,index-1,NULL);
  // push the subhanlder
  luaL_getmetafield(L,1,"__elt_subhandler");
  // push a copy of ourselves with target_address
  {
    char tmp_name[1024];
    snprintf(tmp_name,sizeof(tmp_name),"%s.%s",type->type_name,field->header.name);
    luaA_push_type(L,luaA_type_find(L,tmp_name),&target_address);

    // keep a pointer on the original object for GC
    lua_getuservalue(L,1);
    lua_getuservalue(L,-2);
    lua_getfield(L,-2,"data_object");
    lua_setfield(L,-2,"data_object");
    lua_pop(L,2);
  }
  // push a copy of the key
  lua_pushvalue(L,2);
  if(!is_write) {
    dt_lua_do_chunk_raise(L,2,1);

    return 1;
  } else {
    printf("TBSL : write to array");
    lua_pushstring(L,"TBSL : write to array");
    return 0;
  }
}

static int create_introspection_subtype(lua_State * L,const char * name,dt_introspection_t * orig)
{
  luaA_Type new_type = luaA_type_add(L,name,sizeof(void*));
  dt_lua_init_type_type(L,new_type);
  luaL_getmetatable(L,orig->type_name);

  char tmp_name[1024];
  snprintf(tmp_name,sizeof(tmp_name),"%s%%internal",orig->type_name);
  lua_pushstring(L,tmp_name);
  lua_setfield(L,-2,"__introspection_data_type");

  lua_pop(L,1);
  return new_type;
}

/*
   Given a field, create a cclosure to handle that field and push it on the stack
   */
int get_subfield_handler(
    lua_State *L, // State to work with
    dt_introspection_field_t * field, // the field to work on
    dt_introspection_t * orig, // the original field
    size_t offset  // offset into the parent object
    )
{


  switch(field->header.type) {
    case DT_INTROSPECTION_TYPE_FLOAT:
      lua_pushlightuserdata(L,field);
      lua_pushnumber(L,offset);
      lua_pushcclosure(L,range_float_handler,2);

      return 1;
    case DT_INTROSPECTION_TYPE_INT:
      lua_pushlightuserdata(L,field);
      lua_pushnumber(L,offset);
      lua_pushcclosure(L,range_int_handler,2);
      return 1;
    case DT_INTROSPECTION_TYPE_UINT:
      lua_pushlightuserdata(L,field);
      lua_pushnumber(L,offset);
      lua_pushcclosure(L,range_uint_handler,2);
      return 1;
    case DT_INTROSPECTION_TYPE_BOOL:
      lua_pushlightuserdata(L,field);
      lua_pushnumber(L,offset);
      lua_pushcclosure(L,range_bool_handler,2);
      return 1;
    case DT_INTROSPECTION_TYPE_CHAR:
      lua_pushlightuserdata(L,field);
      lua_pushnumber(L,offset);
      lua_pushcclosure(L,range_char_handler,2);
      return 1;
    case DT_INTROSPECTION_TYPE_OPAQUE:
      lua_pushnil(L);
      return 1;
    case DT_INTROSPECTION_TYPE_ENUM:
      lua_pushlightuserdata(L,field);
      lua_pushnumber(L,offset);
      lua_pushcclosure(L,range_enum_handler,2);
      return 1;
    case DT_INTROSPECTION_TYPE_STRUCT:
      {
        char tmp_name[1024];
        snprintf(tmp_name,sizeof(tmp_name),"%s.%s",orig->type_name,field->header.name);
        luaA_Type id = create_introspection_subtype(L,tmp_name,orig);

        int cur_index = 0;
        while(field->Struct.fields[cur_index]) {
          get_subfield_handler(L,field->Struct.fields[cur_index],orig,
              field->Struct.fields[cur_index]->header.offset - offset);

          dt_lua_type_register_type(L,id,field->Struct.fields[cur_index]->header.field_name);
          cur_index++;
        }
        lua_pushlightuserdata(L,field);
        lua_pushnumber(L,offset);
        lua_pushcclosure(L,subobject_handler,2);
        return 1;
      }
      break;
    case DT_INTROSPECTION_TYPE_ARRAY:
      {
        char tmp_name[1024];
        snprintf(tmp_name,sizeof(tmp_name),"%s.%s",orig->type_name,field->header.name);
        luaA_Type array_id = create_introspection_subtype(L,tmp_name,orig);

        lua_pushlightuserdata(L,field);
        lua_pushnumber(L,0);
        lua_pushcclosure(L,length_handler,2);
        lua_pushlightuserdata(L,field);
        lua_pushnumber(L,0);
        lua_pushcclosure(L,number_handler,2);
        dt_lua_type_register_number_type(L,array_id);

        luaL_getmetatable(L,tmp_name);


        get_subfield_handler(L,field->Array.field,orig,0);
        lua_setfield(L,-2,"__elt_subhandler");
        lua_pop(L,1);



        lua_pushlightuserdata(L,field);
        lua_pushnumber(L,offset);
        lua_pushcclosure(L,subobject_handler,2);
        return 1;
      }
    case DT_INTROSPECTION_TYPE_NONE:
    case DT_INTROSPECTION_TYPE_DOUBLE:
    case DT_INTROSPECTION_TYPE_UCHAR:
    case DT_INTROSPECTION_TYPE_SHORT:
    case DT_INTROSPECTION_TYPE_USHORT:
    case DT_INTROSPECTION_TYPE_LONG:
    case DT_INTROSPECTION_TYPE_ULONG:
    default:
      printf("don't know how to deal with %s of type %d\n",field->header.name,field->header.type);
      lua_pushnil(L);
      return 1;
  }
}


static int init_toplevel_introspected_subtype(lua_State *L)
{
  luaL_getmetafield(L,1,"__introspection_data_type");
  luaA_Type data_type = luaA_type_find(L,luaL_checkstring(L,-1));
  lua_pop(L,1);

  void* data = lua_touserdata(L,2);
  luaA_push_type(L,data_type,data);
  lua_getuservalue(L,1);
  lua_pushvalue(L,-2);

  lua_setfield(L,-2,"data_object"); // keep a pointer for GC
  lua_pop(L,1);

  void** obj_pointer = lua_touserdata(L,1);
  *obj_pointer = lua_touserdata(L,-1);

  lua_pop(L,1);
  return 0;
}

void dt_lua_register_introspected_type(lua_State *L, dt_introspection_t * type)
{
  if(!type || type->field->header.type == DT_INTROSPECTION_TYPE_OPAQUE) {
    printf("TBSL : deal cleanly with opaque %p\n",type);
    if(type) printf("%s\n",type->type_name);
    return;
  }
  char tmp_name[1024];
  snprintf(tmp_name,sizeof(tmp_name),"%s%%internal",type->type_name);
  luaA_Type id = luaA_type_add(L,tmp_name,type->size);
  dt_lua_init_type_type(L,id);

  luaA_Type new_id = luaA_type_add(L,type->type_name,sizeof(void*));
  dt_lua_init_type_type(L,new_id);
  luaL_getmetatable(L,type->type_name);

  lua_pushstring(L,tmp_name);
  lua_setfield(L,-2,"__introspection_data_type");

  lua_pushcfunction(L,init_toplevel_introspected_subtype);
  lua_setfield(L,-2,"__init");

  lua_pop(L,1);

  int cur_index = 0;
  while(type->field->Struct.fields[cur_index]) {
    get_subfield_handler(L,type->field->Struct.fields[cur_index],type, type->field->Struct.fields[cur_index]->header.offset);
    dt_lua_type_register_type(L,new_id,type->field->Struct.fields[cur_index]->header.field_name);
    cur_index++;
  }

}
