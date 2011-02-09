/* $Id: aspect_attacks.cpp 48196 2011-01-03 22:49:18Z crab $ */
/*
   Copyright (C) 2009 - 2011 by Yurii Chernyi <terraninfo@terraninfo.net>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

/**
 * Stage: fallback to other AI
 * @file
 */

#include "aspect_attacks.hpp"

#include "../manager.hpp"
#include "../../actions.hpp"
#include "../../foreach.hpp"
#include "../../log.hpp"
#include "../../map.hpp"
#include "../../team.hpp"
#include "../../tod_manager.hpp"
#include "../../resources.hpp"
#include "../../unit.hpp"
#include "../../pathfind/pathfind.hpp"

namespace ai {

namespace testing_ai_default {

static lg::log_domain log_ai_testing_aspect_attacks("ai/aspect/attacks");
#define DBG_AI LOG_STREAM(debug, log_ai_testing_aspect_attacks)
#define LOG_AI LOG_STREAM(info, log_ai_testing_aspect_attacks)
#define ERR_AI LOG_STREAM(err, log_ai_testing_aspect_attacks)

aspect_attacks::aspect_attacks(readonly_context &context, const config &cfg, const std::string &id)
	: typesafe_aspect<attacks_vector>(context,cfg,id)
	, filter_own_()
	, filter_enemy_()
{
	if (const config &filter_own = cfg.child("filter_own")) {
		filter_own_ = filter_own;
	}
	if (const config &filter_enemy = cfg.child("filter_enemy")) {
		filter_enemy_ = filter_enemy;
	}
}

aspect_attacks::~aspect_attacks()
{
}

void aspect_attacks::recalculate() const
{
	this->value_ = analyze_targets();
	this->valid_ = true;
}

boost::shared_ptr<attacks_vector> aspect_attacks::analyze_targets() const
{
		const move_map& srcdst = get_srcdst();
		const move_map& dstsrc = get_dstsrc();
		const move_map& enemy_srcdst = get_enemy_srcdst();
		const move_map& enemy_dstsrc = get_enemy_dstsrc();

		boost::shared_ptr<attacks_vector> res(new attacks_vector());
		unit_map& units_ = *resources::units;

		std::vector<map_location> unit_locs;
		for(unit_map::const_iterator i = units_.begin(); i != units_.end(); ++i) {
			if (i->side() == get_side() && i->attacks_left() && !(i->can_recruit() && get_passive_leader())) {
				if (!i->matches_filter(vconfig(filter_own_), i->get_location())) {
					continue;
				}
				unit_locs.push_back(i->get_location());
			}
		}

		bool used_locations[6];
		std::fill(used_locations,used_locations+6,false);

		moves_map dummy_moves;
		move_map fullmove_srcdst, fullmove_dstsrc;
		calculate_possible_moves(dummy_moves,fullmove_srcdst,fullmove_dstsrc,false,true);

		unit_stats_cache().clear();

		for(unit_map::const_iterator j = units_.begin(); j != units_.end(); ++j) {

		// Attack anyone who is on the enemy side,
		// and who is not invisible or petrified.
		if (current_team().is_enemy(j->side()) && !j->incapacitated() &&
		    !j->invisible(j->get_location()))
		{
			if (!j->matches_filter(vconfig(filter_enemy_), j->get_location())) {
				continue;
			}
			map_location adjacent[6];
			get_adjacent_tiles(j->get_location(), adjacent);
			attack_analysis analysis;
			analysis.target = j->get_location();
			analysis.vulnerability = 0.0;
			analysis.support = 0.0;
			do_attack_analysis(j->get_location(), srcdst, dstsrc,
				fullmove_srcdst, fullmove_dstsrc, enemy_srcdst, enemy_dstsrc,
				adjacent,used_locations,unit_locs,*res,analysis, current_team(), this);
		}
	}
	return res;
}



void aspect_attacks::do_attack_analysis(
	                 const map_location& loc,
	                 const move_map& srcdst, const move_map& dstsrc,
					 const move_map& fullmove_srcdst, const move_map& fullmove_dstsrc,
	                 const move_map& enemy_srcdst, const move_map& enemy_dstsrc,
					 const map_location* tiles, bool* used_locations,
	                 std::vector<map_location>& units,
	                 std::vector<attack_analysis>& result,
					 attack_analysis& cur_analysis,
					 const team &current_team,
					 const readonly_context *ai_obj
	                )
{
	// This function is called fairly frequently, so interact with the user here.

	ai::manager::raise_user_interact();
	const int default_attack_depth = 5;
	if(cur_analysis.movements.size() >= size_t(default_attack_depth)) {
		//std::cerr << "ANALYSIS " << cur_analysis.movements.size() << " >= " << get_attack_depth() << "\n";
		return;
	}
	gamemap &map_ = *resources::game_map;
	unit_map &units_ = *resources::units;
	std::vector<team> &teams_ = *resources::teams;


	const size_t max_positions = 1000;
	if(result.size() > max_positions && !cur_analysis.movements.empty()) {
		LOG_AI << "cut analysis short with number of positions\n";
		return;
	}

	for(size_t i = 0; i != units.size(); ++i) {
		const map_location current_unit = units[i];

		unit_map::iterator unit_itor = units_.find(current_unit);
		assert(unit_itor != units_.end());

		// See if the unit has the backstab ability.
		// Units with backstab will want to try to have a
		// friendly unit opposite the position they move to.
		//
		// See if the unit has the slow ability -- units with slow only attack first.
		bool backstab = false, slow = false;
		std::vector<attack_type>& attacks = unit_itor->attacks();
		for(std::vector<attack_type>::iterator a = attacks.begin(); a != attacks.end(); ++a) {
			a->set_specials_context(map_location(), map_location(), units_, true, NULL);
			if(a->get_special_bool("backstab")) {
				backstab = true;
			}

			if(a->get_special_bool("slow")) {
				slow = true;
			}
		}

		if(slow && cur_analysis.movements.empty() == false) {
			continue;
		}

               // Check if the friendly unit is surrounded,
			   // A unit is surrounded if it is flanked by enemy units
			   // and at least one other enemy unit is nearby
			   // or if the unit is totaly surrounded by enemies
			   // with max. one tile to escape.
               bool is_surrounded = false;
               bool is_flanked = false;
               int enemy_units_around = 0;
               int accessible_tiles = 0;
               map_location adj[6];
               get_adjacent_tiles(current_unit, adj);

               size_t tile;
               for(tile = 0; tile != 3; ++tile) {

                       const unit_map::const_iterator tmp_unit = units_.find(adj[tile]);
                       bool possible_flanked = false;

                       if(map_.on_board(adj[tile]))
                       {
                               accessible_tiles++;
					   if (tmp_unit != units_.end() && current_team.is_enemy(tmp_unit->side()))
                               {
                                       enemy_units_around++;
                                       possible_flanked = true;
                               }
                       }

                       const unit_map::const_iterator tmp_opposite_unit = units_.find(adj[tile + 3]);
                        if(map_.on_board(adj[tile + 3]))
                       {
                               accessible_tiles++;
					   if (tmp_opposite_unit != units_.end() && current_team.is_enemy(tmp_opposite_unit->side()))
                               {
                                       enemy_units_around++;
                                       if(possible_flanked)
                                       {
                                               is_flanked = true;
                                       }
                               }
                       }
               }

               if((is_flanked && enemy_units_around > 2) || enemy_units_around >= accessible_tiles - 1)
                       is_surrounded = true;



		double best_vulnerability = 0.0, best_support = 0.0;
		int best_rating = 0;
		int cur_position = -1;

		// Iterate over positions adjacent to the unit, finding the best rated one.
		for(int j = 0; j != 6; ++j) {

			// If in this planned attack, a unit is already in this location.
			if(used_locations[j]) {
				continue;
			}

			// See if the current unit can reach that position.
			if (tiles[j] != current_unit) {
				typedef std::multimap<map_location,map_location>::const_iterator Itor;
				std::pair<Itor,Itor> its = dstsrc.equal_range(tiles[j]);
				while(its.first != its.second) {
					if(its.first->second == current_unit)
						break;
					++its.first;
				}

				// If the unit can't move to this location.
				if(its.first == its.second || units_.find(tiles[j]) != units_.end()) {
					continue;
				}
			}

			unit_ability_list abil = unit_itor->get_abilities("leadership",tiles[j]);
			int best_leadership_bonus = abil.highest("value").first;
			double leadership_bonus = static_cast<double>(best_leadership_bonus+100)/100.0;
			if (leadership_bonus > 1.1) {
				LOG_AI << unit_itor->name() << " is getting leadership " << leadership_bonus << "\n";
			}

			// Check to see whether this move would be a backstab.
			int backstab_bonus = 1;
			double surround_bonus = 1.0;

			if(tiles[(j+3)%6] != current_unit) {
				const unit_map::const_iterator itor = units_.find(tiles[(j+3)%6]);

				// Note that we *could* also check if a unit plans to move there
				// before we're at this stage, but we don't because, since the
				// attack calculations don't actually take backstab into account (too complicated),
				// this could actually make our analysis look *worse* instead of better.
				// So we only check for 'concrete' backstab opportunities.
				// That would also break backstab_check, since it assumes
				// the defender is in place.
				if(itor != units_.end() &&
					backstab_check(tiles[j], loc, units_, teams_)) {
					if(backstab) {
						backstab_bonus = 2;
					}

					// No surround bonus if target is skirmisher
					if (!itor->get_ability_bool("skirmisker"))
						surround_bonus = 1.2;
				}


			}

			// See if this position is the best rated we've seen so far.
			int rating = static_cast<int>(rate_terrain(*unit_itor, tiles[j]) * backstab_bonus * leadership_bonus);
			if(cur_position >= 0 && rating < best_rating) {
				continue;
			}

			// Find out how vulnerable we are to attack from enemy units in this hex.
			//FIXME: suokko's r29531 multiplied this by a constant 1.5. ?
			const double vulnerability = power_projection2(tiles[j],enemy_dstsrc);//?

			// Calculate how much support we have on this hex from allies.
			const double support = power_projection2(tiles[j], fullmove_dstsrc);//?

			// If this is a position with equal defense to another position,
			// but more vulnerability then we don't want to use it.
#ifdef SUOKKO
			//FIXME: this code was in sukko's r29531  Correct?
			// scale vulnerability to 60 hp unit
			if(cur_position >= 0 && rating < best_rating
					&& (vulnerability/surround_bonus*30.0)/unit_itor->second.hitpoints() -
						(support*surround_bonus*30.0)/unit_itor->second.max_hitpoints()
						> best_vulnerability - best_support) {
				continue;
			}
#else
			if(cur_position >= 0 && rating == best_rating && vulnerability/surround_bonus - support*surround_bonus >= best_vulnerability - best_support) {
				continue;
			}
#endif
			cur_position = j;
			best_rating = rating;
#ifdef SUOKKO
			//FIXME: this code was in sukko's r29531  Correct?
			best_vulnerability = (vulnerability/surround_bonus*30.0)/unit_itor->second.hitpoints();
			best_support = (support*surround_bonus*30.0)/unit_itor->second.max_hitpoints();
#else
			best_vulnerability = vulnerability/surround_bonus;
			best_support = support*surround_bonus;
#endif
		}

		if(cur_position != -1) {
			units.erase(units.begin() + i);

			cur_analysis.movements.push_back(std::pair<map_location,map_location>(current_unit,tiles[cur_position]));

			cur_analysis.vulnerability += best_vulnerability;

			cur_analysis.support += best_support;

			cur_analysis.is_surrounded = is_surrounded;
			if (ai_obj!=NULL) {
			   cur_analysis.analyze(map_, units_, *ai_obj, dstsrc, srcdst, enemy_dstsrc, ai_obj->get_aggression());
			}
			result.push_back(cur_analysis);
			used_locations[cur_position] = true;
			do_attack_analysis(loc,srcdst,dstsrc,fullmove_srcdst,fullmove_dstsrc,enemy_srcdst,enemy_dstsrc,
		                   tiles,used_locations,
		                   units,result,cur_analysis, current_team, ai_obj);
			used_locations[cur_position] = false;


			cur_analysis.vulnerability -= best_vulnerability;
			cur_analysis.support -= best_support;

			cur_analysis.movements.pop_back();

			units.insert(units.begin() + i, current_unit);
		}
	}
}

int aspect_attacks::rate_terrain(const unit& u, const map_location& loc)
{
	gamemap &map_ = *resources::game_map;
	const t_translation::t_terrain terrain = map_.get_terrain(loc);
	const int defense = u.defense_modifier(terrain);
	int rating = 100 - defense;

	const int healing_value = 10;
	const int friendly_village_value = 5;
	const int neutral_village_value = 10;
	const int enemy_village_value = 15;

	if(map_.gives_healing(terrain) && u.get_ability_bool("regenerates",loc) == false) {
		rating += healing_value;
	}

	if(map_.is_village(terrain)) {
		int owner = village_owner(loc, *resources::teams) + 1;

		if(owner == u.side()) {
			rating += friendly_village_value;
		} else if(owner == 0) {
			rating += neutral_village_value;
		} else {
			rating += enemy_village_value;
		}
	}

	return rating;
}


config aspect_attacks::to_config() const
{
	config cfg = typesafe_aspect<attacks_vector>::to_config();
	if (!filter_own_.empty()) {
		cfg.add_child("filter_own",filter_own_);
	}
	if (!filter_enemy_.empty()) {
		cfg.add_child("filter_enemy",filter_enemy_);
	}
	return cfg;
}

double aspect_attacks::power_projection(const map_location& loc, const move_map& dstsrc) const
{
	map_location used_locs[6];
	int ratings[6];
	int num_used_locs = 0;

	map_location locs[6];
	get_adjacent_tiles(loc,locs);

	const int lawful_bonus = resources::tod_manager->get_time_of_day().lawful_bonus;
	gamemap& map_ = *resources::game_map;
	unit_map& units_ = *resources::units;

	int res = 0;

	bool changed = false;
	for (int i = 0;; ++i) {
		if (i == 6) {
			if (!changed) break;
			// Loop once again, in case a unit found a better spot
			// and freed the place for another unit.
			changed = false;
			i = 0;
		}

		if (map_.on_board(locs[i]) == false) {
			continue;
		}

		const t_translation::t_terrain terrain = map_[locs[i]];

		typedef move_map::const_iterator Itor;
		typedef std::pair<Itor,Itor> Range;
		Range its = dstsrc.equal_range(locs[i]);

		map_location* const beg_used = used_locs;
		map_location* end_used = used_locs + num_used_locs;

		int best_rating = 0;
		map_location best_unit;

		for(Itor it = its.first; it != its.second; ++it) {
			const unit_map::const_iterator u = units_.find(it->second);

			// Unit might have been killed, and no longer exist
			if(u == units_.end()) {
				continue;
			}

			const unit &un = *u;

			int tod_modifier = 0;
			if(un.alignment() == unit_type::LAWFUL) {
				tod_modifier = lawful_bonus;
			} else if(un.alignment() == unit_type::CHAOTIC) {
				tod_modifier = -lawful_bonus;
			}

			// The 0.5 power avoids underestimating too much the damage of a wounded unit.
			int hp = int(sqrt(double(un.hitpoints()) / un.max_hitpoints()) * 1000);
			int most_damage = 0;
			foreach (const attack_type &att, un.attacks())
			{
				int damage = att.damage() * att.num_attacks() * (100 + tod_modifier);
				if (damage > most_damage) {
					most_damage = damage;
				}
			}

			int village_bonus = map_.is_village(terrain) ? 3 : 2;
			int defense = 100 - un.defense_modifier(terrain);
			int rating = hp * defense * most_damage * village_bonus / 200;
			if(rating > best_rating) {
				map_location *pos = std::find(beg_used, end_used, it->second);
				// Check if the spot is the same or better than an older one.
				if (pos == end_used || rating >= ratings[pos - beg_used]) {
					best_rating = rating;
					best_unit = it->second;
				}
			}
		}

		if (!best_unit.valid()) continue;
		map_location *pos = std::find(beg_used, end_used, best_unit);
		int index = pos - beg_used;
		if (index == num_used_locs)
			++num_used_locs;
		else if (best_rating == ratings[index])
			continue;
		else {
			// The unit was in another spot already, so remove its older rating
			// from the final result, and require a new run to fill its old spot.
			res -= ratings[index];
			changed = true;
		}
		used_locs[index] = best_unit;
		ratings[index] = best_rating;
		res += best_rating;
	}

	return res / 100000.;
}
double aspect_attacks::power_projection2(const map_location& loc, const move_map& dstsrc)
{
	map_location used_locs[6];
	int ratings[6];
	int num_used_locs = 0;

	map_location locs[6];
	get_adjacent_tiles(loc,locs);

	const int lawful_bonus = resources::tod_manager->get_time_of_day().lawful_bonus;
	gamemap& map_ = *resources::game_map;
	unit_map& units_ = *resources::units;

	int res = 0;

	bool changed = false;
	for (int i = 0;; ++i) {
		if (i == 6) {
			if (!changed) break;
			// Loop once again, in case a unit found a better spot
			// and freed the place for another unit.
			changed = false;
			i = 0;
		}

		if (map_.on_board(locs[i]) == false) {
			continue;
		}

		const t_translation::t_terrain terrain = map_[locs[i]];

		typedef move_map::const_iterator Itor;
		typedef std::pair<Itor,Itor> Range;
		Range its = dstsrc.equal_range(locs[i]);

		map_location* const beg_used = used_locs;
		map_location* end_used = used_locs + num_used_locs;

		int best_rating = 0;
		map_location best_unit;

		for(Itor it = its.first; it != its.second; ++it) {
			const unit_map::const_iterator u = units_.find(it->second);

			// Unit might have been killed, and no longer exist
			if(u == units_.end()) {
				continue;
			}

			const unit &un = *u;

			int tod_modifier = 0;
			if(un.alignment() == unit_type::LAWFUL) {
				tod_modifier = lawful_bonus;
			} else if(un.alignment() == unit_type::CHAOTIC) {
				tod_modifier = -lawful_bonus;
			}

			// The 0.5 power avoids underestimating too much the damage of a wounded unit.
			int hp = int(sqrt(double(un.hitpoints()) / un.max_hitpoints()) * 1000);
			int most_damage = 0;
			foreach (const attack_type &att, un.attacks())
			{
				int damage = att.damage() * att.num_attacks() * (100 + tod_modifier);
				if (damage > most_damage) {
					most_damage = damage;
				}
			}

			int village_bonus = map_.is_village(terrain) ? 3 : 2;
			int defense = 100 - un.defense_modifier(terrain);
			int rating = hp * defense * most_damage * village_bonus / 200;
			if(rating > best_rating) {
				map_location *pos = std::find(beg_used, end_used, it->second);
				// Check if the spot is the same or better than an older one.
				if (pos == end_used || rating >= ratings[pos - beg_used]) {
					best_rating = rating;
					best_unit = it->second;
				}
			}
		}

		if (!best_unit.valid()) continue;
		map_location *pos = std::find(beg_used, end_used, best_unit);
		int index = pos - beg_used;
		if (index == num_used_locs)
			++num_used_locs;
		else if (best_rating == ratings[index])
			continue;
		else {
			// The unit was in another spot already, so remove its older rating
			// from the final result, and require a new run to fill its old spot.
			res -= ratings[index];
			changed = true;
		}
		used_locs[index] = best_unit;
		ratings[index] = best_rating;
		res += best_rating;
	}

	return res / 100000.;
}


} // end of namespace testing_ai_default

} // end of namespace ai
