/* $Id: mouse_events.cpp 48153 2011-01-01 15:57:50Z mordante $ */
/*
   Copyright (C) 2006 - 2011 by Joerg Hinrichs <joerg.hinrichs@alice-dsl.de>
   wesnoth playturn Copyright (C) 2003 by David White <dave@whitevine.net>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#include "global.hpp"

#include "mouse_events.hpp"

#include "actions.hpp"
#include "attack_prediction_display.hpp"
#include "dialogs.hpp"
#include "foreach.hpp"
#include "game_end_exceptions.hpp"
#include "game_events.hpp"
#include "gettext.hpp"
#include "gui/dialogs/unit_attack.hpp"
#include "gui/widgets/settings.hpp"
#include "gui/dialogs/transient_message.hpp"
#include "gui/widgets/window.hpp"
#include "language.hpp"
#include "log.hpp"
#include "map.hpp"
#include "marked-up_text.hpp"
#include "menu_events.hpp"
#include "play_controller.hpp"
#include "sound.hpp"
#include "replay.hpp"
#include "resources.hpp"
#include "rng.hpp"
#include "tod_manager.hpp"
#include "wml_separators.hpp"
#include "whiteboard/manager.hpp"

#include <boost/bind.hpp>

static lg::log_domain log_engine("engine");
#define ERR_NG LOG_STREAM(err, log_engine)
#define LOG_NG LOG_STREAM(info, log_engine)

namespace events{


mouse_handler::mouse_handler(game_display* gui, std::vector<team>& teams,
		unit_map& units, gamemap& map, tod_manager& tod_mng) :
	mouse_handler_base(),
	map_(map),
	gui_(gui),
	teams_(teams),
	units_(units),
	tod_manager_(tod_mng),
	previous_hex_(),
	previous_free_hex_(),
	selected_hex_(),
	next_unit_(),
	current_route_(),
	waypoints_(),
	current_paths_(),
	enemy_paths_(false),
	path_turns_(0),
	side_num_(1),
	undo_(false),
	over_route_(false),
	reachmap_invalid_(false),
	show_partial_move_(false)
{
	singleton_ = this;
}

mouse_handler::~mouse_handler()
{
	rand_rng::clear_new_seed_callback();
	singleton_ = NULL;
}

void mouse_handler::set_side(int side_number)
{
	side_num_ = side_number;
}

int mouse_handler::drag_threshold() const
{
	return 14;
}

void mouse_handler::mouse_motion(int x, int y, const bool browse, bool update)
{
	// we ignore the position coming from event handler
	// because it's always a little obsolete and we don't need
	// to hightlight all the hexes where the mouse passed.
	// Also, sometimes it seems to have one *very* obsolete
	// and isolated mouse motion event when using drag&drop
	SDL_GetMouseState(&x,&y);  // <-- modify x and y

	if (mouse_handler_base::mouse_motion_default(x, y, update)) return;

	const map_location new_hex = gui().hex_clicked_on(x,y);

	if(new_hex != last_hex_) {
		update = true;
		if (last_hex_.valid()) {
			// we store the previous hexes used to propose attack direction
			previous_hex_ = last_hex_;
			// the hex of the selected unit is also "free"
			{ // start planned pathfind map scope
				wb::scoped_planned_pathfind_map planned_pathfind_map;
				if (last_hex_ == selected_hex_ || find_unit(last_hex_) == units_.end()) {
					previous_free_hex_ = last_hex_;
				}
			} // end planned pathfind map scope
		}
		last_hex_ = new_hex;
	}


	if (reachmap_invalid_) update = true;

	if (update) {
		if (reachmap_invalid_) {
			reachmap_invalid_ = false;
			if (!current_paths_.destinations.empty() && !show_partial_move_) {
				unit_map::iterator u;
				{ // start planned unit map scope
					wb::scoped_planned_unit_map planned_unit_map;
					 u = find_unit(selected_hex_);
				} // end planned unit map scope
				if(selected_hex_.valid() && u != units_.end() ) {
					// reselect the unit without firing events (updates current_paths_)
					select_hex(selected_hex_, true);
				}
				// we do never deselect here, mainly because of canceled attack-move
			}
		}

		// reset current_route_ and current_paths if not valid anymore
		// we do it before cursor selection, because it uses current_paths_
		if(new_hex.valid() == false) {
			current_route_.steps.clear();
			gui().set_route(NULL);
			resources::whiteboard->erase_temp_move();
		}

		if(enemy_paths_) {
			enemy_paths_ = false;
			current_paths_ = pathfind::paths();
			gui().unhighlight_reach();
		} else if(over_route_) {
			over_route_ = false;
			current_route_.steps.clear();
			gui().set_route(NULL);
			resources::whiteboard->erase_temp_move();
		}

		gui().highlight_hex(new_hex);
		resources::whiteboard->on_mouseover_change(new_hex);

		unit_map::iterator selected_unit;
		unit_map::iterator mouseover_unit;
		map_location attack_from;

		{ // start planned unit map scope
			wb::scoped_planned_unit_map planned_unit_map;
			selected_unit = find_unit(selected_hex_);
			mouseover_unit = find_unit(new_hex);

			// we search if there is an attack possibility and where
			attack_from = current_unit_attacks_from(new_hex);

			//see if we should show the normal cursor, the movement cursor, or
			//the attack cursor
			//If the cursor is on WAIT, we don't change it and let the setter
			//of this state end it
			if (cursor::get() != cursor::WAIT) {
				if (selected_unit != units_.end() &&
					selected_unit->side() == side_num_ &&
					!selected_unit->incapacitated() && !browse)
				{
					if (attack_from.valid()) {
						cursor::set(dragging_started_ ? cursor::ATTACK_DRAG : cursor::ATTACK);
					}
					else if (mouseover_unit==units_.end() &&
							 current_paths_.destinations.contains(new_hex))
					{
						cursor::set(dragging_started_ ? cursor::MOVE_DRAG : cursor::MOVE);
					} else {
						// selected unit can't attack or move there
						cursor::set(cursor::NORMAL);
					}
				} else {
					// no selected unit or we can't move it
					cursor::set(cursor::NORMAL);
				}
			}
		} // end planned unit map scope

		// show (or cancel) the attack direction indicator
		if (attack_from.valid() && (!browse || resources::whiteboard->is_active())) {
			gui().set_attack_indicator(attack_from, new_hex);
		} else {
			gui().clear_attack_indicator();
		}

		// the destination is the pointed hex or the adjacent hex
		// used to attack it
		map_location dest;
		unit_map::const_iterator dest_un;
		{ // start planned pathfind map scope
			wb::scoped_planned_pathfind_map planned_pathfind_map;
			if (attack_from.valid()) {
				dest = attack_from;
				dest_un = find_unit(dest);
			}	else {
				dest = new_hex;
				dest_un = find_unit(new_hex);
			}
		} // end planned pathfind map scope

		if(dest == selected_hex_ || dest_un != units_.end()) {
			current_route_.steps.clear();
			gui().set_route(NULL);
			resources::whiteboard->erase_temp_move();
		}
		else if (!current_paths_.destinations.empty() &&
				 map_.on_board(selected_hex_) && map_.on_board(new_hex))
		{
			if (selected_unit != units_.end() && !selected_unit->incapacitated()) {

				// Show the route from selected unit to mouseover hex
				// the movement_reset is active only if it's not the unit's turn
				// Note: we should activate the whiteboard's planned unit map only after this is done,
				// since the future state includes changes to the units' movement points
				unit_movement_resetter move_reset(*selected_unit,
						selected_unit->side() != side_num_);

				{ wb::scoped_planned_pathfind_map future; //< start planned pathfind map scope
					current_route_ = get_route(&*selected_unit, dest, waypoints_, viewing_team());
				} // end planned pathfind map scope

				resources::whiteboard->create_temp_move();

				if(!browse) {
					gui().set_route(&current_route_);
				}
			}
		}

		unit* un;
		{ // start planned unit map scope
			wb::scoped_planned_unit_map planned_unit_map;
			unit_map::iterator iter = mouseover_unit;
			if (iter != units_.end())
				un = &*iter;
			else
				un = NULL;
		} //end planned unit map scope

		if (un && current_paths_.destinations.empty() &&
			!gui().fogged(un->get_location()))
		{
			if (un->side() != side_num_) {
				//unit under cursor is not on our team, highlight reach
				//Note: planned unit map must be activated after this is done,
				//since the future state includes changes to units' movement.
				unit_movement_resetter move_reset(*un);

				bool teleport = un->get_ability_bool("teleport");

				{ // start planned unit map scope
					wb::scoped_planned_pathfind_map planned_pathfind_map;
					current_paths_ = pathfind::paths(map_,units_,new_hex,teams_,
														false,teleport,viewing_team(),path_turns_);
				} // end planned unit map scope

				gui().highlight_reach(current_paths_);
				enemy_paths_ = true;
			} else {
				//unit is on our team, show path if the unit has one
				const map_location go_to = un->get_goto();
				if(map_.on_board(go_to)) {
					pathfind::marked_route route;
					{ // start planned unit map scope
						wb::scoped_planned_pathfind_map planned_pathfind_map;
						route = get_route(un, go_to, un->waypoints(), current_team());
					} // end planned unit map scope
					gui().set_route(&route);
				}
				over_route_ = true;
			}
		}
	}
}

unit_map::iterator mouse_handler::selected_unit()
{
	unit_map::iterator res = find_unit(selected_hex_);
	if(res != units_.end()) {
		return res;
	} else {
		return find_unit(last_hex_);
	}
}

unit_map::iterator mouse_handler::find_unit(const map_location& hex)
{
	unit_map::iterator it = find_visible_unit(hex, viewing_team());
	if (it.valid())
		return it;
	else
		return resources::units->end();
}

unit_map::const_iterator mouse_handler::find_unit(const map_location& hex) const
{
	return find_visible_unit(hex, viewing_team());
}

map_location mouse_handler::current_unit_attacks_from(const map_location& loc)
{
	if(loc == selected_hex_)
		return map_location();

	bool wb_active = resources::whiteboard->is_active();

	{
		const unit_map::const_iterator current = find_unit(selected_hex_);
		bool eligible = current != units_.end();
		if (!eligible) return map_location();

		eligible &= current->side() == side_num_ ||
				(wb_active && current->side() == resources::screen->viewing_side());
		eligible &= current->attacks_left() != 0;
		if (!eligible) return map_location();
	}

	{
		team viewing_team = (*resources::teams)[resources::screen->viewing_team()];

		const unit_map::const_iterator enemy = find_unit(loc);
		bool eligible = enemy != units_.end();
		if (!eligible) return map_location();

		bool show_for_whiteboard = wb_active &&
				viewing_team.is_enemy(enemy->side());
		eligible &= show_for_whiteboard || current_team().is_enemy(enemy->side());
		eligible &= !enemy->incapacitated();
		if (!eligible) return map_location();
	}

	const map_location::DIRECTION preferred = loc.get_relative_dir(previous_hex_);
	const map_location::DIRECTION second_preferred = loc.get_relative_dir(previous_free_hex_);

	int best_rating = 100;//smaller is better
	map_location res;
	map_location adj[6];
	get_adjacent_tiles(loc,adj);

	for(size_t n = 0; n != 6; ++n) {
		if(map_.on_board(adj[n]) == false) {
			continue;
		}

		if(adj[n] != selected_hex_ && find_unit(adj[n]) != units_.end()) {
			continue;
		}

		if (current_paths_.destinations.contains(adj[n]))
		{
			static const size_t NDIRECTIONS = map_location::NDIRECTIONS;
			unsigned int difference = abs(int(preferred - n));
			if(difference > NDIRECTIONS/2) {
				difference = NDIRECTIONS - difference;
			}
			unsigned int second_difference = abs(int(second_preferred - n));
			if(second_difference > NDIRECTIONS/2) {
				second_difference = NDIRECTIONS - second_difference;
			}
			const int rating = difference * 2 + (second_difference > difference);
			if(rating < best_rating || res.valid() == false) {
				best_rating = rating;
				res = adj[n];
			}
		}
	}

	return res;
}

void mouse_handler::add_waypoint(const map_location& loc) {
	std::vector<map_location>::iterator w = std::find(waypoints_.begin(), waypoints_.end(), loc);
	//toggle between add a new one and remove an old one
	if(w != waypoints_.end()){
		waypoints_.erase(w);
	} else {
		waypoints_.push_back(loc);
	}

	// we need to update the route, simulate a mouse move for the moment
	// (browse is supposed false here, 0,0 are dummy values)
	mouse_motion(0,0, false, true);
}

pathfind::marked_route mouse_handler::get_route(unit* un, map_location go_to, const std::vector<map_location>& waypoints, team &team)
{
	// The pathfinder will check unit visibility (fogged/stealthy).
	const pathfind::shortest_path_calculator calc(*un, team, units_, teams_, map_);

	std::set<map_location> allowed_teleports = pathfind::get_teleport_locations(*un, viewing_team());

	pathfind::plain_route route;

	if (waypoints.empty()) {
		// standard shortest path
		route = pathfind::a_star_search(un->get_location(), go_to, 10000.0, &calc, map_.w(), map_.h(), &allowed_teleports);
	} else {
		// initialize the main route with the first step
		route.steps.push_back(un->get_location());
		route.move_cost = 0;

		//copy waypoints and add first source and last destination
		//TODO: don't copy but use vector index trick
		std::vector<map_location> waypts;
		waypts.push_back(un->get_location());
		waypts.insert(waypts.end(), waypoints.begin(), waypoints.end());
		waypts.push_back(go_to);

		std::vector<map_location>::iterator src = waypts.begin(),
			dst = ++waypts.begin();
		for(; dst != waypts.end(); ++src,++dst){
			if (*src == *dst) continue;
			pathfind::plain_route inter_route = pathfind::a_star_search(*src, *dst, 10000.0, &calc, map_.w(), map_.h(), &allowed_teleports);
			if(inter_route.steps.size()>=1) {
				// add to the main route but skip the head (already in)
				route.steps.insert(route.steps.end(),
					inter_route.steps.begin()+1,inter_route.steps.end());
				route.move_cost+=inter_route.move_cost;
			} else {
				// we can't reach dst, stop the route at the last src
				// as the normal case do
				break;
			}
		}
	}

	return mark_route(route, waypoints);
}

void mouse_handler::mouse_press(const SDL_MouseButtonEvent& event, const bool browse)
{
	mouse_handler_base::mouse_press(event, browse);
}

bool mouse_handler::right_click_show_menu(int x, int y, const bool browse)
{
	// The first right-click cancel the selection if any,
	// the second open the context menu
	unit_map::iterator unit;
	{
		wb::scoped_planned_unit_map wb_modifiers;
		unit = find_unit(selected_hex_);
	}
	if (selected_hex_.valid() && unit != units_.end()) {
		select_hex(map_location(), browse);
		return false;
	} else {
		return point_in_rect(x, y, gui().map_area());
	}
}

bool mouse_handler::left_click(int x, int y, const bool browse)
{
	undo_ = false;
	if (mouse_handler_base::left_click(x, y, browse)) return false;

	//we use the last registered highlighted hex
	//since it's what update our global state
	map_location hex = last_hex_;

	unit_map::iterator u;
	unit_map::iterator clicked_u;
	map_location src;
	pathfind::paths orig_paths;
	map_location attack_from;
	{ // start planned unit map scope
		wb::scoped_planned_unit_map planned_unit_map;
		u = find_unit(selected_hex_);

		//if the unit is selected and then itself clicked on,
		//any goto command and waypoints are cancelled
		if (u != units_.end() && !browse && selected_hex_ == hex && u->side() == side_num_) {
			u->set_goto(map_location());
			u->waypoints().clear();
			waypoints_.clear();
		}

		clicked_u = find_unit(hex);

		src = selected_hex_;
		orig_paths = current_paths_;
		attack_from = current_unit_attacks_from(hex);
	} // end planned unit map scope

	//see if we're trying to do a attack or move-and-attack
	if(((!browse && !commands_disabled) || resources::whiteboard->is_active()) && attack_from.valid()) {
		if (resources::whiteboard->is_active() && clicked_u.valid()) {
			// Unselect the current hex, and create planned attack for whiteboard
			selected_hex_ = map_location();
			gui().select_hex(map_location());
			gui().clear_attack_indicator();
			gui().set_route(NULL);
			waypoints_.clear();
			show_partial_move_ = false;
			gui().unhighlight_reach();
			current_paths_ = pathfind::paths();
			current_route_.steps.clear();

			resources::whiteboard->save_temp_attack(attack_from, clicked_u->get_location());
			return false;

		} else if (u.valid() && clicked_u.valid() && u->side() == side_num_) {
			if (attack_from == selected_hex_) { //no move needed
				int choice = show_attack_dialog(attack_from, clicked_u->get_location());
				if (choice >=0 ) {
					attack_enemy(u->get_location(), clicked_u->get_location(), choice);
				}
				return false;
			}
			else {
				// we will now temporary move next to the enemy
				pathfind::paths::dest_vect::const_iterator itor =
						current_paths_.destinations.find(attack_from);
				if(itor == current_paths_.destinations.end()) {
					// can't reach the attacking location
					// not supposed to happen, so abort
					return false;
				}
				// update movement_left as if we did the move
				int move_left_dst = itor->move_left;
				int move_left_src = u->movement_left();
				u->set_movement(move_left_dst);

				int choice = -1;
				// block where we temporary move the unit
				{
					temporary_unit_mover temp_mover(units_, src, attack_from);
					choice = show_attack_dialog(attack_from, clicked_u->get_location());
				}
				// restore unit as before
				u = units_.find(src);
				u->set_movement(move_left_src);
				u->set_standing();

				if (choice < 0) {
					// user hit cancel, don't start move+attack
					return false;
				}

				//register the mouse-UI waypoints into the unit's waypoints
				u->waypoints() = waypoints_;

				// store side, since u may be invalidated later
				int side = u->side();
				//record visible enemies adjacent to destination
				std::set<map_location> adj_enemies = get_adj_enemies(attack_from, side);

				// move the unit without clearing fog (to avoid interruption)
				//TODO: clear fog and interrupt+resume move
				if(!move_unit_along_current_route(false)) {
					// interrupted move
					// we assume that move_unit() did the cleaning
					// (update shroud/fog, clear undo if needed)
					return false;
				}

				//check if new enemies are now visible
				if(get_adj_enemies(attack_from, side) != adj_enemies)
					return false; //ambush, interrupt attack

				attack_enemy(attack_from, hex, choice); // Fight !!
				return false;
			}
		}
	}
	//otherwise we're trying to move to a hex
	else if(((!commands_disabled && !browse) || resources::whiteboard->is_active()) &&
			selected_hex_.valid() && selected_hex_ != hex &&
	         u != units_.end() && u.valid() &&
	         (u->side() == side_num_ || resources::whiteboard->is_active()) &&
		     clicked_u == units_.end() &&
		     !current_route_.steps.empty() &&
		     current_route_.steps.front() == selected_hex_) {

		gui().unhighlight_reach();

		// If the whiteboard is active, it intercepts any unit movement
		if (resources::whiteboard->is_active()) {
				// Unselect the current hex, and create planned move for whiteboard
				selected_hex_ = map_location();
				gui().select_hex(map_location());
				gui().clear_attack_indicator();
				gui().set_route(NULL);
				waypoints_.clear();
				show_partial_move_ = false;
				gui().unhighlight_reach();
				current_paths_ = pathfind::paths();
				current_route_.steps.clear();

				resources::whiteboard->save_temp_move();

		// Otherwise proceed to normal unit movement
		} else {
			//Don't move if the selected unit already has actions
			//from the whiteboard.
			if (resources::whiteboard->unit_has_actions(&*u)) {
				return false;
			}

			//If this is a leader on a keep, ask permission to the whiteboard to move it
			//since otherwise it may cause planned recruits to be erased.
			if (u->can_recruit() &&	u->side() == gui().viewing_side() &&
				resources::game_map->is_keep(u->get_location()) &&
				!resources::whiteboard->allow_leader_to_move(*u))
			{
				gui2::show_transient_message(gui_->video(), "",
						_("You cannot move your leader away from the keep with some planned recruits left."));
				return false;
			}

			//register the mouse-UI waypoints into the unit's waypoints
			u->waypoints() = waypoints_;

			move_unit_along_current_route(current_team().auto_shroud_updates());
			// during the move, we may have selected another unit
			// (but without triggering a select event (command was disabled)
			// in that case reselect it now to fire the event (+ anim & sound)
			if (selected_hex_ != src) {
				select_hex(selected_hex_, browse);
			}
		}
		return false;
	} else {
		// we select a (maybe empty) hex
		// we block selection during attack+move (because motion is blocked)
		select_hex(hex, browse);
	}
	return false;
	//FIXME: clean all these "return false"
}

void mouse_handler::select_hex(const map_location& hex, const bool browse) {
	selected_hex_ = hex;
	gui().select_hex(hex);
	gui().clear_attack_indicator();
	gui().set_route(NULL);
	waypoints_.clear();
	show_partial_move_ = false;

	wb::scoped_planned_unit_map planned_unit_map; //lasts for whole method

	unit_map::iterator u = find_unit(hex);

	if (hex.valid() && u != units_.end() && u.valid() && !u->get_hidden()) {

		next_unit_ = u->get_location();

		{
			// if it's not the unit's turn, we reset its moves
			// and we restore them before the "select" event is raised
			// Ensure the planned unit map is reapplied afterwards, otherwise it screws up the future state
			{ // start enforced real unit map scope
				wb::scoped_real_unit_map real_unit_map;
				unit_movement_resetter move_reset(*u, u->side() != side_num_);
			} // end enforced real unit map scope
			bool teleport = u->get_ability_bool("teleport");

			current_paths_ = pathfind::paths(map_, units_, hex, teams_,
				false, teleport, viewing_team(), path_turns_);
		}
		show_attack_options(u);
		gui().highlight_reach(current_paths_);
		// the highlight now comes from selection
		// and not from the mouseover on an enemy
		enemy_paths_ = false;
		gui().set_route(NULL);

		// selection have impact only if we are not observing and it's our unit
		if ((!commands_disabled || resources::whiteboard->is_active()) && u->side() == gui().viewing_side()) {
			if (!(browse || resources::whiteboard->unit_has_actions(&*u)))
			{
				sound::play_UI_sound("select-unit.wav");
				u->set_selecting();
				game_events::fire("select", hex);
			}
		}

	} else {
		gui().unhighlight_reach();
		current_paths_ = pathfind::paths();
		current_route_.steps.clear();
		resources::whiteboard->on_deselect_hex();
	}
}

void mouse_handler::deselect_hex() {
	select_hex(map_location(), true);
}

bool mouse_handler::move_unit_along_current_route(bool check_shroud)
{
	// do not show footsteps during movement
	gui().set_route(NULL);

	// do not keep the hex highlighted that we started from
	selected_hex_ = map_location();
	gui().select_hex(map_location());

	bool finished_moves = move_unit_along_route(current_route_, &next_unit_, check_shroud);

	// invalid after the move
	current_paths_ = pathfind::paths();
	current_route_.steps.clear();

	return finished_moves;
}

bool mouse_handler::move_unit_along_route(pathfind::marked_route const& route, map_location* next_unit, bool check_shroud)
{
	const std::vector<map_location> steps = route.steps;
	if(steps.empty()) {
		return false;
	}

	size_t moves = 0;
	try {
			moves = ::move_unit(NULL, steps, &recorder, resources::undo_stack, true, next_unit, false, check_shroud);
	} catch(end_turn_exception&) {
		cursor::set(cursor::NORMAL);
		gui().invalidate_game_status();
		throw;
	}

	cursor::set(cursor::NORMAL);
	gui().invalidate_game_status();

	if(moves == 0)
		return false;

	resources::redo_stack->clear();

	assert(moves <= steps.size());
	const map_location& dst = steps[moves-1];
	const unit_map::const_iterator u = units_.find(dst);

	//u may be equal to units_.end() in the case of e.g. a [teleport]
	if(u != units_.end()) {
		if(dst != steps.back()) {
			// the move was interrupted (or never started)
			if (u->movement_left() > 0) {
				// reselect the unit (for "press t to continue")
				select_hex(dst, false);
				// the new discovery is more important than the new movement range
				show_partial_move_ = true;
				gui().unhighlight_reach();
			}
		}
	}

	return moves == steps.size();
}

int mouse_handler::fill_weapon_choices(std::vector<battle_context>& bc_vector, unit_map::iterator attacker, unit_map::iterator defender)
{
	int best = 0;
	for (unsigned int i = 0; i < attacker->attacks().size(); i++) {
		// skip weapons with attack_weight=0
		if (attacker->attacks()[i].attack_weight() > 0) {
			battle_context bc(*resources::units, attacker->get_location(), defender->get_location(), i);
			bc_vector.push_back(bc);
			if (bc.better_attack(bc_vector[best], 0.5)) {
				// as some weapons can be hidden, i is not a valid index into the resulting vector
				best = bc_vector.size() - 1;
			}
		}
	}
	return best;
}

int mouse_handler::show_attack_dialog(const map_location& attacker_loc, const map_location& defender_loc)
{

	unit_map::iterator attacker = units_.find(attacker_loc);
	unit_map::iterator defender = units_.find(defender_loc);
	if(attacker == units_.end() || defender == units_.end()) {
		ERR_NG << "One fighter is missing, can't attack";
		return -1; // abort, click will do nothing
	}

	std::vector<battle_context> bc_vector;
	const int best = fill_weapon_choices(bc_vector, attacker, defender);

	if(gui2::new_widgets) {
		gui2::tunit_attack dlg(
				  attacker
				, defender
				, bc_vector
				, best);

		dlg.show(resources::screen->video());

		if(dlg.get_retval() == gui2::twindow::OK) {
			return dlg.get_selected_weapon();
		} else {
			return -1;
		}
	}

	if (bc_vector.empty())
	{
		dialogs::units_list_preview_pane attacker_preview(&*attacker, dialogs::unit_preview_pane::SHOW_BASIC, true);
		dialogs::units_list_preview_pane defender_preview(&*defender, dialogs::unit_preview_pane::SHOW_BASIC, false);
		std::vector<gui::preview_pane*> preview_panes;
		preview_panes.push_back(&attacker_preview);
		preview_panes.push_back(&defender_preview);

		gui::show_dialog(gui(), NULL, _("Attack Enemy"),
			_("No usable weapon"), gui::CANCEL_ONLY, NULL,
			&preview_panes, "", NULL, -1, NULL, -1, -1, NULL, NULL);
		return -1;
	}


	std::vector<std::string> items;

	for (unsigned int i = 0; i < bc_vector.size(); i++) {
		const battle_context_unit_stats& att = bc_vector[i].get_attacker_stats();
		const battle_context_unit_stats& def = bc_vector[i].get_defender_stats();
		config tmp_config;
		attack_type no_weapon(tmp_config);
		const attack_type& attw = attack_type(*att.weapon);
		const attack_type& defw = attack_type(def.weapon ? *def.weapon : no_weapon);

		attw.set_specials_context(attacker->get_location(), defender->get_location(), *attacker, true);
		defw.set_specials_context(attacker->get_location(), defender->get_location(), *attacker, false);

		// if missing, add dummy special, to be sure to have
		// big enough minimum width (weapon's name can be very short)
		std::string att_weapon_special = attw.weapon_specials();
		if (att_weapon_special.empty())
			att_weapon_special += "       ";
		std::string def_weapon_special = defw.weapon_specials();
		if (def_weapon_special.empty())
			def_weapon_special += "       ";

		std::stringstream atts;
		if (static_cast<int>(i) == best) {
			atts << DEFAULT_ITEM;
		}

		std::string range = attw.range().empty() ? defw.range() : attw.range();
		if (!range.empty()) {
			range = string_table["range_" + range];
		}

		// add dummy names if missing, to keep stats aligned
		std::string attw_name = attw.name();
		if(attw_name.empty())
			attw_name = " ";
		std::string defw_name = defw.name();
		if(defw_name.empty())
			defw_name = " ";

		// color CtH in red-yellow-green
		SDL_Color att_cth_color =
				int_to_color( game_config::red_to_green(att.chance_to_hit) );
		SDL_Color def_cth_color =
				int_to_color( game_config::red_to_green(def.chance_to_hit) );

		atts << IMAGE_PREFIX << attw.icon() << COLUMN_SEPARATOR
			 << font::BOLD_TEXT << attw_name  << "\n"
			 << att.damage << font::weapon_numbers_sep << att.num_blows
			 << "  " << att_weapon_special << "\n"
			 << font::color2markup(att_cth_color) << att.chance_to_hit << "%"
			 << COLUMN_SEPARATOR << font::weapon_details << "- " << range << " -" << COLUMN_SEPARATOR
			 << font::BOLD_TEXT << defw_name  << "\n"
			 << def.damage << font::weapon_numbers_sep << def.num_blows
			 << "  " << def_weapon_special << "\n"
			 << font::color2markup(def_cth_color) << def.chance_to_hit << "%"
			 << COLUMN_SEPARATOR << IMAGE_PREFIX << defw.icon();

		items.push_back(atts.str());
	}

	attack_prediction_displayer ap_displayer(bc_vector, attacker_loc, defender_loc);
	std::vector<gui::dialog_button_info> buttons;
	buttons.push_back(gui::dialog_button_info(&ap_displayer, _("Damage Calculations")));

	int res = 0;
	{
		dialogs::units_list_preview_pane attacker_preview(&*attacker, dialogs::unit_preview_pane::SHOW_BASIC, true);
		dialogs::units_list_preview_pane defender_preview(&*defender, dialogs::unit_preview_pane::SHOW_BASIC, false);
		std::vector<gui::preview_pane*> preview_panes;
		preview_panes.push_back(&attacker_preview);
		preview_panes.push_back(&defender_preview);

		res = gui::show_dialog(gui(),NULL,_("Attack Enemy"),
				_("Choose weapon:")+std::string("\n"),
				gui::OK_CANCEL,&items,&preview_panes,"",NULL,-1,NULL,-1,-1,
				NULL,&buttons);
	}
	cursor::set(cursor::NORMAL);

	return res;
}

void mouse_handler::attack_enemy(const map_location& attacker_loc, const map_location& defender_loc, int choice)
{
	try {
		attack_enemy_(attacker_loc, defender_loc, choice);
	} catch(std::bad_alloc) {
		lg::wml_error << "Memory exhausted a unit has either a lot hitpoints or a negative amount.\n";
	}
}

void mouse_handler::attack_enemy_(const map_location& att_loc
		, const map_location& def_loc
		, int choice)
{
	//NOTE: copy the values because the const reference may change!
	//(WML events and mouse inputs during animations may modify
	// the data of the caller)
	const map_location attacker_loc = att_loc;
	const map_location defender_loc = def_loc;

	//may fire event and modify things
	apply_shroud_changes(*resources::undo_stack, side_num_);
	resources::undo_stack->clear();
	resources::redo_stack->clear();

	unit_map::iterator attacker = find_unit(attacker_loc);
	if(attacker == units_.end()
			|| attacker->side() != side_num_
			|| attacker->incapacitated())
		return;

	unit_map::iterator defender = find_unit(defender_loc);
	if(defender == units_.end()
			|| current_team().is_enemy(defender->side()) == false
			|| defender->incapacitated())
		return;

	std::vector<battle_context> bc_vector;
	fill_weapon_choices(bc_vector, attacker, defender);

	if(size_t(choice) >= bc_vector.size()) {
		return;
	}

	commands_disabled++;
	const battle_context_unit_stats &att = bc_vector[choice].get_attacker_stats();
	const battle_context_unit_stats &def = bc_vector[choice].get_defender_stats();

	attacker->set_goto(map_location());

	current_paths_ = pathfind::paths();
	// make the attacker's stats appear during the attack
	gui().display_unit_hex(attacker_loc);
	// remove highlighted hexes etc..
	gui().select_hex(map_location());
	gui().highlight_hex(map_location());
	gui().clear_attack_indicator();
	gui().unhighlight_reach();
	gui().draw();

	///@todo change ToD to be location specific for the defender
	recorder.add_attack(attacker_loc, defender_loc, att.attack_num, def.attack_num,
		attacker->type_id(), defender->type_id(), att.level,
		def.level, resources::tod_manager->turn(), resources::tod_manager->get_time_of_day());
	rand_rng::invalidate_seed();
	if (rand_rng::has_valid_seed()) { //means SRNG is disabled
		perform_attack(attacker_loc, defender_loc, att.attack_num, def.attack_num, rand_rng::get_last_seed());
	} else {
		rand_rng::set_new_seed_callback(boost::bind(&mouse_handler::perform_attack,
			this, attacker_loc, defender_loc, att.attack_num, def.attack_num, _1));
	}
}

void mouse_handler::perform_attack(
	map_location attacker_loc, map_location defender_loc,
	int attacker_weapon, int defender_weapon, int seed)
{
	// this function gets it's arguments by value because the calling function
	// object might get deleted in the clear callback call below, invalidating
	// const ref arguments
	rand_rng::clear_new_seed_callback();
	LOG_NG << "Performing attack with seed " << seed << "\n";
	recorder.add_seed("attack", seed);
	//MP_COUNTDOWN grant time bonus for attacking
	current_team().set_action_bonus_count(1 + current_team().action_bonus_count());

	try {
		events::command_disabler disabler; // Rather than decrementing for every possible exception, use RAII
		commands_disabled--;
		attack_unit(attacker_loc, defender_loc, attacker_weapon, defender_weapon);
	} catch(end_level_exception&) {
		//if the level ends due to a unit being killed, still see if
		//either the attacker or defender should advance
		dialogs::advance_unit(attacker_loc);
		unit_map::const_iterator defu = units_.find(defender_loc);
		if (defu != units_.end()) {
			bool defender_human = teams_[defu->side() - 1].is_human();
			dialogs::advance_unit(defender_loc, !defender_human);
		}
		throw;
	}

	dialogs::advance_unit(attacker_loc);
	unit_map::const_iterator defu = units_.find(defender_loc);
	if (defu != units_.end()) {
		bool defender_human = teams_[defu->side() - 1].is_human();
		dialogs::advance_unit(defender_loc, !defender_human);
	}

	resources::controller->check_victory();
	gui().draw();
}

std::set<map_location> mouse_handler::get_adj_enemies(const map_location& loc, int side) const
{
	std::set<map_location> res;

	const team& uteam = teams_[side-1];

	map_location adj[6];
	get_adjacent_tiles(loc, adj);
	foreach (const map_location &aloc, adj) {
		unit_map::const_iterator i = find_unit(aloc);
		if (i != units_.end() && uteam.is_enemy(i->side()))
			res.insert(aloc);
	}
	return res;
}

void mouse_handler::show_attack_options(const unit_map::const_iterator &u)
{
	map_location adj[6];
	get_adjacent_tiles(u->get_location(), adj);
	foreach (const map_location &loc, adj)
	{
		if (!map_.on_board(loc)) continue;
		unit_map::const_iterator i = units_.find(loc);
		if (i == units_.end()) continue;
		const unit &target = *i;
		if (current_team().is_enemy(target.side()) && !target.incapacitated())
			current_paths_.destinations.insert(loc);
	}
}

bool mouse_handler::unit_in_cycle(unit_map::const_iterator it)
{
	if (it == units_.end())
		return false;

	if (it->side() != side_num_ || it->user_end_turn()
	    || gui().fogged(it->get_location()) || !unit_can_move(*it))
		return false;

	if (current_team().is_enemy(int(gui().viewing_team()+1)) &&
	    it->invisible(it->get_location()))
		return false;

	if (it->get_hidden())
		return false;

	return true;

}

void mouse_handler::cycle_units(const bool browse, const bool reverse)
{
	if (units_.begin() == units_.end()) {
		return;
	}

	unit_map::const_iterator it = find_unit(next_unit_);
	if (it == units_.end())
		it = units_.begin();
	const unit_map::const_iterator itx = it;

	do {
		if (reverse) {
			if (it == units_.begin())
				it = units_.end();
			--it;
		} else {
			if (it == units_.end())
				it = units_.begin();
			else
				++it;
		}
	} while (it != itx && !unit_in_cycle(it));

	if (unit_in_cycle(it)) {
		gui().scroll_to_tile(it->get_location(), game_display::WARP);
		select_hex(it->get_location(), browse);
		mouse_update(browse);
	}
}

void mouse_handler::set_current_paths(pathfind::paths new_paths) {
	gui().unhighlight_reach();
	current_paths_ = new_paths;
	current_route_.steps.clear();
	gui().set_route(NULL);
	resources::whiteboard->erase_temp_move();
}

mouse_handler *mouse_handler::singleton_ = NULL;
}
