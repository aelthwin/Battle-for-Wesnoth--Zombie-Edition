/* $Id: mapgen_dialog.hpp 48153 2011-01-01 15:57:50Z mordante $ */
/*
   Copyright (C) 2003 - 2011 by David White <dave@whitevine.net>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#ifndef MAPGEN_DIALOG_HPP_INCLUDED
#define MAPGEN_DIALOG_HPP_INCLUDED

#include "config.hpp"
#include "mapgen.hpp"

class default_map_generator : public map_generator
{
public:
	default_map_generator(const config &game_config);

	bool allow_user_config() const;
	void user_config(display& disp);

	std::string name() const;

	std::string config_name() const;

	std::string create_map(const std::vector<std::string>& args);
	config create_scenario(const std::vector<std::string>& args);

private:

	std::string generate_map(const std::vector<std::string>& args, std::map<map_location,std::string>* labels=NULL);

	size_t default_width_, default_height_, width_, height_, island_size_, iterations_, hill_size_, max_lakes_, nvillages_, castle_size_, nplayers_;
	bool link_castles_;
	config cfg_;
};

#endif
