#include <luabind/luabind.hpp>
#include <string>

static const std::string obj_name(const luabind::object& o)
{
    lua_State* L = o.interpreter();
    std::string name;

    o.push(L);
    luabind::detail::object_rep* obj = luabind::detail::get_instance(L, -1);
    if (!obj)
    {
        name = lua_typename(L, lua_type(L, -1));
        lua_pop(L, 1);
        return name;
    }

    lua_pop(L, 1);
    name = obj->crep()->name();
    return name;
}

static const var toVar(lua_State* L, int index)
{
    var v;
    luabind::object obj(luabind::from_stack(L,index));

	switch(luabind::type(obj))
	{
		case LUA_TSTRING:
			v = luabind::object_cast<String>(obj);
            return v;

		case LUA_TNUMBER:
			v = luabind::object_cast<double>(obj);
            return v;
    }
    
    return v;
}

static const void fromVar (lua_State* L, var const& v)
{
	if(v.isString())
	{
		lua_pushstring(L, v.toString().toUTF8());
        return;
    }
	else if(v.isInt())
	{
        lua_pushnumber(L, (int)v);
        return;
    }
	else if(v.isDouble())
	{
        lua_pushnumber(L, (double)v);
        return;
    }
    lua_pushnil(L);
}

namespace luabind
{
	template <> struct default_converter<var> : native_converter_base<var>
	{
		static int compute_score(lua_State* L, int index)
		{
			lua_type(L, index);
			return 0;
		}

		var from(lua_State* L, int index)
		{
			return toVar(L,index);
		}

		void to(lua_State* L, var const& v)
		{
			fromVar (L,v);
		}
	};

	template <> struct default_converter<var const&> : default_converter<var>
	{};

	template <> struct default_converter<String> : native_converter_base<String>
	{
		static int compute_score(lua_State* L, int index)
		{
			object obj(luabind::from_stack(L,index));
			std::string name = obj_name(obj);
            
			if(name == "String")
			{
				return (0);
			}
			return (lua_type(L, index) == LUA_TSTRING) ? 0 : -1;
		}
	
		String from(lua_State* L, int index)
		{
			object obj(luabind::from_stack(L,index));
			std::string name = obj_name(obj);
            
			if(name == "String")
			{
                String* wp = object_cast<String*>(obj);
                return String(*wp);
            }
			return String(lua_tostring(L, index));
		}
	
		void to(lua_State* L, String const &value)
		{
			lua_pushlstring(L, value.toUTF8(), value.length());
		}
	};

	template <> struct default_converter<String const&> : default_converter<String>
	{
	};

	template <> struct default_converter<Identifier> : native_converter_base<Identifier>
	{
		static int compute_score(lua_State* L, int index)
		{
			object obj(luabind::from_stack(L,index));
			std::string name = obj_name(obj);
            
			if(name == "Identifier")
			{
				return (0);
			}
			return (lua_type(L, index) == LUA_TSTRING) ? 0 : -1;
		}
	
		Identifier from(lua_State* L, int index)
		{
			object obj(luabind::from_stack(L,index));
			std::string name = obj_name(obj);
            
			if(name == "Identifier")
			{
                Identifier* wp = object_cast<Identifier*>(obj);
                return Identifier(*wp);
            }
			return Identifier(lua_tostring(L, index));
		}
	
		void to(lua_State* L, Identifier const &value)
		{
			lua_pushlstring(L, value.toString().toUTF8(), strlen(value.toString().toUTF8()));
		}
	};

	template <> struct default_converter<Identifier const&> : default_converter<Identifier>
	{
	};
}