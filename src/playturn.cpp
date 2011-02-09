/* $Id: playturn.cpp 48153 2011-01-01 15:57:50Z mordante $ */
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

#include "playturn.hpp"

#include "construct_dialog.hpp"
#include "foreach.hpp"
#include "game_display.hpp"
#include "game_end_exceptions.hpp"
#include "game_preferences.hpp"
#include "gettext.hpp"
#include "gui/dialogs/simple_item_selector.hpp"
#include "log.hpp"
#include "replay.hpp"
#include "resources.hpp"
#include "rng.hpp"
#include "formula_string_utils.hpp"
#include "play_controller.hpp"

#include <ctime>

static lg::log_domain log_network("network");
#define ERR_NW LOG_STREAM(err, log_network)

turn_info::turn_info(unsigned team_num, replay_network_sender &replay_sender) :
	team_num_(team_num),
	replay_sender_(replay_sender),
	host_transfer_("host_transfer"), replay_()
{
	/**
	 * We do network sync so [init_side] is transferred to network hosts
	 */
	if(network::nconnections() > 0)
		send_data();
}

turn_info::~turn_info(){
	resources::undo_stack->clear();
}

void turn_info::sync_network()
{
	if(network::nconnections() > 0) {

		//receive data first, and then send data. When we sent the end of
		//the AI's turn, we don't want there to be any chance where we
		//could get data back pertaining to the next turn.
		config cfg;
		while(network::connection res = network::receive_data(cfg)) {
			std::deque<config> backlog;
			process_network_data(cfg,res,backlog,false);
			cfg.clear();
		}

		send_data();
	}
}

void turn_info::send_data()
{
	if(resources::undo_stack->empty()) {
		replay_sender_.commit_and_sync();
	} else {
		replay_sender_.sync_non_undoable();
	}
}

turn_info::PROCESS_DATA_RESULT turn_info::process_network_data(const config& cfg,
		network::connection from, std::deque<config>& backlog, bool skip_replay)
{
	if (const config &rnd_seed = cfg.child("random_seed")) {
		rand_rng::set_seed(rnd_seed["seed"]);
		//may call a callback function, see rand_rng::set_seed_callback
	}

	if (const config &msg = cfg.child("message"))
	{
		resources::screen->add_chat_message(time(NULL), msg["sender"], msg["side"],
				msg["message"], events::chat_handler::MESSAGE_PUBLIC,
				preferences::message_bell());
	}

	if (const config &msg = cfg.child("whisper") /*&& is_observer()*/)
	{
		resources::screen->add_chat_message(time(NULL), "whisper: " + msg["sender"].str(), 0,
				msg["message"], events::chat_handler::MESSAGE_PRIVATE,
				preferences::message_bell());
	}

	foreach (const config &ob, cfg.child_range("observer")) {
		resources::screen->add_observer(ob["name"]);
	}

	foreach (const config &ob, cfg.child_range("observer_quit")) {
		resources::screen->remove_observer(ob["name"]);
	}

	if (cfg.child("leave_game")) {
		throw network::error("");
	}

	bool turn_end = false;

	config::const_child_itors turns = cfg.child_range("turn");
	if (turns.first != turns.second && from != network::null_connection) {
		//forward the data to other peers
		network::send_data_all_except(cfg, from);
	}

	foreach (const config &t, turns)
	{
		if(turn_end == false) {
			/** @todo FIXME: Check what commands we execute when it's our turn! */
			replay_.append(t);
			replay_.set_skip(skip_replay);

			turn_end = do_replay(team_num_, &replay_);
			recorder.add_config(t, replay::MARK_AS_SENT);
		} else {

			//this turn has finished, so push the remaining moves
			//into the backlog
			backlog.push_back(config());
			backlog.back().add_child("turn", t);
		}
	}

	if (const config &change= cfg.child("change_controller"))
	{
		//don't use lexical_cast_default it's "safer" to end on error
		const int side = lexical_cast<int>(change["side"]);
		const size_t index = static_cast<size_t>(side-1);

		const std::string &controller = change["controller"];
		const std::string &player = change["player"];

		if(index < resources::teams->size()) {
			team &tm = (*resources::teams)[index];
			if (!player.empty())
				tm.set_current_player(player);
			unit_map::iterator leader = resources::units->find_leader(side);
			bool restart = resources::screen->playing_side() == side;
			if (!player.empty() && leader.valid())
				leader->rename(player);


			if (controller == "human" && !tm.is_human()) {
				if (!(*resources::teams)[resources::screen->playing_team()].is_human())
				{
					resources::screen->set_team(index);
				}
				tm.make_human();
			} else if (controller == "human_ai" && !tm.is_human_ai()) {
				tm.make_human_ai();
			} else if (controller == "network" && !tm.is_network_human()) {
				tm.make_network();
			} else if (controller == "network_ai" && !tm.is_network_ai()) {
				tm.make_network_ai();
			} else if (controller == "ai" && !tm.is_ai()) {
				tm.make_ai();
			}
			else
			{
				restart = false;
			}

			return restart ? PROCESS_RESTART_TURN : PROCESS_CONTINUE;
		}
	}

	//if a side has dropped out of the game.
	if(cfg["side_drop"] != "") {
		const std::string controller = cfg["controller"];
		const std::string side_str = cfg["side_drop"];
		int side = atoi(side_str.c_str());
		const size_t side_index = side-1;

		bool restart = side == resources::screen->playing_side();

		if (side_index >= resources::teams->size()) {
			ERR_NW << "unknown side " << side_index << " is dropping game\n";
			throw network::error("");
		}

		team &tm = (*resources::teams)[side_index];
		unit_map::iterator leader = resources::units->find_leader(side);
		const bool have_leader = leader.valid();

		if (controller == "ai"){
			tm.make_ai();
			tm.set_current_player("ai" + side_str);
			if (have_leader) leader->rename("ai" + side_str);
			return restart?PROCESS_RESTART_TURN:PROCESS_CONTINUE;
		}

		int action = 0;

		std::vector<std::string> observers;
		std::vector<team*> allies;
		std::vector<std::string> options;

		// We want to give host chance to decide what to do for side
		{
			utils::string_map t_vars;
			options.push_back(_("Replace with AI"));
			options.push_back(_("Replace with local player"));
			options.push_back(_("Abort game"));

			//get all observers in as options to transfer control
			foreach (const std::string &ob, resources::screen->observers())
			{
				t_vars["player"] = ob;
				options.push_back(vgettext("Replace with $player", t_vars));
				observers.push_back(ob);
			}

			//get all allies in as options to transfer control
			foreach (team &t, *resources::teams)
			{
				if (!t.is_enemy(side) && !t.is_human() && !t.is_ai() && !t.is_empty()
					&& t.current_player() != tm.current_player())
				{
					//if this is an ally of the dropping side and it is not us (choose local player
					//if you want that) and not ai or empty and if it is not the dropping side itself,
					//get this team in as well
					t_vars["player"] = t.current_player();
					options.push_back(vgettext("Replace with $player", t_vars));
					allies.push_back(&t);
				}
			}

			t_vars["player"] = tm.current_player();
			const std::string msg =  vgettext("$player has left the game. What do you want to do?", t_vars);
			gui2::tsimple_item_selector dlg("", msg, options);
			dlg.set_single_button(true);
			dlg.show(resources::screen->video());
			action = dlg.selected_index();
		}

		//make the player an AI, and redo this turn, in case
		//it was the current player's team who has just changed into
		//an AI.
		switch(action) {
			case 0:
				tm.make_human_ai();
				tm.set_current_player("ai" + side_str);
				if (have_leader) leader->rename("ai" + side_str);
				change_controller(side_str, "human_ai");
				return restart?PROCESS_RESTART_TURN:PROCESS_CONTINUE;

			case 1:
				tm.make_human();
				tm.set_current_player("human" + side_str);
				if (have_leader) leader->rename("human" + side_str);
				return restart?PROCESS_RESTART_TURN:PROCESS_CONTINUE;
			case 2:
				//The user pressed "end game". Don't throw a network error here or he will get
				//thrown back to the title screen.
				throw end_level_exception(QUIT);
			default:
				if (action > 2) {

					{
						// Server thinks this side is ours now so in case of error transferring side we have to make local state to same as what server thinks it is.
						tm.make_human();
						tm.set_current_player("human"+side_str);
						if (have_leader) leader->rename("human"+side_str);
					}

					const size_t index = static_cast<size_t>(action - 3);
					if (index < observers.size()) {
						change_side_controller(side_str, observers[index], false /*not our own side*/);
					} else if (index < options.size() - 1) {
						size_t i = index - observers.size();
						change_side_controller(side_str, allies[i]->current_player(), false /*not our own side*/);
					} else {
						tm.make_human_ai();
						tm.set_current_player("ai"+side_str);
						if (have_leader) leader->rename("ai" + side_str);
						change_controller(side_str, "human_ai");
					}
					return restart?PROCESS_RESTART_TURN:PROCESS_CONTINUE;
				}
				break;
		}
		throw network::error("");
	}

	// The host has ended linger mode in a campaign -> enable the "End scenario" button
	// and tell we did get the notification.
	if (cfg.child("notify_next_scenario")) {
		gui::button* btn_end = resources::screen->find_button("button-endturn");
		if(btn_end) {
			btn_end->enable(true);
		}
		return PROCESS_END_LINGER;
	}

	//If this client becomes the new host, notify the play_controller object about it
	if (const config &cfg_host_transfer = cfg.child("host_transfer")){
		if (cfg_host_transfer["value"] == "1") {
			host_transfer_.notify_observers();
		}
	}

	return turn_end ? PROCESS_END_TURN : PROCESS_CONTINUE;
}

void turn_info::change_controller(const std::string& side, const std::string& controller)
{
	config cfg;
	config& change = cfg.add_child("change_controller");
	change["side"] = side;
	change["controller"] = controller;

	network::send_data(cfg, 0);
}


void turn_info::change_side_controller(const std::string& side, const std::string& player, bool own_side)
{
	config cfg;
	config& change = cfg.add_child("change_controller");
	change["side"] = side;
	change["player"] = player;
	if (own_side) change["own_side"] = true;
	network::send_data(cfg, 0);
}

#if 0
void turn_info::take_side(const std::string& side, const std::string& controller)
{
	config cfg;
	cfg["side"] = side;
	cfg["controller"] = controller;
	cfg["name"] = controller+side;
	network::send_data(cfg, 0, true);
}
#endif
