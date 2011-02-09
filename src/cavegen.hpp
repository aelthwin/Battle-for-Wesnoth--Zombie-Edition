/* $Id: cavegen.hpp 48153 2011-01-01 15:57:50Z mordante $ */
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

/** @file */

#ifndef CAVEGEN_HPP_INCLUDED
#define CAVEGEN_HPP_INCLUDED

#include "config.hpp"
#include "mapgen.hpp"
#include "terrain_translation.hpp"

#include <set>

class cave_map_generator : public map_generator
{
public:
	cave_map_generator(const config &game_config);

	bool allow_user_config() const { return true; }
	// This is a pure virtual function in the base class, so must be here
	void user_config(display& /* disp*/) { return; }

	std::string name() const { return "cave"; }

	std::string config_name() const;

	std::string create_map(const std::vector<std::string>& args);
	config create_scenario(const std::vector<std::string>& args);

private:

	struct chamber {
		chamber() :
			center(),
			locs(),
			items(0)
		{
		}

		map_location center;
		std::set<map_location> locs;
		const config *items;
	};

	struct passage {
		passage(map_location s, map_location d, const config& c)
			: src(s), dst(d), cfg(c)
		{}
		map_location src, dst;
		config cfg;
	};

	void generate_chambers();
	void build_chamber(map_location loc, std::set<map_location>& locs, size_t size, size_t jagged);

	void place_chamber(const chamber& c);

	void place_passage(const passage& p);

	bool on_board(const map_location& loc) const
	{
		return loc.x >= 0 && loc.y >= 0 && loc.x < width_ && loc.y < height_;
	}

	void set_terrain(map_location loc, t_translation::t_terrain t);
	void place_castle(int starting_position, const map_location &loc);

	t_translation::t_terrain wall_, clear_, village_, castle_, keep_;
	t_translation::t_map map_;
	std::map<int, t_translation::coordinate> starting_positions_;

	std::map<std::string,size_t> chamber_ids_;
	std::vector<chamber> chambers_;
	std::vector<passage> passages_;

	config res_;
	config cfg_;
	int width_, height_, village_density_;

	// The scenario may have a chance to flip all x values or y values
	// to make the scenario appear all random. This is kept track of here.
	bool flipx_, flipy_;

	size_t translate_x(size_t x) const;
	size_t translate_y(size_t y) const;
};

#endif
