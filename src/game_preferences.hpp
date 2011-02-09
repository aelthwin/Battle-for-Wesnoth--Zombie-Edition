/* $Id: game_preferences.hpp 48153 2011-01-01 15:57:50Z mordante $ */
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
#ifndef GAME_PREFERENCES_HPP_INCLUDED
#define GAME_PREFERENCES_HPP_INCLUDED

class gamemap;
class game_state;
class team;
class unit_map;

#include "preferences.hpp"

#include <set>

namespace preferences {

	struct manager
	{
		manager();
		~manager();

		base_manager base;
	};

	bool is_authenticated();
	void parse_admin_authentication(const std::string& sender, const std::string& message);

	bool parse_should_show_lobby_join(const std::string& sender, const std::string& message);
	int lobby_joins();
	void _set_lobby_joins(int show);
	enum { SHOW_NONE, SHOW_FRIENDS, SHOW_ALL };

	bool new_lobby();
	void set_new_lobby(bool value);

	const std::set<std::string> & get_friends();
	const std::set<std::string> & get_ignores();
	bool add_friend(const std::string& nick);
	bool add_ignore(const std::string& nick);
	void add_completed_campaign(const std::string& campaign_id);
	void remove_friend(const std::string& nick);
	void remove_ignore(const std::string& nick);
	bool is_friend(const std::string& nick);
	bool is_ignored(const std::string& nick);
	bool is_campaign_completed(const std::string& campaign_id);

	const std::vector<game_config::server_info>& server_list();

	std::string network_host();
	void set_network_host(const std::string& host);

	unsigned int get_ping_timeout();

	std::string campaign_server();
	void set_campaign_server(const std::string& host);

	std::string login();
	void set_login(const std::string& username);

	// If password remembering is turned off use
	// prv::password instead. This way we will not
	// have to worry about whether to remember the
	// password or not anywhere else in the code.
	//
	// It is put into a separate namespace to make clear
	// it is "private" and not supposed to be edit outside
	// of the preferences functions.
	namespace prv {
		extern std::string password;
	}

	std::string password();
	void set_password(const std::string& password);

	bool remember_password();
	void set_remember_password(bool remember);

	bool turn_dialog();
	void set_turn_dialog(bool ison);

	bool enable_whiteboard_mode_on_start();
	void set_enable_whiteboard_mode_on_start(bool value);

	bool show_combat();

	bool allow_observers();
	void set_allow_observers(bool value);

	bool use_map_settings();
	void set_use_map_settings(bool value);

	int mp_server_warning_disabled();
	void set_mp_server_warning_disabled(int value);

	void set_mp_server_program_name(const std::string&);
	std::string get_mp_server_program_name();

	bool random_start_time();
	void set_random_start_time(bool value);

	bool fog();
	void set_fog(bool value);

	bool shroud();
	void set_shroud(bool value);

	int turns();
	void set_turns(int value);

	bool skip_mp_replay();
	void set_skip_mp_replay(bool value);

	bool countdown();
	void set_countdown(bool value);
	int countdown_init_time();
	void set_countdown_init_time(int value);
	int countdown_turn_bonus();
	void set_countdown_turn_bonus(int value);
	int countdown_reservoir_time();
	void set_countdown_reservoir_time(int value);
	int countdown_action_bonus();
	void set_countdown_action_bonus(int value);

	int village_gold();
	void set_village_gold(int value);

	int xp_modifier();
	void set_xp_modifier(int value);

	int era();
	void set_era(int value);

	int map();
	void set_map(int value);

	bool show_ai_moves();
	void set_show_ai_moves(bool value);

	void set_show_side_colors(bool value);
	bool show_side_colors();

	bool save_replays();
	void set_save_replays(bool value);

	bool delete_saves();
	void set_delete_saves(bool value);

	void set_ask_delete_saves(bool value);
	bool ask_delete_saves();

    void set_interrupt_when_ally_sighted(bool value);
    bool interrupt_when_ally_sighted();

	void set_autosavemax(int value);
	int autosavemax();

	const int INFINITE_AUTO_SAVES = 61;

	bool show_floating_labels();
	void set_show_floating_labels(bool value);

	bool message_private();
	void set_message_private(bool value);

	bool show_haloes();
	void set_show_haloes(bool value);


	bool flip_time();
	void set_flip_time(bool value);

	// Multiplayer functions
	std::string get_chat_timestamp(const time_t& t);
	bool chat_timestamping();
	void set_chat_timestamping(bool value);

	int chat_lines();
	void set_chat_lines(int lines);

	bool compress_saves();

	bool startup_effect();

	std::set<std::string> &encountered_units();
	std::set<t_translation::t_terrain> &encountered_terrains();

	std::string custom_command();
	void set_custom_command(const std::string& command);

	std::vector<std::string>* get_history(const std::string& id);

	std::string client_type();

	std::string clock_format();

	void set_theme(const std::string& theme);
	std::string theme();

	bool compare_resolutions(const std::pair<int,int>& lhs, const std::pair<int,int>& rhs);

	// Ask for end turn confirmation
	bool yellow_confirm();
	bool green_confirm();
	bool confirm_no_moves();

	// Add all recruitable units as encountered so that information
	// about them are displayed to the user in the help system.
	void encounter_recruitable_units(std::vector<team>& teams);
	// Add all units that exist at the start to the encountered units so
	// that information about them are displayed to the user in the help
	// system.
	void encounter_start_units(unit_map& units);
	// Add all units that are recallable as encountred units.
	void encounter_recallable_units(std::vector<team>& teams);
	// Add all terrains on the map as encountered terrains.
	void encounter_map_terrain(gamemap& map);

}

#endif
