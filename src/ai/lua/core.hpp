/* $Id: core.hpp 48153 2011-01-01 15:57:50Z mordante $ */
/*
   Copyright (C) 2010 - 2011 by Yurii Chernyi <terraninfo@terraninfo.net>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#ifndef AI_LUA_CORE_HPP
#define AI_LUA_CORE_HPP

struct lua_State;
class LuaKernel;
class config;

namespace ai {

class engine_lua;

/**
 * Proxy table for the AI context
 */
class lua_ai_context
{
private:
	lua_State *L;
	int num_;
	int side_;
	lua_ai_context(lua_State *l, int num, int side) : L(l), num_(num), side_(side)
	{
	}
	static lua_ai_context* create(lua_State *L, char const *code, engine_lua *engine);
public:
	~lua_ai_context();
	void load();
	static void init(lua_State *L);
	friend class ::LuaKernel;
};


/**
 * Proxy class for calling AI action handlers defined in Lua.
 */
class lua_ai_action_handler
{
private:
	lua_State *L;
	lua_ai_context &context_;
	int num_;
	lua_ai_action_handler(lua_State *l, lua_ai_context &context, int num) : L(l), context_(context),num_(num)
	{
	}
	static lua_ai_action_handler* create(lua_State *L, char const *code, lua_ai_context &context);
public:
	~lua_ai_action_handler();
	void handle(config &, bool configOut = false);
	friend class ::LuaKernel;
};

}//of namespace ai

#endif
