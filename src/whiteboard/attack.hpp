/* $Id: attack.hpp 48153 2011-01-01 15:57:50Z mordante $ */
/*
 Copyright (C) 2010 - 2011 by Gabriel Morin <gabrielmorin (at) gmail (dot) com>
 Part of the Battle for Wesnoth Project http://www.wesnoth.org

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
 */

#ifndef ATTACK_HPP_
#define ATTACK_HPP_

#include "move.hpp"

namespace wb
{

class attack: public move
{
public:
	friend class validate_visitor;
	friend class highlight_visitor;

	///Future unit map must be valid during construction, so that attack can find its units
	attack(size_t team_index, const map_location& target_hex, int weapon_choice, const pathfind::marked_route& route,
			arrow_ptr arrow, fake_unit_ptr fake_unit);
	virtual ~attack();

	virtual std::ostream& print(std::ostream& s) const;

	virtual void accept(visitor& v);

	virtual bool execute();

	/** Applies temporarily the result of this action to the specified unit map. */
	virtual void apply_temp_modifier(unit_map& unit_map);
	/** Removes the result of this action from the specified unit map. */
	virtual void remove_temp_modifier(unit_map& unit_map);

	/** Gets called by display when drawing a hex, to allow actions to draw to the screen. */
	virtual void draw_hex(const map_location& hex);

	map_location const& get_target_hex() const {return target_hex_; }

private:
		///the target of the attack
		map_location target_hex_;

		int weapon_choice_;
		int attack_movement_cost_;
		int temp_movement_subtracted_;
};

/** Dumps an attack on a stream, for debug purposes. */
std::ostream& operator<<(std::ostream &s, attack_ptr attack);
std::ostream& operator<<(std::ostream &s, attack_const_ptr attack);

} // end namespace wb

#endif /* ATTACK_HPP_ */
