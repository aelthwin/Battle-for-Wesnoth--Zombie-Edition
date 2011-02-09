/* $Id: attack.cpp 48153 2011-01-01 15:57:50Z mordante $ */
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

#include "attack.hpp"

#include "visitor.hpp"

#include "arrow.hpp"
#include "play_controller.hpp"
#include "resources.hpp"
#include "unit.hpp"
#include "unit_map.hpp"

namespace wb
{

std::ostream &operator<<(std::ostream &s, attack_ptr attack)
{
	assert(attack);
	return attack->print(s);
}

std::ostream &operator<<(std::ostream &s, attack_const_ptr attack)
{
	assert(attack);
	return attack->print(s);
}

std::ostream& attack::print(std::ostream& s) const
{
	s << "Attack on (" << get_target_hex() << ") preceded by ";
	move::print(s);
	return s;
}

attack::attack(size_t team_index, const map_location& target_hex, int weapon_choice, const pathfind::marked_route& route,
		arrow_ptr arrow, fake_unit_ptr fake_unit)
	: move(team_index, route, arrow, fake_unit),
	target_hex_(target_hex),
	weapon_choice_(weapon_choice),
	attack_movement_cost_(get_unit()->attacks()[weapon_choice_].movement_used()),
	temp_movement_subtracted_(0)
{
}

attack::~attack()
{
	if(resources::screen)
	{
		//invalidate dest and target hex so attack indicator is properly cleared
		resources::screen->invalidate(get_dest_hex());
		resources::screen->invalidate(target_hex_);
	}
}

void attack::accept(visitor& v)
{
	v.visit_attack(boost::static_pointer_cast<attack>(shared_from_this()));
}

bool attack::execute()
{
	bool execute_successful = true;

	if (!valid_)
		execute_successful = false;

	LOG_WB << "Executing: " << shared_from_this() << "\n";

	events::mouse_handler const& mouse_handler = resources::controller->get_mouse_handler_base();

	std::set<map_location> adj_enemies = mouse_handler.get_adj_enemies(get_dest_hex(), side_number());

	if (execute_successful && route_->steps.size() >= 2)
	{
		if (!move::execute())
		{
			//Move didn't complete for some reason, so we're not at
			//the right hex to execute the attack.
			execute_successful = false;
		}
		//check if new enemies are now visible
		else if(mouse_handler.get_adj_enemies(get_dest_hex(), side_number()) != adj_enemies)
		{
			execute_successful = false; //ambush, interrupt attack
		}
	}

	if (execute_successful)
	{
		resources::controller->get_mouse_handler_base().attack_enemy(get_dest_hex(), get_target_hex(), weapon_choice_);
		//only path that returns execute_successful = true
	}
	return execute_successful;
}

void attack::apply_temp_modifier(unit_map& unit_map)
{
	move::apply_temp_modifier(unit_map);
	assert(get_unit());
	unit& unit = *get_unit();
	LOG_WB << unit.name() << " [" << unit.id()
					<< "] has " << unit.attacks_left() << " attacks, decreasing by one" << "\n";
	assert(unit.attacks_left() > 0);
	unit.set_attacks(unit.attacks_left() - 1);

	//Calculate movement to subtract
	temp_movement_subtracted_ = unit.movement_left() >= attack_movement_cost_ ? attack_movement_cost_ : 0 ;
	DBG_WB << "Attack: Changing movement points for unit " << unit.name() << " [" << unit.id()
				<< "] from " << unit.movement_left() << " to "
				<< unit.movement_left() - temp_movement_subtracted_ << ".\n";
	unit.set_movement(unit.movement_left() - temp_movement_subtracted_);
}

void attack::remove_temp_modifier(unit_map& unit_map)
{
	assert(get_unit());
	unit& unit = *get_unit();
	LOG_WB << unit.name() << " [" << unit.id()
					<< "] has " << unit.attacks_left() << " attacks, increasing by one" << "\n";
	unit.set_attacks(unit.attacks_left() + 1);
	DBG_WB << "Attack: Changing movement points for unit " << unit.name() << " [" << unit.id()
				<< "] from " << unit.movement_left() << " to "
				<< unit.movement_left() + temp_movement_subtracted_ << ".\n";
	unit.set_movement(unit.movement_left() + temp_movement_subtracted_);
	temp_movement_subtracted_ = 0;
	move::remove_temp_modifier(unit_map);
}

void attack::draw_hex(const map_location& hex)
{
	if (hex == get_dest_hex() || hex == target_hex_) //draw attack indicator
	{
		//@todo: replace this by either the use of transparency + LAYER_ATTACK_INDICATOR,
		//or a dedicated layer
		const display::tdrawing_layer layer = display::LAYER_FOOTSTEPS;

		//calculate direction (valid for both hexes)
		std::string direction_text = map_location::write_direction(
				get_dest_hex().get_relative_dir(target_hex_));

		if (hex == get_dest_hex()) //add symbol to attacker hex
		{
			int xpos = resources::screen->get_location_x(get_dest_hex());
			int ypos = resources::screen->get_location_y(get_dest_hex());

			resources::screen->drawing_buffer_add(layer, get_dest_hex(), xpos, ypos,
					image::get_image("whiteboard/attack-indicator-src-" + direction_text + ".png", image::SCALED_TO_HEX));
		}
		else if (hex == target_hex_) //add symbol to defender hex
		{
			int xpos = resources::screen->get_location_x(target_hex_);
			int ypos = resources::screen->get_location_y(target_hex_);

			resources::screen->drawing_buffer_add(layer, target_hex_, xpos, ypos,
					image::get_image("whiteboard/attack-indicator-dst-" + direction_text + ".png", image::SCALED_TO_HEX));
		}
	}
}

} // end namespace wb
