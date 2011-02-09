/* $Id: game_config_manager.hpp 48153 2011-01-01 15:57:50Z mordante $ */
/*
   Copyright (C) 2008 - 2011 by Pauli Nieminen <paniemin@cc.hut.fi>
   Part of thie Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#ifndef TESTS_UTILS_GAME_CONFIG_MANAGER_HPP_INCLUDED
#define TESTS_UTILS_GAME_CONFIG_MANAGER_HPP_INCLUDED
class config;

namespace test_utils {
	const config& get_test_config_ref();
	config get_test_config();
}

#endif
