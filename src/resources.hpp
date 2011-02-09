/* $Id: resources.hpp 48153 2011-01-01 15:57:50Z mordante $ */
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

#ifndef RESOURCES_H_
#define RESOURCES_H_

#include <vector>

class game_display;
class gamemap;
class game_state;
class LuaKernel;
class play_controller;
class team;
class tod_manager;
class unit_map;
class persist_manager;
struct undo_action;

namespace soundsource { class manager; }

namespace wb { class manager; } //whiteboard manager

namespace resources
{
	extern game_display *screen;
	extern soundsource::manager *soundsources;
	extern gamemap *game_map;
	extern unit_map *units;
	extern std::vector<team> *teams;
	extern game_state *state_of_game;
	extern LuaKernel *lua_kernel;
	extern play_controller *controller;
	extern tod_manager *tod_manager;
	extern wb::manager *whiteboard;
	extern std::vector<undo_action> *undo_stack;
	extern std::vector<undo_action> *redo_stack;
	extern persist_manager *persist;
}

#endif
