/* $Id: persist_var.cpp 48153 2011-01-01 15:57:50Z mordante $ */
/*
   Copyright (C) 2010 - 2011 by Jody Northup
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

#include "ai/manager.hpp"
#include "gamestatus.hpp"
#include "log.hpp"
#include "network.hpp"
#include "persist_context.hpp"
#include "persist_manager.hpp"
#include "persist_var.hpp"
#include "play_controller.hpp"
#include "replay.hpp"
#include "resources.hpp"
#include "team.hpp"
#include "util.hpp"

struct persist_choice: mp_sync::user_choice {
	const persist_context &ctx;
	std::string var_name;
	int side;
	persist_choice(const persist_context &context,const std::string &name, int side_num)
		: ctx(context)
		, var_name(name)
		, side(side_num) {
	}
	virtual config query_user() const {
		config ret;
		ret["side"] = side;
		ret.add_child("variables",ctx.get_var(var_name));
		return ret;
	}
	virtual config random_choice(rand_rng::simple_rng &) const {
		return config();
	}
};

static void get_global_variable(persist_context &ctx, const vconfig &pcfg)
{
	std::string global = pcfg["from_global"];
	std::string local = pcfg["to_local"];
	config::attribute_value pcfg_side = pcfg["side"];
	int side = pcfg_side.str() == "global" ? resources::controller->current_side() : pcfg_side.to_int();
	persist_choice choice(ctx,global,side);
	config cfg = mp_sync::get_user_choice("global_variable",choice,side,true).child("variables");
	if (cfg) {
		size_t arrsize = cfg.child_count(global);
		if (arrsize == 0) {
			resources::state_of_game->set_variable(local,cfg[global]);
		} else {
			resources::state_of_game->clear_variable(local);
			for (size_t i = 0; i < arrsize; i++)
				resources::state_of_game->add_variable_cfg(local,cfg.child(global,i));
		}
	} else {
		resources::state_of_game->set_variable(local,"");
	}
}

static void clear_global_variable(persist_context &ctx, const vconfig &pcfg)
{
	std::string global = pcfg["global"];
	ctx.clear_var(global, pcfg["immediate"].to_bool());
}

static void set_global_variable(persist_context &ctx, const vconfig &pcfg)
{
	if (pcfg["from_local"].empty()) {
		clear_global_variable(ctx, pcfg);
	} else {
		std::string global = pcfg["to_global"];
		std::string local = pcfg["from_local"];
		config val;
		const config &vars = resources::state_of_game->get_variables();
		size_t arraylen = vars.child_count(local);
		if (arraylen == 0) {
			val = pack_scalar(global,resources::state_of_game->get_variable(local));
		} else {
			for (size_t i = 0; i < arraylen; i++)
				val.add_child(global,vars.child(local,i));
		}
		ctx.set_var(global, val, pcfg["immediate"].to_bool());
	}
}
void verify_and_get_global_variable(const vconfig &pcfg)
{
	bool valid = true;
	if (!pcfg.has_attribute("from_global")) {
		LOG_PERSIST << "Error: [get_global_variable] missing required attribute \"from_global\"";
		valid = false;
	}
	if (!pcfg.has_attribute("to_local")) {
		LOG_PERSIST << "Error: [get_global_variable] missing required attribute \"to_local\"";
		valid = false;
	}
	// TODO: allow for global namespace.
	if (!pcfg.has_attribute("namespace")) {
		LOG_PERSIST << "Error: [get_global_variable] missing attribute \"namespace\" and no global namespace provided.";
		valid = false;
	}
	if (network::nconnections() != 0) {
		if (!pcfg.has_attribute("side")) {
			LOG_PERSIST << "Error: [get_global_variable] missing attribute \"side\" required in multiplayer context.";
			valid = false;
		}
		else {
			config::attribute_value pcfg_side = pcfg["side"];
			int side = pcfg_side.str() == "global" ? resources::controller->current_side() : pcfg_side.to_int();
			if (unsigned (side - 1) >= resources::teams->size()) {
				LOG_PERSIST << "Error: [get_global_variable] attribute \"side\" specifies invalid side number.";
				valid = false;
			} else {
				if ((side != resources::controller->current_side())
					&& !((*resources::teams)[side - 1].is_local())) {
					if ((*resources::teams)[resources::controller->current_side() - 1].is_local()) {
						config data;
						data.add_child("wait_global");
						data.child("wait_global")["side"] = side;
						network::send_data(data,0);
					}
					while (get_replay_source().at_end()) {
						ai::manager::raise_user_interact();
						ai::manager::raise_sync_network();
						SDL_Delay(10);
					}
				}
			}
		}
	}
	if (valid)
	{
		persist_context &ctx = resources::persist->get_context((pcfg["namespace"]));
		if (ctx.valid()) {
			get_global_variable(ctx,pcfg);
		} else {
			LOG_PERSIST << "Error: [get_global_variable] attribute \"namespace\" is not valid.";
		}
	}
}
void verify_and_set_global_variable(const vconfig &pcfg)
{
	bool valid = true;
	if (!pcfg.has_attribute("to_global")) {
		LOG_PERSIST << "Error: [set_global_variable] missing required attribute \"from_global\"";
		valid = false;
	}
	if (!pcfg.has_attribute("from_local")) {
		LOG_PERSIST << "Warning: [set_global_variable] missing attribute \"to_local\", global variable will be cleared";
	}
	// TODO: allow for global namespace.
	if (!pcfg.has_attribute("namespace")) {
		LOG_PERSIST << "Error: [set_global_variable] missing attribute \"namespace\" and no global namespace provided.";
		valid = false;
	}
	if (network::nconnections() != 0) {
		if (!pcfg.has_attribute("side")) {
			LOG_PERSIST << "Error: [set_global_variable] missing attribute \"side\" required in multiplayer context.";
			valid = false;
		} else {
			config::attribute_value pcfg_side = pcfg["side"];
			int side = pcfg_side;
			//Check side matching only if the side is not "global".
			if (pcfg_side.str() != "global") {
				//Ensure that the side is valid.
				if (unsigned(side-1) > resources::teams->size()) {
					LOG_PERSIST << "Error: [set_global_variable] attribute \"side\" specifies invalid side number.";
					valid = false;
				} else {
					//Set the variable only if it is meant for a side we control
					valid = (*resources::teams)[side - 1].is_local();
				}
			}
		}
	}
	if (valid)
	{
		persist_context &ctx = resources::persist->get_context((pcfg["namespace"]));
		if (ctx.valid()) {
			set_global_variable(ctx,pcfg);
		} else {
			LOG_PERSIST << "Error: [set_global_variable] attribute \"namespace\" is not valid.";
		}
	}
}
void verify_and_clear_global_variable(const vconfig &pcfg)
{
	bool valid = true;
	if (!pcfg.has_attribute("global")) {
		LOG_PERSIST << "Error: [clear_global_variable] missing required attribute \"from_global\"";
		valid = false;
	}
	if (!pcfg.has_attribute("namespace")) {
		LOG_PERSIST << "Error: [clear_global_variable] missing attribute \"namespace\" and no global namespace provided.";
		valid = false;
	}
	if (network::nconnections() != 0) {
		if (!pcfg.has_attribute("side")) {
			LOG_PERSIST << "Error: [set_global_variable] missing attribute \"side\" required in multiplayer context.";
			valid = false;
		} else {
			config::attribute_value pcfg_side = pcfg["side"];
			int side = pcfg_side;
			//Check side matching only if the side is not "global".
			if (pcfg_side.str() != "global") {
				//Ensure that the side is valid.
				if (unsigned(side-1) > resources::teams->size()) {
					LOG_PERSIST << "Error: [clear_global_variable] attribute \"side\" specifies invalid side number.";
					valid = false;
				} else {
					//Clear the variable only if it is meant for a side we control
					valid = (*resources::teams)[side - 1].is_local();
				}
			}
		}
	}
	if (valid)
	{
		persist_context &ctx = resources::persist->get_context((pcfg["namespace"]));
		if (ctx.valid()) {
			clear_global_variable(ctx,pcfg);
		} else {
			LOG_PERSIST << "Error: [clear_global_variable] attribute \"namespace\" is not valid.";
		}
	}
}
