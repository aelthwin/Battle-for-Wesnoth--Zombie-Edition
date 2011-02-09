/* $Id: resources.cpp 48153 2011-01-01 15:57:50Z mordante $ */
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

#include "resources.hpp"

namespace resources
{
	game_display *screen;
	soundsource::manager *soundsources;
	gamemap *game_map;
	unit_map *units;
	std::vector<team> *teams;
	game_state *state_of_game;
	LuaKernel *lua_kernel;
	play_controller *controller;
	::tod_manager *tod_manager;
	wb::manager *whiteboard;
	std::vector<undo_action> *undo_stack;
	std::vector<undo_action> *redo_stack;
	persist_manager *persist;
}
