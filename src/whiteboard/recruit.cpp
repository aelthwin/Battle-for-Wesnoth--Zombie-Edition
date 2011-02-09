/* $Id: recruit.cpp 48153 2011-01-01 15:57:50Z mordante $ */
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

#include "recruit.hpp"

#include "manager.hpp"
#include "side_actions.hpp"
#include "utility.hpp"
#include "visitor.hpp"

#include "game_display.hpp"
#include "menu_events.hpp"
#include "play_controller.hpp"
#include "resources.hpp"
#include "unit.hpp"
#include "unit_map.hpp"
#include "unit_types.hpp"

namespace wb
{

std::ostream& operator<<(std::ostream& s, recruit_ptr recruit)
{
	assert(recruit);
	return recruit->print(s);
}
std::ostream& operator<<(std::ostream& s, recruit_const_ptr recruit)
{
	assert(recruit);
	return recruit->print(s);
}

std::ostream& recruit::print(std::ostream &s) const
{
	s << "Recruiting " << unit_name_ << " on hex " << recruit_hex_;
	return s;
}

recruit::recruit(size_t team_index, const std::string& unit_name, const map_location& recruit_hex):
		action(team_index),
		unit_name_(unit_name),
		recruit_hex_(recruit_hex),
		temp_unit_(create_corresponding_unit()),
		valid_(true),
		fake_unit_(create_corresponding_unit(), wb::fake_unit_deleter()),
		temp_cost_()
{
	fake_unit_->set_location(recruit_hex_);
	fake_unit_->set_movement(0);
	fake_unit_->set_attacks(0);
	fake_unit_->set_ghosted(false);
	resources::screen->place_temporary_unit(fake_unit_.get());
}

recruit::~recruit()
{
}

void recruit::accept(visitor& v)
{
	v.visit_recruit(shared_from_this());
}

bool recruit::execute()
{
	assert(valid_);
	fake_unit_->set_hidden(true);
	int side_num = team_index() + 1;
	resources::controller->get_menu_handler().do_recruit(unit_name_, side_num, recruit_hex_);
	return true;
}

void recruit::apply_temp_modifier(unit_map& unit_map)
{
	assert(valid_);
	temp_unit_->set_location(recruit_hex_);

	DBG_WB << "Inserting future recruit [" << temp_unit_->id()
			<< "] at position " << temp_unit_->get_location() << ".\n";

	temp_cost_ = temp_unit_->type()->cost();

	// Add cost to money spent on recruits.
	resources::teams->at(team_index()).get_side_actions()->change_gold_spent_by(temp_cost_);

	// Temporarily insert unit into unit_map
	unit_map.insert(temp_unit_);
	// unit map takes ownership of temp_unit
	temp_unit_ = NULL;

	// Update gold in the top bar
	resources::screen->invalidate_game_status();
}

void recruit::remove_temp_modifier(unit_map& unit_map)
{
	temp_unit_ = unit_map.extract(recruit_hex_);
	assert(temp_unit_);

	/*
	 * Remove cost from money spent on recruits.
	 */
	resources::teams->at(team_index()).get_side_actions()->change_gold_spent_by(-temp_cost_);
	temp_cost_ = 0;
	resources::screen->invalidate_game_status();
}

void recruit::draw_hex(map_location const& hex)
{
	if (hex == recruit_hex_)
	{
		const double x_offset = 0.5;
		const double y_offset = 0.7;
		//position 0,0 in the hex is the upper left corner
		std::stringstream number_text;
		number_text << "-" << temp_unit_->type()->cost();
		size_t font_size = 16;
		SDL_Color color; color.r = 255; color.g = 0; color.b = 0; //red
		resources::screen->draw_text_in_hex(hex, display::LAYER_ACTIONS_NUMBERING,
						number_text.str(), font_size, color, x_offset, y_offset);
	}
}

unit* recruit::create_corresponding_unit()
{
	unit_type const* type = unit_types.find(unit_name_);
	assert(type);
	int side_num = team_index() + 1;
	//real_unit = false needed to avoid generating random traits and causing OOS
	bool real_unit = false;
	return new unit(type, side_num, real_unit);
}

}
