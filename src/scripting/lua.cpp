/* $Id: lua.cpp 48278 2011-01-15 10:19:16Z silene $ */
/*
   Copyright (C) 2009 - 2011 by Guillaume Melquiond <guillaume.melquiond@gmail.com>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

/**
 * @file
 * Provides a Lua interpreter.
 *
 * @note Naming conventions:
 *   - intf_ functions are exported in the wesnoth domain,
 *   - impl_ functions are hidden inside metatables,
 *   - cfun_ functions are closures,
 *   - luaW_ functions are helpers in Lua style.
 */

#include "lua/lualib.h"
#include "lua/lauxlib.h"

#include <cassert>
#include <cstring>
#include <boost/bind.hpp>
#include <boost/variant.hpp>

#include "scripting/lua.hpp"
#include "scripting/lua_api.hpp"

#include "actions.hpp"
#include "attack_prediction.hpp"
#include "filesystem.hpp"
#include "foreach.hpp"
#include "game_display.hpp"
#include "gamestatus.hpp"
#include "log.hpp"
#include "map.hpp"
#include "pathfind/pathfind.hpp"
#include "play_controller.hpp"
#include "replay.hpp"
#include "reports.hpp"
#include "resources.hpp"
#include "terrain_filter.hpp"
#include "terrain_translation.hpp"
#include "side_filter.hpp"
#include "sound.hpp"
#include "unit.hpp"
#include "ai/lua/core.hpp"
#ifdef GUI2_EXPERIMENTAL_LISTBOX
#include "gui/widgets/list.hpp"
#else
#include "gui/widgets/listbox.hpp"
#endif
#include "gui/widgets/multi_page.hpp"
#include "gui/widgets/settings.hpp"
#include "gui/widgets/text_box.hpp"
#include "gui/widgets/toggle_button.hpp"
#include "gui/widgets/window.hpp"

static lg::log_domain log_scripting_lua("scripting/lua");
#define LOG_LUA LOG_STREAM(info, log_scripting_lua)
#define ERR_LUA LOG_STREAM(err, log_scripting_lua)

static std::vector<config> preload_scripts;
static config preload_config;

void extract_preload_scripts(config const &game_config)
{
	preload_scripts.clear();
	foreach (config const &cfg, game_config.child_range("lua")) {
		preload_scripts.push_back(cfg);
	}
	preload_config = game_config.child("game_config");
}

/**
 * Stack storing the queued_event objects needed for calling WML actions.
 */
struct queued_event_context
{
	typedef game_events::queued_event qe;
	static qe default_qe;
	static qe const *current_qe;
	static qe const &get()
	{ return *(current_qe ? current_qe : &default_qe); }
	qe const *previous_qe;

	queued_event_context(qe const *new_qe)
		: previous_qe(current_qe)
	{
		current_qe = new_qe;
	}

	~queued_event_context()
	{ current_qe = previous_qe; }
};

game_events::queued_event const *queued_event_context::current_qe = 0;
game_events::queued_event queued_event_context::default_qe
	("_from_lua", map_location(), map_location(), config());

/* Dummy pointer for getting unique keys for Lua's registry. */
static char const dlgclbkKey = 0;
static char const executeKey = 0;
static char const getsideKey = 0;
static char const gettextKey = 0;
static char const gettypeKey = 0;
static char const getunitKey = 0;
static char const tstringKey = 0;
static char const unitvarKey = 0;
static char const ustatusKey = 0;
static char const vconfigKey = 0;

/**
 * Displays a message in the chat window.
 */
static void chat_message(std::string const &caption, std::string const &msg)
{
	resources::screen->add_chat_message(time(NULL), caption, 0, msg,
		events::chat_handler::MESSAGE_PUBLIC, false);
}

/**
 * Pushes a vconfig on the top of the stack.
 */
static void luaW_pushvconfig(lua_State *L, vconfig const &cfg)
{
	new(lua_newuserdata(L, sizeof(vconfig))) vconfig(cfg);
	lua_pushlightuserdata(L, (void *)&vconfigKey);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_setmetatable(L, -2);
}

/**
 * Pushes a t_string on the top of the stack.
 */
static void luaW_pushtstring(lua_State *L, t_string const &v)
{
	new(lua_newuserdata(L, sizeof(t_string))) t_string(v);
	lua_pushlightuserdata(L, (void *)&tstringKey);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_setmetatable(L, -2);
}

struct luaW_pushscalar_visitor : boost::static_visitor<>
{
	lua_State *L;
	luaW_pushscalar_visitor(lua_State *l): L(l) {}
	void operator()(boost::blank const &) const
	{ lua_pushnil(L); }
	void operator()(bool b) const
	{ lua_pushboolean(L, b); }
	void operator()(double d) const
	{ lua_pushnumber(L, d); }
	void operator()(std::string const &s) const
	{ lua_pushstring(L, s.c_str()); }
	void operator()(t_string const &s) const
	{ luaW_pushtstring(L, s); }
};

/**
 * Converts a string into a Lua object pushed at the top of the stack.
 */
void luaW_pushscalar(lua_State *L, config::attribute_value const &v)
{
	boost::apply_visitor(luaW_pushscalar_visitor(L), v.value);
}

/**
 * Returns true if the metatable of the object is the one found in the registry.
 */
static bool luaW_hasmetatable(lua_State *L
		, int index
		, char const &key)
{
	if (!lua_getmetatable(L, index))
		return false;
	lua_pushlightuserdata(L, (void *)&key);
	lua_rawget(L, LUA_REGISTRYINDEX);
	bool ok = lua_rawequal(L, -1, -2);
	lua_pop(L, 2);
	return ok;
}

/**
 * Converts a scalar to a translatable string.
 */
static bool luaW_totstring(lua_State *L, int index, t_string &str)
{
	switch (lua_type(L, index)) {
		case LUA_TBOOLEAN:
			str = lua_toboolean(L, index) ? "yes" : "no";
			break;
		case LUA_TNUMBER:
		case LUA_TSTRING:
			str = lua_tostring(L, index);
			break;
		case LUA_TUSERDATA:
		{
			if (!luaW_hasmetatable(L, index, tstringKey)) return false;
			str = *static_cast<t_string *>(lua_touserdata(L, index));
			break;
		}
		default:
			return false;
	}
	return true;
}

/**
 * Converts a scalar to a translatable string.
 */
static t_string luaW_checktstring(lua_State *L, int index)
{
	t_string result;
	if (!luaW_totstring(L, index, result))
		luaL_typerror(L, index, "translatable string");
	return result;
}

/**
 * Converts a config object to a Lua table.
 * The destination table should be at the top of the stack on entry. It is
 * still at the top on exit.
 */
static void luaW_filltable(lua_State *L, config const &cfg)
{
	if (!lua_checkstack(L, LUA_MINSTACK))
		return;

	int k = 1;
	foreach (const config::any_child &ch, cfg.all_children_range())
	{
		lua_createtable(L, 2, 0);
		lua_pushstring(L, ch.key.c_str());
		lua_rawseti(L, -2, 1);
		lua_newtable(L);
		luaW_filltable(L, ch.cfg);
		lua_rawseti(L, -2, 2);
		lua_rawseti(L, -2, k++);
	}
	foreach (const config::attribute &attr, cfg.attribute_range())
	{
		luaW_pushscalar(L, attr.second);
		lua_setfield(L, -2, attr.first.c_str());
	}
}

/**
 * Converts a config object to a Lua table pushed at the top of the stack.
 */
void luaW_pushconfig(lua_State *L, config const &cfg)
{
	lua_newtable(L);
	luaW_filltable(L, cfg);
}

#define return_misformed() \
  do { lua_settop(L, initial_top); return false; } while (0)

/**
 * Converts an optional table or vconfig to a config object.
 * @param tstring_meta absolute stack position of t_string's metatable, or 0 if none.
 * @return false if some attributes had not the proper type.
 * @note If the table has holes in the integer keys or floating-point keys,
 *       some keys will be ignored and the error will go undetected.
 */
bool luaW_toconfig(lua_State *L, int index, config &cfg, int tstring_meta)
{
	if (!lua_checkstack(L, LUA_MINSTACK))
		return false;

	// Get the absolute index of the table.
	int initial_top = lua_gettop(L);
	if (-initial_top <= index && index <= -1)
		index = initial_top + index + 1;

	switch (lua_type(L, index))
	{
		case LUA_TTABLE:
			break;
		case LUA_TUSERDATA:
		{
			if (!luaW_hasmetatable(L, index, vconfigKey))
				return false;
			cfg = static_cast<vconfig *>(lua_touserdata(L, index))->get_parsed_config();
			return true;
		}
		case LUA_TNONE:
		case LUA_TNIL:
			return true;
		default:
			return false;
	}

	// Get t_string's metatable, so that it can be used later to detect t_string object.
	if (!tstring_meta) {
		lua_pushlightuserdata(L, (void *)&tstringKey);
		lua_rawget(L, LUA_REGISTRYINDEX);
		tstring_meta = initial_top + 1;
	}

	// First convert the children (integer indices).
	for (int i = 1, i_end = lua_objlen(L, index); i <= i_end; ++i)
	{
		lua_rawgeti(L, index, i);
		if (!lua_istable(L, -1)) return_misformed();
		lua_rawgeti(L, -1, 1);
		char const *m = lua_tostring(L, -1);
		if (!m) return_misformed();
		lua_rawgeti(L, -2, 2);
		if (!luaW_toconfig(L, -1, cfg.add_child(m), tstring_meta))
			return_misformed();
		lua_pop(L, 3);
	}

	// Then convert the attributes (string indices).
	for (lua_pushnil(L); lua_next(L, index); lua_pop(L, 1))
	{
		if (lua_isnumber(L, -2)) continue;
		if (!lua_isstring(L, -2)) return_misformed();
		config::attribute_value &v = cfg[lua_tostring(L, -2)];
		switch (lua_type(L, -1)) {
			case LUA_TBOOLEAN:
				v = bool(lua_toboolean(L, -1));
				break;
			case LUA_TNUMBER:
				v = lua_tonumber(L, -1);
				break;
			case LUA_TSTRING:
				v = lua_tostring(L, -1);
				break;
			case LUA_TUSERDATA:
			{
				if (!lua_getmetatable(L, -1)) return_misformed();
				bool tstr = lua_rawequal(L, -1, tstring_meta) != 0;
				lua_pop(L, 1);
				if (!tstr) return_misformed();
				v = *static_cast<t_string *>(lua_touserdata(L, -1));
				break;
			}
			default:
				return_misformed();
		}
	}

	lua_settop(L, initial_top);
	return true;
}

#undef return_misformed

/**
 * Converts an optional table or vconfig to a config object.
 */
static config luaW_checkconfig(lua_State *L, int index)
{
	config result;
	if (!luaW_toconfig(L, index, result))
		luaL_typerror(L, index, "WML table");
	return result;
}

/**
 * Gets an optional vconfig from either a table or a userdata.
 * @return false in case of failure.
 */
static bool luaW_tovconfig(lua_State *L, int index, vconfig &vcfg)
{
	switch (lua_type(L, index))
	{
		case LUA_TTABLE:
		{
			config cfg;
			bool ok = luaW_toconfig(L, index, cfg);
			if (!ok) return false;
			vcfg = vconfig(cfg, true);
			break;
		}
		case LUA_TUSERDATA:
			if (!luaW_hasmetatable(L, index, vconfigKey))
				return false;
			vcfg = *static_cast<vconfig *>(lua_touserdata(L, index));
			break;
		case LUA_TNONE:
		case LUA_TNIL:
			break;
		default:
			return false;
	}
	return true;
}

/**
 * Gets an optional vconfig from either a table or a userdata.
 * @param allow_missing true if missing values are allowed; the function
 *        then returns an unconstructed vconfig.
 */
static vconfig luaW_checkvconfig(lua_State *L, int index, bool allow_missing = false)
{
	vconfig result = vconfig::unconstructed_vconfig();
	if (!luaW_tovconfig(L, index, result) || (!allow_missing && result.null()))
		luaL_typerror(L, index, "WML table");
	return result;
}

/**
 * Calls a Lua function stored below its @a nArgs arguments at the top of the stack.
 * @param nRets LUA_MULTRET for unbounded return values.
 * @return true if the call was successful and @a nRets return values are available.
 */
bool luaW_pcall(lua_State *L
		, int nArgs, int nRets, bool allow_wml_error)
{
	// Load the error handler before the function and its arguments.
	lua_pushlightuserdata(L, (void *)&executeKey);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_insert(L, -2 - nArgs);

	int error_handler_index = lua_gettop(L) - nArgs - 1;

	// Call the function.
	int res = lua_pcall(L, nArgs, nRets, -2 - nArgs);
	game::exception::rethrow();

	if (res)
	{
		char const *m = lua_tostring(L, -1);
		if (allow_wml_error && strncmp(m, "~wml:", 5) == 0) {
			m += 5;
			char const *e = strstr(m, "stack traceback");
			lg::wml_error << std::string(m, e ? e - m : strlen(m));
		} else if (allow_wml_error && strncmp(m, "~lua:", 5) == 0) {
			m += 5;
			char const *e = NULL, *em = m;
			while (em[0] && ((em = strstr(em + 1, "stack traceback"))))
				e = em;
			chat_message("Lua error", std::string(m, e ? e - m : strlen(m)));
		} else {
			ERR_LUA << m << '\n';
			chat_message("Lua error", m);
		}
		lua_pop(L, 2);
		return false;
	}

	// Remove the error handler.
	lua_remove(L, error_handler_index);

	return true;
}

/**
 * Pushes the value found by following the variadic names (char *), if the
 * value is not nil.
 * @return true if an element was pushed.
 */
#ifdef __GNUC__
__attribute__((sentinel))
#endif
static bool luaW_getglobal(lua_State *L, ...)
{
	lua_pushvalue(L, LUA_GLOBALSINDEX);

	va_list ap;
	va_start(ap, L);
	while (const char *s = va_arg(ap, const char *))
	{
		if (!lua_istable(L, -1)) goto discard;
		lua_pushstring(L, s);
		lua_rawget(L, -2);
		lua_remove(L, -2);
	}
	va_end(ap);

	if (lua_isnil(L, -1)) {
		discard:
		lua_pop(L, 1);
		return false;
	}
	return true;
}

lua_unit::~lua_unit()
{
	delete ptr;
}

unit *lua_unit::get()
{
	if (ptr) return ptr;
	if (side) {
		foreach (unit &u, (*resources::teams)[side - 1].recall_list()) {
			if (u.underlying_id() == uid) return &u;
		}
		return NULL;
	}
	unit_map::unit_iterator ui = resources::units->find(uid);
	if (!ui.valid()) return NULL;
	return &*ui;
}

/**
 * Converts a Lua value to a unit pointer.
 */
unit *luaW_tounit(lua_State *L, int index, bool only_on_map)
{
	if (!luaW_hasmetatable(L, index, getunitKey)) return NULL;
	lua_unit *lu = static_cast<lua_unit *>(lua_touserdata(L, index));
	if (only_on_map && !lu->on_map()) return NULL;
	return lu->get();
}

/**
 * Converts a Lua value to a unit pointer.
 */
static unit *luaW_checkunit(lua_State *L, int index, bool only_on_map = false)
{
	unit *u = luaW_tounit(L, index, only_on_map);
	if (!u) luaL_typerror(L, index, "unit");
	return u;
}

/**
 * Creates a t_string object (__call metamethod).
 * - Arg 1: userdata containing the domain.
 * - Arg 2: string to translate.
 * - Ret 1: string containing the translatable string.
 */
static int impl_gettext(lua_State *L)
{
	char const *m = luaL_checkstring(L, 2);
	char const *d = static_cast<char *>(lua_touserdata(L, 1));
	// Hidden metamethod, so d has to be a string. Use it to create a t_string.
	luaW_pushtstring(L, t_string(m, d));
	return 1;
}

/**
 * Creates an interface for gettext
 * - Arg 1: string containing the domain.
 * - Ret 1: a full userdata with __call pointing to lua_gettext.
 */
static int intf_textdomain(lua_State *L)
{
	size_t l;
	char const *m = luaL_checklstring(L, 1, &l);
	void *p = lua_newuserdata(L, l + 1);
	memcpy(p, m, l + 1);
	lua_pushlightuserdata(L, (void *)&gettextKey);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_setmetatable(L, -2);
	return 1;
}

/**
 * Converts a Lua value at position @a src and appends it to @a dst.
 * @note This function is private to lua_tstring_concat. It expects two things.
 *       First, the t_string metatable is at the top of the stack on entry. (It
 *       is still there on exit.) Second, the caller hasn't any valuable object
 *       with dynamic lifetime, since they would be leaked on error.
 */
static void tstring_concat_aux(lua_State *L, t_string &dst, int src)
{
	switch (lua_type(L, src)) {
		case LUA_TNUMBER:
		case LUA_TSTRING:
			dst += lua_tostring(L, src);
			break;
		case LUA_TUSERDATA:
			// Compare its metatable with t_string's metatable.
			if (!lua_getmetatable(L, src) || !lua_rawequal(L, -1, -2))
				luaL_typerror(L, src, "string");
			dst += *static_cast<t_string *>(lua_touserdata(L, src));
			lua_pop(L, 1);
			break;
		default:
			luaL_typerror(L, src, "string");
	}
}

/**
 * Appends a scalar to a t_string object (__concat metamethod).
 */
static int impl_tstring_concat(lua_State *L)
{
	// Create a new t_string.
	t_string *t = new(lua_newuserdata(L, sizeof(t_string))) t_string;

	lua_pushlightuserdata(L, (void *)&tstringKey);
	lua_rawget(L, LUA_REGISTRYINDEX);

	// Append both arguments to t.
	tstring_concat_aux(L, *t, 1);
	tstring_concat_aux(L, *t, 2);

	lua_setmetatable(L, -2);
	return 1;
}

/**
 * Destroys a t_string object before it is collected (__gc metamethod).
 */
static int impl_tstring_collect(lua_State *L)
{
	t_string *t = static_cast<t_string *>(lua_touserdata(L, 1));
	t->t_string::~t_string();
	return 0;
}

/**
 * Converts a t_string object to a string (__tostring metamethod);
 * that is, performs a translation.
 */
static int impl_tstring_tostring(lua_State *L)
{
	t_string *t = static_cast<t_string *>(lua_touserdata(L, 1));
	lua_pushstring(L, t->c_str());
	return 1;
}

/**
 * Gets the parsed field of a vconfig object (_index metamethod).
 * Special fields __literal, __shallow_literal, __parsed, and
 * __shallow_parsed, return Lua tables.
 */
static int impl_vconfig_get(lua_State *L)
{
	vconfig *v = static_cast<vconfig *>(lua_touserdata(L, 1));

	if (lua_isnumber(L, 2))
	{
		vconfig::all_children_iterator i = v->ordered_begin();
		unsigned len = std::distance(i, v->ordered_end());
		unsigned pos = lua_tointeger(L, 2) - 1;
		if (pos >= len) return 0;
		std::advance(i, pos);
		lua_createtable(L, 2, 0);
		lua_pushstring(L, i.get_key().c_str());
		lua_rawseti(L, -2, 1);
		new(lua_newuserdata(L, sizeof(vconfig))) vconfig(i.get_child());
		lua_pushlightuserdata(L, (void *)&vconfigKey);
		lua_rawget(L, LUA_REGISTRYINDEX);
		lua_setmetatable(L, -2);
		lua_rawseti(L, -2, 2);
		return 1;
	}

	char const *m = luaL_checkstring(L, 2);
	if (strcmp(m, "__literal") == 0) {
		luaW_pushconfig(L, v->get_config());
		return 1;
	}
	if (strcmp(m, "__parsed") == 0) {
		luaW_pushconfig(L, v->get_parsed_config());
		return 1;
	}

	bool shallow_literal = strcmp(m, "__shallow_literal") == 0;
	if (shallow_literal || strcmp(m, "__shallow_parsed") == 0)
	{
		lua_newtable(L);
		foreach (const config::attribute &a, v->get_config().attribute_range()) {
			if (shallow_literal)
				luaW_pushscalar(L, a.second);
			else
				luaW_pushscalar(L, v->expand(a.first));
			lua_setfield(L, -2, a.first.c_str());
		}
		vconfig::all_children_iterator i = v->ordered_begin(),
			i_end = v->ordered_end();
		if (shallow_literal) {
			i.disable_insertion();
			i_end.disable_insertion();
		}
		for (int j = 1; i != i_end; ++i, ++j)
		{
			lua_createtable(L, 2, 0);
			lua_pushstring(L, i.get_key().c_str());
			lua_rawseti(L, -2, 1);
			luaW_pushvconfig(L, i.get_child());
			lua_rawseti(L, -2, 2);
			lua_rawseti(L, -2, j);
		}
		return 1;
	}

	if (v->null() || !v->has_attribute(m)) return 0;
	luaW_pushscalar(L, (*v)[m]);
	return 1;
}

/**
 * Returns the number of a child of a vconfig object.
 */
static int impl_vconfig_size(lua_State *L)
{
	vconfig *v = static_cast<vconfig *>(lua_touserdata(L, 1));
	lua_pushinteger(L, v->null() ? 0 :
		std::distance(v->ordered_begin(), v->ordered_end()));
	return 1;
}

/**
 * Destroys a vconfig object before it is collected (__gc metamethod).
 */
static int impl_vconfig_collect(lua_State *L)
{
	vconfig *v = static_cast<vconfig *>(lua_touserdata(L, 1));
	v->vconfig::~vconfig();
	return 0;
}

#define return_tstring_attrib(name, accessor) \
	if (strcmp(m, name) == 0) { \
		luaW_pushtstring(L, accessor); \
		return 1; \
	}

#define return_cstring_attrib(name, accessor) \
	if (strcmp(m, name) == 0) { \
		lua_pushstring(L, accessor); \
		return 1; \
	}

#define return_string_attrib(name, accessor) \
	return_cstring_attrib(name, accessor.c_str())

#define return_int_attrib(name, accessor) \
	if (strcmp(m, name) == 0) { \
		lua_pushinteger(L, accessor); \
		return 1; \
	}

#define return_float_attrib(name, accessor) \
	if (strcmp(m, name) == 0) { \
		lua_pushnumber(L, accessor); \
		return 1; \
	}

#define return_bool_attrib(name, accessor) \
	if (strcmp(m, name) == 0) { \
		lua_pushboolean(L, accessor); \
		return 1; \
	}

#define return_cfg_attrib(name, accessor) \
	if (strcmp(m, name) == 0) { \
		config cfg; \
		accessor; \
		luaW_pushconfig(L, cfg); \
		return 1; \
	}

#define return_cfgref_attrib(name, accessor) \
	if (strcmp(m, name) == 0) { \
		luaW_pushconfig(L, accessor); \
		return 1; \
	}

#define modify_tstring_attrib(name, accessor) \
	if (strcmp(m, name) == 0) { \
		t_string value = luaW_checktstring(L, 3); \
		accessor; \
		return 0; \
	}

#define modify_string_attrib(name, accessor) \
	if (strcmp(m, name) == 0) { \
		const char *value = luaL_checkstring(L, 3); \
		accessor; \
		return 0; \
	}

#define modify_int_attrib(name, accessor) \
	if (strcmp(m, name) == 0) { \
		int value = luaL_checkinteger(L, 3); \
		accessor; \
		return 0; \
	}

#define modify_bool_attrib(name, accessor) \
	if (strcmp(m, name) == 0) { \
		bool value = lua_toboolean(L, 3); \
		accessor; \
		return 0; \
	}

/**
 * Gets some data on a unit type (__index metamethod).
 * - Arg 1: table containing an "id" field.
 * - Arg 2: string containing the name of the property.
 * - Ret 1: something containing the attribute.
 */
static int impl_unit_type_get(lua_State *L)
{
	char const *m = luaL_checkstring(L, 2);
	lua_pushstring(L, "id");
	lua_rawget(L, 1);
	const unit_type *utp = unit_types.find(lua_tostring(L, -1));
	if (!utp) return luaL_argerror(L, 1, "unknown unit type");
	unit_type const &ut = *utp;
	
	// --- ZOMBIES ---
	// Get attack value for max-damage attack
	int max_attack = 0;
	int curr_dmg;
	//std::cout << "Number of Attack Types: " ;
	//std::cout << ut.attacks().size() << std::endl;
	attack_type *curr_attack;
	for (int i = 0; i < ut.attacks().size(); i++) {
		curr_attack = &ut.attacks().at(i);
		curr_dmg = curr_attack->damage();
		if (curr_dmg > max_attack) max_attack = curr_dmg;
		//std::cout << "curr_dmg: " << curr_dmg << std::endl;
	}
	//std::cout << "max_attack : " << max_attack << std::endl;
	// ----------------

	// Find the corresponding attribute.
	return_tstring_attrib("name", ut.type_name());
	return_string_attrib("race", ut.race());
	return_int_attrib("max_hitpoints", ut.hitpoints());
	return_int_attrib("max_moves", ut.movement());
	return_int_attrib("max_experience", ut.experience_needed());
	return_int_attrib("max_attack", max_attack); // set to value found above
	return_int_attrib("cost", ut.cost());
	return_int_attrib("level", ut.level());
	return_cfgref_attrib("__cfg", ut.get_cfg());
	return 0;
}

/**
 * Destroys a unit object before it is collected (__gc metamethod).
 */
static int impl_unit_collect(lua_State *L)
{
	lua_unit *u = static_cast<lua_unit *>(lua_touserdata(L, 1));
	u->lua_unit::~lua_unit();
	return 0;
}

/**
 * Gets some data on a unit (__index metamethod).
 * - Arg 1: full userdata containing the unit id.
 * - Arg 2: string containing the name of the property.
 * - Ret 1: something containing the attribute.
 */
static int impl_unit_get(lua_State *L)
{
	lua_unit *lu = static_cast<lua_unit *>(lua_touserdata(L, 1));
	char const *m = luaL_checkstring(L, 2);
	unit const *pu = lu->get();

	if (strcmp(m, "valid") == 0)
	{
		if (!pu) return 0;
		if (lu->on_map())
			lua_pushstring(L, "map");
		else if (lu->on_recall_list())
			lua_pushstring(L, "recall");
		else
			lua_pushstring(L, "private");
		return 1;
	}

	if (!pu) return luaL_argerror(L, 1, "unknown unit");
	unit const &u = *pu;

	// Find the corresponding attribute.
	return_int_attrib("x", u.get_location().x + 1);
	return_int_attrib("y", u.get_location().y + 1);
	if (strcmp(m, "loc") == 0) {
		lua_pushinteger(L, u.get_location().x + 1);
		lua_pushinteger(L, u.get_location().y + 1);
		return 2;
	}
	return_int_attrib("side", u.side());
	return_string_attrib("id", u.id());
	return_string_attrib("type", u.type_id());
	return_int_attrib("hitpoints", u.hitpoints());
	return_int_attrib("max_hitpoints", u.max_hitpoints());
	return_int_attrib("experience", u.experience());
	return_int_attrib("max_experience", u.max_experience());
	return_int_attrib("moves", u.movement_left());
	return_int_attrib("max_moves", u.total_movement());
	return_tstring_attrib("name", u.name());
	return_bool_attrib("canrecruit", u.can_recruit());
	if (strcmp(m, "status") == 0) {
		lua_createtable(L, 1, 0);
		lua_pushvalue(L, 1);
		lua_rawseti(L, -2, 1);
		lua_pushlightuserdata(L, (void *)&ustatusKey);
		lua_rawget(L, LUA_REGISTRYINDEX);
		lua_setmetatable(L, -2);
		return 1;
	}
	if (strcmp(m, "variables") == 0) {
		lua_createtable(L, 1, 0);
		lua_pushvalue(L, 1);
		lua_rawseti(L, -2, 1);
		lua_pushlightuserdata(L, (void *)&unitvarKey);
		lua_rawget(L, LUA_REGISTRYINDEX);
		lua_setmetatable(L, -2);
		return 1;
	}
	return_bool_attrib("hidden", u.get_hidden());
	return_bool_attrib("petrified", u.incapacitated());
	return_bool_attrib("resting", u.resting());
	return_string_attrib("role", u.get_role());
	return_string_attrib("facing", map_location::write_direction(u.facing()));
	return_cfg_attrib("__cfg", u.write(cfg); u.get_location().write(cfg));
	return 0;
}

/**
 * Sets some data on a unit (__newindex metamethod).
 * - Arg 1: full userdata containing the unit id.
 * - Arg 2: string containing the name of the property.
 * - Arg 3: something containing the attribute.
 */
static int impl_unit_set(lua_State *L)
{
	lua_unit *lu = static_cast<lua_unit *>(lua_touserdata(L, 1));
	char const *m = luaL_checkstring(L, 2);
	unit *pu = lu->get();
	if (!pu) return luaL_argerror(L, 1, "unknown unit");
	unit &u = *pu;

	// Find the corresponding attribute.
	modify_int_attrib("side", u.set_side(value));
	modify_int_attrib("moves", u.set_movement(value));
	modify_int_attrib("hitpoints", u.set_hitpoints(value));
	modify_int_attrib("experience", u.set_experience(value));
	modify_bool_attrib("resting", u.set_resting(value));
	modify_tstring_attrib("name", u.set_name(value));
	modify_string_attrib("role", u.set_role(value));
	modify_string_attrib("facing", u.set_facing(map_location::parse_direction(value)));
	modify_bool_attrib("hidden", u.set_hidden(value));
	if (!lu->on_map()) {
		map_location loc = u.get_location();
		modify_int_attrib("x", loc.x = value - 1; u.set_location(loc));
		modify_int_attrib("y", loc.y = value - 1; u.set_location(loc));
	}

	return luaL_argerror(L, 2, "unknown modifiable property");
}

/**
 * Gets the status of a unit (__index metamethod).
 * - Arg 1: table containing the userdata containing the unit id.
 * - Arg 2: string containing the name of the status.
 * - Ret 1: boolean.
 */
static int impl_unit_status_get(lua_State *L)
{
	if (!lua_istable(L, 1))
		return luaL_typerror(L, 1, "unit status");
	lua_rawgeti(L, 1, 1);
	unit const *u = luaW_tounit(L, -1);
	if (!u) return luaL_argerror(L, 1, "unknown unit");
	char const *m = luaL_checkstring(L, 2);
	lua_pushboolean(L, u->get_state(m));
	return 1;
}

/**
 * Sets the status of a unit (__newindex metamethod).
 * - Arg 1: table containing the userdata containing the unit id.
 * - Arg 2: string containing the name of the status.
 * - Arg 3: boolean.
 */
static int impl_unit_status_set(lua_State *L)
{
	if (!lua_istable(L, 1))
		return luaL_typerror(L, 1, "unit status");
	lua_rawgeti(L, 1, 1);
	unit *u = luaW_tounit(L, -1);
	if (!u) return luaL_argerror(L, 1, "unknown unit");
	char const *m = luaL_checkstring(L, 2);
	u->set_state(m, lua_toboolean(L, 3));
	return 0;
}

/**
 * Gets the variable of a unit (__index metamethod).
 * - Arg 1: table containing the userdata containing the unit id.
 * - Arg 2: string containing the name of the status.
 * - Ret 1: boolean.
 */
static int impl_unit_variables_get(lua_State *L)
{
	if (!lua_istable(L, 1))
		return luaL_typerror(L, 1, "unit variables");
	lua_rawgeti(L, 1, 1);
	unit const *u = luaW_tounit(L, -1);
	if (!u) return luaL_argerror(L, 1, "unknown unit");
	char const *m = luaL_checkstring(L, 2);
	return_cfgref_attrib("__cfg", u->variables());
	luaW_pushscalar(L, u->variables()[m]);
	return 1;
}

/**
 * Sets the variable of a unit (__newindex metamethod).
 * - Arg 1: table containing the userdata containing the unit id.
 * - Arg 2: string containing the name of the status.
 * - Arg 3: scalar.
 */
static int impl_unit_variables_set(lua_State *L)
{
	if (!lua_istable(L, 1))
		return luaL_typerror(L, 1, "unit variables");
	lua_rawgeti(L, 1, 1);
	unit *u = luaW_tounit(L, -1);
	if (!u) return luaL_argerror(L, 1, "unknown unit");
	char const *m = luaL_checkstring(L, 2);
	if (strcmp(m, "__cfg") == 0) {
		u->variables() = luaW_checkconfig(L, 3);
		return 0;
	}
	config::attribute_value &v = u->variables()[m];
	switch (lua_type(L, 3)) {
		case LUA_TNIL:
			u->variables().remove_attribute(m);
			break;
		case LUA_TBOOLEAN:
			v = bool(lua_toboolean(L, 3));
			break;
		case LUA_TNUMBER:
			v = lua_tonumber(L, 3);
			break;
		case LUA_TSTRING:
			v = lua_tostring(L, 3);
			break;
		case LUA_TUSERDATA:
			if (luaW_hasmetatable(L, 3, tstringKey)) {
				v = *static_cast<t_string *>(lua_touserdata(L, 3));
				break;
			}
			// no break
		default:
			return luaL_typerror(L, 3, "WML scalar");
	}
	return 0;
}

/**
 * Gets the unit at the given location or with the given id.
 * - Arg 1: integer.
 * - Args 1, 2: location.
 * - Ret 1: full userdata with __index pointing to impl_unit_get and
 *          __newindex pointing to impl_unit_set.
 */
static int intf_get_unit(lua_State *L)
{
	int x = luaL_checkinteger(L, 1) - 1;
	int y = luaL_optint(L, 2, 0) - 1;

	unit_map &units = *resources::units;
	unit_map::const_iterator ui;

	if (lua_isnoneornil(L, 2)) {
		ui = units.find(x + 1);
	} else {
		ui = units.find(map_location(x, y));
	}

	if (!ui.valid()) return 0;

	new(lua_newuserdata(L, sizeof(lua_unit))) lua_unit(ui->underlying_id());
	lua_pushlightuserdata(L, (void *)&getunitKey);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_setmetatable(L, -2);
	return 1;
}

/**
 * Gets the unit displayed in the sidebar.
 * - Ret 1: full userdata with __index pointing to impl_unit_get and
 *          __newindex pointing to impl_unit_set.
 */
static int intf_get_displayed_unit(lua_State *L)
{
	unit_map::const_iterator ui = find_visible_unit(
		resources::screen->displayed_unit_hex(),
		(*resources::teams)[resources::screen->viewing_team()],
		resources::screen->show_everything());
	if (!ui.valid()) return 0;

	new(lua_newuserdata(L, sizeof(lua_unit))) lua_unit(ui->underlying_id());
	lua_pushlightuserdata(L, (void *)&getunitKey);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_setmetatable(L, -2);
	return 1;
}

/**
 * Gets all the units matching a given filter.
 * - Arg 1: optional table containing a filter
 * - Ret 1: table containing full userdata with __index pointing to
 *          impl_unit_get and __newindex pointing to impl_unit_set.
 */
static int intf_get_units(lua_State *L)
{
	vconfig filter = luaW_checkvconfig(L, 1, true);

	// Go through all the units while keeping the following stack:
	// 1: metatable, 2: return table, 3: userdata, 4: metatable copy
	lua_settop(L, 0);
	lua_pushlightuserdata(L, (void *)&getunitKey);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_newtable(L);
	int i = 1;
	unit_map &units = *resources::units;
	for (unit_map::const_unit_iterator ui = units.begin(), ui_end = units.end();
	     ui != ui_end; ++ui)
	{
		if (!filter.null() && !ui->matches_filter(filter, ui->get_location()))
			continue;
		new(lua_newuserdata(L, sizeof(lua_unit))) lua_unit(ui->underlying_id());
		lua_pushvalue(L, 1);
		lua_setmetatable(L, 3);
		lua_rawseti(L, 2, i);
		++i;
	}
	return 1;
}

/**
 * Matches a unit against the given filter.
 * - Arg 1: full userdata.
 * - Arg 2: table containing a filter
 * - Ret 1: boolean.
 */
static int intf_match_unit(lua_State *L)
{
	if (!luaW_hasmetatable(L, 1, getunitKey))
		return luaL_typerror(L, 1, "unit");

	lua_unit *lu = static_cast<lua_unit *>(lua_touserdata(L, 1));
	unit *u = lu->get();
	if (!u) return luaL_argerror(L, 1, "unit not found");

	vconfig filter = luaW_checkvconfig(L, 2, true);

	if (filter.null()) {
		lua_pushboolean(L, true);
		return 1;
	}

	if (int side = lu->on_recall_list()) {
		team &t = (*resources::teams)[side - 1];
		scoped_recall_unit auto_store("this_unit",
			t.save_id(), u - &t.recall_list()[0]);
		lua_pushboolean(L, u->matches_filter(filter, map_location()));
		return 1;
	}

	lua_pushboolean(L, u->matches_filter(filter, u->get_location()));
	return 1;
}

/**
 * Gets the numeric ids of all the units matching a given filter on the recall lists.
 * - Arg 1: optional table containing a filter
 * - Ret 1: table containing full userdata with __index pointing to
 *          impl_unit_get and __newindex pointing to impl_unit_set.
 */
static int intf_get_recall_units(lua_State *L)
{
	vconfig filter = luaW_checkvconfig(L, 1, true);

	// Go through all the units while keeping the following stack:
	// 1: metatable, 2: return table, 3: userdata, 4: metatable copy
	lua_settop(L, 0);
	lua_pushlightuserdata(L, (void *)&getunitKey);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_newtable(L);
	int i = 1, s = 1;
	foreach (team &t, *resources::teams)
	{
		foreach (unit &u, t.recall_list())
		{
			if (!filter.null()) {
				scoped_recall_unit auto_store("this_unit",
					t.save_id(), &u - &t.recall_list()[0]);
				if (!u.matches_filter(filter, map_location()))
					continue;
			}
			new(lua_newuserdata(L, sizeof(lua_unit))) lua_unit(s, u.underlying_id());
			lua_pushvalue(L, 1);
			lua_setmetatable(L, 3);
			lua_rawseti(L, 2, i);
			++i;
		}
		++s;
	}
	return 1;
}

/**
 * Fires an event.
 * - Arg 1: string containing the event name.
 * - Arg 2,3: optional first location.
 * - Arg 4,5: optional second location.
 * - Arg 6: optional WML table used as the [weapon] tag.
 * - Arg 7: optional WML table used as the [second_weapon] tag.
 * - Ret 1: boolean indicating whether the event was processed or not.
 */
static int intf_fire_event(lua_State *L)
{
	char const *m = luaL_checkstring(L, 1);

	int pos = 2;
	map_location l1, l2;
	config data;

	if (lua_isnumber(L, 2)) {
		l1.x = lua_tointeger(L, 2) - 1;
		l1.y = luaL_checkinteger(L, 3) - 1;
		if (lua_isnumber(L, 4)) {
			l2.x = lua_tointeger(L, 4) - 1;
			l2.y = luaL_checkinteger(L, 5) - 1;
			pos = 6;
		} else pos = 4;
	}

	if (!lua_isnoneornil(L, pos)) {
		data.add_child("first", luaW_checkconfig(L, pos));
	}
	++pos;
	if (!lua_isnoneornil(L, pos)) {
		data.add_child("second", luaW_checkconfig(L, pos));
	}

	bool b = game_events::fire(m, l1, l2, data);
	lua_pushboolean(L, b);
	return 1;
}

/**
 * Gets a WML variable.
 * - Arg 1: string containing the variable name.
 * - Arg 2: optional bool indicating if tables for containers should be left empty.
 * - Ret 1: value of the variable, if any.
 */
static int intf_get_variable(lua_State *L)
{
	char const *m = luaL_checkstring(L, 1);
	variable_info v(m, false, variable_info::TYPE_SCALAR);
	if (v.is_valid) {
		luaW_pushscalar(L, v.as_scalar());
		return 1;
	} else {
		variable_info w(m, false, variable_info::TYPE_CONTAINER);
		if (w.is_valid) {
			lua_newtable(L);
			if (lua_toboolean(L, 2))
				luaW_filltable(L, w.as_container());
			return 1;
		}
	}
	return 0;
}

/**
 * Sets a WML variable.
 * - Arg 1: string containing the variable name.
 * - Arg 2: boolean/integer/string/table containing the value.
 */
static int intf_set_variable(lua_State *L)
{
	char const *m = luaL_checkstring(L, 1);
	if (lua_isnoneornil(L, 2)) {
		resources::state_of_game->clear_variable(m);
		return 0;
	}

	variable_info v(m);
	switch (lua_type(L, 2)) {
		case LUA_TBOOLEAN:
			v.as_scalar() = bool(lua_toboolean(L, 2));
			break;
		case LUA_TNUMBER:
			v.as_scalar() = lua_tonumber(L, 2);
			break;
		case LUA_TSTRING:
			v.as_scalar() = lua_tostring(L, 2);
			break;
		case LUA_TUSERDATA:
			if (luaW_hasmetatable(L, 2, tstringKey)) {
				v.as_scalar() = *static_cast<t_string *>(lua_touserdata(L, 2));
				break;
			}
			// no break
		case LUA_TTABLE:
		{
			config &cfg = v.as_container();
			cfg.clear();
			if (luaW_toconfig(L, 2, cfg))
				break;
			// no break
		}
		default:
			return luaL_typerror(L, 2, "WML table or scalar");
	}
	return 0;
}

/**
 * Loads and executes a Lua file.
 * - Arg 1: string containing the file name.
 * - Ret *: values returned by executing the file body.
 */
static int intf_dofile(lua_State *L)
{
	char const *m = luaL_checkstring(L, 1);
	std::string p = get_wml_location(m);
	if (p.empty())
		return luaL_argerror(L, 1, "file not found");

	lua_settop(L, 0);
	if (luaL_loadfile(L, p.c_str()))
		return lua_error(L);

	lua_call(L, 0, LUA_MULTRET);
	return lua_gettop(L);
}

/**
 * Loads and executes a Lua file, if there is no corresponding entry in wesnoth.package.
 * Stores the result of the script in wesnoth.package and returns it.
 * - Arg 1: string containing the file name.
 * - Ret 1: value returned by the script.
 */
static int intf_require(lua_State *L)
{
	char const *m = luaL_checkstring(L, 1);

	// Check if there is already an entry.
	lua_pushstring(L, "wesnoth");
	lua_rawget(L, LUA_GLOBALSINDEX);
	lua_pushstring(L, "package");
	lua_rawget(L, -2);
	lua_pushvalue(L, 1);
	lua_rawget(L, -2);
	if (!lua_isnil(L, -1)) return 1;
	lua_pop(L, 1);

	std::string p = get_wml_location(m);
	if (p.empty())
		return luaL_argerror(L, 1, "file not found");

	// Compile the file.
	int res = luaL_loadfile(L, p.c_str());
	if (res)
	{
		char const *m = lua_tostring(L, -1);
		chat_message("Lua error", m);
		ERR_LUA << m << '\n';
		return 0;
	}

	// Execute it.
	if (!luaW_pcall(L, 0, 1)) return 0;

	// Add the return value to the table.
	lua_pushvalue(L, 1);
	lua_pushvalue(L, -2);
	lua_settable(L, -4);
	return 1;
}

/**
 * Highlights the given location on the map.
 * - Args 1,2: location.
 */
static int intf_highlight_hex(lua_State *L)
{
	int x = luaL_checkinteger(L, 1) - 1;
	int y = luaL_checkinteger(L, 2) - 1;
	const map_location loc(x, y);
	resources::screen->highlight_hex(loc);
	resources::screen->display_unit_hex(loc);

	unit_map::const_unit_iterator i = resources::units->find(loc);
	if(i != resources::units->end()) {
		resources::screen->highlight_reach(pathfind::paths(
			*resources::game_map, *resources::units, loc, *resources::teams, false,
			(*i).get_ability_bool("teleport"), resources::teams->front()));
	}

	return 0;
}

/**
 * Returns whether the first side is an enemy of the second one.
 * - Args 1,2: side numbers.
 * - Ret 1: boolean.
 */
static int intf_is_enemy(lua_State *L)
{
	unsigned side_1 = luaL_checkint(L, 1) - 1;
	unsigned side_2 = luaL_checkint(L, 2) - 1;
	std::vector<team> &teams = *resources::teams;
	if (side_1 >= teams.size() || side_2 >= teams.size()) return 0;
	lua_pushboolean(L, teams[side_1].is_enemy(side_2 + 1));
	return 1;
}

/**
 * Gets some data on a side (__index metamethod).
 * - Arg 1: full userdata containing the team.
 * - Arg 2: string containing the name of the property.
 * - Ret 1: something containing the attribute.
 */
static int impl_side_get(lua_State *L)
{
	// Hidden metamethod, so arg1 has to be a pointer to a team.
	team &t = **static_cast<team **>(lua_touserdata(L, 1));
	char const *m = luaL_checkstring(L, 2);

	// Find the corresponding attribute.
	return_int_attrib("gold", t.gold());
	return_tstring_attrib("objectives", t.objectives());
	return_int_attrib("village_gold", t.village_gold());
	return_int_attrib("recall_cost", t.recall_cost());
	return_int_attrib("base_income", t.base_income());
	return_int_attrib("total_income", t.total_income());
	return_bool_attrib("objectives_changed", t.objectives_changed());
	return_tstring_attrib("user_team_name", t.user_team_name());
	return_string_attrib("team_name", t.team_name());
	return_cstring_attrib("controller", t.controller_string());

	if (strcmp(m, "recruit") == 0) {
		std::set<std::string> const &recruits = t.recruits();
		lua_createtable(L, recruits.size(), 0);
		int i = 1;
		foreach (std::string const &r, t.recruits()) {
			lua_pushstring(L, r.c_str());
			lua_rawseti(L, -2, i++);
		}
		return 1;
	}

	return_cfg_attrib("__cfg", t.write(cfg));
	return 0;
}

/**
 * Sets some data on a side (__newindex metamethod).
 * - Arg 1: full userdata containing the team.
 * - Arg 2: string containing the name of the property.
 * - Arg 3: something containing the attribute.
 */
static int impl_side_set(lua_State *L)
{
	// Hidden metamethod, so arg1 has to be a pointer to a team.
	team &t = **static_cast<team **>(lua_touserdata(L, 1));
	char const *m = luaL_checkstring(L, 2);

	// Find the corresponding attribute.
	modify_int_attrib("gold", t.set_gold(value));
	modify_tstring_attrib("objectives", t.set_objectives(value, true));
	modify_int_attrib("village_gold", t.set_village_gold(value));
	modify_int_attrib("recall_cost", t.set_recall_cost(value));
	modify_int_attrib("base_income", t.set_base_income(value));
	modify_bool_attrib("objectives_changed", t.set_objectives_changed(value));
	modify_tstring_attrib("user_team_name", t.change_team(t.team_name(), value));
	modify_string_attrib("team_name", t.change_team(value, t.user_team_name()));
	modify_string_attrib("controller", t.change_controller(value));

	if (strcmp(m, "recruit") == 0) {
		t.set_recruits(std::set<std::string>());
		if (!lua_istable(L, 3)) return 0;
		for (int i = 1;; ++i) {
			lua_rawgeti(L, 3, i);
			if (lua_isnil(L, -1)) break;
			t.add_recruit(lua_tostring(L, -1));
			lua_pop(L, 1);
		}
		return 0;
	}

	return luaL_argerror(L, 2, "unknown modifiable property");
}

/**
 * Gets a terrain code.
 * - Args 1,2: map location.
 * - Ret 1: string.
 */
static int intf_get_terrain(lua_State *L)
{
	int x = luaL_checkint(L, 1);
	int y = luaL_checkint(L, 2);

	t_translation::t_terrain const &t = resources::game_map->
		get_terrain(map_location(x - 1, y - 1));
	lua_pushstring(L, t_translation::write_terrain_code(t).c_str());
	return 1;
}

/**
 * Sets a terrain code.
 * - Args 1,2: map location.
 * - Arg 3: terrain code string.
 * - Arg 4: layer: (overlay|base|both, default=both)
 * - Arg 5: replace_if_failed, default = no
 */
static int intf_set_terrain(lua_State *L)
{
	int x = luaL_checkint(L, 1);
	int y = luaL_checkint(L, 2);
	t_translation::t_terrain terrain = t_translation::read_terrain_code(luaL_checkstring(L, 3));
	if (terrain == t_translation::NONE_TERRAIN) return 0;

	gamemap::tmerge_mode mode = gamemap::BOTH;
	bool replace_if_failed = false;
	if (!lua_isnone(L, 4)) {
		if (!lua_isnil(L, 4)) {
			const char* const layer = luaL_checkstring(L, 4);
			if (strcmp(layer, "base") == 0) mode = gamemap::BASE;
			else if (strcmp(layer, "overlay") == 0) mode = gamemap::OVERLAY;
		}

		if(!lua_isnoneornil(L, 5)) {
			replace_if_failed = lua_toboolean(L, 5);
		}
	}

	change_terrain(map_location(x - 1, y - 1), terrain, mode, replace_if_failed);
	return 0;
}

/**
 * Gets details about a terrain.
 * - Arg 1: terrain code string.
 * - Ret 1: table.
 */
static int intf_get_terrain_info(lua_State *L)
{
	char const *m = luaL_checkstring(L, 1);
	t_translation::t_terrain t = t_translation::read_terrain_code(m);
	if (t == t_translation::NONE_TERRAIN) return 0;
	terrain_type const &info = resources::game_map->get_terrain_info(t);

	lua_newtable(L);
	lua_pushstring(L, info.id().c_str());
	lua_setfield(L, -2, "id");
	luaW_pushtstring(L, info.name());
	lua_setfield(L, -2, "name");
	luaW_pushtstring(L, info.description());
	lua_setfield(L, -2, "description");
	lua_pushboolean(L, info.is_village());
	lua_setfield(L, -2, "village");
	lua_pushboolean(L, info.is_castle());
	lua_setfield(L, -2, "castle");
	lua_pushboolean(L, info.is_keep());
	lua_setfield(L, -2, "keep");
	lua_pushinteger(L, info.gives_healing());
	lua_setfield(L, -2, "healing");

	return 1;
}

/**
 * Gets the side of a village owner.
 * - Args 1,2: map location.
 * - Ret 1: integer.
 */
static int intf_get_village_owner(lua_State *L)
{
	int x = luaL_checkint(L, 1);
	int y = luaL_checkint(L, 2);

	map_location loc(x - 1, y - 1);
	if (!resources::game_map->is_village(loc))
		return 0;

	int side = village_owner(loc, *resources::teams) + 1;
	if (!side) return 0;
	lua_pushinteger(L, side);
	return 1;
}

/**
 * Sets the owner of a village.
 * - Args 1,2: map location.
 * - Arg 3: integer for the side or empty to remove ownership.
 */
static int intf_set_village_owner(lua_State *L)
{
	int x = luaL_checkint(L, 1);
	int y = luaL_checkint(L, 2);
	int new_side = lua_isnoneornil(L, 3) ? 0 : luaL_checkint(L, 3);

	std::vector<team> &teams = *resources::teams;
	map_location loc(x - 1, y - 1);
	if (!resources::game_map->is_village(loc))
		return 0;

	int old_side = village_owner(loc, teams) + 1;
	if (new_side == old_side || new_side < 0 || new_side > (int)teams.size()
		|| (new_side && !resources::units->find_leader(new_side).valid()))
		return 0;

	if (old_side) teams[old_side - 1].lose_village(loc);
	if (new_side) teams[new_side - 1].get_village(loc);
	return 0;
}

/**
 * Returns the map size.
 * - Ret 1: width.
 * - Ret 2: height.
 * - Ret 3: border size.
 */
static int intf_get_map_size(lua_State *L)
{
	const gamemap &map = *resources::game_map;
	lua_pushinteger(L, map.w());
	lua_pushinteger(L, map.h());
	lua_pushinteger(L, map.border_size());
	return 3;
}

/**
 * Returns the currently overed tile.
 * - Ret 1: x.
 * - Ret 2: y.
 */
static int intf_get_mouseover_tile(lua_State *L)
{
	const map_location &loc = resources::screen->mouseover_hex();
	if (!resources::game_map->on_board(loc)) return 0;
	lua_pushinteger(L, loc.x + 1);
	lua_pushinteger(L, loc.y + 1);
	return 2;
}

/**
 * Returns the currently selected tile.
 * - Ret 1: x.
 * - Ret 2: y.
 */
static int intf_get_selected_tile(lua_State *L)
{
	const map_location &loc = resources::screen->selected_hex();
	if (!resources::game_map->on_board(loc)) return 0;
	lua_pushinteger(L, loc.x + 1);
	lua_pushinteger(L, loc.y + 1);
	return 2;
}

/**
 * Gets some game_config data (__index metamethod).
 * - Arg 1: userdata (ignored).
 * - Arg 2: string containing the name of the property.
 * - Ret 1: something containing the attribute.
 */
static int impl_game_config_get(lua_State *L)
{
	char const *m = luaL_checkstring(L, 2);

	// Find the corresponding attribute.
	return_int_attrib("base_income", game_config::base_income);
	return_int_attrib("village_income", game_config::village_income);
	return_int_attrib("poison_amount", game_config::poison_amount);
	return_int_attrib("rest_heal_amount", game_config::rest_heal_amount);
	return_int_attrib("recall_cost", game_config::recall_cost);
	return_int_attrib("kill_experience", game_config::kill_experience);
	return_int_attrib("last_turn", resources::tod_manager->number_of_turns());
	return_string_attrib("version", game_config::version);
	return_bool_attrib("debug", game_config::debug);
	return 0;
}

/**
 * Sets some game_config data (__newindex metamethod).
 * - Arg 1: userdata (ignored).
 * - Arg 2: string containing the name of the property.
 * - Arg 3: something containing the attribute.
 */
static int impl_game_config_set(lua_State *L)
{
	char const *m = luaL_checkstring(L, 2);

	// Find the corresponding attribute.
	modify_int_attrib("base_income", game_config::base_income = value);
	modify_int_attrib("village_income", game_config::village_income = value);
	modify_int_attrib("poison_amount", game_config::poison_amount = value);
	modify_int_attrib("rest_heal_amount", game_config::rest_heal_amount = value);
	modify_int_attrib("recall_cost", game_config::recall_cost = value);
	modify_int_attrib("kill_experience", game_config::kill_experience = value);
	modify_int_attrib("last_turn", resources::tod_manager->set_number_of_turns(value));
	return luaL_argerror(L, 2, "unknown modifiable property");
}

/**
 * Gets some data about current point of game (__index metamethod).
 * - Arg 1: userdata (ignored).
 * - Arg 2: string containing the name of the property.
 * - Ret 1: something containing the attribute.
 */
static int impl_current_get(lua_State *L)
{
	char const *m = luaL_checkstring(L, 2);

	// Find the corresponding attribute.
	return_int_attrib("side", resources::controller->current_side());
	return_int_attrib("turn", resources::controller->turn());

	if (strcmp(m, "event_context") == 0)
	{
		const game_events::queued_event &ev = queued_event_context::get();
		config cfg;
		cfg["name"] = ev.name;
		if (const config &weapon = ev.data.child("first")) {
			cfg.add_child("weapon", weapon);
		}
		if (const config &weapon = ev.data.child("second")) {
			cfg.add_child("second_weapon", weapon);
		}
		if (ev.loc1.valid()) {
			cfg["x1"] = ev.loc1.x + 1;
			cfg["y1"] = ev.loc1.y + 1;
		}
		if (ev.loc2.valid()) {
			cfg["x2"] = ev.loc2.x + 1;
			cfg["y2"] = ev.loc2.y + 1;
		}
		luaW_pushconfig(L, cfg);
		return 1;
	}

	return 0;
}

/**
 * Displays a message in the chat window and in the logs.
 * - Arg 1: optional message header.
 * - Arg 2 (or 1): message.
 */
static int intf_message(lua_State *L)
{
	char const *m = luaL_checkstring(L, 1);
	char const *h = m;
	if (lua_isnone(L, 2)) {
		h = "Lua";
	} else {
		m = luaL_checkstring(L, 2);
	}
	chat_message(h, m);
	LOG_LUA << "Script says: \"" << m << "\"\n";
	return 0;
}

/**
 * Removes all messages from the chat window.
 */
static int intf_clear_messages(lua_State*)
{
	resources::screen->clear_chat_messages();
	return 0;
}

/**
 * Evaluates a boolean WML conditional.
 * - Arg 1: WML table.
 * - Ret 1: boolean.
 */
static int intf_eval_conditional(lua_State *L)
{
	vconfig cond = luaW_checkvconfig(L, 1);
	bool b = game_events::conditional_passed(cond);
	lua_pushboolean(L, b);
	return 1;
}

/**
 * Cost function object relying on a Lua function.
 * @note The stack index of the Lua function must be valid each time the cost is computed.
 */
struct lua_calculator : pathfind::cost_calculator
{
	lua_State *L;
	int index;

	lua_calculator(lua_State *L_, int i): L(L_), index(i) {}
	double cost(const map_location &loc, double so_far) const;
};

double lua_calculator::cost(const map_location &loc, double so_far) const
{
	// Copy the user function and push the location and current cost.
	lua_pushvalue(L, index);
	lua_pushinteger(L, loc.x + 1);
	lua_pushinteger(L, loc.y + 1);
	lua_pushnumber(L, so_far);

	// Execute the user function.
	if (!luaW_pcall(L, 3, 1)) return 1.;

	// Return a cost of at least 1 mp to avoid issues in pathfinder.
	// (Condition is inverted to detect NaNs.)
	double cost = lua_tonumber(L, -1);
	lua_pop(L, 1);
	return !(cost >= 1.) ? 1. : cost;
}

/**
 * Finds a path between two locations.
 * - Args 1,2: source location. (Or Arg 1: unit.)
 * - Args 3,4: destination.
 * - Arg 5: optional cost function or
 *          table (optional fields: ignore_units, ignore_teleport, max_cost, viewing_side).
 * - Ret 1: array of pairs containing path steps.
 * - Ret 2: path cost.
 */
static int intf_find_path(lua_State *L)
{
	int arg = 1;
	map_location src, dst;
	unit_map &units = *resources::units;
	const unit *u = NULL;

	if (lua_isuserdata(L, arg))
	{
		u = luaW_checkunit(L, 1);
		src = u->get_location();
		++arg;
	}
	else
	{
		src.x = luaL_checkinteger(L, arg) - 1;
		++arg;
		src.y = luaL_checkinteger(L, arg) - 1;
		unit_map::const_unit_iterator ui = units.find(src);
		if (ui.valid()) u = &*ui;
		++arg;
	}

	dst.x = luaL_checkinteger(L, arg) - 1;
	++arg;
	dst.y = luaL_checkinteger(L, arg) - 1;
	++arg;

	if (!resources::game_map->on_board(src))
		return luaL_argerror(L, 1, "invalid location");
	if (!resources::game_map->on_board(dst))
		return luaL_argerror(L, arg - 2, "invalid location");

	std::vector<team> &teams = *resources::teams;
	gamemap &map = *resources::game_map;
	int viewing_side = 0;
	bool ignore_units = false, see_all = false, ignore_teleport = false;
	double stop_at = 10000;
	pathfind::cost_calculator *calc = NULL;

	if (lua_istable(L, arg))
	{
		lua_pushstring(L, "ignore_units");
		lua_rawget(L, arg);
		ignore_units = lua_toboolean(L, -1);
		lua_pop(L, 1);

		lua_pushstring(L, "ignore_teleport");
		lua_rawget(L, arg);
		ignore_teleport = lua_toboolean(L, -1);
		lua_pop(L, 1);

		lua_pushstring(L, "max_cost");
		lua_rawget(L, arg);
		if (!lua_isnil(L, -1))
			stop_at = luaL_checknumber(L, -1);
		lua_pop(L, 1);

		lua_pushstring(L, "viewing_side");
		lua_rawget(L, arg);
		if (!lua_isnil(L, -1)) {
			int i = luaL_checkinteger(L, -1);
			if (i >= 1 && i <= int(teams.size())) viewing_side = i;
			else see_all = true;
		}
		lua_pop(L, 1);
	}
	else if (lua_isfunction(L, arg))
	{
		calc = new lua_calculator(L, arg);
	}

	pathfind::teleport_map teleport_locations;

	if (!calc) {
		if (!u) return luaL_argerror(L, 1, "unit not found");

		team &viewing_team = teams[(viewing_side ? viewing_side : u->side()) - 1];
		if (!ignore_teleport) {
			teleport_locations = pathfind::get_teleport_locations(
				*u, viewing_team, see_all, ignore_units);
		}
		calc = new pathfind::shortest_path_calculator(*u, viewing_team,
			units, teams, map, ignore_units, false, see_all);
	}

	pathfind::plain_route res = pathfind::a_star_search(src, dst, stop_at, calc, map.w(), map.h(),
		&teleport_locations);
	delete calc;

	int nb = res.steps.size();
	lua_createtable(L, nb, 0);
	for (int i = 0; i < nb; ++i)
	{
		lua_createtable(L, 2, 0);
		lua_pushinteger(L, res.steps[i].x + 1);
		lua_rawseti(L, -2, 1);
		lua_pushinteger(L, res.steps[i].y + 1);
		lua_rawseti(L, -2, 2);
		lua_rawseti(L, -2, i + 1);
	}
	lua_pushinteger(L, res.move_cost);

	return 2;
}

/**
 * Finds all the locations reachable by a unit.
 * - Args 1,2: source location. (Or Arg 1: unit.)
 * - Arg 3: optional table (optional fields: ignore_units, ignore_teleport, additional_turns, viewing_side).
 * - Ret 1: array of triples (coordinates + remaining movement).
 */
static int intf_find_reach(lua_State *L)
{
	int arg = 1;
	map_location src;
	unit_map &units = *resources::units;
	const unit *u = NULL;

	if (lua_isuserdata(L, arg))
	{
		u = luaW_checkunit(L, 1);
		src = u->get_location();
		++arg;
	}
	else
	{
		src.x = luaL_checkinteger(L, arg) - 1;
		++arg;
		src.y = luaL_checkinteger(L, arg) - 1;
		unit_map::const_unit_iterator ui = units.find(src);
		if (!ui.valid())
			return luaL_argerror(L, 1, "unit not found");
		u = &*ui;
		++arg;
	}

	std::vector<team> &teams = *resources::teams;
	gamemap &map = *resources::game_map;
	int viewing_side = 0;
	bool ignore_units = false, see_all = false, ignore_teleport = false;
	int additional_turns = 0;

	if (lua_istable(L, arg))
	{
		lua_pushstring(L, "ignore_units");
		lua_rawget(L, arg);
		ignore_units = lua_toboolean(L, -1);
		lua_pop(L, 1);

		lua_pushstring(L, "ignore_teleport");
		lua_rawget(L, arg);
		ignore_teleport = lua_toboolean(L, -1);
		lua_pop(L, 1);

		lua_pushstring(L, "additional_turns");
		lua_rawget(L, arg);
		additional_turns = lua_tointeger(L, -1);
		lua_pop(L, 1);

		lua_pushstring(L, "viewing_side");
		lua_rawget(L, arg);
		if (!lua_isnil(L, -1)) {
			int i = luaL_checkinteger(L, -1);
			if (i >= 1 && i <= int(teams.size())) viewing_side = i;
			else see_all = true;
		}
		lua_pop(L, 1);
	}

	team &viewing_team = teams[(viewing_side ? viewing_side : u->side()) - 1];
	pathfind::paths res(map, units, src, teams, ignore_units, !ignore_teleport,
		viewing_team, additional_turns, see_all, ignore_units);

	int nb = res.destinations.size();
	lua_createtable(L, nb, 0);
	for (int i = 0; i < nb; ++i)
	{
		pathfind::paths::step &s = res.destinations[i];
		lua_createtable(L, 2, 0);
		lua_pushinteger(L, s.curr.x + 1);
		lua_rawseti(L, -2, 1);
		lua_pushinteger(L, s.curr.y + 1);
		lua_rawseti(L, -2, 2);
		lua_pushinteger(L, s.move_left);
		lua_rawseti(L, -2, 3);
		lua_rawseti(L, -2, i + 1);
	}

	return 1;
}

/**
 * Places a unit on the map.
 * - Args 1,2: (optional) location.
 * - Arg 3: WML table describing a unit, or nothing/nil to delete.
 */
static int intf_put_unit(lua_State *L)
{
	int unit_arg = 1;

	lua_unit *lu = NULL;
	unit *u = NULL;
	map_location loc;
	if (lua_isnumber(L, 1)) {
		unit_arg = 3;
		loc.x = lua_tointeger(L, 1) - 1;
		loc.y = luaL_checkinteger(L, 2) - 1;
		if (!resources::game_map->on_board(loc))
			return luaL_argerror(L, 1, "invalid location");
	}

	if (luaW_hasmetatable(L, unit_arg, getunitKey))
	{
		lu = static_cast<lua_unit *>(lua_touserdata(L, unit_arg));
		u = lu->get();
		if (!u) return luaL_argerror(L, unit_arg, "unit not found");
		if (lu->on_map()) {
			if (unit_arg == 1 || u->get_location() == loc) return 0;
			resources::units->erase(loc);
			resources::units->move(u->get_location(), loc);
			return 0;
		} else if (int side = lu->on_recall_list()) {
			team &t = (*resources::teams)[side - 1];
			unit *v = new unit(*u);
			std::vector<unit> &rl = t.recall_list();
			rl.erase(rl.begin() + (u - &rl[0]));
			u = v;
		}
		if (unit_arg == 1) {
			loc = u->get_location();
			if (!resources::game_map->on_board(loc))
				return luaL_argerror(L, 1, "invalid location");
		}
	}
	else if (!lua_isnoneornil(L, unit_arg))
	{
		config cfg = luaW_checkconfig(L, unit_arg);
		if (unit_arg == 1) {
			loc.x = cfg["x"] - 1;
			loc.y = cfg["y"] - 1;
			if (!resources::game_map->on_board(loc))
				return luaL_argerror(L, 1, "invalid location");
		}
		u = new unit(cfg, true, resources::state_of_game);
	}

	resources::units->erase(loc);
	if (!u) return 0;

	if (lu) {
		resources::units->add(loc, *u);
		size_t uid = u->underlying_id();
		lu->lua_unit::~lua_unit();
		new(lu) lua_unit(uid);
	} else {
		u->set_location(loc);
		resources::units->insert(u);
	}

	return 0;
}

/**
 * Puts a unit on a recall list.
 * - Arg 1: WML table or unit.
 * - Arg 2: (optional) side.
 */
static int intf_put_recall_unit(lua_State *L)
{
	lua_unit *lu = NULL;
	unit *u = NULL;
	int side = lua_tointeger(L, 2);
	if (unsigned(side) > resources::teams->size()) side = 0;

	if (luaW_hasmetatable(L, 1, getunitKey))
	{
		lu = static_cast<lua_unit *>(lua_touserdata(L, 1));
		u = lu->get();
		if (!u || lu->on_recall_list())
			return luaL_argerror(L, 1, "unit not found");
	}
	else
	{
		config cfg = luaW_checkconfig(L, 1);
		u = new unit(cfg, true, resources::state_of_game);
	}

	if (!side) side = u->side();
	team &t = (*resources::teams)[side - 1];
	if (!t.persistent())
		return luaL_argerror(L, 2, "nonpersistent side");
	std::vector<unit> &rl = t.recall_list();

	// Avoid duplicates in the recall list.
	size_t uid = u->underlying_id();
	std::vector<unit>::iterator i = rl.begin();
	while (i != rl.end()) {
		if (i->underlying_id() == u->underlying_id()) {
			i = rl.erase(i);
		} else ++i;
	}

	rl.push_back(*u);
	if (lu) {
		if (lu->on_map())
			resources::units->erase(u->get_location());
		lu->lua_unit::~lua_unit();
		new(lu) lua_unit(side, uid);
	}

	return 0;
}

/**
 * Extracts a unit from the map or a recall list and gives it to Lua.
 * - Arg 1: unit userdata.
 */
static int intf_extract_unit(lua_State *L)
{
	if (!luaW_hasmetatable(L, 1, getunitKey))
		return luaL_typerror(L, 1, "unit");
	lua_unit *lu = static_cast<lua_unit *>(lua_touserdata(L, 1));
	unit *u = lu->get();
	if (!u) return luaL_argerror(L, 1, "unit not found");

	if (lu->on_map()) {
		u = resources::units->extract(u->get_location());
		assert(u);
	} else if (int side = lu->on_recall_list()) {
		team &t = (*resources::teams)[side - 1];
		unit *v = new unit(*u);
		std::vector<unit> &rl = t.recall_list();
		rl.erase(rl.begin() + (u - &rl[0]));
		u = v;
	} else {
		return 0;
	}

	lu->lua_unit::~lua_unit();
	new(lu) lua_unit(u);
	return 0;
}

/**
 * Finds a vacant tile.
 * - Args 1,2: location.
 * - Arg 3: optional unit for checking movement type.
 * - Rets 1,2: location.
 */
static int intf_find_vacant_tile(lua_State *L)
{
	int x = luaL_checkint(L, 1) - 1, y = luaL_checkint(L, 2) - 1;

	const unit *u = NULL;
	bool fake_unit = false;
	if (!lua_isnoneornil(L, 3)) {
		if (luaW_hasmetatable(L, 3, getunitKey)) {
			u = static_cast<lua_unit *>(lua_touserdata(L, 3))->get();
		} else {
			config cfg = luaW_checkconfig(L, 3);
			u = new unit(cfg, false, resources::state_of_game);
			fake_unit = true;
		}
	}

	map_location res = find_vacant_tile(*resources::game_map,
		*resources::units, map_location(x, y), pathfind::VACANT_ANY, u);

	if (fake_unit) delete u;

	if (!res.valid()) return 0;
	lua_pushinteger(L, res.x + 1);
	lua_pushinteger(L, res.y + 1);
	return 2;
}

/**
 * Floats some text on the map.
 * - Args 1,2: location.
 * - Arg 3: string.
 */
static int intf_float_label(lua_State *L)
{
	map_location loc;
	loc.x = luaL_checkinteger(L, 1) - 1;
	loc.y = luaL_checkinteger(L, 2) - 1;

	t_string text = luaW_checktstring(L, 3);
	resources::screen->float_label(loc, text, font::LABEL_COLOR.r,
		font::LABEL_COLOR.g, font::LABEL_COLOR.b);
	return 0;
}

/**
 * Creates a unit from its WML description.
 * - Arg 1: WML table.
 * - Ret 1: unit userdata.
 */
static int intf_create_unit(lua_State *L)
{
	config cfg = luaW_checkconfig(L, 1);
	unit *u = new unit(cfg, true, resources::state_of_game);
	new(lua_newuserdata(L, sizeof(lua_unit))) lua_unit(u);
	lua_pushlightuserdata(L, (void *)&getunitKey);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_setmetatable(L, -2);
	return 1;
}

/**
 * Copies a unit.
 * - Arg 1: unit userdata.
 * - Ret 1: unit userdata.
 */
static int intf_copy_unit(lua_State *L)
{
	unit const *u = luaW_checkunit(L, 1);
	new(lua_newuserdata(L, sizeof(lua_unit))) lua_unit(new unit(*u));
	lua_pushlightuserdata(L, (void *)&getunitKey);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_setmetatable(L, -2);
	return 1;
}

/**
 * Returns unit resistance against a given attack type.
 * - Arg 1: unit userdata.
 * - Arg 2: string containing the attack type.
 * - Arg 3: boolean indicating if attacker.
 * - Args 4,5: optional location.
 * - Ret 1: integer.
 */
static int intf_unit_resistance(lua_State *L)
{
	unit const *u = luaW_checkunit(L, 1);
	char const *m = luaL_checkstring(L, 2);
	bool a = lua_toboolean(L, 3);

	map_location loc = u->get_location();
	if (!lua_isnoneornil(L, 4)) {
		loc.x = luaL_checkinteger(L, 4) - 1;
		loc.y = luaL_checkinteger(L, 5) - 1;
	}

	lua_pushinteger(L, u->resistance_against(m, a, loc));
	return 1;
}

/**
 * Returns unit movement cost on a given terrain.
 * - Arg 1: unit userdata.
 * - Arg 2: string containing the terrain type.
 * - Ret 1: integer.
 */
static int intf_unit_movement_cost(lua_State *L)
{
	unit const *u = luaW_checkunit(L, 1);
	char const *m = luaL_checkstring(L, 2);
	t_translation::t_terrain t = t_translation::read_terrain_code(m);
	lua_pushinteger(L, u->movement_cost(t));
	return 1;
}

/**
 * Returns unit defense on a given terrain.
 * - Arg 1: unit userdata.
 * - Arg 2: string containing the terrain type.
 * - Ret 1: integer.
 */
static int intf_unit_defense(lua_State *L)
{
	unit const *u = luaW_checkunit(L, 1);
	char const *m = luaL_checkstring(L, 2);
	t_translation::t_terrain t = t_translation::read_terrain_code(m);
	lua_pushinteger(L, u->defense_modifier(t));
	return 1;
}

/**
 * Returns true if the unit has the given ability enabled.
 * - Arg 1: unit userdata.
 * - Arg 2: string.
 * - Ret 1: boolean.
 */
static int intf_unit_ability(lua_State *L)
{
	unit const *u = luaW_checkunit(L, 1);
	char const *m = luaL_checkstring(L, 2);
	lua_pushboolean(L, u->get_ability_bool(m));
	return 1;
}

/**
 * Changes a unit to the given unit type.
 * - Arg 1: unit userdata.
 * - Arg 2: string.
 */
static int intf_transform_unit(lua_State *L)
{
	unit *u = luaW_checkunit(L, 1);
	char const *m = luaL_checkstring(L, 2);
	const unit_type *utp = unit_types.find(m);
	if (!utp) return luaL_argerror(L, 2, "unknown unit type");
	u->advance_to(utp);
	return 0;
}

/**
 * Puts a table at the top of the stack with some combat result.
 */
static void luaW_pushsimdata(lua_State *L, const combatant &cmb)
{
	int n = cmb.hp_dist.size();
	lua_createtable(L, 0, 4);
	lua_pushnumber(L, cmb.poisoned);
	lua_setfield(L, -2, "poisoned");
	lua_pushnumber(L, cmb.slowed);
	lua_setfield(L, -2, "slowed");
	lua_pushnumber(L, cmb.average_hp());
	lua_setfield(L, -2, "average_hp");
	lua_createtable(L, n, 0);
	for (int i = 0; i < n; ++i) {
		lua_pushnumber(L, cmb.hp_dist[i]);
		lua_rawseti(L, -2, i);
	}
	lua_setfield(L, -2, "hp_chance");
}

/**
 * Simulates a combat between two units.
 * - Arg 1: attacker userdata.
 * - Arg 2: optional weapon index.
 * - Arg 3: defender userdata.
 * - Arg 4: optional weapon index.
 * - Ret 1: attacker results.
 * - Ret 2: defender results.
 */
static int intf_simulate_combat(lua_State *L)
{
	int arg_num = 1, att_w = -1, def_w = -1;

	unit const *att = luaW_checkunit(L, arg_num);
	++arg_num;
	if (lua_isnumber(L, arg_num)) {
		att_w = lua_tointeger(L, arg_num) - 1;
		if (att_w < 0 || att_w >= int(att->attacks().size()))
			return luaL_argerror(L, arg_num, "weapon index out of bounds");
		++arg_num;
	}

	unit const *def = luaW_checkunit(L, arg_num, true);
	++arg_num;
	if (lua_isnumber(L, arg_num)) {
		def_w = lua_tointeger(L, arg_num) - 1;
		if (def_w < 0 || def_w >= int(def->attacks().size()))
			return luaL_argerror(L, arg_num, "weapon index out of bounds");
		++arg_num;
	}

	battle_context context(*resources::units, att->get_location(),
		def->get_location(), att_w, def_w, 0.0, NULL, att);

	luaW_pushsimdata(L, context.get_attacker_combatant());
	luaW_pushsimdata(L, context.get_defender_combatant());
	return 2;
}

/**
 * Creates a vconfig containing the WML table.
 * - Arg 1: WML table.
 * - Ret 1: vconfig userdata.
 */
static int intf_tovconfig(lua_State *L)
{
	vconfig vcfg = luaW_checkvconfig(L, 1);
	luaW_pushvconfig(L, vcfg);
	return 1;
}

/**
 * Modifies the music playlist.
 * - Arg 1: WML table, or nil to force changes.
 */
static int intf_set_music(lua_State *L)
{
	if (lua_isnoneornil(L, 1)) {
		sound::commit_music_changes();
		return 0;
	}

	config cfg = luaW_checkconfig(L, 1);
	sound::play_music_config(cfg);
	return 0;
}

/**
 * Plays a sound, possibly repeated.
 * - Arg 1: string.
 * - Arg 2: optional integer.
 */
static int intf_play_sound(lua_State *L)
{
	char const *m = luaL_checkstring(L, 1);
	if (resources::controller->is_skipping_replay()) return 0;
	int repeats = lua_tointeger(L, 2);
	sound::play_sound(m, sound::SOUND_FX, repeats);
	return 0;
}

/**
 * Scrolls to given tile.
 * - Args 1,2: location.
 * - Arg 3: boolean preventing scroll to fog.
 */
static int intf_scroll_to_tile(lua_State *L)
{
	int x = luaL_checkinteger(L, 1) - 1;
	int y = luaL_checkinteger(L, 2) - 1;
	bool check_fogged = lua_toboolean(L, 3);
	resources::screen->scroll_to_tile(map_location(x, y),
		game_display::SCROLL, check_fogged);
	return 0;
}

/**
 * Selects the given location on the map.
 * - Args 1,2: location.
 */
static int intf_select_hex(lua_State *L)
{
	int x = luaL_checkinteger(L, 1) - 1;
	int y = luaL_checkinteger(L, 2) - 1;
	resources::screen->select_hex(map_location(x, y));
	return 0;
}

struct lua_synchronize : mp_sync::user_choice
{
	lua_State *L;
	lua_synchronize(lua_State *l): L(l) {}

	virtual config query_user() const
	{
		config cfg;
		int index = 1;
		if (!lua_isnoneornil(L, 2)) {
			int side = resources::controller->current_side();
			if ((*resources::teams)[side - 1].is_ai())
				index = 2;
		}
		lua_settop(L, index);
		if (luaW_pcall(L, 0, 1, false))
			luaW_toconfig(L, -1, cfg);
		return cfg;
	}

	virtual config random_choice(rand_rng::simple_rng &) const
	{
		return config();
	}
};

/**
 * Ensures a value is synchronized among all the clients.
 * - Arg 1: function to compute the value, called if the client is the master.
 * - Arg 2: optional function, called instead of the first function if the user is not human.
 * - Ret 1: WML table returned by the function.
 */
static int intf_synchronize_choice(lua_State *L)
{
	config cfg = mp_sync::get_user_choice("input", lua_synchronize(L));
	luaW_pushconfig(L, cfg);
	return 1;
}

struct scoped_dialog
{
	lua_State *L;
	scoped_dialog *prev;
	static scoped_dialog *current;
	gui2::twindow *window;
	typedef std::map<gui2::twidget *, int> callback_map;
	callback_map callbacks;

	scoped_dialog(lua_State *l, gui2::twindow *w);
	~scoped_dialog();
private:
	scoped_dialog(const scoped_dialog &);
};

scoped_dialog *scoped_dialog::current = NULL;

scoped_dialog::scoped_dialog(lua_State *l, gui2::twindow *w)
	: L(l), prev(current), window(w), callbacks()
{
	lua_pushlightuserdata(L, (void *)&dlgclbkKey);
	lua_createtable(L, 1, 0);
	lua_pushvalue(L, -2);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_rawseti(L, -2, 1);
	lua_rawset(L, LUA_REGISTRYINDEX);
	current = this;
}

scoped_dialog::~scoped_dialog()
{
	delete window;
	current = prev;
	lua_pushlightuserdata(L, (void *)&dlgclbkKey);
	lua_pushvalue(L, -1);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_rawgeti(L, -1, 1);
	lua_remove(L, -2);
	lua_rawset(L, LUA_REGISTRYINDEX);
}

static gui2::twidget *find_widget(lua_State *L, int i, bool readonly)
{
	if (!scoped_dialog::current) {
		luaL_error(L, "no visible dialog");
		error_call_destructors_1:
		luaL_argerror(L, i, "out of bounds");
		error_call_destructors_2:
		luaL_typerror(L, i, "string");
		error_call_destructors_3:
		luaL_argerror(L, i, "widget not found");
		return NULL;
	}

	gui2::twidget *w = scoped_dialog::current->window;
	for (; !lua_isnoneornil(L, i); ++i)
	{
#ifdef GUI2_EXPERIMENTAL_LISTBOX
		if (gui2::tlist *l = dynamic_cast<gui2::tlist *>(w))
#else
		if (gui2::tlistbox *l = dynamic_cast<gui2::tlistbox *>(w))
#endif
		{
			int v = lua_tointeger(L, i);
			if (v < 1)
				goto error_call_destructors_1;
			int n = l->get_item_count();
			if (v > n) {
				if (readonly)
					goto error_call_destructors_1;
				utils::string_map dummy;
				for (; n < v; ++n)
					l->add_row(dummy);
			}
			w = l->get_row_grid(v - 1);
		}
		else if (gui2::tmulti_page *l = dynamic_cast<gui2::tmulti_page *>(w))
		{
			int v = lua_tointeger(L, i);
			if (v < 1)
				goto error_call_destructors_1;
			int n = l->get_page_count();
			if (v > n) {
				if (readonly)
					goto error_call_destructors_1;
				utils::string_map dummy;
				for (; n < v; ++n)
					l->add_page(dummy);
			}
			w = &l->page_grid(v - 1);
		}
		else
		{
			char const *m = lua_tostring(L, i);
			if (!m) goto error_call_destructors_2;
			w = w->find(m, false);
		}
		if (!w) goto error_call_destructors_3;
	}

	return w;
}

/**
 * Displays a window.
 * - Arg 1: WML table describing the window.
 * - Arg 2: function called at pre-show.
 * - Arg 3: function called at post-show.
 * - Ret 1: integer.
 */
static int intf_show_dialog(lua_State *L)
{
	config def_cfg = luaW_checkconfig(L, 1);

	gui2::twindow_builder::tresolution def(def_cfg);
	scoped_dialog w(L, gui2::build(resources::screen->video(), &def));

	if (!lua_isnoneornil(L, 2)) {
		lua_pushvalue(L, 2);
		lua_call(L, 0, 0);
	}

	int v = scoped_dialog::current->window->show(true, 0);

	if (!lua_isnoneornil(L, 3)) {
		lua_pushvalue(L, 3);
		lua_call(L, 0, 0);
	}

	lua_pushinteger(L, v);
	return 1;
}

/**
 * Sets the value of a widget on the current dialog.
 * - Arg 1: scalar.
 * - Args 2..n: path of strings and integers.
 */
static int intf_set_dialog_value(lua_State *L)
{
	gui2::twidget *w = find_widget(L, 2, false);

#ifdef GUI2_EXPERIMENTAL_LISTBOX
	if (gui2::tlist *l = dynamic_cast<gui2::tlist *>(w))
#else
	if (gui2::tlistbox *l = dynamic_cast<gui2::tlistbox *>(w))
#endif
	{
		int v = luaL_checkinteger(L, 1);
		int n = l->get_item_count();
		if (1 <= v && v <= n)
			l->select_row(v - 1);
		else
			return luaL_argerror(L, 1, "out of bounds");
	}
	else if (gui2::tmulti_page *l = dynamic_cast<gui2::tmulti_page *>(w))
	{
		int v = luaL_checkinteger(L, 1);
		int n = l->get_page_count();
		if (1 <= v && v <= n)
			l->select_page(v - 1);
		else
			return luaL_argerror(L, 1, "out of bounds");
	}
	else if (gui2::ttoggle_button *b = dynamic_cast<gui2::ttoggle_button *>(w))
	{
		b->set_value(lua_toboolean(L, 1));
	}
	else
	{
		t_string v = luaW_checktstring(L, 1);
		gui2::tcontrol *c = dynamic_cast<gui2::tcontrol *>(w);
		if (!c) return luaL_argerror(L, lua_gettop(L), "unsupported widget");
		c->set_label(v);
	}

	return 0;
}

/**
 * Gets the value of a widget on the current dialog.
 * - Args 1..n: path of strings and integers.
 * - Ret 1: scalar.
 */
static int intf_get_dialog_value(lua_State *L)
{
	gui2::twidget *w = find_widget(L, 1, true);

#ifdef GUI2_EXPERIMENTAL_LISTBOX
	if (gui2::tlist *l = dynamic_cast<gui2::tlist *>(w))
#else
	if (gui2::tlistbox *l = dynamic_cast<gui2::tlistbox *>(w))
#endif
	{
		lua_pushinteger(L, l->get_selected_row() + 1);
	} else if (gui2::tmulti_page *l = dynamic_cast<gui2::tmulti_page *>(w)) {
		lua_pushinteger(L, l->get_selected_page() + 1);
	} else if (gui2::ttoggle_button *b = dynamic_cast<gui2::ttoggle_button *>(w)) {
		lua_pushboolean(L, b->get_value());
	} else if (gui2::ttext_box *t = dynamic_cast<gui2::ttext_box *>(w)) {
		lua_pushstring(L, t->get_value().c_str());
	} else
		return luaL_argerror(L, lua_gettop(L), "unsupported widget");

	return 1;
}

static void dialog_callback(gui2::twidget *w)
{
	int cb;
	{
		scoped_dialog::callback_map &m = scoped_dialog::current->callbacks;
		scoped_dialog::callback_map::const_iterator i = m.find(w);
		if (i == m.end()) return;
		cb = i->second;
	}
	lua_State *L = scoped_dialog::current->L;
	lua_pushlightuserdata(L, (void *)&dlgclbkKey);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_rawgeti(L, -1, cb);
	lua_remove(L, -2);
	lua_call(L, 0, 0);
}

/** Helper struct for intf_set_dialog_callback. */
struct tdialog_callback_wrapper
{
	void forward(gui2::twidget* widget)
	{
		dialog_callback(widget);
	}
};
/**
 * Sets a callback on a widget of the current dialog.
 * - Arg 1: function.
 * - Args 2..n: path of strings and integers.
 */
static int intf_set_dialog_callback(lua_State *L)
{
	gui2::twidget *w = find_widget(L, 2, true);

	scoped_dialog::callback_map &m = scoped_dialog::current->callbacks;
	scoped_dialog::callback_map::iterator i = m.find(w);
	if (i != m.end())
	{
		lua_pushlightuserdata(L, (void *)&dlgclbkKey);
		lua_rawget(L, LUA_REGISTRYINDEX);
		lua_pushnil(L);
		lua_rawseti(L, -2, i->second);
		lua_pop(L, 1);
		m.erase(i);
	}

	if (lua_isnil(L, 1)) return 0;

#ifdef GUI2_EXPERIMENTAL_LISTBOX
	if (gui2::tlist *l = dynamic_cast<gui2::tlist *>(w)) {
		static tdialog_callback_wrapper wrapper;
		connect_signal_notify_modified(*l
				, boost::bind(
					  &tdialog_callback_wrapper::forward
					, wrapper
					, w));
	}
#else
	if (gui2::tlistbox *l = dynamic_cast<gui2::tlistbox *>(w)) {
		l->set_callback_value_change(&dialog_callback);
	}
#endif
	  else if (gui2::ttoggle_button *b = dynamic_cast<gui2::ttoggle_button *>(w)) {
		b->set_callback_state_change(&dialog_callback);
	} else
		return luaL_argerror(L, lua_gettop(L), "unsupported widget");

	lua_pushlightuserdata(L, (void *)&dlgclbkKey);
	lua_rawget(L, LUA_REGISTRYINDEX);
	int n = lua_objlen(L, -1) + 1;
	m[w] = n;
	lua_pushvalue(L, 1);
	lua_rawseti(L, -2, n);
	lua_pop(L, 1);

	return 0;
}

/**
 * Sets a canvas on a widget of the current dialog.
 * - Arg 1: integer.
 * - Arg 2: WML table.
 * - Args 3..n: path of strings and integers.
 */
static int intf_set_dialog_canvas(lua_State *L)
{
	int i = luaL_checkinteger(L, 1);
	gui2::twidget *w = find_widget(L, 3, true);
	gui2::tcontrol *c = dynamic_cast<gui2::tcontrol *>(w);
	if (!c) return luaL_argerror(L, lua_gettop(L), "unsupported widget");

	std::vector<gui2::tcanvas> &cv = c->canvas();
	if (i < 1 || unsigned(i) > cv.size())
		return luaL_argerror(L, 1, "out of bounds");

	config cfg = luaW_checkconfig(L, 2);
	cv[i - 1].set_cfg(cfg);
	return 0;
}

/**
 * Gets all the locations matching a given filter.
 * - Arg 1: WML table.
 * - Ret 1: array of integer pairs.
 */
static int intf_get_locations(lua_State *L)
{
	vconfig filter = luaW_checkvconfig(L, 1);

	std::set<map_location> res;
	terrain_filter t_filter(filter, *resources::units);
	t_filter.restrict_size(game_config::max_loop);
	t_filter.get_locations(res, true);

	lua_createtable(L, res.size(), 0);
	int i = 1;
	foreach (map_location const &loc, res)
	{
		lua_createtable(L, 2, 0);
		lua_pushinteger(L, loc.x + 1);
		lua_rawseti(L, -2, 1);
		lua_pushinteger(L, loc.y + 1);
		lua_rawseti(L, -2, 2);
		lua_rawseti(L, -2, i);
		++i;
	}
	return 1;
}

/**
 * Matches a location against the given filter.
 * - Args 1,2: integers.
 * - Arg 3: WML table.
 * - Ret 1: boolean.
 */
static int intf_match_location(lua_State *L)
{
	int x = luaL_checkinteger(L, 1) - 1;
	int y = luaL_checkinteger(L, 2) - 1;
	vconfig filter = luaW_checkvconfig(L, 3, true);

	if (filter.null()) {
		lua_pushboolean(L, true);
		return 1;
	}

	terrain_filter t_filter(filter, *resources::units);
	t_filter.restrict_size(game_config::max_loop);
	lua_pushboolean(L, t_filter.match(map_location(x, y)));
	return 1;
}



/**
 * Matches a side against the given filter.
 * - Args 1: side number.
 * - Arg 2: WML table.
 * - Ret 1: boolean.
 */
static int intf_match_side(lua_State *L)
{
	unsigned side = luaL_checkinteger(L, 1) - 1;
	if (side >= resources::teams->size()) return 0;
	vconfig filter = luaW_checkvconfig(L, 2, true);

	if (filter.null()) {
		lua_pushboolean(L, true);
		return 1;
	}

	side_filter s_filter(filter);
	lua_pushboolean(L, s_filter.match(side + 1));
	return 1;
}

/**
 * Adds a modification to a unit.
 * - Arg 1: unit.
 * - Arg 2: string.
 * - Arg 3: WML table.
 */
static int intf_add_modification(lua_State *L)
{
	unit *u = luaW_checkunit(L, 1);
	char const *m = luaL_checkstring(L, 2);
	std::string sm = m;
	if (sm != "advance" && sm != "object" && sm != "trait")
		return luaL_argerror(L, 2, "unknown modification type");

	config cfg = luaW_checkconfig(L, 3);
	u->add_modification(sm, cfg);
	return 0;
}

/**
 * Adds an overlay on a tile.
 * - Args 1,2: location.
 * - Arg 3: WML table.
 */
static int intf_add_tile_overlay(lua_State *L)
{
	int x = luaL_checkinteger(L, 1) - 1;
	int y = luaL_checkinteger(L, 2) - 1;
	config cfg = luaW_checkconfig(L, 3);

	resources::screen->add_overlay(map_location(x, y), cfg["image"], cfg["halo"],
		cfg["team_name"], cfg["visible_in_fog"].to_bool(true));
	return 0;
}

/**
 * Adds an overlay on a tile.
 * - Args 1,2: location.
 * - Arg 3: optional string.
 */
static int intf_remove_tile_overlay(lua_State *L)
{
	int x = luaL_checkinteger(L, 1) - 1;
	int y = luaL_checkinteger(L, 2) - 1;
	char const *m = lua_tostring(L, 3);

	if (m) {
		resources::screen->remove_single_overlay(map_location(x, y), m);
	} else {
		resources::screen->remove_overlay(map_location(x, y));
	}
	return 0;
}

/**
 * Delays engine for a while.
 * - Arg 1: integer.
 */
static int intf_delay(lua_State *L)
{
	unsigned final = SDL_GetTicks() + luaL_checkinteger(L, 1);
	do {
		resources::controller->play_slice(false);
		resources::screen->delay(10);
	} while (int(final - SDL_GetTicks()) > 0);
	return 0;
}

/**
 * Gets the dimension of an image.
 * - Arg 1: string.
 * - Ret 1: width.
 * - Ret 2: height.
 */
static int intf_get_image_size(lua_State *L)
{
	char const *m = luaL_checkstring(L, 1);
	image::locator img(m);
	if (!img.file_exists()) return 0;
	surface s = get_image(img);
	lua_pushinteger(L, s->w);
	lua_pushinteger(L, s->h);
	return 2;
}

struct lua_report_generator : reports::generator
{
	lua_State *mState;
	std::string name;
	lua_report_generator(lua_State *L, const std::string &n)
		: mState(L), name(n) {}
	virtual config generate();
};

config lua_report_generator::generate()
{
	lua_State *L = mState;
	config cfg;
	if (!luaW_getglobal(L, "wesnoth", "theme_items", name.c_str(), NULL))
		return cfg;
	if (!luaW_pcall(L, 0, 1)) return cfg;
	luaW_toconfig(L, -1, cfg);
	lua_pop(L, 1);
	return cfg;
}

/**
 * Executes its upvalue as a theme item generator.
 */
static int cfun_theme_item(lua_State *L)
{
	const char *m = lua_tostring(L, lua_upvalueindex(1));
	luaW_pushconfig(L, reports::generate_report(m, true));
	return 1;
}

/**
 * Creates a field of the theme_items table and returns it (__index metamethod).
 */
static int impl_theme_items_get(lua_State *L)
{
	char const *m = luaL_checkstring(L, 2);
	lua_pushvalue(L, 2);
	lua_pushcclosure(L, cfun_theme_item, 1);
	lua_pushvalue(L, 2);
	lua_pushvalue(L, -2);
	lua_rawset(L, 1);
	reports::register_generator(m, new lua_report_generator(L, m));
	return 1;
}

/**
 * Sets a field of the theme_items table (__newindex metamethod).
 */
static int impl_theme_items_set(lua_State *L)
{
	char const *m = luaL_checkstring(L, 2);
	lua_pushvalue(L, 2);
	lua_pushvalue(L, 3);
	lua_rawset(L, 1);
	reports::register_generator(m, new lua_report_generator(L, m));
	return 0;
}

LuaKernel::LuaKernel(const config &cfg)
	: mState(luaL_newstate()), level_(cfg)
{
	lua_State *L = mState;

	// Open safe libraries. (Debug is not, but it will be closed below.)
	static const luaL_Reg safe_libs[] = {
		{ "",       luaopen_base   },
		{ "table",  luaopen_table  },
		{ "string", luaopen_string },
		{ "math",   luaopen_math   },
		{ "debug",  luaopen_debug  },
		{ "os",     luaopen_os     },
		{ "io",     luaopen_io     },
		{ NULL, NULL }
	};
	for (luaL_Reg const *lib = safe_libs; lib->func; ++lib)
	{
		lua_pushcfunction(L, lib->func);
		lua_pushstring(L, lib->name);
		lua_call(L, 1, 0);
	}

	// Put some callback functions in the scripting environment.
	static luaL_reg const callbacks[] = {
		{ "add_modification",         &intf_add_modification         },
		{ "add_tile_overlay",         &intf_add_tile_overlay         },
		{ "clear_messages",           &intf_clear_messages           },
		{ "copy_unit",                &intf_copy_unit                },
		{ "create_unit",              &intf_create_unit              },
		{ "delay",                    &intf_delay                    },
		{ "dofile",                   &intf_dofile                   },
		{ "eval_conditional",         &intf_eval_conditional         },
		{ "extract_unit",             &intf_extract_unit             },
		{ "find_path",                &intf_find_path                },
		{ "find_reach",               &intf_find_reach               },
		{ "find_vacant_tile",         &intf_find_vacant_tile         },
		{ "fire_event",               &intf_fire_event               },
		{ "float_label",              &intf_float_label              },
		{ "get_dialog_value",         &intf_get_dialog_value         },
		{ "get_displayed_unit",       &intf_get_displayed_unit       },
		{ "get_image_size",           &intf_get_image_size           },
		{ "get_locations",            &intf_get_locations            },
		{ "get_map_size",             &intf_get_map_size             },
		{ "get_mouseover_tile",       &intf_get_mouseover_tile       },
		{ "get_recall_units",         &intf_get_recall_units         },
		{ "get_selected_tile",        &intf_get_selected_tile        },
		{ "get_terrain",              &intf_get_terrain              },
		{ "get_terrain_info",         &intf_get_terrain_info         },
		{ "get_unit",                 &intf_get_unit                 },
		{ "get_units",                &intf_get_units                },
		{ "get_variable",             &intf_get_variable             },
		{ "get_village_owner",        &intf_get_village_owner        },
		{ "highlight_hex",            &intf_highlight_hex            },
		{ "is_enemy",                 &intf_is_enemy                 },
		{ "match_location",           &intf_match_location           },
		{ "match_side",               &intf_match_side               },
		{ "match_unit",               &intf_match_unit               },
		{ "message",                  &intf_message                  },
		{ "play_sound",               &intf_play_sound               },
		{ "put_recall_unit",          &intf_put_recall_unit          },
		{ "put_unit",                 &intf_put_unit                 },
		{ "remove_tile_overlay",      &intf_remove_tile_overlay      },
		{ "require",                  &intf_require                  },
		{ "scroll_to_tile",           &intf_scroll_to_tile           },
		{ "select_hex",               &intf_select_hex               },
		{ "set_dialog_callback",      &intf_set_dialog_callback      },
		{ "set_dialog_canvas",        &intf_set_dialog_canvas        },
		{ "set_dialog_value",         &intf_set_dialog_value         },
		{ "set_music",                &intf_set_music                },
		{ "set_terrain",              &intf_set_terrain              },
		{ "set_variable",             &intf_set_variable             },
		{ "set_village_owner",        &intf_set_village_owner        },
		{ "show_dialog",              &intf_show_dialog              },
		{ "simulate_combat",          &intf_simulate_combat          },
		{ "synchronize_choice",       &intf_synchronize_choice       },
		{ "textdomain",               &intf_textdomain               },
		{ "tovconfig",                &intf_tovconfig                },
		{ "transform_unit",           &intf_transform_unit           },
		{ "unit_ability",             &intf_unit_ability             },
		{ "unit_defense",             &intf_unit_defense             },
		{ "unit_movement_cost",       &intf_unit_movement_cost       },
		{ "unit_resistance",          &intf_unit_resistance          },
		{ NULL, NULL }
	};
	luaL_register(L, "wesnoth", callbacks);

	// Create the getside metatable.
	lua_pushlightuserdata(L, (void *)&getsideKey);
	lua_createtable(L, 0, 3);
	lua_pushcfunction(L, impl_side_get);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, impl_side_set);
	lua_setfield(L, -2, "__newindex");
	lua_pushstring(L, "side");
	lua_setfield(L, -2, "__metatable");
	lua_rawset(L, LUA_REGISTRYINDEX);

	// Create the gettext metatable.
	lua_pushlightuserdata(L, (void *)&gettextKey);
	lua_createtable(L, 0, 2);
	lua_pushcfunction(L, impl_gettext);
	lua_setfield(L, -2, "__call");
	lua_pushstring(L, "message domain");
	lua_setfield(L, -2, "__metatable");
	lua_rawset(L, LUA_REGISTRYINDEX);

	// Create the gettype metatable.
	lua_pushlightuserdata(L, (void *)&gettypeKey);
	lua_createtable(L, 0, 2);
	lua_pushcfunction(L, impl_unit_type_get);
	lua_setfield(L, -2, "__index");
	lua_pushstring(L, "unit type");
	lua_setfield(L, -2, "__metatable");
	lua_rawset(L, LUA_REGISTRYINDEX);

	// Create the getunit metatable.
	lua_pushlightuserdata(L, (void *)&getunitKey);
	lua_createtable(L, 0, 4);
	lua_pushcfunction(L, impl_unit_collect);
	lua_setfield(L, -2, "__gc");
	lua_pushcfunction(L, impl_unit_get);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, impl_unit_set);
	lua_setfield(L, -2, "__newindex");
	lua_pushstring(L, "unit");
	lua_setfield(L, -2, "__metatable");
	lua_rawset(L, LUA_REGISTRYINDEX);

	// Create the tstring metatable.
	lua_pushlightuserdata(L, (void *)&tstringKey);
	lua_createtable(L, 0, 4);
	lua_pushcfunction(L, impl_tstring_concat);
	lua_setfield(L, -2, "__concat");
	lua_pushcfunction(L, impl_tstring_collect);
	lua_setfield(L, -2, "__gc");
	lua_pushcfunction(L, impl_tstring_tostring);
	lua_setfield(L, -2, "__tostring");
	lua_pushstring(L, "translatable string");
	lua_setfield(L, -2, "__metatable");
	lua_rawset(L, LUA_REGISTRYINDEX);

	// Create the unit status metatable.
	lua_pushlightuserdata(L, (void *)&ustatusKey);
	lua_createtable(L, 0, 3);
	lua_pushcfunction(L, impl_unit_status_get);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, impl_unit_status_set);
	lua_setfield(L, -2, "__newindex");
	lua_pushstring(L, "unit status");
	lua_setfield(L, -2, "__metatable");
	lua_rawset(L, LUA_REGISTRYINDEX);

	// Create the unit variables metatable.
	lua_pushlightuserdata(L, (void *)&unitvarKey);
	lua_createtable(L, 0, 3);
	lua_pushcfunction(L, impl_unit_variables_get);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, impl_unit_variables_set);
	lua_setfield(L, -2, "__newindex");
	lua_pushstring(L, "unit variables");
	lua_setfield(L, -2, "__metatable");
	lua_rawset(L, LUA_REGISTRYINDEX);

	// Create the vconfig metatable.
	lua_pushlightuserdata(L, (void *)&vconfigKey);
	lua_createtable(L, 0, 4);
	lua_pushcfunction(L, impl_vconfig_collect);
	lua_setfield(L, -2, "__gc");
	lua_pushcfunction(L, impl_vconfig_get);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, impl_vconfig_size);
	lua_setfield(L, -2, "__len");
	lua_pushstring(L, "wml object");
	lua_setfield(L, -2, "__metatable");
	lua_rawset(L, LUA_REGISTRYINDEX);

	// Create the ai elements table.
	ai::lua_ai_context::init(L);

	// Delete dofile and loadfile.
	/*  TLB commented out because we need loadfile!
	lua_pushnil(L);
	lua_setglobal(L, "dofile");
	lua_pushnil(L);
	lua_setglobal(L, "loadfile");
	*/

	// Create the game_config variable with its metatable.
	lua_getglobal(L, "wesnoth");
	lua_newuserdata(L, 0);
	lua_createtable(L, 0, 3);
	lua_pushcfunction(L, impl_game_config_get);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, impl_game_config_set);
	lua_setfield(L, -2, "__newindex");
	lua_pushstring(L, "game config");
	lua_setfield(L, -2, "__metatable");
	lua_setmetatable(L, -2);
	lua_setfield(L, -2, "game_config");
	lua_pop(L, 1);

	// Create the current variable with its metatable.
	lua_getglobal(L, "wesnoth");
	lua_newuserdata(L, 0);
	lua_createtable(L, 0, 2);
	lua_pushcfunction(L, impl_current_get);
	lua_setfield(L, -2, "__index");
	lua_pushstring(L, "current config");
	lua_setfield(L, -2, "__metatable");
	lua_setmetatable(L, -2);
	lua_setfield(L, -2, "current");
	lua_pop(L, 1);

	// Create the package table.
	lua_getglobal(L, "wesnoth");
	lua_newtable(L);
	lua_setfield(L, -2, "package");
	lua_pop(L, 1);

	// Create the wml_actions table.
	lua_getglobal(L, "wesnoth");
	lua_newtable(L);
	lua_setfield(L, -2, "wml_actions");
	lua_pop(L, 1);

	// Create the game_events table.
	lua_getglobal(L, "wesnoth");
	lua_newtable(L);
	lua_setfield(L, -2, "game_events");
	lua_pop(L, 1);

	// Create the theme_items table.
	lua_getglobal(L, "wesnoth");
	lua_newtable(L);
	lua_createtable(L, 0, 2);
	lua_pushcfunction(L, impl_theme_items_get);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, impl_theme_items_set);
	lua_setfield(L, -2, "__newindex");
	lua_setmetatable(L, -2);
	lua_setfield(L, -2, "theme_items");
	lua_pop(L, 1);

	// Store the error handler, then close debug.
	lua_pushlightuserdata(L, (void *)&executeKey);
	lua_getglobal(L, "debug");
	lua_getfield(L, -1, "traceback");
	lua_remove(L, -2);
	lua_rawset(L, LUA_REGISTRYINDEX);
	lua_pushnil(L);
	lua_setglobal(L, "debug");

	lua_settop(L, 0);
}

void LuaKernel::initialize()
{
	lua_State *L = mState;

	// Create the sides table.
	lua_getglobal(L, "wesnoth");
	std::vector<team> &teams = *resources::teams;
	lua_pushlightuserdata(L, (void *)&getsideKey);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_createtable(L, teams.size(), 0);
	for (unsigned i = 0; i != teams.size(); ++i)
	{
		// Create a full userdata containing a pointer to the team.
		team **p = static_cast<team **>(lua_newuserdata(L, sizeof(team *)));
		*p = &teams[i];
		lua_pushvalue(L, -3);
		lua_setmetatable(L, -2);
		lua_rawseti(L, -2, i + 1);
	}
	lua_setfield(L, -3, "sides");
	lua_pop(L, 2);

	// Create the unit_types table.
	lua_getglobal(L, "wesnoth");
	lua_pushlightuserdata(L, (void *)&gettypeKey);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_newtable(L);
	foreach (const unit_type_data::unit_type_map::value_type &ut, unit_types.types())
	{
		lua_createtable(L, 0, 1);
		lua_pushstring(L, ut.first.c_str());
		lua_setfield(L, -2, "id");
		lua_pushvalue(L, -3);
		lua_setmetatable(L, -2);
		lua_setfield(L, -2, ut.first.c_str());
	}
	lua_setfield(L, -3, "unit_types");
	lua_pop(L, 2);

	// Execute the preload scripts.
	game_config::load_config(preload_config);
	foreach (const config &cfg, preload_scripts) {
		execute(cfg["code"].str().c_str(), 0, 0);
	}
	foreach (const config &cfg, level_.child_range("lua")) {
		execute(cfg["code"].str().c_str(), 0, 0);
	}

	load_game();
}

static char const *handled_file_tags[] = {
	"color_palette", "color_range", "era", "event", "illuminated_time",
	"label", "lua", "menu_item", "music", "side", "sound_source", "story",
	"terrain_graphics", "time", "time_area", "variables"
};

static bool is_handled_file_tag(const std::string &s)
{
	foreach (char const *t, handled_file_tags) {
		if (s == t) return true;
	}
	return false;
}

/**
 * Executes the game_events.on_load function and passes to it all the
 * scenario tags not yet handled.
 */
void LuaKernel::load_game()
{
	lua_State *L = mState;

	if (!luaW_getglobal(L, "wesnoth", "game_events", "on_load", NULL))
		return;

	lua_newtable(L);
	int k = 1;
	foreach (const config::any_child &v, level_.all_children_range())
	{
		if (is_handled_file_tag(v.key)) continue;
		lua_createtable(L, 2, 0);
		lua_pushstring(L, v.key.c_str());
		lua_rawseti(L, -2, 1);
		luaW_pushconfig(L, v.cfg);
		lua_rawseti(L, -2, 2);
		lua_rawseti(L, -2, k++);
	}

	luaW_pcall(L, 1, 0, true);
}

/**
 * Executes the game_events.on_save function and adds to @a cfg the
 * returned tags. Also flushes the [lua] tags.
 */
void LuaKernel::save_game(config &cfg)
{
	foreach (const config &v, level_.child_range("lua")) {
		cfg.add_child("lua", v);
	}

	lua_State *L = mState;

	if (!luaW_getglobal(L, "wesnoth", "game_events", "on_save", NULL))
		return;

	if (!luaW_pcall(L, 0, 1, false))
		return;

	config v;
	luaW_toconfig(L, -1, v);
	lua_pop(L, 1);

	for (;;)
	{
		config::all_children_iterator i = v.ordered_begin();
		if (i == v.ordered_end()) break;
		if (is_handled_file_tag(i->key)) continue;
		cfg.splice_children(v, i->key);
	}
}

/**
 * Executes the game_events.on_event function.
 */
void LuaKernel::run_event(game_events::queued_event const &ev)
{
	lua_State *L = mState;

	if (!luaW_getglobal(L, "wesnoth", "game_events", "on_event", NULL))
		return;

	queued_event_context dummy(&ev);
	lua_pushstring(L, ev.name.c_str());
	luaW_pcall(L, 1, 0, false);
}

LuaKernel::~LuaKernel()
{
	lua_close(mState);
}

/**
 * Executes its upvalue as a wml action.
 */
static int cfun_wml_action(lua_State *L)
{
	game_events::action_handler h = reinterpret_cast<game_events::action_handler>
		(lua_touserdata(L, lua_upvalueindex(1)));

	vconfig vcfg = luaW_checkvconfig(L, 1);
	h(queued_event_context::get(), vcfg);
	return 0;
}

/**
 * Registers a function for use as an action handler.
 */
void LuaKernel::set_wml_action(std::string const &cmd, game_events::action_handler h)
{
	lua_State *L = mState;

	lua_getglobal(L, "wesnoth");
	lua_pushstring(L, "wml_actions");
	lua_rawget(L, -2);
	lua_pushstring(L, cmd.c_str());
	lua_pushlightuserdata(L, (void *)h);
	lua_pushcclosure(L, cfun_wml_action, 1);
	lua_rawset(L, -3);
	lua_pop(L, 2);
}

/**
 * Runs a command from an event handler.
 * @return true if there is a handler for the command.
 * @note @a cfg should be either volatile or long-lived since the Lua
 *       code may grab it for an arbitrary long time.
 */
bool LuaKernel::run_wml_action(std::string const &cmd, vconfig const &cfg,
	game_events::queued_event const &ev)
{
	lua_State *L = mState;


	if (!luaW_getglobal(L, "wesnoth", "wml_actions", cmd.c_str(), NULL))
		return false;

	queued_event_context dummy(&ev);
	luaW_pushvconfig(L, cfg);
	luaW_pcall(L, 1, 0, true);
	return true;
}

/**
 * Runs a script from a unit filter.
 * The script is an already compiled function given by its name.
 */
bool LuaKernel::run_filter(char const *name, unit const &u)
{
	lua_State *L = mState;

	unit_map::const_unit_iterator ui = resources::units->find(u.get_location());
	if (!ui.valid()) return false;

	// Get the user filter by name.
	lua_pushstring(L, name);
	lua_rawget(L, LUA_GLOBALSINDEX);

	// Pass the unit as argument.
	new(lua_newuserdata(L, sizeof(lua_unit))) lua_unit(ui->underlying_id());
	lua_pushlightuserdata(L, (void *)&getunitKey);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_setmetatable(L, -2);

	if (!luaW_pcall(L, 1, 1)) return false;

	bool b = lua_toboolean(L, -1);
	lua_pop(L, 1);
	return b;
}

/**
 * Runs a script on a stack containing @a nArgs arguments.
 * @return true if the script was successful and @a nRets return values are available.
 */
bool LuaKernel::execute(char const *prog, int nArgs, int nRets)
{
	lua_State *L = mState;

	// Compile script into a variadic function.
	int res = luaL_loadstring(L, prog);
	if (res)
	{
		char const *m = lua_tostring(L, -1);
		chat_message("Lua error", m);
		ERR_LUA << m << '\n';
		lua_pop(L, 2);
		return false;
	}

	// Place the function before its arguments.
	if (nArgs)
		lua_insert(L, -1 - nArgs);

	return luaW_pcall(L, nArgs, nRets);
}

/**
 * Loads the "package" package into the Lua environment.
 * This action is inherently unsafe, as Lua scripts will now be able to
 * load C libraries on their own, hence granting them the same privileges
 * as the Wesnoth binary itsef.
 */
void LuaKernel::load_package()
{
	lua_State *L = mState;
	lua_pushcfunction(L, luaopen_package);
	lua_pushstring(L, "package");
	lua_call(L, 1, 0);
}

ai::lua_ai_context* LuaKernel::create_lua_ai_context(char const *code, ai::engine_lua *engine)
{
	return ai::lua_ai_context::create(mState,code,engine);
}

ai::lua_ai_action_handler* LuaKernel::create_lua_ai_action_handler(char const *code, ai::lua_ai_context &context)
{
	return ai::lua_ai_action_handler::create(mState,code,context);
}
