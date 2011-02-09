/* $Id: mp_game_settings.cpp 48153 2011-01-01 15:57:50Z mordante $ */
/*
   Copyright (C) 2006 - 2011 by Joerg Hinrichs <joerg.hinrichs@alice-dsl.de>
   wesnoth playlevel Copyright (C) 2003 by David White <dave@whitevine.net>
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
 *  @file
 *  Container for multiplayer game-creation parameters.
 */

#include "mp_game_settings.hpp"

mp_game_settings::mp_game_settings() :
	savegame_config(),
	name(),
	password(),
	hash(),
	mp_era(),
	mp_scenario(),
	village_gold(0),
	xp_modifier(0),
	mp_countdown_init_time(0),
	mp_countdown_reservoir_time(0),
	mp_countdown_turn_bonus(0),
	mp_countdown_action_bonus(0),
	mp_countdown(false),
	use_map_settings(false),
	random_start_time(false),
	fog_game(false),
	shroud_game(false),
	allow_observers(false),
	share_view(false),
	share_maps(false),
	saved_game(false),
	scenario_data()

{ reset(); }

mp_game_settings::mp_game_settings(const config& cfg) :
	savegame_config(),
	name(),
	password(),
	hash(),
	mp_era(),
	mp_scenario(),
	village_gold(0),
	xp_modifier(0),
	mp_countdown_init_time(0),
	mp_countdown_reservoir_time(0),
	mp_countdown_turn_bonus(0),
	mp_countdown_action_bonus(0),
	mp_countdown(false),
	use_map_settings(false),
	random_start_time(false),
	fog_game(false),
	shroud_game(false),
	allow_observers(false),
	share_view(false),
	share_maps(false),
	saved_game(false),
	scenario_data()
{
	set_from_config(cfg);
}

mp_game_settings::mp_game_settings(const mp_game_settings& settings)
	: savegame_config()
	, name(settings.name)
	, password(settings.password)
	, hash(settings.hash)
	, mp_era(settings.mp_era)
	, mp_scenario(settings.mp_scenario)
	, village_gold(settings.village_gold)
	, xp_modifier(settings.xp_modifier)
	, mp_countdown_init_time(settings.mp_countdown_init_time)
	, mp_countdown_reservoir_time(settings.mp_countdown_reservoir_time)
	, mp_countdown_turn_bonus(settings.mp_countdown_turn_bonus)
	, mp_countdown_action_bonus(settings.mp_countdown_action_bonus)
	, mp_countdown(settings.mp_countdown)
	, use_map_settings(settings.use_map_settings)
	, random_start_time(settings.random_start_time)
	, fog_game(settings.fog_game)
	, shroud_game(settings.shroud_game)
	, allow_observers(settings.allow_observers)
	, share_view(settings.share_view)
	, share_maps(settings.share_maps)
	, saved_game(settings.saved_game)
	, scenario_data(settings.scenario_data)
{
}

void mp_game_settings::set_from_config(const config& game_cfg)
{
	const config& mp = game_cfg.child("multiplayer");
	const config& rs = game_cfg.child("replay_start");
	// if it's a replay the multiplayer section can be in the replay_start tag else fallback to top level
	const config& cfg = mp ? mp : rs ? (rs.child("multiplayer") ? rs.child("multiplayer") : game_cfg) : game_cfg;
	name = cfg["scenario"].str();
	hash = cfg["hash"].str();
	mp_era = cfg["mp_era"].str();
	mp_scenario = cfg["mp_scenario"].str();
	xp_modifier = cfg["experience_modifier"];
	use_map_settings = cfg["mp_use_map_settings"].to_bool();
	fog_game = cfg["mp_fog"].to_bool();
	shroud_game = cfg["mp_shroud"].to_bool();
	mp_countdown = cfg["mp_countdown"].to_bool();
	mp_countdown_init_time = cfg["mp_countdown_init_time"];
	mp_countdown_turn_bonus = cfg["mp_countdown_turn_bonus"];
	mp_countdown_reservoir_time = cfg["mp_countdown_reservoir_time"];
	mp_countdown_action_bonus = cfg["mp_countdown_action_bonus"];
	village_gold = cfg["mp_village_gold"];
	allow_observers = cfg["observer"].to_bool();
	saved_game = cfg["savegame"].to_bool();
}

void mp_game_settings::reset()
{
	name = "";
	password = "";
	hash = "";
	mp_era = "";
	mp_scenario = "";
	village_gold = 0;
	xp_modifier = 0;
	mp_countdown_init_time=0;
	mp_countdown_reservoir_time=0;
	mp_countdown_turn_bonus=0;
	mp_countdown_action_bonus=0;
	mp_countdown=false;
	use_map_settings = random_start_time = fog_game = shroud_game = allow_observers = share_view = share_maps = false;

	scenario_data.clear();
}

config mp_game_settings::to_config() const
{
	config cfg;

	cfg["scenario"] = name;
	cfg["hash"] = hash;
	cfg["mp_era"] = mp_era;
	cfg["mp_scenario"] = mp_scenario;
	cfg["experience_modifier"] = xp_modifier;
	cfg["mp_countdown"] = mp_countdown;
	cfg["mp_countdown_init_time"] = mp_countdown_init_time;
	cfg["mp_countdown_turn_bonus"] = mp_countdown_turn_bonus;
	cfg["mp_countdown_reservoir_time"] = mp_countdown_reservoir_time;
	cfg["mp_countdown_action_bonus"] = mp_countdown_action_bonus;
	cfg["mp_village_gold"] = village_gold;
	cfg["mp_fog"] = fog_game;
	cfg["mp_shroud"] = shroud_game;
	cfg["mp_use_map_settings"] = use_map_settings;
	cfg["observer"] = allow_observers;
	cfg["savegame"] = saved_game;

	return cfg;
}
