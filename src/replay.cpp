/* $Id: replay.cpp 48153 2011-01-01 15:57:50Z mordante $ */
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

/**
 *  @file
 *  Replay control code.
 *
 *  See http://www.wesnoth.org/wiki/ReplayWML for more info.
 */

#include "global.hpp"

#include "dialogs.hpp"
#include "foreach.hpp"
#include "game_display.hpp"
#include "game_end_exceptions.hpp"
#include "game_events.hpp"
#include "game_preferences.hpp"
#include "gamestatus.hpp"
#include "log.hpp"
#include "map_label.hpp"
#include "map_location.hpp"
#include "play_controller.hpp"
#include "replay.hpp"
#include "resources.hpp"
#include "rng.hpp"
#include "statistics.hpp"
#include "wesconfig.h"

#include <boost/bind.hpp>

static lg::log_domain log_replay("replay");
#define DBG_REPLAY LOG_STREAM(debug, log_replay)
#define LOG_REPLAY LOG_STREAM(info, log_replay)
#define WRN_REPLAY LOG_STREAM(warn, log_replay)
#define ERR_REPLAY LOG_STREAM(err, log_replay)

static lg::log_domain log_random("random");
#define DBG_RND LOG_STREAM(debug, log_random)
#define LOG_RND LOG_STREAM(info, log_random)
#define WRN_RND LOG_STREAM(warn, log_random)
#define ERR_RND LOG_STREAM(err, log_random)


//functions to verify that the unit structure on both machines is identical

static void verify(const unit_map& units, const config& cfg) {
	std::stringstream errbuf;
	LOG_REPLAY << "verifying unit structure...\n";

	const size_t nunits = lexical_cast_default<size_t>(cfg["num_units"]);
	if(nunits != units.size()) {
		errbuf << "SYNC VERIFICATION FAILED: number of units from data source differ: "
			   << nunits << " according to data source. " << units.size() << " locally\n";

		std::set<map_location> locs;
		foreach (const config &u, cfg.child_range("unit"))
		{
			const map_location loc(u, resources::state_of_game);
			locs.insert(loc);

			if(units.count(loc) == 0) {
				errbuf << "data source says there is a unit at "
					   << loc << " but none found locally\n";
			}
		}

		for(unit_map::const_iterator j = units.begin(); j != units.end(); ++j) {
			if (locs.count(j->get_location()) == 0) {
				errbuf << "local unit at " << j->get_location()
					   << " but none in data source\n";
			}
		}
		replay::process_error(errbuf.str());
		errbuf.clear();
	}

	foreach (const config &un, cfg.child_range("unit"))
	{
		const map_location loc(un, resources::state_of_game);
		const unit_map::const_iterator u = units.find(loc);
		if(u == units.end()) {
			errbuf << "SYNC VERIFICATION FAILED: data source says there is a '"
				   << un["type"] << "' (side " << un["side"] << ") at "
				   << loc << " but there is no local record of it\n";
			replay::process_error(errbuf.str());
			errbuf.clear();
		}

		config cfg;
		u->write(cfg);

		bool is_ok = true;
		static const std::string fields[] = {"type","hitpoints","experience","side",""};
		for(const std::string* str = fields; str->empty() == false; ++str) {
			if (cfg[*str] != un[*str]) {
				errbuf << "ERROR IN FIELD '" << *str << "' for unit at "
					   << loc << " data source: '" << un[*str]
					   << "' local: '" << cfg[*str] << "'\n";
				is_ok = false;
			}
		}

		if(!is_ok) {
			errbuf << "(SYNC VERIFICATION FAILED)\n";
			replay::process_error(errbuf.str());
			errbuf.clear();
		}
	}

	LOG_REPLAY << "verification passed\n";
}

// FIXME: this one now has to be assigned with set_random_generator
// from play_level or similar.  We should surely hunt direct
// references to it from this very file and move it out of here.
replay recorder;

replay::replay() :
	cfg_(),
	pos_(0),
	current_(NULL),
	skip_(false),
	message_locations(),
	expected_advancements_()
{}

replay::replay(const config& cfg) :
	cfg_(cfg),
	pos_(0),
	current_(NULL),
	skip_(false),
	message_locations(),
	expected_advancements_()
{}

void replay::append(const config& cfg)
{
	cfg_.append(cfg);
}

void replay::process_error(const std::string& msg)
{
	ERR_REPLAY << msg;

	resources::controller->process_oos(msg); // might throw end_level_exception(QUIT)
}

void replay::set_skip(bool skip)
{
	skip_ = skip;
}

bool replay::is_skipping() const
{
	return skip_;
}

void replay::add_unit_checksum(const map_location& loc,config* const cfg)
{
	if(! game_config::mp_debug) {
		return;
	}
	config& cc = cfg->add_child("checksum");
	loc.write(cc);
	unit_map::const_iterator u = resources::units->find(loc);
	assert(u.valid());
	cc["value"] = get_checksum(*u);
}

void replay::add_start()
{
	config* const cmd = add_command(true);
	cmd->add_child("start");
}

void replay::add_recruit(int value, const map_location& loc)
{
	config* const cmd = add_command();

	config val;
	val["value"] = value;
	loc.write(val);

	cmd->add_child("recruit",val);
}

void replay::add_recall(const std::string& unit_id, const map_location& loc)
{
	config* const cmd = add_command();

	config val;

	val["value"] = unit_id;

	loc.write(val);

	cmd->add_child("recall",val);
}

void replay::add_disband(const std::string& unit_id)
{
	config* const cmd = add_command();

	config val;

	val["value"] = unit_id;

	cmd->add_child("disband",val);
}

void replay::add_countdown_update(int value, int team)
{
	config* const cmd = add_command();
	config val;
	val["value"] = value;
	val["team"] = team;
	cmd->add_child("countdown_update",val);
}


void replay::add_movement(const std::vector<map_location>& steps)
{
	if(steps.empty()) { // no move, nothing to record
		return;
	}

	config* const cmd = add_command();

	config move;
	write_locations(steps, move);

	cmd->add_child("move",move);
}

void replay::add_attack(const map_location& a, const map_location& b,
	int att_weapon, int def_weapon, const std::string& attacker_type_id,
	const std::string& defender_type_id, int attacker_lvl,
	int defender_lvl, const size_t turn, const time_of_day &t)
{
	add_pos("attack",a,b);
	config &cfg = current_->child("attack");

	cfg["weapon"] = att_weapon;
	cfg["defender_weapon"] = def_weapon;
	cfg["attacker_type"] = attacker_type_id;
	cfg["defender_type"] = defender_type_id;
	cfg["attacker_lvl"] = attacker_lvl;
	cfg["defender_lvl"] = defender_lvl;
	cfg["turn"] = int(turn);
	cfg["tod"] = t.id;
	add_unit_checksum(a,current_);
	add_unit_checksum(b,current_);
}

void replay::add_seed(const char* child_name, int seed)
{
	LOG_REPLAY << "Setting seed for child type " << child_name << ": " << seed << "\n";
	random()->child(child_name)["seed"] = seed;
}

void replay::add_pos(const std::string& type,
                     const map_location& a, const map_location& b)
{
	config* const cmd = add_command();

	config move, src, dst;
	a.write(src);
	b.write(dst);

	move.add_child("source",src);
	move.add_child("destination",dst);
	cmd->add_child(type,move);
}

void replay::user_input(const std::string &name, const config &input)
{
	add_command()->add_child(name, input);
}

void replay::add_label(const terrain_label* label)
{
	assert(label);
	config* const cmd = add_command(false);

	(*cmd)["undo"] = false;

	config val;

	label->write(val);

	cmd->add_child("label",val);
}

void replay::clear_labels(const std::string& team_name, bool force)
{
	config* const cmd = add_command(false);

	(*cmd)["undo"] = false;
	config val;
	val["team_name"] = team_name;
	val["force"] = force;
	cmd->add_child("clear_labels",val);
}

void replay::add_rename(const std::string& name, const map_location& loc)
{
	config* const cmd = add_command(false);
	(*cmd)["async"] = true; // Not undoable, but depends on moves/recruits that are
	config val;
	loc.write(val);
	val["name"] = name;
	cmd->add_child("rename", val);
}

void replay::init_side()
{
	config* const cmd = add_command();
	cmd->add_child("init_side");
}

void replay::end_turn()
{
	config* const cmd = add_command();
	cmd->add_child("end_turn");
}

void replay::add_event(const std::string& name, const map_location& loc)
{
	config* const cmd = add_command();
	config& ev = cmd->add_child("fire_event");
	ev["raise"] = name;
	if(loc.valid()) {
		config& source = ev.add_child("source");
		loc.write(source);
	}
	(*cmd)["undo"] = false;
}

void replay::add_log_data(const std::string &key, const std::string &var)
{
	config& ulog = cfg_.child_or_add("upload_log");
	ulog[key] = var;
}

void replay::add_log_data(const std::string &category, const std::string &key, const std::string &var)
{
	config& ulog = cfg_.child_or_add("upload_log");
	config& cat = ulog.child_or_add(category);
	cat[key] = var;
}

void replay::add_log_data(const std::string &category, const std::string &key, const config &c)
{
	config& ulog = cfg_.child_or_add("upload_log");
	config& cat = ulog.child_or_add(category);
	cat.add_child(key,c);
}

void replay::add_checksum_check(const map_location& loc)
{
	if(! game_config::mp_debug || ! (resources::units->find(loc).valid()) ) {
		return;
	}
	config* const cmd = add_command();
	add_unit_checksum(loc,cmd);
}

void replay::add_expected_advancement(const map_location& loc)
{
	expected_advancements_.push_back(loc);
}

const std::deque<map_location>& replay::expected_advancements() const
{
	return expected_advancements_;
}

void replay::pop_expected_advancement()
{
	expected_advancements_.pop_front();
}

void replay::add_advancement(const map_location& loc)
{
	config* const cmd = add_command(false);

	config val;
	(*cmd)["undo"] = false;
	loc.write(val);
	cmd->add_child("advance_unit",val);
	DBG_REPLAY << "added an explicit advance\n";
}

void replay::add_chat_message_location()
{
	message_locations.push_back(pos_-1);
}

void replay::speak(const config& cfg)
{
	config* const cmd = add_command(false);
	if(cmd != NULL) {
		cmd->add_child("speak",cfg);
		(*cmd)["undo"] = false;
		add_chat_message_location();
	}
}

void replay::add_chat_log_entry(const config &cfg, std::ostream &str) const
{
	if (!cfg) return;

	if (!preferences::parse_should_show_lobby_join(cfg["id"], cfg["message"])) return;
	if (preferences::is_ignored(cfg["id"])) return;
	const std::string& team_name = cfg["team_name"];
	if(team_name == "") {
		str << "<" << cfg["id"] << "> ";
	} else {
		str << "*" << cfg["id"] << "* ";
	}
	str << cfg["message"] << "\n";
}

void replay::remove_command(int index)
{
	cfg_.remove_child("command", index);
	std::vector<int>::reverse_iterator loc_it;
	for (loc_it = message_locations.rbegin(); loc_it != message_locations.rend() && index < *loc_it;++loc_it)
	{
		--(*loc_it);
	}
}

// cached message log
std::stringstream message_log;


std::string replay::build_chat_log()
{
	std::vector<int>::iterator loc_it;
	int last_location = 0;
	for (loc_it = message_locations.begin(); loc_it != message_locations.end(); ++loc_it)
	{
		last_location = *loc_it;
		const config &speak = command(last_location).child("speak");
		add_chat_log_entry(speak, message_log);

	}
	message_locations.clear();

#if 0
	for(config::child_list::const_iterator i = cmd.begin() + (last_location + 1); i != cmd.end(); ++i) {
		++last_location;
		const config* speak = (**i).child("speak");
		if(speak != NULL) {
			message_locations.push_back(last_location);
			add_chat_log_entry(speak, str);
		}
	}
#endif
	return message_log.str();
}

config replay::get_data_range(int cmd_start, int cmd_end, DATA_TYPE data_type)
{
	config res;

	for (int cmd = cmd_start; cmd < cmd_end; ++cmd)
	{
		config &c = command(cmd);
		if ((data_type == ALL_DATA || c["undo"] == "no") && c["sent"] != "yes")
		{
			res.add_child("command", c);
			if (data_type == NON_UNDO_DATA) c["sent"] = true;
		}
	}

	return res;
}

struct async_cmd
{
	config *cfg;
	int num;
};

void replay::undo()
{
	std::vector<async_cmd> async_cmds;
	// Remember commands not yet synced and skip over them.
	// We assume that all already sent (sent=yes) data isn't undoable
	// even if not marked explicitly with undo=no.

	/**
	 * @todo Change undo= to default to "no" and explicitly mark all
	 * undoable commands with yes.
	 */

	int cmd;
	for (cmd = ncommands() - 1; cmd >= 0; --cmd)
	{
		config &c = command(cmd);
		if (c["undo"] != "no" && c["async"] != "yes" && c["sent"] != "yes") break;
		if (c["async"] == "yes") {
			async_cmd ac = { &c, cmd };
			async_cmds.push_back(ac);
		}
	}

	if (cmd < 0) return;
	config &c = command(cmd);

	if (const config &child = c.child("move"))
	{
		// A unit's move is being undone.
		// Repair unsynced cmds whose locations depend on that unit's location.
		const std::vector<map_location> steps =
			parse_location_range(child["x"], child["y"]);

		if (steps.empty()) {
			ERR_REPLAY << "trying to undo a move using an empty path";
			return; // nothing to do, I suppose.
		}

		const map_location &src = steps.front();
		const map_location &dst = steps.back();

		foreach (const async_cmd &ac, async_cmds)
		{
			if (config &async_child = ac.cfg->child("rename")) {
				map_location aloc(async_child, resources::state_of_game);
				if (dst == aloc) src.write(async_child);
			}
		}
	}
	else
	{
		const config *chld = &c.child("recruit");
		if (!*chld) chld = &c.child("recall");
		if (*chld) {
			// A unit is being un-recruited or un-recalled.
			// Remove unsynced commands that would act on that unit.
			map_location src(*chld, resources::state_of_game);
			foreach (const async_cmd &ac, async_cmds)
			{
				if (config &async_child = ac.cfg->child("rename"))
				{
					map_location aloc(async_child, resources::state_of_game);
					if (src == aloc) remove_command(ac.num);
				}
			}
		}
	}

	remove_command(cmd);
	current_ = NULL;
	set_random(NULL);
}

config &replay::command(int n)
{
	return cfg_.child("command", n);
}

int replay::ncommands() const
{
	return cfg_.child_count("command");
}

config* replay::add_command(bool update_random_context)
{
	pos_ = ncommands()+1;
	current_ = &cfg_.add_child("command");
	if(update_random_context)
		set_random(current_);

	return current_;
}

void replay::start_replay()
{
	pos_ = 0;
}

void replay::revert_action()
{
	if (pos_ > 0)
		--pos_;
}

config* replay::get_next_action()
{
	if (pos_ >= ncommands())
		return NULL;

	LOG_REPLAY << "up to replay action " << pos_ + 1 << '/' << ncommands() << '\n';

	current_ = &command(pos_);
	set_random(current_);
	++pos_;
	return current_;
}

void replay::pre_replay()
{
	if (rng::random() == NULL && ncommands() > 0) {
		if (at_end())
		{
			add_command(true);
		}
		else
		{
			set_random(&command(pos_));
		}
	}
}

bool replay::at_end() const
{
	return pos_ >= ncommands();
}

void replay::set_to_end()
{
	pos_ = ncommands();
	current_ = NULL;
	set_random(NULL);
}

void replay::clear()
{
	message_locations.clear();
	message_log.str(std::string());
	cfg_ = config();
	pos_ = 0;
	current_ = NULL;
	set_random(NULL);
	skip_ = 0;
}

bool replay::empty()
{
	return ncommands() == 0;
}

void replay::add_config(const config& cfg, MARK_SENT mark)
{
	foreach (const config &cmd, cfg.child_range("command"))
	{
		config &cfg = cfg_.add_child("command", cmd);
		if (cfg.child("speak"))
		{
			pos_ = ncommands();
			add_chat_message_location();
		}
		if(mark == MARK_AS_SENT) {
			cfg["sent"] = true;
		}
	}
}

namespace {

replay* replay_src = NULL;

struct replay_source_manager
{
	replay_source_manager(replay* o) : old_(replay_src)
	{
		replay_src = o;
	}

	~replay_source_manager()
	{
		replay_src = old_;
	}

private:
	replay* const old_;
};

}

replay& get_replay_source()
{
	if(replay_src != NULL) {
		return *replay_src;
	} else {
		return recorder;
	}
}

static void check_checksums(const config &cfg)
{
	if(! game_config::mp_debug) {
		return;
	}
	foreach (const config &ch, cfg.child_range("checksum"))
	{
		map_location loc(ch, resources::state_of_game);
		unit_map::const_iterator u = resources::units->find(loc);
		if (!u.valid()) {
			std::stringstream message;
			message << "non existent unit to checksum at " << loc.x+1 << "," << loc.y+1 << "!";
			resources::screen->add_chat_message(time(NULL), "verification", 1, message.str(),
					events::chat_handler::MESSAGE_PRIVATE, false);
			continue;
		}
		if (get_checksum(*u) != ch["value"]) {
			std::stringstream message;
			message << "checksum mismatch at " << loc.x+1 << "," << loc.y+1 << "!";
			resources::screen->add_chat_message(time(NULL), "verification", 1, message.str(),
					events::chat_handler::MESSAGE_PRIVATE, false);
		}
	}
}

bool do_replay(int side_num, replay *obj)
{
	log_scope("do replay");

	const replay_source_manager replaymanager(obj);

//	replay& replayer = (obj != NULL) ? *obj : recorder;

	if (!get_replay_source().is_skipping()){
		resources::screen->recalculate_minimap();
	}

	const rand_rng::set_random_generator generator_setter(&get_replay_source());

	update_locker lock_update(resources::screen->video(),get_replay_source().is_skipping());
	return do_replay_handle(side_num, "");
}

bool do_replay_handle(int side_num, const std::string &do_untill)
{
	//a list of units that have promoted from the last attack
	std::deque<map_location> advancing_units;

	team &current_team = (*resources::teams)[side_num - 1];


	for(;;) {
		const config *cfg = get_replay_source().get_next_action();

		//do we need to recalculate shroud after this action is processed?

		bool fix_shroud = false;
		if (cfg)
		{
			DBG_REPLAY << "Replay data:\n" << *cfg << "\n";
		}
		else
		{
			DBG_REPLAY << "Replay data at end\n";
		}

		//if there is nothing more in the records
		if(cfg == NULL) {
			//replayer.set_skip(false);
			return false;
		}

		//if we are expecting promotions here`
		if (!get_replay_source().expected_advancements().empty()) {
			//if there is a promotion, we process it and go onto the next command
			//but if this isn't a promotion, we just keep waiting for the promotion
			//command -- it may have been mixed up with other commands such as messages
			if (const config &child = cfg->child("choose")) {
				int val = child["value"];
				map_location loc = get_replay_source().expected_advancements().front();
				dialogs::animate_unit_advancement(loc, val);
				get_replay_source().pop_expected_advancement();

				DBG_REPLAY << "advanced unit " << val << " at " << loc << '\n';

				//if there are no more advancing units, then we check for victory,
				//in case the battle that led to advancement caused the end of scenario
				if(advancing_units.empty()) {
					resources::controller->check_victory();
				}

				if (do_untill == "choose") {
					get_replay_source().revert_action();
					return false;
				}

				continue;
			}
		}

		// We return if caller wants it for this tag
		if (!do_untill.empty() && cfg->child(do_untill))
		{
			get_replay_source().revert_action();
			return false;
		}

		config::all_children_itors ch_itors = cfg->all_children_range();
		//if there is an empty command tag, create by pre_replay() or a start tag
		if (ch_itors.first == ch_itors.second || cfg->child("start"))
		{
			//do nothing
		}
		else if (const config &child = cfg->child("speak"))
		{
			const std::string &team_name = child["team_name"];
			const std::string &speaker_name = child["id"];
			const std::string &message = child["message"];
			//if (!preferences::parse_should_show_lobby_join(speaker_name, message)) return;
			bool is_whisper = (speaker_name.find("whisper: ") == 0);
			get_replay_source().add_chat_message_location();
			if (!get_replay_source().is_skipping() || is_whisper) {
				int side = child["side"];
				resources::screen->add_chat_message(time(NULL), speaker_name, side, message,
						(team_name.empty() ? events::chat_handler::MESSAGE_PUBLIC
						: events::chat_handler::MESSAGE_PRIVATE),
						preferences::message_bell());
			}
		}
		else if (const config &child = cfg->child("label"))
		{
			terrain_label label(resources::screen->labels(), child);

			resources::screen->labels().set_label(label.location(),
						label.text(),
						label.team_name(),
						label.color());
		}
		else if (const config &child = cfg->child("clear_labels"))
		{
			resources::screen->labels().clear(std::string(child["team_name"]), child["force"].to_bool());
		}
		else if (const config &child = cfg->child("rename"))
		{
			const map_location loc(child, resources::state_of_game);
			const std::string &name = child["name"];

			unit_map::iterator u = resources::units->find(loc);
			if (u.valid()) {
				if (u->unrenamable()) {
					std::stringstream errbuf;
					errbuf << "renaming unrenamable unit " << u->id() << '\n';
					replay::process_error(errbuf.str());
					continue;
				}
				u->rename(name);
			} else {
				// Users can rename units while it's being killed at another machine.
				// This since the player can rename units when it's not his/her turn.
				// There's not a simple way to prevent that so in that case ignore the
				// rename instead of throwing an OOS.
				WRN_REPLAY << "attempt to rename unit at location: "
				   << loc << ", where none exists (anymore).\n";
			}
		}

		else if (cfg->child("init_side"))
		{
			resources::controller->do_init_side(side_num - 1);
		}

		//if there is an end turn directive
		else if (cfg->child("end_turn"))
		{
			if (const config &child = cfg->child("verify")) {
				verify(*resources::units, child);
			}

			return true;
		}

		else if (const config &child = cfg->child("recruit"))
		{
			int val = child["value"];

			map_location loc(child, resources::state_of_game);

			const std::set<std::string>& recruits = current_team.recruits();

			if(val < 0 || static_cast<size_t>(val) >= recruits.size()) {
				std::stringstream errbuf;
				errbuf << "recruitment index is illegal: " << val
				       << " while this side only has " << recruits.size()
				       << " units available for recruitment\n";
				replay::process_error(errbuf.str());
				continue;
			}

			std::set<std::string>::const_iterator itor = recruits.begin();
			std::advance(itor,val);
			const unit_type *u_type = unit_types.find(*itor);
			if (!u_type) {
				std::stringstream errbuf;
				errbuf << "recruiting illegal unit: '" << *itor << "'\n";
				replay::process_error(errbuf.str());
				continue;
			}

			const std::string res = find_recruit_location(side_num, loc);
			const unit new_unit(u_type, side_num, true);
			if (res.empty()) {
				place_recruit(new_unit, loc, false, !get_replay_source().is_skipping());
			} else {
				std::stringstream errbuf;
				errbuf << "cannot recruit unit: " << res << "\n";
				replay::process_error(errbuf.str());
			}

			if (u_type->cost() > current_team.gold()) {
				std::stringstream errbuf;
				errbuf << "unit '" << u_type->id() << "' is too expensive to recruit: "
					<< u_type->cost() << "/" << current_team.gold() << "\n";
				replay::process_error(errbuf.str());
			}
			LOG_REPLAY << "recruit: team=" << side_num << " '" << u_type->id() << "' at (" << loc
				<< ") cost=" << u_type->cost() << " from gold=" << current_team.gold() << ' ';


			statistics::recruit_unit(new_unit);

			current_team.spend_gold(u_type->cost());
			LOG_REPLAY << "-> " << (current_team.gold()) << "\n";
			fix_shroud = !get_replay_source().is_skipping();
			check_checksums(*cfg);
		}

		else if (const config &child = cfg->child("recall"))
		{
			const std::string& unit_id = child["value"];
			map_location loc(child, resources::state_of_game);

			std::vector<unit>::iterator recall_unit = std::find_if(current_team.recall_list().begin(),
				current_team.recall_list().end(), boost::bind(&unit::matches_id, _1, unit_id));

			if (recall_unit != current_team.recall_list().end()) {
				statistics::recall_unit(*recall_unit);
				place_recruit(*recall_unit, loc, true, !get_replay_source().is_skipping());
				current_team.recall_list().erase(recall_unit);
				current_team.spend_gold(current_team.recall_cost());
				fix_shroud = !get_replay_source().is_skipping();
			} else {
				replay::process_error("illegal recall: unit_id '" + unit_id + "' could not be found within the recall list.\n");
			}
			check_checksums(*cfg);
		}

		else if (const config &child = cfg->child("disband"))
		{
			const std::string& unit_id = child["value"];
			std::vector<unit>::iterator disband_unit = std::find_if(current_team.recall_list().begin(),
				current_team.recall_list().end(), boost::bind(&unit::matches_id, _1, unit_id));

			if(disband_unit != current_team.recall_list().end()) {
				current_team.recall_list().erase(disband_unit);
			} else {
				replay::process_error("illegal disband\n");
			}
		}
		else if (const config &child = cfg->child("countdown_update"))
		{
			int val = child["value"];
			int tval = child["team"];
			if (tval <= 0  || tval > int(resources::teams->size())) {
				std::stringstream errbuf;
				errbuf << "Illegal countdown update \n"
					<< "Received update for :" << tval << " Current user :"
					<< side_num << "\n" << " Updated value :" << val;

				replay::process_error(errbuf.str());
			} else {
				(*resources::teams)[tval - 1].set_countdown_time(val);
			}
		}
		else if (const config &child = cfg->child("move"))
		{
			const std::string& x = child["x"];
			const std::string& y = child["y"];
			std::vector<map_location> steps = parse_location_range(x,y);

			if(steps.empty()) {
				WRN_REPLAY << "Warning: Missing path data found in [move]\n";
				continue;
			}

			map_location src = steps.front();
			map_location dst = steps.back();

			if (src == dst) {
				WRN_REPLAY << "Warning: Move with identical source and destination. Skipping...";
				continue;
			}

			unit_map::iterator u = resources::units->find(dst);
			if (u.valid()) {
				std::stringstream errbuf;
				errbuf << "destination already occupied: "
				       << dst << '\n';
				replay::process_error(errbuf.str());
				continue;
			}
			u = resources::units->find(src);
			if (!u.valid()) {
				std::stringstream errbuf;
				errbuf << "unfound location for source of movement: "
				       << src << " -> " << dst << '\n';
				replay::process_error(errbuf.str());
				continue;
			}

			bool show_move = preferences::show_ai_moves() || !(current_team.is_ai() || current_team.is_network_ai());
			::move_unit(NULL, steps, NULL, NULL, show_move, NULL, true, true, true);

			//NOTE: The AI fire sighetd event whem moving in the FoV of team 1
			// (supposed to be the human player in SP)
			// That's ugly but let's try to make the replay works like that too
			if (side_num != 1 && resources::teams->front().fog_or_shroud() && !resources::teams->front().fogged(dst)
					 && (current_team.is_ai() || current_team.is_network_ai()))
			{
				// the second parameter is impossible to know
				// and the AI doesn't use it too in the local version
				game_events::fire("sighted",dst);
			}
		}

		else if (const config &child = cfg->child("attack"))
		{
			const config &destination = child.child("destination");
			const config &source = child.child("source");
			check_checksums(*cfg);

			if (!destination || !source) {
				replay::process_error("no destination/source found in attack\n");
				continue;
			}

			//we must get locations by value instead of by references, because the iterators
			//may become invalidated later
			const map_location src(source, resources::state_of_game);
			const map_location dst(destination, resources::state_of_game);

			int weapon_num = child["weapon"];
			int def_weapon_num = child["defender_weapon"].to_int(-2);
			if (def_weapon_num == -2) {
				// Let's not gratuitously destroy backwards compat.
				WRN_REPLAY << "Old data, having to guess weapon\n";
				def_weapon_num = -1;
			}

			unit_map::iterator u = resources::units->find(src);
			if (!u.valid()) {
				replay::process_error("unfound location for source of attack\n");
				continue;
			}

			if (size_t(weapon_num) >= u->attacks().size()) {
				replay::process_error("illegal weapon type in attack\n");
				continue;
			}

			unit_map::const_iterator tgt = resources::units->find(dst);

			if (!tgt.valid()) {
				std::stringstream errbuf;
				errbuf << "unfound defender for attack: " << src << " -> " << dst << '\n';
				replay::process_error(errbuf.str());
				continue;
			}

			if (def_weapon_num >= static_cast<int>(tgt->attacks().size())) {

				replay::process_error("illegal defender weapon type in attack\n");
				continue;
			}

			int seed = child["seed"];
			rand_rng::set_seed(child["seed"]);
			LOG_REPLAY << "Replaying attack with seed " << seed << "\n";

			DBG_REPLAY << "Attacker XP (before attack): " << u->experience() << "\n";

			attack_unit(src, dst, weapon_num, def_weapon_num, !get_replay_source().is_skipping());

			u = resources::units->find(src);
			tgt = resources::units->find(dst);

			if(u.valid()){
				DBG_REPLAY << "Attacker XP (after attack): " << u->experience() << "\n";
				if (u->advances()) {
					get_replay_source().add_expected_advancement(u->get_location());
				}
			}

			DBG_REPLAY << "expected_advancements.size: " << get_replay_source().expected_advancements().size() << "\n";
			if (tgt.valid() && tgt->advances()) {
				get_replay_source().add_expected_advancement(tgt->get_location());
			}

			//check victory now if we don't have any advancements. If we do have advancements,
			//we don't check until the advancements are processed.
			if(get_replay_source().expected_advancements().empty()) {
				resources::controller->check_victory();
			}
			fix_shroud = !get_replay_source().is_skipping();
		}
		else if (const config &child = cfg->child("fire_event"))
		{
			foreach (const config &v, child.child_range("set_variable")) {
				resources::state_of_game->set_variable(v["name"], v["value"]);
			}
			const std::string &event = child["raise"];
			if (const config &source = child.child("source")) {
				game_events::fire(event, map_location(source, resources::state_of_game));
			} else {
				game_events::fire(event);
			}

		}
		else if (const config &child = cfg->child("advance_unit"))
		{
			const map_location loc(child, resources::state_of_game);
			get_replay_source().add_expected_advancement(loc);
			DBG_REPLAY << "got an explicit advance\n";

		} else if (cfg->child("global_variable")) {
		} else {
			if(! cfg->child("checksum")) {
				replay::process_error("unrecognized action:\n" + cfg->debug());
			} else {
				check_checksums(*cfg);
			}
		}

		//Check if we should refresh the shroud, and redraw the minimap/map tiles.
		//This is needed for shared vision to work properly.
		if (fix_shroud && clear_shroud(side_num) && !recorder.is_skipping()) {
			resources::screen->recalculate_minimap();
			resources::screen->invalidate_game_status();
			resources::screen->invalidate_all();
			resources::screen->draw();
		}

		if (const config &child = cfg->child("verify")) {
			verify(*resources::units, child);
		}
	}

	return false; /* Never attained, but silent a gcc warning. --Zas */
}

replay_network_sender::replay_network_sender(replay& obj) : obj_(obj), upto_(obj_.ncommands())
{
}

replay_network_sender::~replay_network_sender()
{
	commit_and_sync();
}

void replay_network_sender::sync_non_undoable()
{
	if(network::nconnections() > 0) {
		config cfg;
		const config& data = cfg.add_child("turn",obj_.get_data_range(upto_,obj_.ncommands(),replay::NON_UNDO_DATA));
		if(data.empty() == false) {
			network::send_data(cfg, 0);
		}
	}
}

void replay_network_sender::commit_and_sync()
{
	if(network::nconnections() > 0) {
		config cfg;
		const config& data = cfg.add_child("turn",obj_.get_data_range(upto_,obj_.ncommands()));
		if(data.empty() == false) {
			network::send_data(cfg, 0);
		}

		upto_ = obj_.ncommands();
	}
}

config mp_sync::get_user_choice(const std::string &name, const user_choice &uch,
	int side, bool force_sp)
{
	if (force_sp && network::nconnections() != 0 &&
	    resources::state_of_game->phase() != game_state::PLAY)
	{
		/* We are in a multiplayer game, during an early event which
		   prevents synchronization, and the WML is not interested
		   in a random result. We cannot silently ignore the issue,
		   since it would lead to a broken replay. To be sure that
		   the WML does not catch the error and keep the game going,
		   we use a sticky exception to forcefully quit. */
		ERR_REPLAY << "MP synchronization does not work during prestart and start events.";
		throw end_level_exception(QUIT);
	}
	if (resources::state_of_game->phase() == game_state::PLAY || force_sp)
	{
		/* We have to communicate with the player and store the
		   choices in the replay. So a decision will be made on
		   one host and shared amongst all of them. */

		/* process the side parameter and ensure it is within boundaries */
		if (unsigned(side - 1) >= resources::teams->size())
			side = resources::controller->current_side();

		int active_side = side;
		if ((*resources::teams)[active_side - 1].is_local() &&
		    get_replay_source().at_end())
		{
			/* The decision is ours, and it will be inserted
			   into the replay. */
			DBG_REPLAY << "MP synchronization: local choice\n";
			config cfg = uch.query_user();
			recorder.user_input(name, cfg);
			return cfg;

		} else {
			/* The decision has already been made, and must
			   be extracted from the replay. */
			DBG_REPLAY << "MP synchronization: remote choice\n";
			do_replay_handle(active_side, name);
			const config *action = get_replay_source().get_next_action();
			if (!action || !*(action = &action->child(name))) {
				replay::process_error("[" + name + "] expected but none found\n");
				return config();
			}
			return *action;
		}
	}
	else
	{
		/* Neither the user nor a replay can be consulted, so a
		   decision will be made at all hosts simultaneously.
		   The result is not stored in the replay, since the
		   other clients have already taken the same decision. */
		DBG_REPLAY << "MP synchronization: synchronized choice\n";
		return uch.random_choice(resources::state_of_game->rng());
	}
}
