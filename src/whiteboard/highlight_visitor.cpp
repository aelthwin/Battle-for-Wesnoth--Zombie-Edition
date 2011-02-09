/* $Id: highlight_visitor.cpp 48153 2011-01-01 15:57:50Z mordante $ */
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

#include "highlight_visitor.hpp"
#include "manager.hpp"
#include "action.hpp"
#include "attack.hpp"
#include "move.hpp"
#include "recall.hpp"
#include "recruit.hpp"
#include "side_actions.hpp"

#include "arrow.hpp"
#include "foreach.hpp"
#include "play_controller.hpp"
#include "unit_map.hpp"
#include "resources.hpp"

namespace wb
{

highlight_visitor::highlight_visitor(const unit_map& unit_map, side_actions_ptr side_actions)
	: visitor (side_actions)
	, mode_(NONE)
	, unit_map_(unit_map)
	, mouseover_hex_()
	, exclusive_display_hexes_()
	, owner_unit_(NULL)
	, selection_candidate_(NULL)
	, main_highlight_()
	, secondary_highlights_()
{
}

highlight_visitor::~highlight_visitor()
{
	if (resources::screen && owner_unit_)
		unhighlight();
}

void highlight_visitor::set_mouseover_hex(const map_location& hex)
{
	clear();

	if (!hex.valid())
		return;

	scoped_real_unit_map ensure_real_map;
	mouseover_hex_ = hex;
	//if we're right over a unit, just highlight all of this unit's actions
	unit_map::const_iterator it = unit_map_.find(hex);
	if (it != unit_map_.end()
			&& it->side() == resources::screen->viewing_side())
	{
		selection_candidate_ = &(*it);

		if(side_actions_->unit_has_actions(&*it))
		{
			owner_unit_ = &(*it);
		}

		//commented code below is to also select the first action of this unit as
		//the main highlight; it doesn't fit too well in the UI
//		side_actions::iterator action_it = side_actions_->find_first_action_of(*it);
//		if (action_it != side_actions_->end())
//		{
//			main_highlight_ = *action_it;
//		}
	}
}

void highlight_visitor::clear()
{
	unhighlight();
	main_highlight_.reset();
	owner_unit_ = NULL;
	secondary_highlights_.clear();
}

void highlight_visitor::highlight()
{
	//Find main action to highlight if any, as well as owner unit
	find_main_highlight();

	if (action_ptr main = main_highlight_.lock())
	{
		//Highlight main highlight
		mode_ = HIGHLIGHT_MAIN;
		main->accept(*this);
	}

	if (owner_unit_)
	{
		//Find secondary actions to highlight
		find_secondary_highlights();

		//Make sure owner unit is the only one displayed in its hex
		resources::screen->add_exclusive_draw(owner_unit_->get_location(), *owner_unit_);
		exclusive_display_hexes_.insert(owner_unit_->get_location());

		if (!secondary_highlights_.empty())
			//Highlight secondary highlights
			mode_ = HIGHLIGHT_SECONDARY;
			foreach(weak_action_ptr weak, secondary_highlights_)
			{
				if (action_ptr action = weak.lock())
				{
					action->accept(*this);
				}
			}
	}
}

void highlight_visitor::unhighlight()
{
	//unhighlight main highlight
	if (action_ptr main = main_highlight_.lock() )
	{
		mode_ = UNHIGHLIGHT_MAIN;
		main->accept(*this);
	}

	//unhighlight secondary highlights
	mode_ = UNHIGHLIGHT_SECONDARY;
	foreach(weak_action_ptr weak, secondary_highlights_)
	{
		if (action_ptr action = weak.lock())
		{
			action->accept(*this);
		}
	}

	//unhide other units if needed
	foreach(map_location hex, exclusive_display_hexes_)
	{
		resources::screen->remove_exclusive_draw(hex);
	}
	exclusive_display_hexes_.clear();
}

action_ptr highlight_visitor::get_execute_target()
{
	action_ptr action;

	if (owner_unit_ && side_actions_->unit_has_actions(owner_unit_))
	{
		action = *side_actions_->find_first_action_of(owner_unit_);
	}
	else
	{
		action = main_highlight_.lock();
	}

	return action;
}
action_ptr highlight_visitor::get_delete_target()
{
	action_ptr action;
	if (owner_unit_ && side_actions_->unit_has_actions(owner_unit_))
	{
		action = *side_actions_->find_last_action_of(owner_unit_);
	}
	else
	{
		action = main_highlight_.lock();
	}
	return action;
}

action_ptr highlight_visitor::get_bump_target()
{
	return main_highlight_.lock();
}

unit* highlight_visitor::get_selection_target()
{
	if (owner_unit_)
		return owner_unit_;
	else
		return selection_candidate_;
}

void highlight_visitor::visit_move(move_ptr move)
{
	switch (mode_)
	{
	case FIND_MAIN_HIGHLIGHT:
		if (move->get_dest_hex() == mouseover_hex_)
		{
			main_highlight_ = move;
		}
		break;
	case FIND_SECONDARY_HIGHLIGHTS:
		if (move->get_unit() == owner_unit_
				&& move != main_highlight_.lock())
		{
			secondary_highlights_.push_back(move);
		}
		break;
	case HIGHLIGHT_MAIN:
		if (move->arrow_)
		{
			move->arrow_->set_style(arrow::STYLE_FOCUS);
		}
		if (move->fake_unit_)
		{
			///@todo find some highlight animation
			move->fake_unit_->set_ghosted(false);
			//Make sure the fake unit is the only one displayed in its hex
			resources::screen->add_exclusive_draw(move->fake_unit_->get_location(), *move->fake_unit_);
			exclusive_display_hexes_.insert(move->fake_unit_->get_location());
		}
		break;
	case HIGHLIGHT_SECONDARY:
		if (move->arrow_)
		{
			move->arrow_->set_style(arrow::STYLE_HIGHLIGHTED);
		}
		if (move->fake_unit_)
		{
			move->fake_unit_->set_ghosted(false);
			//Make sure the fake unit is the only one displayed in its hex
			resources::screen->add_exclusive_draw(move->fake_unit_->get_location(), *move->fake_unit_);
			exclusive_display_hexes_.insert(move->fake_unit_->get_location());
		}
		break;
	case UNHIGHLIGHT_MAIN:
	case UNHIGHLIGHT_SECONDARY:
		if (move->arrow_)
		{
			move->arrow_->set_style(arrow::STYLE_STANDARD);
		}
		if (move->fake_unit_)
		{
			side_actions::iterator last_action = side_actions_->find_last_action_of(move->get_unit());

			if (!(last_action != side_actions_->end() && *last_action == move))
			{
				move->fake_unit_->set_disabled_ghosted(false);
			}
		}
		break;
	default:
		assert (false);
	}
}

void highlight_visitor::visit_attack(attack_ptr attack)
{
	visit_move(boost::static_pointer_cast<move>(attack));
	//@todo: highlight the attack indicator
}

void highlight_visitor::visit_recruit(recruit_ptr recruit)
{
	switch (mode_)
	{
	case FIND_MAIN_HIGHLIGHT:
		if (recruit->recruit_hex_ == mouseover_hex_)
		{
			main_highlight_ = recruit;
		}
		break;
	case FIND_SECONDARY_HIGHLIGHTS:
		break;
	case HIGHLIGHT_MAIN:
		if (recruit->fake_unit_)
		{
			//@todo: find some suitable effect for mouseover on planned recruit.

			//Make sure the fake unit is the only one displayed in its hex
			resources::screen->add_exclusive_draw(recruit->fake_unit_->get_location(), *recruit->fake_unit_);
			exclusive_display_hexes_.insert(recruit->fake_unit_->get_location());
		}
		break;
	case HIGHLIGHT_SECONDARY:
		break;
	case UNHIGHLIGHT_MAIN:
	case UNHIGHLIGHT_SECONDARY:
		if (recruit->fake_unit_)
		{
			//@todo: find some suitable effect for mouseover on planned recruit.
		}
		break;
	default:
		assert (false);
	}
}

void highlight_visitor::visit_recall(recall_ptr recall)
{
	switch (mode_)
	{
	case FIND_MAIN_HIGHLIGHT:
		if (recall->recall_hex_ == mouseover_hex_)
		{
			main_highlight_ = recall;
		}
		break;
	case FIND_SECONDARY_HIGHLIGHTS:
		break;
	case HIGHLIGHT_MAIN:
		if (recall->fake_unit_)
		{
			//@todo: find some suitable effect for mouseover on planned recall.
		}
		break;
	case HIGHLIGHT_SECONDARY:
		break;
	case UNHIGHLIGHT_MAIN:
	case UNHIGHLIGHT_SECONDARY:
		if (recall->fake_unit_)
		{
			//@todo: find some suitable effect for mouseover on planned recall.

			//Make sure the fake unit is the only one displayed in its hex
			resources::screen->add_exclusive_draw(recall->fake_unit_->get_location(), *recall->fake_unit_);
			exclusive_display_hexes_.insert(recall->fake_unit_->get_location());
		}
		break;
	default:
		assert (false);
	}
}

void highlight_visitor::find_main_highlight()
{
	// Even if we already found an owner_unit_ in the mouseover hex,
	// action destination hexes usually take priority over that
	mode_ = FIND_MAIN_HIGHLIGHT;
	assert(main_highlight_.expired());
	//@todo re-enable the following assert once I find out what happends to
	// viewing side assignments after victory
	//assert(side_actions_->team_index() == resources::screen->viewing_team());
	side_actions::reverse_iterator rend = side_actions_->rend();
	side_actions::reverse_iterator action = side_actions_->rbegin();
	for (; action != rend; ++action )
	{
		(*action)->accept(*this);
		if (action_ptr main = main_highlight_.lock())
		{
			owner_unit_ = main->get_unit();
			break;
		}
	}
}

void highlight_visitor::find_secondary_highlights()
{
	assert(owner_unit_);
	assert(secondary_highlights_.empty());
	mode_ = FIND_SECONDARY_HIGHLIGHTS;
	visit_all_actions();
}

} // end namespace wb
