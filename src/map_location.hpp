/* $Id: map_location.hpp 48153 2011-01-01 15:57:50Z mordante $ */
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

#ifndef MAP_LOCATION_H_INCLUDED
#define MAP_LOCATION_H_INCLUDED

class config;
class variable_set;

#include <string>
#include <vector>
#include <set>

#define MAX_MAP_AREA	65536

/**
 * Encapsulates the map of the game.
 *
 * Although the game is hexagonal, the map is stored as a grid.
 * Each type of terrain is represented by a multiletter terrain code.
 * @todo Update for new map-format.
 */
/** Represents a location on the map. */
struct map_location {
	/** Valid directions which can be moved in our hexagonal world. */
	enum DIRECTION { NORTH, NORTH_EAST, SOUTH_EAST, SOUTH,
					 SOUTH_WEST, NORTH_WEST, NDIRECTIONS };

	static DIRECTION parse_direction(const std::string& str);

	/**
	 * Parse_directions takes a comma-separated list, and filters out any
	 * invalid directions
	 */
	static std::vector<DIRECTION> parse_directions(const std::string& str);
	static std::string write_direction(DIRECTION dir);

	map_location() : x(-1000), y(-1000) {}
	map_location(int x, int y) : x(x), y(y) {}
	map_location(const config& cfg, const variable_set *variables);

	void write(config& cfg) const;

	inline bool valid() const { return x >= 0 && y >= 0; }

	inline bool valid(const int parWidth, const int parHeight) const
	{
		return ((x >= 0) && (y >= 0) && (x < parWidth) && (y < parHeight));
	}

	int x, y;
	bool matches_range(const std::string& xloc, const std::string& yloc) const;

	// Inlining those for performance reasons
	bool operator<(const map_location& a) const { return x < a.x || (x == a.x && y < a.y); }
	bool operator==(const map_location& a) const { return x == a.x && y == a.y; }
	bool operator!=(const map_location& a) const { return !operator==(a); }

        /** three-way comparator */
	int do_compare(const map_location& a) const {return x == a.x ? y - a.y : x - a.x; }

	// Adds an absolute location to a "delta" location
	// This is not the mathematically correct behaviour, it is neither
	// commutative nor associative. Negative coordinates may give strange
	// results. It is retained because terain builder code relies in this
	// broken behaviour. Best avoid.
	map_location legacy_negation() const;
	map_location legacy_sum(const map_location &a) const;
	map_location& legacy_sum_assign(const map_location &a);
	map_location legacy_difference(const map_location &a) const;

	// Location arithmetic operations treating the locations as vectors in
	// a hex-based space. These operations form an abelian group, i.e.
	// everything works as you would expect addition and subtraction to
	// work, with associativity and commutativity.
	map_location vector_negation() const;
	map_location vector_sum(const map_location &a) const;
	map_location& vector_sum_assign(const map_location &a);
	map_location &vector_difference_assign(const map_location &a);

	// Do n step in the direction d
	map_location get_direction(DIRECTION d, int n = 1) const;
	DIRECTION get_relative_dir(map_location loc) const;
	static DIRECTION get_opposite_dir(DIRECTION d);

	static const map_location null_location;
};

/** Function which tells if two locations are adjacent. */
bool tiles_adjacent(const map_location& a, const map_location& b);

/**
 * Function which, given a location, will place all adjacent locations in res.
 * res must point to an array of 6 location objects.
 */
void get_adjacent_tiles(const map_location& a, map_location* res);

/**
 * Function which gives the number of hexes between two tiles
 * (i.e. the minimum number of hexes that have to be traversed
 * to get from one hex to the other).
 */
size_t distance_between(const map_location& a, const map_location& b);

/** Parses ranges of locations into a vector of locations. */
std::vector<map_location> parse_location_range(const std::string& xvals,
	const std::string &yvals, bool with_border = false);

/**
 * Write a set of locations into a config using ranges,
 * adding keys x=x1,..,xn and y=y1a-y1b,..,yna-ynb
 */
void write_location_range(const std::set<map_location>& locs, config& cfg);

/** Parse x,y keys of a config into a vector of locations */
void read_locations(const config& cfg, std::vector<map_location>& locs);

/** Write a vector of locations into a config
 *  adding keys x=x1,x2,..,xn and y=y1,y2,..,yn */
void write_locations(const std::vector<map_location>& locs, config& cfg);

/** Dumps a position on a stream, for debug purposes. */
std::ostream &operator<<(std::ostream &s, map_location const &l);
/** Dumps a vector of positions on a stream, for debug purposes. */
std::ostream &operator<<(std::ostream &s, std::vector<map_location> const &v);

#endif
