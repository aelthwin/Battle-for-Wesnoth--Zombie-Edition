/* $Id: validate_visitor.cpp 48153 2011-01-01 15:57:50Z mordante $ */
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

#include "validate_visitor.hpp"
#include "attack.hpp"
#include "manager.hpp"
#include "move.hpp"
#include "recall.hpp"
#include "recruit.hpp"
#include "side_actions.hpp"

#include "arrow.hpp"
#include "foreach.hpp"
#include "pathfind/pathfind.hpp"
#include "play_controller.hpp"
#include "resources.hpp"
#include "team.hpp"

namespace wb
{

validate_visitor::validate_visitor(unit_map& unit_map, side_actions_ptr side_actions)
	: mapbuilder_visitor(unit_map, side_actions, true)
	, actions_to_erase_()
{
	assert(!resources::whiteboard->has_planned_unit_map());
}

validate_visitor::~validate_visitor()
{
}

bool validate_visitor::validate_actions()
{
	int action_number = 1;
	foreach(action_ptr action, *side_actions_)
	{
		DBG_WB << "Action #" << action_number << "\n";
		action->accept(*this);
		++action_number;
	}

	//FIXME: by reverse iterating this can be done in a more efficiant way
	// by using the iterator returned by remove_action it could even be done in the loop above
	if (!actions_to_erase_.empty())
	{
		int side_actions_size_before = side_actions_->actions().size();
		LOG_WB << "Erasing " << actions_to_erase_.size() << " invalid actions.\n";
		foreach(action_ptr action, actions_to_erase_)
		{
			side_actions_->remove_action(side_actions_->get_position_of(action), false);
		}
		assert(side_actions_size_before - side_actions_->actions().size() == actions_to_erase_.size());
		actions_to_erase_.clear();
		return false;
	}
	else
	{
		return true;
	}
}

void validate_visitor::visit_move(move_ptr move)
{
	DBG_WB <<"visiting move from " << move->get_source_hex()
			<< " to " << move->get_dest_hex() << "\n";
	//invalidate start and end hexes so number display is updated properly
	resources::screen->invalidate(move->get_source_hex());
	resources::screen->invalidate(move->get_dest_hex());

	if (!(move->get_source_hex().valid() && move->get_dest_hex().valid()))
		move->set_valid(false);

	unit_map::const_iterator unit_it;
	//Check that the unit still exists in the source hex
	if (move->valid_)
	{
		unit_it = resources::units->find(move->get_source_hex());

		if (unit_it == resources::units->end())
		{
			move->set_valid(false);
			move->unit_ = NULL;
		}
	}

	//check if the unit in the source hex has the same unit id as before,
	//i.e. that it's the same unit
	if (move->valid_ && move->unit_id_ != unit_it->id())
	{
		move->set_valid(false);
		move->unit_ = NULL;
	}

	//Now that we've reliably identified the unit owning this planned move, update the
	//pointer in case there has been some funny business in the unit map
	if (move->valid_)
		move->unit_ = &*unit_it;

	if (move->valid_ && move->get_source_hex() != move->get_dest_hex()) //allow for zero-hex, move, in which case we skip pathfinding
	{
		//verify that the destination hex is free
		if (move->valid_ && (resources::units->find(move->get_dest_hex()) != resources::units->end()))
			move->set_valid(false);

		pathfind::marked_route new_route;
		if (move->valid_)
		{
			//@todo: use something else than empty vector for waypoints?
			new_route = resources::controller->get_mouse_handler_base().get_route(move->get_unit(),move->get_dest_hex(),
					std::vector<map_location>(), resources::teams->at(side_actions_->team_index()));

			if (new_route.move_cost >= pathfind::cost_calculator::getNoPathValue())
			{
				move->set_valid(false);
			}
		}

		if (move->valid_)
		{
			if ((!std::equal(new_route.steps.begin(), new_route.steps.end(), move->get_route().steps.begin()))
				|| new_route.move_cost != move->get_route().move_cost)
			{
				//new valid path differs from the previous one, replace
				move->set_route(new_route);

				//@todo: Since this might lengthen the path, we probably need a special conflict state
				// to warn the player that the initial path is no longer possible.
			}

			//Check that the unit still has enough movement to do this move
			if (move->valid_ && unit_it->movement_left() < move->movement_cost_)
				move->set_valid(false);
		}
	}

	if (move->valid_)
	{
		// Now call the superclass to apply the result of this move to the unit map,
		// so that further pathfinding takes it into account.
		mapbuilder_visitor::visit_move(move);
	}
	else
		//FIXME: temporary until invalid arrow styles are in: delete invalid moves
	{
		LOG_WB << "Invalid move detected, adding to actions_to_erase_.\n";
		actions_to_erase_.insert(move);
	}
}

void validate_visitor::visit_attack(attack_ptr attack)
{
	DBG_WB <<"visiting attack from " << attack->get_dest_hex()
			<< " to " << attack->target_hex_ << "\n";
	//invalidate target hex to make sure attack indicators are updated
	resources::screen->invalidate(attack->get_dest_hex());
	resources::screen->invalidate(attack->target_hex_);
	//Verify that the target hex is still valid
	if (!attack->target_hex_.valid())
	{
		attack->set_valid(false);
	}
	//Verify that the target hex isn't empty
	if (attack->is_valid() && resources::units->find(attack->target_hex_) == resources::units->end())
	{
		attack->set_valid(false);
	}

	//@todo: verify that the target hex contains the same unit that before,
	// comparing for example the unit ID

	//@todo: Verify that the target unit is our enemy

	//If all checks pass, then call the visitor on the superclass
	if (attack->is_valid())
	{
		visit_move(boost::static_pointer_cast<move>(attack));
	}
	else
	{
		LOG_WB << "Invalid attack detected, adding to actions_to_erase_.\n";
		actions_to_erase_.insert(attack);
	}
}

void validate_visitor::visit_recruit(recruit_ptr recruit)
{
	DBG_WB << "visiting recruit on hex " << recruit->recruit_hex_ << "\n";
	//invalidate recruit hex so number display is updated properly
	resources::screen->invalidate(recruit->recruit_hex_);

	int team_index = side_actions_->team_index();

	//Check that destination hex is still free
	if(resources::units->find(recruit->recruit_hex_) != resources::units->end())
	{
		LOG_WB << "Recruit set as invalid because target hex is occupied.\n";
		recruit->set_valid(false);
	}
	//Check that unit to recruit is still in side's recruit list
	if (recruit->is_valid())
	{
		const std::set<std::string>& recruits = (*resources::teams)[team_index].recruits();
		if (recruits.find(recruit->unit_name_) == recruits.end())
		{
			recruit->set_valid(false);
			LOG_WB << " Validate visitor: Planned recruit invalid since unit is not in recruit list anymore.\n";
		}
	}
	//Check that there is still enough gold to recruit this unit
	if (recruit->is_valid() && recruit->temp_unit_->cost() > (*resources::teams)[team_index].gold())
	{
		LOG_WB << "Recruit set as invalid, team doesn't have enough gold.\n";
		recruit->set_valid(false);
	}

	if (recruit->is_valid())
	{
		mapbuilder_visitor::visit_recruit(recruit);
	}
	else
	{
		LOG_WB << "Invalid recruit detected, adding to actions_to_erase_.\n";
		actions_to_erase_.insert(recruit);
	}
}

void validate_visitor::visit_recall(recall_ptr recall)
{
	DBG_WB << "visiting recall on hex " << recall->recall_hex_ << "\n";
	//invalidate recall hex so number display is updated properly
	resources::screen->invalidate(recall->recall_hex_);

	int team_index = side_actions_->team_index();

	//Check that destination hex is still free
	if(resources::units->find(recall->recall_hex_) != resources::units->end())
	{
		LOG_WB << "Recall set as invalid because target hex is occupied.\n";
		recall->set_valid(false);
	}
	//Check that unit to recall is still in side's recall list
	if (recall->is_valid())
	{
		const std::vector<unit>& recalls = (*resources::teams)[team_index].recall_list();
		if (std::find_if(recalls.begin(), recalls.end(),
				unit_comparator_predicate(*recall->temp_unit_)) == recalls.end())
		{
			recall->set_valid(false);
			LOG_WB << " Validate visitor: Planned recall invalid since unit is not in recall list anymore.\n";
		}
	}
	//Check that there is still enough gold to recall this unit
	if (recall->is_valid()
			&& (*resources::teams)[team_index].recall_cost() > (*resources::teams)[team_index].gold())
	{
		LOG_WB << "Recall set as invalid, team doesn't have enough gold.\n";
		recall->set_valid(false);
	}

	if (recall->is_valid())
	{
		mapbuilder_visitor::visit_recall(recall);
	}
	else
	{
		LOG_WB << "Invalid recall detected, adding to actions_to_erase_.\n";
		actions_to_erase_.insert(recall);
	}
}

}//end namespace wb
