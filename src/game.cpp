/* $Id: game.cpp 48216 2011-01-08 20:40:28Z mordante $ */
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

#include "global.hpp"

#include "SDL.h"
#include "SDL_mixer.h"

#ifndef DISABLE_EDITOR
#include "SDL_getenv.h"
#endif

#include "about.hpp"
#include "ai/configuration.hpp"
#include "config.hpp"
#include "config_cache.hpp"
#include "construct_dialog.hpp"
#include "cursor.hpp"
#include "dialogs.hpp"
#include "foreach.hpp"
#include "game_display.hpp"
#include "game_preferences.hpp"
#include "builder.hpp"
#include "filesystem.hpp"
#include "font.hpp"
#include "formula.hpp"
#include "game_config.hpp"
#include "game_errors.hpp"
#include "gamestatus.hpp"
#include "gettext.hpp"
#include "gui/dialogs/addon_connect.hpp"
#include "gui/dialogs/campaign_difficulty.hpp"
#include "gui/dialogs/campaign_selection.hpp"
#include "gui/dialogs/language_selection.hpp"
#include "gui/dialogs/message.hpp"
#include "gui/dialogs/mp_method_selection.hpp"
#include "gui/dialogs/title_screen.hpp"
#include "gui/dialogs/transient_message.hpp"
#ifdef DEBUG_WINDOW_LAYOUT_GRAPHS
#include "gui/widgets/debug.hpp"
#endif
#include "gui/auxiliary/event/handler.hpp"
#include "gui/widgets/settings.hpp"
#include "gui/widgets/window.hpp"
#include "help.hpp"
#include "hotkeys.hpp"
#include "intro.hpp"
#include "language.hpp"
#include "loadscreen.hpp"
#include "log.hpp"
#include "map_exception.hpp"
#include "widgets/menu.hpp"
#include "marked-up_text.hpp"
#include "multiplayer.hpp"
#include "network.hpp"
#include "playcampaign.hpp"
#include "preferences_display.hpp"
#include "addon/manager.hpp"
#include "replay.hpp"
#include "savegame.hpp"
#include "scripting/lua.hpp"
#include "sound.hpp"
#include "statistics.hpp"
#include "thread.hpp"
#include "wml_exception.hpp"
#include "wml_separators.hpp"
#include "serialization/binary_or_text.hpp"
#include "serialization/parser.hpp"
#include "serialization/preprocessor.hpp"
#include "serialization/string_utils.hpp"
#include "sha1.hpp"
#include "version.hpp"

//#ifdef _WIN32
//#include "locale.h"
//#endif

#ifndef DISABLE_EDITOR
#include "editor/editor_main.hpp"
#endif

#include "wesconfig.h"

#include <cerrno>
#include <clocale>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>


#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/gzip.hpp>

// Minimum stack cookie to prevent stack overflow on AmigaOS4
#ifdef __amigaos4__
const char __attribute__((used)) stackcookie[] = "\0$STACK: 16000000";
#endif

static lg::log_domain log_config("config");
#define ERR_CONFIG LOG_STREAM(err, log_config)
#define WRN_CONFIG LOG_STREAM(warn, log_config)
#define LOG_CONFIG LOG_STREAM(info, log_config)

#define LOG_GENERAL LOG_STREAM(info, lg::general)
#define WRN_GENERAL LOG_STREAM(warn, lg::general)
#define DBG_GENERAL LOG_STREAM(debug, lg::general)

static lg::log_domain log_network("network");
#define ERR_NET LOG_STREAM(err, log_network)

static lg::log_domain log_preprocessor("preprocessor");
#define LOG_PREPROC LOG_STREAM(info,log_preprocessor)

static bool less_campaigns_rank(const config &a, const config &b) {
	return a["rank"].to_int(1000) < b["rank"].to_int(1000);
}

namespace {
struct jump_to_campaign_info
{
public:
	jump_to_campaign_info(bool jump,int difficulty, std::string campaign_id,std::string scenario_id)
		: jump_(jump)
		, difficulty_(difficulty)
		, campaign_id_(campaign_id)
		, scenario_id_(scenario_id)
	{
	}
	bool jump_;
	int difficulty_;
	std::string campaign_id_,scenario_id_;
};
class game_controller
{
public:
	game_controller(int argc, char** argv);
	~game_controller();

	game_display& disp();

	bool init_video();
	bool init_config(const bool force=false);
	bool init_language();
	bool play_test();
	bool play_multiplayer_mode();
	bool play_screenshot_mode();

	void reload_changed_game_config();

	bool is_loading() const;
	void clear_loaded_game() { game::load_game_exception::game.clear(); }
	bool load_game();
	void set_tutorial();

	std::string jump_to_campaign_id() const;
	bool new_campaign();
	bool goto_campaign();
	bool goto_multiplayer();
#ifndef DISABLE_EDITOR
	bool goto_editor();
#endif
	bool play_multiplayer();
	bool change_language();

	void show_preferences();

	enum RELOAD_GAME_DATA { RELOAD_DATA, NO_RELOAD_DATA };
	void launch_game(RELOAD_GAME_DATA reload=RELOAD_DATA);
	void play_replay();
#ifndef DISABLE_EDITOR
	editor::EXIT_STATUS start_editor(const std::string& filename = "");
#endif
	void start_wesnothd();
	const config& game_config() const { return game_config_; }

private:
	game_controller(const game_controller&);
	void operator=(const game_controller&);

	void load_game_cfg(const bool force=false);
	void set_unit_data();

	void mark_completed_campaigns(std::vector<config>& campaigns);

	const int argc_;
	int arg_;
	const char* const * const argv_;

	//this should get destroyed *after* the video, since we want
	//to clean up threads after the display disappears.
	const threading::manager thread_manager;

	CVideo video_;

	const font::manager font_manager_;
	const preferences::manager prefs_manager_;
	const image::manager image_manager_;
	const events::event_context main_event_context_;
	const hotkey::manager hotkey_manager_;
	sound::music_thinker music_thinker_;
	resize_monitor resize_monitor_;
	binary_paths_manager paths_manager_;

	std::string test_scenario_;

	bool test_mode_, multiplayer_mode_, no_gui_, screenshot_mode_;
	std::string screenshot_map_, screenshot_filename_;
	int force_bpp_;

	config game_config_;
	preproc_map old_defines_map_;

	util::scoped_ptr<game_display> disp_;

	game_state state_;

	std::string multiplayer_server_;
	bool jump_to_multiplayer_;
	jump_to_campaign_info jump_to_campaign_;
#ifndef DISABLE_EDITOR
	bool jump_to_editor_;
#endif
	game_config::config_cache& cache_;
};

game_controller::game_controller(int argc, char** argv) :
	argc_(argc),
	arg_(1),
	argv_(argv),
	thread_manager(),
	video_(),
	font_manager_(),
	prefs_manager_(),
	image_manager_(),
	main_event_context_(),
	hotkey_manager_(),
	music_thinker_(),
	resize_monitor_(),
	paths_manager_(),
	test_scenario_("test"),
	test_mode_(false),
	multiplayer_mode_(false),
	no_gui_(false),
	screenshot_mode_(false),
	screenshot_map_(),
	screenshot_filename_(),
	force_bpp_(-1),
	game_config_(),
	old_defines_map_(),
	disp_(NULL),
	state_(),
	multiplayer_server_(),
	jump_to_multiplayer_(false),
	jump_to_campaign_(false,-1,"","")
#ifndef DISABLE_EDITOR
	 ,jump_to_editor_(false)
#endif
	,cache_(game_config::config_cache::instance())
{
	bool no_music = false;
	bool no_sound = false;

	// The path can be hardcoded and it might be a relative path.
	if(!game_config::path.empty() &&
#ifdef _WIN32
		// use c_str to ensure that index 1 points to valid element since c_str() returns null-terminated string
		game_config::path.c_str()[1] != ':'
#else
		game_config::path[0] != '/'
#endif
	)
	{
		game_config::path = get_cwd() + '/' + game_config::path;
		font_manager_.update_font_path();
	}

#ifndef DISABLE_EDITOR
	const std::string app_basename = file_name(argv[0]);
	jump_to_editor_ = app_basename.find("editor") != std::string::npos;
#endif

	for(arg_ = 1; arg_ != argc_; ++arg_) {
		const std::string val(argv_[arg_]);
		if(val.empty()) {
			continue;
		}

		if(val == "--fps") {
			preferences::set_show_fps(true);
		} else if(val == "--nocache") {
			cache_.set_use_cache(false);
		} else if(val == "--max-fps") {
			if(arg_+1 != argc_) {
				++arg_;
				int fps = lexical_cast_default<int>(argv_[arg_], 50);
				fps = std::min<int>(fps, 1000);
				fps = std::max<int>(fps, 1);
				fps = 1000 / fps;
				// increase the delay to avoid going above the maximum
				if(1000 % fps != 0) {
					++fps;
				}
				preferences::set_draw_delay(fps);
			}
		} else if(val == "--validcache") {
			cache_.set_force_valid_cache(true);
		} else if(val == "--resolution" || val == "-r") {
			if(arg_+1 != argc_) {
				++arg_;
				const std::string val(argv_[arg_]);
				const std::vector<std::string> res = utils::split(val, 'x');
				if(res.size() == 2) {
					const int xres = lexical_cast_default<int>(res.front());
					const int yres = lexical_cast_default<int>(res.back());
					if(xres > 0 && yres > 0) {
						const std::pair<int,int> resolution(xres,yres);
						preferences::set_resolution(resolution);
					}
				}
			}
		} else if(val == "--bpp") {
			if(arg_+1 != argc_) {
				++arg_;
				force_bpp_ = lexical_cast_default<int>(argv_[arg_],-1);
			}
		} else if(val == "--load" || val == "-l") {
			if(arg_+1 != argc_) {
				++arg_;
				game::load_game_exception::game = argv_[arg_];
			}
		} else if(val == "--with-replay") {
			game::load_game_exception::show_replay = true;

		} else if(val == "--nogui") {
			no_gui_ = true;
			no_sound = true;
			preferences::disable_preferences_save();
		}
#ifndef DISABLE_EDITOR
		else if(val == "--screenshot") {
			if(arg_+2 != argc_) {
				++arg_;
				screenshot_map_ = argv_[arg_];
				++arg_;
				screenshot_filename_ = argv_[arg_];
				no_sound = true;
				screenshot_mode_ = true;
				preferences::disable_preferences_save();
				force_bpp_ = 32;
			}
		}
#endif
		else if(val == "--smallgui") {
			game_config::small_gui = true;
		} else if(val == "--config-dir" || val == "--data-dir") {
			if (argc_ <= ++arg_)
				break;
		} else if(val == "--windowed" || val == "-w") {
			preferences::set_fullscreen(false);
		} else if(val == "--fullscreen" || val == "-f") {
			preferences::set_fullscreen(true);

		} else if(val.find("--campaign") == 0 || val.find("-c") == 0) {
			// campaign starting template:
			// -c[[<difficulty>] <id_campaign> [<id_scenario>]]
			// --campaign[[<difficulty>] <id_campaign> [<id_scenario>]]
			jump_to_campaign_.jump_ = true;

			// we don't know if the next argument is from --campaign
			// or for setting the data directory, so we assume is the latter
			if (arg_ + 2 < argc_ && argv_[arg_+1][0] != '-')
			{
				// we parse difficulty only here, since a campaign is supplied from command line
				if (isdigit(val[val.size()-1]))
					jump_to_campaign_.difficulty_ = lexical_cast<int>(val[val.size()-1]);

				++arg_;
				jump_to_campaign_.campaign_id_ = std::string(argv_[arg_]);
				std::cerr<<"selected campaign id: ["<<jump_to_campaign_.campaign_id_
					<<"] | difficulty: ["<<jump_to_campaign_.difficulty_<<"]\n";
			}

			if (arg_ + 2 < argc_ && argv_[arg_+1][0] != '-')
			{
				++arg_;
				jump_to_campaign_.scenario_id_ = std::string(argv_[arg_]);
				std::cerr<<"selected scenario id: ["<<jump_to_campaign_.scenario_id_<<"]\n";
			}
		} else if(val == "--server" || val == "-s"){
			jump_to_multiplayer_ = true;
			//Do we have any server specified ?
			if(argc_ > arg_+1){
				multiplayer_server_ = argv_[arg_+1];
				++arg_;
			//Pick the first server in config
			}else{
				if(game_config::server_list.size() > 0)
					multiplayer_server_ = preferences::network_host();
				else
					multiplayer_server_ = "";
			}

		} else if(val == "--multiplayer" || val == "-m") {
			multiplayer_mode_ = true;
			break; //parse the rest of the arguments when we set up the game
		} else if(val == "--test" || val == "-t") {
			test_mode_ = true;
			// If we have -t foo it's ambiguous whether it foo is the parameter
			// for Wesnoth or the start directory so we assume it's the starting
			// directory.
			if(arg_ + 2 < argc_ && argv_[arg_ + 1][0] != '-') {
				++arg_;
				test_scenario_ = argv_[arg_];
			}
		} else if(val == "--debug" || val == "-d") {
			game_config::debug = true;
			game_config::mp_debug = true;
			//set the wml log domain to level "debug"
			std::string s = "wml";
			int severity = 3;
			lg::set_log_domain_severity(s, severity);
#ifdef DEBUG_WINDOW_LAYOUT_GRAPHS
		 } else if (val.substr(0, 18) == "--debug-dot-level=") {
			 gui2::tdebug_layout_graph::set_level(val.substr(18));
		 } else if (val.substr(0, 19) == "--debug-dot-domain=") {
			 gui2::tdebug_layout_graph::set_domain(val.substr(19));
#endif
		} else if(val == "--no-delay") {
			game_config::no_delay = true;
		} else if (val.substr(0, 6) == "--log-") {
		} else if (val == "--rng-seed") {
			++arg_;
		} else if(val == "--nosound") {
			no_sound = true;
		} else if(val == "--nomusic") {
			no_music = true;
		} else if(val == "--new-storyscreens") {
			// This is a hidden option to help testing
			// the work-in-progress new storyscreen code.
			// Don't document.
			set_new_storyscreen(true);
        }  //These commented lines should be used to implement support of connection
             //through a proxy via command line options.
             //The ANA network module should implement these methods (while the SDL_net won't.)
            else if(val == "--proxy") {
            network::enable_connection_through_proxy();
        } else if(val == "--proxy-address") {
            if ( argv_[ arg_ + 1][0] != '-')
            {
                network::enable_connection_through_proxy();
                network::set_proxy_address( argv_[ ++arg_ ] );
            }
            else
                throw std::runtime_error("Proxy address option requires address");

        } else if(val == "--proxy-port") {
            if ( argv_[ arg_ + 1][0] != '-')
            {
                network::enable_connection_through_proxy();
                network::set_proxy_port( argv_[ ++arg_ ] );
            }
            else
                throw std::runtime_error("Proxy port option requires port");
        } else if(val == "--proxy-user") {
            if ( argv_[ arg_ + 1][0] != '-')
            {
                network::enable_connection_through_proxy();
                network::set_proxy_user( argv_[ ++arg_ ] );
            }
            else
                throw std::runtime_error("Proxy user option requires user name");
        } else if(val == "--proxy-password") {
            if ( argv_[ arg_ + 1][0] != '-')
            {
                network::enable_connection_through_proxy();
                network::set_proxy_password( argv_[ ++arg_ ] );
            }
            else
                throw std::runtime_error("Proxy password option requires password");
        } else if(val == "--new-widgets") {
			// This is a hidden option to enable the new widget toolkit.
			gui2::new_widgets = true;
        } else if(val == "--clock") {
			gui2::show_debug_clock_button = true;
#ifndef DISABLE_EDITOR
		} else if(val == "-e" || val == "--editor") {
			jump_to_editor_ = true;
			if(arg_+1 != argc_) {
				if (argv_[arg_ + 1][0] != '-') {
					++arg_;
					game::load_game_exception::game = argv_[arg_];
				}
			}
#endif
		} else if(val[0] == '-') {
			std::cerr << "unknown option: " << val << std::endl;
			throw config::error("unknown option");
		} else {
			std::cerr << "Overriding data directory with " << val << std::endl;
#ifdef _WIN32
			// use c_str to ensure that index 1 points to valid element since c_str() returns null-terminated string
			if(val.c_str()[1] == ':') {
#else
			if(val[0] == '/') {
#endif
				game_config::path = val;
			} else {
				game_config::path = get_cwd() + '/' + val;
			}

			if(!is_directory(game_config::path)) {
				std::cerr << "Could not find directory '" << game_config::path << "'\n";
				throw config::error("directory not found");
			}

			font_manager_.update_font_path();
		}
	}
	std::cerr << '\n';
	std::cerr << "Data directory: " << game_config::path
		<< "\nUser configuration directory: " << get_user_config_dir()
		<< "\nUser data directory: " << get_user_data_dir()
		<< "\nCache directory: " << get_cache_dir()
		<< '\n';

	// disable sound in nosound mode, or when sound engine failed to initialize
	if (no_sound || ((preferences::sound_on() || preferences::music_on() ||
	                  preferences::turn_bell() || preferences::UI_sound_on()) &&
	                 !sound::init_sound())) {
		preferences::set_sound(false);
		preferences::set_music(false);
		preferences::set_turn_bell(false);
		preferences::set_UI_sound(false);
	}
	else if (no_music) { // else disable the music in nomusic mode
		preferences::set_music(false);
	}
}

game_display& game_controller::disp()
{
	if(disp_.get() == NULL) {
		if(get_video_surface() == NULL) {
			throw CVideo::error();
		}
		disp_.assign(game_display::create_dummy_display(video_));
	}
	return *disp_.get();
}

bool game_controller::init_video()
{
	if(no_gui_) {
		if( !(multiplayer_mode_ || screenshot_mode_) ) {
			std::cerr << "--nogui flag is only valid with --multiplayer flag or --screenshot flag\n";
			return false;
		}
		video_.make_fake();
		game_config::no_delay = true;
		return true;
	}

#if !(defined(__APPLE__))
	surface icon(image::get_image("game-icon.png", image::UNSCALED));
	if(icon != NULL) {
		///must be called after SDL_Init() and before setting video mode
		::SDL_WM_SetIcon(icon,NULL);
	}
#endif

	std::pair<int,int> resolution;
	int bpp = 0;
	int video_flags = 0;

	bool found_matching = preferences::detect_video_settings(video_, resolution, bpp, video_flags);

	if(force_bpp_ > 0) {
		bpp = force_bpp_;
	}

	if(!found_matching) {
		std::cerr << "Video mode " << resolution.first << 'x'
			<< resolution.second << 'x' << bpp
			<< " is not supported.\n";

		if ((video_flags & FULL_SCREEN)) {
			std::cerr << "Try running the program with the --windowed option "
				<< "using a " << bpp << "bpp setting for your display adapter.\n";
		} else {
			std::cerr << "Try running the program with the --fullscreen option.\n";
		}

		return false;
	}

	std::cerr << "setting mode to " << resolution.first << "x" << resolution.second << "x" << bpp << "\n";
	const int res = video_.setMode(resolution.first,resolution.second,bpp,video_flags);
	video_.setBpp(bpp);
	if(res == 0) {
		std::cerr << "required video mode, " << resolution.first << "x"
		          << resolution.second << "x" << bpp << " is not supported\n";
		return false;
	}

	return true;
}

bool game_controller::init_config(const bool force)
{
	cache_.clear_defines();

	// make sure that multiplayer mode is set if command line parameter is selected
	if (multiplayer_mode_)
		cache_.add_define("MULTIPLAYER");

	if (test_mode_)
		cache_.add_define("TEST");

#ifndef DISABLE_EDITOR
	if (jump_to_editor_)
		cache_.add_define("EDITOR");

	if (!multiplayer_mode_ && !test_mode_ && !jump_to_editor_)
		cache_.add_define("TITLE_SCREEN");
#else
	if (!multiplayer_mode_ && !test_mode_)
		cache_.add_define("TITLE_SCREEN");
#endif

	load_game_cfg(force);

	game_config::load_config(game_config_.child("game_config"));

	hotkey::deactivate_all_scopes();
	hotkey::set_scope_active(hotkey::SCOPE_GENERAL);
	hotkey::set_scope_active(hotkey::SCOPE_GAME);

	hotkey::load_hotkeys(game_config());
	paths_manager_.set_paths(game_config());
	::init_textdomains(game_config());
	about::set_about(game_config());
	ai::configuration::init(game_config());

	return true;
}

bool game_controller::init_language()
{
	if(!::load_language_list())
		return false;

	::set_language(get_locale());

	if(!no_gui_) {
		std::string wm_title_string = _("The Battle for Wesnoth");
		wm_title_string += " - " + game_config::revision;
		SDL_WM_SetCaption(wm_title_string.c_str(), NULL);
	}

	hotkey::load_descriptions();

	return true;
}

bool game_controller::play_test()
{
	static bool first_time = true;

	if(test_mode_ == false) {
		return true;
	}
	if(!first_time)
		return false;

	first_time = false;

	state_.classification().campaign_type = "test";
	state_.classification().scenario = test_scenario_;
	state_.classification().campaign_define = "TEST";
	cache_.add_define("TEST");

	load_game_cfg();

	paths_manager_.set_paths(game_config());

	try {
		play_game(disp(),state_,game_config());
	} catch (game::load_game_exception &) {
		test_mode_ = false;
		return true;
	}

	return false;
}

bool game_controller::play_screenshot_mode()
{
	if(!screenshot_mode_) {
		return true;
	}

#ifndef DISABLE_EDITOR
	cache_.clear_defines();
	cache_.add_define("EDITOR");
	load_game_cfg();
	const binary_paths_manager bin_paths_manager(game_config());
	::init_textdomains(game_config());

	editor::start(game_config(), video_, screenshot_map_, true, screenshot_filename_);
	return false;
#else
	return false;
#endif
}

bool game_controller::play_multiplayer_mode()
{
	state_ = game_state();

	if(!multiplayer_mode_) {
		return true;
	}

	std::string era = "era_default";
	std::string scenario = "multiplayer_The_Freelands";
	std::map<int,std::string> side_types, side_controllers, side_algorithms, side_ai_configs;
	std::map<int,utils::string_map> side_parameters;
	std::string turns = "50";
	std::string label = "";

	size_t sides_counted = 0;

	for(++arg_; arg_ < argc_; ++arg_) {
		const std::string val(argv_[arg_]);
		if(val.empty()) {
			continue;
		}

		std::vector<std::string> name_value = utils::split(val, '=');
		if(name_value.size() > 2) {
			std::cerr << "invalid argument '" << val << "'\n";
			return false;
		} else if(name_value.size() == 2) {
			const std::string name = name_value.front();
			const std::string value = name_value.back();

			const std::string name_head = name.substr(0,name.size()-1);
			const char name_tail = name[name.size()-1];
			const bool last_digit = isdigit(name_tail) ? true:false;
			const size_t side = name_tail - '0';

			if(last_digit && side > sides_counted) {
				std::cerr << "counted sides: " << side << "\n";
				sides_counted = side;
			}

			if(name == "--scenario") {
				scenario = value;
			} else if(name == "--turns") {
				turns = value;
			} else if(name == "--era") {
				era = value;
			} else if(name == "--label") {
				label = value;
			} else if(last_digit && name_head == "--controller") {
				side_controllers[side] = value;
			} else if(last_digit && name_head == "--ai_config") {
				side_ai_configs[side] = value;
			} else if(last_digit && name_head == "--algorithm") {
				side_algorithms[side] = value;
			} else if(last_digit && name_head == "--side") {
				side_types[side] = value;
			} else if(last_digit && name_head == "--parm") {
				const std::vector<std::string> name_value = utils::split(value, ':');
				if(name_value.size() != 2) {
					std::cerr << "argument to '" << name << "' must be in the format name:value\n";
					return false;
				}

				side_parameters[side][name_value.front()] = name_value.back();
			} else {
				std::cerr << "unrecognized option: '" << name << "'\n";
				return false;
			}
		} else {
			if (val == "--exit-at-end") {
				game_config::exit_at_end = true;
			}
		}
	}

	const config &lvl = game_config().find_child("multiplayer", "id", scenario);
	if (!lvl) {
		std::cerr << "Could not find scenario '" << scenario << "'\n";
		return false;
	}

	state_.classification().campaign_type = "multiplayer";
	state_.classification().scenario = "";
	state_.snapshot = config();

	config level = lvl;
	std::vector<config*> story;

	const config &era_cfg = game_config().find_child("era","id",era);
	if (!era_cfg) {
		std::cerr << "Could not find era '" << era << "'\n";
		return false;
	}

	level["turns"] = turns;

	const config &side = era_cfg.child("multiplayer_side");
	if (!side) {
		std::cerr << "Could not find multiplayer side\n";
		return false;
	}

	while (level.child_count("side") < sides_counted) {
		std::cerr << "now adding side...\n";
		level.add_child("side");
	}

	int side_num = 1;
	foreach (config &s, level.child_range("side"))
	{
		std::map<int,std::string>::const_iterator type = side_types.find(side_num),
		                                          controller = side_controllers.find(side_num),
		                                          algorithm = side_algorithms.find(side_num),
							  ai_config = side_ai_configs.find(side_num);

		const config* side = type == side_types.end() ?
			&era_cfg.find_child("multiplayer_side", "random_faction", "yes") :
			&era_cfg.find_child("multiplayer_side", "id", type->second);

		if (!*side) {
			std::string side_name = (type == side_types.end() ? "default" : type->second);
			std::cerr << "Could not find side '" << side_name << "' for side " << side_num << "\n";
			return false;
		}

		if ((*side)["random_faction"].to_bool())
		{
			std::vector<std::string> faction_choices, faction_excepts;
			faction_choices = utils::split((*side)["choices"]);
			if(faction_choices.size() == 1 && faction_choices.front() == "") {
				faction_choices.clear();
			}
			faction_excepts = utils::split((*side)["except"]);;
			if(faction_excepts.size() == 1 && faction_excepts.front() == "") {
				faction_excepts.clear();
			}
			unsigned j = 0;
			foreach (const config &faction, era_cfg.child_range("multiplayer_side"))
			{
				if (faction["random_faction"].to_bool()) continue;
				const std::string &faction_id = faction["id"];
				if (!faction_choices.empty() &&
				    std::find(faction_choices.begin(), faction_choices.end(), faction_id) == faction_choices.end())
					continue;
				if (!faction_excepts.empty() &&
				    std::find(faction_excepts.begin(), faction_excepts.end(), faction_id) != faction_excepts.end())
					continue;
				if (rand() % ++j == 0)
					side = &faction;
			}
			if ((*side)["random_faction"].to_bool()) {
				std::string side_name = (type == side_types.end() ? "default" : type->second);
				std::cerr << "Could not find any non-random faction for side " << side_num << "\n";
				return false;
			}
			std::cerr << " Faction " << (*side)["name"] <<
				" selected for side " << side_num << ".\n";
		}

		s["side"] = side_num;
		s["canrecruit"] = true;

		s.append(*side);

		if(controller != side_controllers.end()) {
			s["controller"] = controller->second;
		}

		if(algorithm != side_algorithms.end()) {
			s["ai_algorithm"] = algorithm->second;
		}

		if(ai_config != side_ai_configs.end()) {
			s["ai_config"] = ai_config->second;
		}

		config& ai_params = s.add_child("ai");

		//now add in any arbitrary parameters given to the side
		for(utils::string_map::const_iterator j = side_parameters[side_num].begin(); j != side_parameters[side_num].end(); ++j) {
			s[j->first] = j->second;
			ai_params[j->first] = j->second;
		}
		++side_num;
	}
	level.add_child("era", era_cfg);

	try {
		//check if all sides are AI and we are using new uploader -> log these games
		bool all_ai = true;
		foreach (config &s, level.child_range("side")) {
			if( s["controller"] != "ai" ) {
				all_ai = false;
			}
		}

		recorder.add_log_data("ai_log","ai_label",label);

		state_.snapshot = level;
		play_game(disp(), state_, game_config());
	} catch (game::load_game_exception &) {
		//the user's trying to load a game, so go into the normal title screen loop and load one
		return true;
	} catch(twml_exception& e) {
		e.show(disp());
		return false;
	} catch(std::exception& e) {
		std::cerr << "caught exception: " << e.what() << "\n";
	} catch(...) {
		std::cerr << "caught unknown error playing level...\n";
	}

	return false;
}

bool game_controller::is_loading() const
{
	return !game::load_game_exception::game.empty();
}

bool game_controller::load_game()
{
	savegame::loadgame load(disp(), game_config(), state_);

	try {
		load.load_game(game::load_game_exception::game, game::load_game_exception::show_replay, game::load_game_exception::cancel_orders);

		cache_.clear_defines();
		game_config::scoped_preproc_define dificulty_def(state_.classification().difficulty);

		game_config::scoped_preproc_define campaign_define_def(state_.classification().campaign_define, !state_.classification().campaign_define.empty());

		game_config::scoped_preproc_define campaign_type_def("MULTIPLAYER", state_.classification().campaign_define.empty() && (state_.classification().campaign_type == "multiplayer"));

		typedef boost::shared_ptr<game_config::scoped_preproc_define> define_ptr;
		std::deque<define_ptr> extra_defines;
		for(std::vector<std::string>::const_iterator i = state_.classification().campaign_xtra_defines.begin(); i != state_.classification().campaign_xtra_defines.end(); ++i) {
			define_ptr newdefine(new game_config::scoped_preproc_define(*i));
			extra_defines.push_back(newdefine);
		}

		try {
			load_game_cfg();
		} catch(config::error&) {
			cache_.clear_defines();
			load_game_cfg();
			return false;
		}

		paths_manager_.set_paths(game_config());
		load.set_gamestate();

	} catch(load_game_cancelled_exception&) {
		clear_loaded_game();
		return false;
	} catch(config::error& e) {
		if(e.message.empty()) {
			gui2::show_error_message(disp().video(), _("The file you have tried to load is corrupt"));
		}
		else {
			gui2::show_error_message(disp().video(), _("The file you have tried to load is corrupt: '") + e.message + '\'');
		}
		return false;
	} catch(twml_exception& e) {
		e.show(disp());
		return false;
	} catch(io_exception& e) {
		if(e.message.empty()) {
			gui2::show_error_message(disp().video(), _("File I/O Error while reading the game"));
		} else {
			gui2::show_error_message(disp().video(), _("File I/O Error while reading the game: '") + e.message + '\'');
		}
		return false;
	} catch(game::error& e) {
		if(e.message.empty()) {
			gui2::show_error_message(disp().video(), _("The file you have tried to load is corrupt"));
		}
		else {
			gui2::show_error_message(disp().video(), _("The file you have tried to load is corrupt: '") + e.message + '\'');
		}
		return false;
	}
	recorder = replay(state_.replay_data);
	recorder.start_replay();
	recorder.set_skip(false);

	LOG_CONFIG << "has snapshot: " << (state_.snapshot.child("side") ? "yes" : "no") << "\n";

	if (!state_.snapshot.child("side")) {
		// No snapshot; this is a start-of-scenario
		if (load.show_replay()) {
			// There won't be any turns to replay, but the
			// user gets to watch the intro sequence again ...
			LOG_CONFIG << "replaying (start of scenario)\n";
		} else {
			LOG_CONFIG << "skipping...\n";
			recorder.set_skip(false);
		}
	} else {
		// We have a snapshot. But does the user want to see a replay?
		if(load.show_replay()) {
			statistics::clear_current_scenario();
			LOG_CONFIG << "replaying (snapshot)\n";
		} else {
			LOG_CONFIG << "setting replay to end...\n";
			recorder.set_to_end();
			if(!recorder.at_end()) {
				WRN_CONFIG << "recorder is not at the end!!!\n";
			}
		}
	}

	if(state_.classification().campaign_type == "multiplayer") {
		foreach (config &side, state_.snapshot.child_range("side"))
		{
			if (side["controller"] == "network")
				side["controller"] = "human";
			if (side["controller"] == "network_ai")
				side["controller"] = "human_ai";
		}
	}

	if (load.cancel_orders()) {
		foreach (config &side, state_.snapshot.child_range("side"))
		{
			if (side["controller"] != "human") continue;
			foreach (config &unit, side.child_range("unit"))
			{
				unit["goto_x"] = -999;
				unit["goto_y"] = -999;
			}
		}
	}

	return true;
}

void game_controller::set_tutorial()
{
	state_ = game_state();
	state_.classification().campaign_type = "tutorial";
	state_.classification().scenario = "tutorial";
	state_.classification().campaign_define = "TUTORIAL";
	cache_.clear_defines();
	cache_.add_define("TUTORIAL");

}

void game_controller::mark_completed_campaigns(std::vector<config> &campaigns)
{
	foreach (config &campaign, campaigns) {
		campaign["completed"] = preferences::is_campaign_completed(campaign["id"]);
	}
}

bool game_controller::new_campaign()
{
	state_ = game_state();
	state_.classification().campaign_type = "scenario";

	const config::const_child_itors &ci = game_config().child_range("campaign");
	std::vector<config> campaigns(ci.first, ci.second);
	mark_completed_campaigns(campaigns);
	std::sort(campaigns.begin(),campaigns.end(),less_campaigns_rank);


	if(campaigns.begin() == campaigns.end()) {
	  gui2::show_error_message(disp().video(),
				  _("No campaigns are available.\n"));
		return false;
	}

	int campaign_num = -1;
	// No campaign selected from command line
	if (jump_to_campaign_.campaign_id_.empty() == true)
	{
		gui2::tcampaign_selection dlg(campaigns);

		try {
			dlg.show(disp().video());
		} catch(twml_exception& e) {
			e.show(disp());
			return false;
		}

		if(dlg.get_retval() != gui2::twindow::OK) {
			return false;
		}

		campaign_num = dlg.get_choice();
	}
	else
	{
		// don't reset the campaign_id_ so we can know
		// if we should quit the game or return to the main menu

		// checking for valid campaign name
		for(size_t i = 0; i < campaigns.size(); ++i)
		{
			if (campaigns[i]["id"] == jump_to_campaign_.campaign_id_)
			{
				campaign_num = i;
				break;
			}
		}

		// didn't found any campaign with that id
		if (campaign_num == -1)
		{
			std::cerr<<"No such campaign id to jump to: ["<<jump_to_campaign_.campaign_id_<<"]\n";
			return false;
		}
	}

	const config &campaign = campaigns[campaign_num];
	state_.classification().campaign = campaign["id"].str();
	state_.classification().abbrev = campaign["abbrev"].str();

	// we didn't specify in the command line the scenario to be started
	if (jump_to_campaign_.scenario_id_.empty())
		state_.classification().scenario = campaign["first_scenario"].str();
	else
	{
		bool found = false;
		foreach(const config &sc, campaign.child_range("scenario"))
		{
			if (sc["id"] == jump_to_campaign_.scenario_id_)
				found = true;
		}

		if (!found)
		{
			std::cerr<<"No such scenario id to jump to: ["<<jump_to_campaign_.scenario_id_<<"]\n";
			return false;
		}
		state_.classification().scenario = jump_to_campaign_.scenario_id_;
	}
	state_.classification().end_text = campaign["end_text"].str();
	state_.classification().end_text_duration = campaign["end_text_duration"];

	const std::string difficulty_descriptions = campaign["difficulty_descriptions"];
	std::vector<std::string> difficulty_options = utils::split(difficulty_descriptions, ';');

	const std::vector<std::string> difficulties = utils::split(campaign["difficulties"]);

	if(difficulties.empty() == false) {
		int difficulty = 0;
		if (jump_to_campaign_.difficulty_ == -1){
			if(difficulty_options.size() != difficulties.size()) {
				difficulty_options.resize(difficulties.size());
				std::copy(difficulties.begin(),difficulties.end(),difficulty_options.begin());
			}

			gui2::tcampaign_difficulty dlg(difficulty_options);
			dlg.show(disp().video());

			if(dlg.selected_index() == -1) {
				if (jump_to_campaign_.campaign_id_.empty() == false)
				{
					jump_to_campaign_.campaign_id_ = "";
				}
				// canceled difficulty dialog, relaunch the campaign selection dialog
				return new_campaign();
			}
			difficulty = dlg.selected_index();
		}
		else
		{
			if (jump_to_campaign_.difficulty_
					> static_cast<int>(difficulties.size()))
			{
				std::cerr << "incorrect difficulty number: [" <<
					jump_to_campaign_.difficulty_ << "]. maximum is [" <<
					difficulties.size() << "].\n";
				return false;
			}
			else
			{
				difficulty = jump_to_campaign_.difficulty_ - 1;
			}
		}

		state_.classification().difficulty = difficulties[difficulty];
		cache_.clear_defines();
		cache_.add_define(difficulties[difficulty]);
	} else {
		//clear even when there is no difficulty
		cache_.clear_defines();
	}

	state_.classification().campaign_define = campaign["define"].str();
	state_.classification().campaign_xtra_defines = utils::split(campaign["extra_defines"]);

	return true;
}

}

std::string game_controller::jump_to_campaign_id() const
{
	return jump_to_campaign_.campaign_id_;
}

bool game_controller::goto_campaign()
{
	if(jump_to_campaign_.jump_){
		if(new_campaign()) {
			jump_to_campaign_.jump_ = false;
			launch_game(game_controller::RELOAD_DATA);
		}else{
			jump_to_campaign_.jump_ = false;
			return false;
		}
	}
	return true;
}

bool game_controller::goto_multiplayer()
{
	if(jump_to_multiplayer_){
		jump_to_multiplayer_ = false;
		if(play_multiplayer()){
			;
		}else{
			return false;
		}
	}
	return true;
}

#ifndef DISABLE_EDITOR
bool game_controller::goto_editor()
{
	if(jump_to_editor_){
		jump_to_editor_ = false;
		if (start_editor(normalize_path(game::load_game_exception::game)) ==
		    editor::EXIT_QUIT_TO_DESKTOP)
		{
			return false;
		}
		clear_loaded_game();
	}
	return true;
}
#endif

namespace
{
	void game_controller::reload_changed_game_config()
	{
		// rebuild addon version info cache
		refresh_addon_version_info_cache();

		//force a reload of configuration information
		cache_.recheck_filetree_checksum();
		old_defines_map_.clear();
		clear_binary_paths_cache();
		init_config(true);
	}

void game_controller::start_wesnothd()
{
	const std::string wesnothd_program =
		preferences::get_mp_server_program_name().empty() ?
		get_program_invocation("wesnothd") : preferences::get_mp_server_program_name();

	std::string config = get_user_config_dir() + "/lan_server.cfg";
	if (!file_exists(config)) {
		// copy file if it isn't created yet
		write_file(config, read_file(get_wml_location("lan_server.cfg")));
	}

#ifndef _WIN32
	std::string command = "\"" + wesnothd_program +"\" -c \"" + config + "\" -d -t 2 -T 5";
#else
	// start wesnoth as background job
	std::string command = "cmd /C start \"wesnoth server\" /B \"" + wesnothd_program + "\" -c \"" + config + "\" -t 2 -T 5";
#endif
	LOG_GENERAL << "Starting wesnothd: "<< command << "\n";
	if (std::system(command.c_str()) == 0) {
		// Give server a moment to start up
		SDL_Delay(50);
		return;
	}
	preferences::set_mp_server_program_name("");

	// Couldn't start server so throw error
	WRN_GENERAL << "Failed to run server start script\n";
	throw game::mp_server_error("Starting MP server failed!");
}

bool game_controller::play_multiplayer()
{
	int res;

	state_ = game_state();
	state_.classification().campaign_type = "multiplayer";
	state_.classification().campaign_define = "MULTIPLAYER";

	//Print Gui only if the user hasn't specified any server
	if( multiplayer_server_.empty() ){

		int start_server;
		do {
			start_server = 0;

			gui2::tmp_method_selection dlg;

			dlg.show(disp().video());

			if(dlg.get_retval() == gui2::twindow::OK) {
				res = dlg.get_choice();
			} else {
				return false;

			}

			if (res == 2 && preferences::mp_server_warning_disabled() < 2)
			{
				gui::dialog d(disp(), _("Do you really want to start the server?"),
					_("The server will run in a background process until all users have disconnected.")
					, gui::OK_CANCEL);
				bool checked = preferences::mp_server_warning_disabled() != 1;

				d.add_option(_("Don’t show again"), checked, gui::dialog::BUTTON_CHECKBOX_LEFT);
				start_server = d.show();
				if (start_server == 0)
					preferences::set_mp_server_warning_disabled(d.option_checked()?2:1);

			}
		} while (start_server);
		if (res < 0) {
			return false;
		}

	}else{
		res = 4;
	}

	try {
		if (res == 2)
		{
			try {
				start_wesnothd();
			} catch(game::mp_server_error&)
			{
				std::string path = preferences::show_wesnothd_server_search(disp());

				if (!path.empty())
				{
					preferences::set_mp_server_program_name(path);
					start_wesnothd();
				}
				else
				{
					throw game::mp_server_error("No path given for mp server program.");
				}
			}


		}

		/* do */ {
			cache_.clear_defines();
			game_config::scoped_preproc_define multiplayer(state_.classification().campaign_define);
			load_game_cfg();
			events::discard(INPUT_MASK); // prevent the "keylogger" effect
			cursor::set(cursor::NORMAL);
			// update binary paths
			paths_manager_.set_paths(game_config());
			clear_binary_paths_cache();
		}

		if(res == 3) {
			std::vector<std::string> chat;
			config game_data;

			const mp::controller cntr = mp::CNTR_LOCAL;

			mp::start_local_game(disp(), game_config(), cntr);

		} else if((res >= 0 && res <= 2) || res == 4) {
			std::string host;
			if(res == 0) {
				host = preferences::server_list().front().address;
			}else if(res == 2) {
				host = "localhost";
			}else if(res == 4){
				host = multiplayer_server_;
				multiplayer_server_ = "";
			}
			mp::start_client(disp(), game_config(), host);
		}

	} catch(game::mp_server_error& e) {
		gui2::show_error_message(disp().video(), _("Error while starting server: ") + e.message);
	} catch(game::load_game_failed& e) {
		gui2::show_error_message(disp().video(), _("The game could not be loaded: ") + e.message);
	} catch(game::game_error& e) {
		gui2::show_error_message(disp().video(), _("Error while playing the game: ") + e.message);
	} catch(network::error& e) {
		if(e.message != "") {
			ERR_NET << "caught network::error: " << e.message << "\n";
			gui2::show_transient_message(disp().video()
					, ""
					, gettext(e.message.c_str()));
		} else {
			ERR_NET << "caught network::error\n";
		}
	} catch(config::error& e) {
		if(e.message != "") {
			ERR_CONFIG << "caught config::error: " << e.message << "\n";
			gui2::show_transient_message(disp().video(), "", e.message);
		} else {
			ERR_CONFIG << "caught config::error\n";
		}
	} catch(incorrect_map_format_error& e) {
		gui2::show_error_message(disp().video(), std::string(_("The game map could not be loaded: ")) + e.message);
	} catch (game::load_game_exception &) {
		//this will make it so next time through the title screen loop, this game is loaded
	} catch(twml_exception& e) {
		e.show(disp());
	}

	return false;
}

bool game_controller::change_language()
{
	gui2::tlanguage_selection dlg;
	dlg.show(disp().video());
	if (dlg.get_retval() != gui2::twindow::OK) return false;

	if (!no_gui_) {
		std::string wm_title_string = _("The Battle for Wesnoth");
		wm_title_string += " - " + game_config::revision;
		SDL_WM_SetCaption(wm_title_string.c_str(), NULL);
	}

	return true;
}

void game_controller::show_preferences()
{
	const preferences::display_manager disp_manager(&disp());
	preferences::show_preferences_dialog(disp(),game_config());

	disp().redraw_everything();
}

void game_controller::set_unit_data()
{
	loadscreen::start_stage("load unit types");
	if (config &units = game_config_.child("units")) {
		unit_types.set_config(units);
	}
}

void game_controller::load_game_cfg(const bool force)
{
	// make sure that 'debug mode' symbol is set if command line parameter is selected
	// also if we're in multiplayer and actual debug mode is disabled
	if (game_config::debug || game_config::mp_debug) {
		cache_.add_define("DEBUG_MODE");
	}

	if (!game_config_.empty() && !force
			&& old_defines_map_ == cache_.get_preproc_map())
		return; // game_config already holds requested config in memory
	old_defines_map_ = cache_.get_preproc_map();
	loadscreen::global_loadscreen_manager loadscreen_manager(disp().video());
	cursor::setter cur(cursor::WAIT);
	// The loadscreen will erase the titlescreen
	// NOTE: even without loadscreen, needed after MP lobby
	try {
		/**
		 * Read all game configs
		 * First we should load data/
		 * Then handle terrains so that they are last loaded from data/
		 * 2nd everything in userdata
		 **/
		loadscreen::start_stage("verify cache");
		data_tree_checksum();
		loadscreen::start_stage("create cache");

		// start transaction so macros are shared
		game_config::config_cache_transaction main_transaction;

		cache_.get_config(game_config::path +"/data", game_config_);

		main_transaction.lock();

		/* Put the gfx rules aside so that we can prepend the add-on
		   rules to them. */
		config core_terrain_rules;
		core_terrain_rules.splice_children(game_config_, "terrain_graphics");

		// load usermade add-ons
		const std::string user_campaign_dir = get_addon_campaigns_dir();
		std::vector< std::string > error_addons;
		// Scan addon directories
		std::vector<std::string> user_dirs;
		// Scan for standalone files
		std::vector<std::string> user_files;

		// The addons that we'll actually load
		std::vector<std::string> addons_to_load;

		get_files_in_dir(user_campaign_dir,&user_files,&user_dirs,ENTIRE_FILE_PATH);
		std::string user_error_log;

		// Append the $user_campaign_dir/*.cfg files to addons_to_load.
		for(std::vector<std::string>::const_iterator uc = user_files.begin(); uc != user_files.end(); ++uc) {
			const std::string file = *uc;
			if(file.substr(file.size() - 4, file.size()) == ".cfg")
				addons_to_load.push_back(file);
		}

		// Append the $user_campaign_dir/*/_main.cfg files to addons_to_load.
		for(std::vector<std::string>::const_iterator uc = user_dirs.begin(); uc != user_dirs.end(); ++uc){
			const std::string main_cfg = *uc + "/_main.cfg";
			if (file_exists(main_cfg))
				addons_to_load.push_back(main_cfg);
		}

		// Load the addons
		for(std::vector<std::string>::const_iterator uc = addons_to_load.begin(); uc != addons_to_load.end(); ++uc) {
			const std::string toplevel = *uc;
			try {
				config umc_cfg;
				cache_.get_config(toplevel, umc_cfg);

				game_config_.append(umc_cfg);
			} catch(config::error& err) {
				ERR_CONFIG << "error reading usermade add-on '" << *uc << "'\n";
				error_addons.push_back(*uc);
				user_error_log += err.message + "\n";
			} catch(preproc_config::error& err) {
				ERR_CONFIG << "error reading usermade add-on '" << *uc << "'\n";
				error_addons.push_back(*uc);
				user_error_log += err.message + "\n";
			} catch(io_exception&) {
				ERR_CONFIG << "error reading usermade add-on '" << *uc << "'\n";
				error_addons.push_back(*uc);
			}
			if(error_addons.empty() == false) {
				std::stringstream msg;
				msg << _n("The following add-on had errors and could not be loaded:",
						"The following add-ons had errors and could not be loaded:",
						error_addons.size());
				for(std::vector<std::string>::const_iterator i = error_addons.begin(); i != error_addons.end(); ++i) {
					msg << "\n" << *i;
				}

				msg << '\n' << _("ERROR DETAILS:") << '\n' << user_error_log;

				gui2::show_error_message(disp().video(),msg.str());
			}
		}

		// Extract the Lua scripts at toplevel.
		extract_preload_scripts(game_config_);
		game_config_.clear_children("lua");

		game_config_.merge_children("units");
		game_config_.splice_children(core_terrain_rules, "terrain_graphics");

		config& hashes = game_config_.add_child("multiplayer_hashes");
		foreach (const config &ch, game_config_.child_range("multiplayer")) {
			hashes[ch["id"]] = ch.hash();
		}

		set_unit_data();

		terrain_builder::set_terrain_rules_cfg(game_config());

		::init_strings(game_config());

	} catch(game::error& e) {
		ERR_CONFIG << "Error loading game configuration files\n";
		gui2::show_error_message(disp().video(), _("Error loading game configuration files: '") +
			e.message + _("' (The game will now exit)"));
		throw;
	}
}


void game_controller::launch_game(RELOAD_GAME_DATA reload)
{
	loadscreen::global_loadscreen_manager loadscreen_manager(disp().video());
	loadscreen::start_stage("load data");
	if(reload == RELOAD_DATA) {
		game_config::scoped_preproc_define campaign_define(state_.classification().campaign_define, state_.classification().campaign_define.empty() == false);

		typedef boost::shared_ptr<game_config::scoped_preproc_define> define_ptr;
		std::deque<define_ptr> extra_defines;
		for(std::vector<std::string>::const_iterator i = state_.classification().campaign_xtra_defines.begin(); i != state_.classification().campaign_xtra_defines.end(); ++i) {
			define_ptr newdefine(new game_config::scoped_preproc_define(*i));
			extra_defines.push_back(newdefine);
		}
		try {
			load_game_cfg();
		} catch(config::error&) {
			cache_.clear_defines();
			load_game_cfg();
			return;
		}
	}

	const binary_paths_manager bin_paths_manager(game_config());

	try {
		const LEVEL_RESULT result = play_game(disp(),state_,game_config());
		// don't show The End for multiplayer scenario
		// change this if MP campaigns are implemented
		if(result == VICTORY && (state_.classification().campaign_type.empty() || state_.classification().campaign_type != "multiplayer")) {
			preferences::add_completed_campaign(state_.classification().campaign);
			the_end(disp(), state_.classification().end_text, state_.classification().end_text_duration);
			about::show_about(disp(),state_.classification().campaign);
		}

		clear_loaded_game();
	} catch (game::load_game_exception &) {
		//this will make it so next time through the title screen loop, this game is loaded
	} catch(twml_exception& e) {
		e.show(disp());
	}
}

} //end anon namespace

void game_controller::play_replay()
{
	const binary_paths_manager bin_paths_manager(game_config());

	try {
		::play_replay(disp(),state_,game_config(),video_);

		clear_loaded_game();
	} catch (game::load_game_exception &) {
		//this will make it so next time through the title screen loop, this game is loaded
	} catch(twml_exception& e) {
		e.show(disp());
	}
}

#ifndef DISABLE_EDITOR
editor::EXIT_STATUS game_controller::start_editor(const std::string& filename)
{
	while(true){
		cache_.clear_defines();
		cache_.add_define("EDITOR");
		load_game_cfg();
		const binary_paths_manager bin_paths_manager(game_config());
		::init_textdomains(game_config());

		editor::EXIT_STATUS res = editor::start(game_config(), video_, filename);

		if(res != editor::EXIT_RELOAD_DATA)
			return res;

		reload_changed_game_config();
		image::flush_cache();
	}
	return editor::EXIT_ERROR; // not supposed to happen
}
#endif

game_controller::~game_controller()
{
	delete gui::empty_menu;
	sound::close_sound();
}

// this is needed to allow identical functionality with clean refactoring
// play_game only returns on an error, all returns within play_game can
// be replaced with this
static void safe_exit(int res) {

	LOG_GENERAL << "exiting with code " << res << "\n";
#ifdef OS2 /* required to correctly shutdown SDL on OS/2 */
        SDL_Quit();
#endif
	exit(res);
}

// maybe this should go in a util file somewhere?
static void gzip_codec(const std::string & input_file, const std::string & output_file, bool encode)
{
	try {
	std::ofstream ofile(output_file.c_str(), std::ios_base::out
			| std::ios_base::binary | std::ios_base::binary);
			std::ifstream ifile(input_file.c_str(),
			std::ios_base::in | std::ios_base::binary);
		boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
		if(encode)
			in.push(boost::iostreams::gzip_compressor());
		else
			in.push(boost::iostreams::gzip_decompressor());
		in.push(ifile);
		boost::iostreams::copy(in, ofile);
			ifile.close();
		safe_exit(remove(input_file.c_str()));
		}  catch(io_exception& e) {
		std::cerr << "IO error: " << e.what() << "\n";
	}
}

static void gzip_encode(const std::string & input_file, const std::string & output_file)
{
	gzip_codec(input_file, output_file, true);
}

static void gzip_decode(const std::string & input_file, const std::string & output_file)
{
	gzip_codec(input_file, output_file, false);
}

struct preprocess_options
{
public:
	preprocess_options(): output_macros_path_("false"), input_macros_()
	{
	}
	std::string output_macros_path_;
	preproc_map input_macros_;
};

/** Process commandline-arguments */
static int process_command_args(int argc, char** argv) {
	const std::string program = argv[0];
	game_config::wesnoth_program_dir = directory_name(program);
	preprocess_options preproc;

	//parse arguments that shouldn't require a display device
	int arg;
	for(arg = 1; arg != argc; ++arg) {
		const std::string val(argv[arg]);
		if(val.empty()) {
			continue;
		}

		if(val == "--help" || val == "-h") {
			// When adding items don't forget to update doc/man/wesnoth.6
			// Options are sorted alphabetically by --long-option.
			// Please keep the output to 80 chars per line.
			std::cout << "usage: " << argv[0]
			<< " [<options>] [<data-directory>]\n"
			<< "Available options:\n"
			<< "  --bpp <number>               sets BitsPerPixel value. Example: --bpp 32\n"
			<< "  -c, --campaign[[<difficulty>] <id_c> [<id_s>]]\n"
			<< "                               goes directly to the campaign.\n"
			<< "                               - difficulty : the difficulty of the specified\n"
			<< "                                          campaign (1 to max - Default is 1)\n"
			<< "                               - id_c: the id of the campaign. A selection \n"
			<< "                                       menu will appear if none specified\n"
			<< "                               - id_s: the id of the scenario from the\n"
			<< "                                       specified campaign\n"
			<< "  --config-dir <name>          sets the path of the user config directory to\n"
			<< "                               $HOME/<name> or My Documents\\My Games\\<name> for windows.\n"
			<< "                               You can specify also an absolute path outside the\n"
			<< "                               $HOME or My Documents\\My Games directory.\n"
			<< "  --config-path                prints the path of the user config directory and\n"
			<< "                               exits.\n"
			<< "  --data-dir <directory>       overrides the data directory with the one specified.\n"
			<< "  -d, --debug                  enables additional command mode options in-game.\n"
#ifdef DEBUG_WINDOW_LAYOUT_GRAPHS
			<< "  --debug-dot-level=<level1>,<level2>,...\n"
			<< "                               sets the level of the debug dot files.\n"
			<< "                               These files are used for debugging the widgets\n"
			<< "                               especially the for the layout engine. When enabled\n"
			<< "                               the engine will produce dot files which can be\n"
			<< "                               converted to images with the dot tool.\n"
			<< "                               Available levels:\n"
			<< "                               - size  : generate the size info of the widget.\n"
			<< "                               - state : generate the state info of the widget.\n"
			<< "  --debug-dot-domain=<domain1>,<domain2>,...\n"
			<< "                               sets the domain of the debug dot files.\n"
			<< "                               see --debug-dot-level for more info.\n"
			<< "                               Available domains:\n"
			<< "                               show   : generate the data when the dialog is\n"
			<< "                                        about to be shown.\n"
			<< "                               layout : generate the data during the layout\n"
			<< "                                        phase (might result in multiple files. \n"
			<< "                               The data can also be generated when the F12 is\n"
			<< "                               pressed in a dialog.\n"
#endif
#ifndef DISABLE_EDITOR
			<< "  -e, --editor [<file>]        starts the in-game map editor directly. If <file>\n"
			<< "                               is specified, equivalent to -e --load <file>.\n"
#endif
			<< "  --fps                        displays the number of frames per second the\n"
			<< "                               game is currently running at, in a corner of\n"
			<< "                               the screen.\n"
			<< "  -f, --fullscreen             runs the game in full screen mode.\n"
			<< "  --gunzip <infile>.gz         decompresses a file (<infile>.gz) in gzip format\n"
			<< "                               and stores it without the .gz suffix.\n"
			<< "                               <infile>.gz will be removed.\n"
			<< "  --gzip <infile>              compresses a file (<infile>) in gzip format,\n"
			<< "                               stores it as <infile>.gz and removes <infile>.\n"
			<< "  -h, --help                   prints this message and exits.\n"
			<< "  -l, --load <file>            loads the save <file> from the standard save\n"
			<< "                               game directory.\n"
#ifndef DISABLE_EDITOR
			<< "                               When launching the map editor via -e, the map\n"
			<< "                               <file> is loaded, relative to the current\n"
			<< "                               directory. If it is a directory, the editor\n"
			<< "                               will start with a load map dialog opened there.\n"
#endif
			<< "  --log-<level>=<domain1>,<domain2>,...\n"
			<< "                               sets the severity level of the log domains.\n"
			<< "                               'all' can be used to match any log domain.\n"
			<< "                               Available levels: error, warning, info, debug.\n"
			<< "                               By default the 'error' level is used.\n"
			<< "  --logdomains [filter]        lists defined log domains (only the ones containing\n"
			<< "                               [filter] if used) and exits.\n"
			<< "  --max-fps                    the maximum fps the game tries to run at. Values\n"
			<< "                               should be between 1 and 1000, the default is 50.\n"
			<< "  -m, --multiplayer            starts a multiplayer game. There are additional\n"
			<< "                               options that can be used as explained below:\n"
			<< "    --ai_config<number>=value  selects a configuration file to load for this side.\n"
			<< "    --algorithm<number>=value  selects a non-standard algorithm to be used by\n"
			<< "                               the AI controller for this side.\n"
			<< "    --controller<number>=value selects the controller for this side.\n"
			<< "    --era=value                selects the era to be played in by its id.\n"
			<< "    --exit-at-end              exit Wesnoth at the end of the scenario.\n"
			<< "    --nogui                    runs the game without the GUI. Must appear before\n"
			<< "                               --multiplayer to have the desired effect.\n"
			<< "    --parm<number>=name:value  sets additional parameters for this side.\n"
			<< "    --scenario=value           selects a multiplayer scenario. The default\n"
			<< "                               scenario is \"multiplayer_The_Freelands\".\n"
			<< "    --side<number>=value       selects a faction of the current era for this\n"
			<< "                               side by id.\n"
			<< "    --turns=value              sets the number of turns. The default is \"50\".\n"
			<< "  --no-delay                   runs the game without any delays.\n"
			<< "  --nocache                    disables caching of game data.\n"
			<< "  --nomusic                    runs the game without music.\n"
			<< "  --nosound                    runs the game without sounds and music.\n"
			<< "  --path                       prints the path to the data directory and exits.\n"
			<< "  --preprocess, -p[=<define1>,<define2>,...] <file/folder> <target directory>\n"
			<< "                               preprocesses a specified file/folder. The preprocessed\n"
			<< "                               file(s) will be written in the specified target\n"
			<< "                               directory: a plain cfg file and a processed cfg file.\n"
			<< "                               define1,define2,...  - the extra defines will\n"
			<< "                               be added before processing the files. If you add\n"
			<< "                               them you must add the '=' character before.\n"
			<< "                               If 'SKIP_CORE' is in the define list the\n"
			<< "                               data/core won't be preprocessed.\n"
			<< " --preprocess-input-macros <source file>\n"
			<< "                               used only by the '--preprocess' command.\n"
			<< "                               Specifies a file that contains [preproc_define]s\n"
			<< "                               to be included before preprocessing.\n"
			<< " --preprocess-output-macros [<target file>]\n"
			<< "                               used only by the '--preprocess' command.\n"
			<< "                               Will output all preprocessed macros in the target file.\n"
			<< "                               If the file is not specified the output will be\n"
			<< "                               file '_MACROS_.cfg' in the target directory of\n"
			<< "                               preprocess's command. This switch should be typed\n"
			<< "                               before the --preprocess command.\n"
			<< "  -r, --resolution XxY         sets the screen resolution. Example: -r 800x600\n"
			<< "  --rng-seed <number>          seeds the random number generator with number\n"
			<< "                               Example: --rng-seed 0\n"
			<< "  --smallgui                   allows to use screen resolutions down to 800x480\n"
			<< "                               and resizes a few interface elements.\n"
			<< "  --screenshot <map> <output>  Saves a screenshot of <map> to <output> without\n"
			<< "                               initializing a screen. Editor must be compiled\n"
			<< "                               in for this to work.\n"
			<< "  -s, --server [<host>]        connects to the host if specified\n"
			<< "                               or to the first host in your preferences.\n"
			<< "  -t, --test                   runs the game in a small test scenario.\n"
			<< "  --validcache                 assumes that the cache is valid. (dangerous)\n"
			<< "  -v, --version                prints the game's version number and exits.\n"
			<< "  -w, --windowed               runs the game in windowed mode.\n"
			<< "  --with-replay                replays the file loaded with the --load option.\n"
			<< "  --new-widgets                there is a new WIP widget toolkit this switch\n"
			<< "                               enables the new toolkit (VERY EXPERIMENTAL don't\n"
			<< "                               file bug reports since most are known).\n"
			<< "                               Parts of the library are deemed stable and will\n"
			<< "                               work without this switch.\n"
			<< "  --clock                      Adds the option to show a clock for testing the\n"
			<< "                               drawing timer.\n"
			;
			return 0;
		} else if(val == "--version" || val == "-v") {
			std::cout << "Battle for Wesnoth" << " " << game_config::version
			          << "\n";
			return 0;
		} else if (val == "--config-path") {
			std::cout << get_user_data_dir() << '\n';
			return 0;
		} else if(val == "--path") {
			std::cout <<  game_config::path
			          << "\n";
			return 0;
		}
#ifndef DISABLE_EDITOR
		else if (val == "--screenshot" ) {
			if(!(argc > arg + 2)) {
				std::cerr << "format of " << val << " command: " << val << " <map file> <output file>\n";
				return 2;
			}
			static char opt[] = "SDL_VIDEODRIVER=dummy";
			SDL_putenv(opt);
		}
#endif
		else if(val == "--config-dir") {
			if (argc <= ++arg)
				break;
			set_preferences_dir(argv[arg]);
		} else if(val == "--data-dir") {
			if(arg +1 != argc) {
				++arg;
				const std::string datadir(argv[arg]);
				std::cerr << "Overriding data directory with " << datadir << std::endl;
#ifdef _WIN32
				// use c_str to ensure that index 1 points to valid element since c_str() returns null-terminated string
				if(datadir.c_str()[1] == ':') {
#else
				if(datadir[0] == '/') {
#endif
					game_config::path = datadir;
				} else {
					game_config::path = get_cwd() + '/' + datadir;
				}

				if(!is_directory(game_config::path)) {
					std::cerr << "Could not find directory '" << game_config::path << "'\n";
					throw config::error("directory not found");
				}

				// don't update font as we already updating it in game ctor
			}
			else
				std::cerr << "please specify a data directory\n";
		} else if (val.substr(0, 6) == "--log-") {
			size_t p = val.find('=');
			if (p == std::string::npos) {
				std::cerr << "unknown option: " << val << '\n';
				return 2;
			}
			std::string s = val.substr(6, p - 6);
			int severity;
			if (s == "error") severity = 0;
			else if (s == "warning") severity = 1;
			else if (s == "info") severity = 2;
			else if (s == "debug") severity = 3;
			else {
				std::cerr << "unknown debug level: " << s << '\n';
				return 2;
			}
			while (p != std::string::npos) {
				size_t q = val.find(',', p + 1);
				s = val.substr(p + 1, q == std::string::npos ? q : q - (p + 1));
				if (!lg::set_log_domain_severity(s, severity)) {
					std::cerr << "unknown debug domain: " << s << '\n';
					return 2;
				}
				p = q;
			}
		} else if(val == "--gzip") {
			if(argc != arg + 2) {
				std::cerr << "format of " << val << " command: " << val << " <input file>\n";
				return 2;
			}

			const std::string input_file(argv[arg + 1]);
			const std::string output_file(input_file + ".gz");
			gzip_encode(input_file, output_file);

		} else if(val == "--gunzip") {
			if(argc != arg + 2) {
				std::cerr << "format of " << val << " command: " << val << " <input file>\n";
				return 2;
			}

			const std::string input_file(argv[arg + 1]);
			if(! is_gzip_file(input_file)) {

				std::cerr << "file '" << input_file << "'isn't a .gz file\n";
				return 2;
			}
			const std::string output_file(
				input_file, 0, input_file.length() - 3);

			gzip_decode(input_file, output_file);

		} else if(val == "--logdomains") {
			std::string filter;
			if(arg + 1 != argc) {
				++arg;
				filter = argv[arg];
			}
			std::cout << lg::list_logdomains(filter);
			return 0;
		} else if(val == "--rng-seed") {
			if (argc <= ++arg) {
				std::cerr << "format of \" " << val << " " << argv[arg] << " \" is bad\n";
				return 2;
			}
			srand(lexical_cast_default<unsigned int>(argv[arg]));
		} else if (val == "--preprocess-input-macros") {
			if (arg + 1 < argc)
			{
				++arg;
				std::string file = argv[arg];
				if (file_exists(file) == false)
				{
					std::cerr << "please specify an existing file. File "<< file <<" doesn't exist.\n";
					return 1;
				}

				std::cerr << SDL_GetTicks() << " Reading cached defines from: " << file << "\n";

				config cfg;
				std::string error_log;
				scoped_istream stream = istream_file(file);
				read(cfg, *stream);

				int read = 0;
				// use static preproc_define::read_pair(config) to make a object
				foreach (const config::any_child &value, cfg.all_children_range()) {
					const preproc_map::value_type def = preproc_define::read_pair(value.cfg);
					preproc.input_macros_[def.first] = def.second;
					++read;
				}
				std::cerr << SDL_GetTicks() << " Read " << read << " defines.\n";
			}
			else {
				std::cerr << "please specify input macros file.\n";
				return 2;
			}
		} else if (val == "--preprocess-output-macros") {
			preproc.output_macros_path_ = "true";
			if (arg + 1 < argc && argv[arg+1][0] != '-')
			{
				++arg;
				preproc.output_macros_path_ = argv[arg];
			}
		} else if (val.find("--preprocess") == 0 || val.find("-p") == 0){
			if (arg + 2 < argc){
				++arg;
				const std::string resourceToProcess(argv[arg]);
				++arg;
				const std::string targetDir(argv[arg]);

				Uint32 startTime = SDL_GetTicks();
				// if the users add the SKIP_CORE define we won't preprocess data/core
				bool skipCore = false;
				bool skipTerrainGFX = false;
				// the 'core_defines_map' is the one got from data/core macros
				preproc_map defines_map(preproc.input_macros_);
				std::string error_log;

				// add the specified defines
				size_t pos=std::string::npos;
				if (val.find("--preprocess=") == 0)
					pos = val.find("=");
				else if (val.find("-p=") == 0)
					pos = val.find("=");

				// we have some defines specified
				if (pos != std::string::npos)
				{
					std::string tmp_val = val.substr(pos+1);
					while (pos != std::string::npos)
					{
						size_t tmpPos = val.find(',', pos+1);
						tmp_val = val.substr(pos + 1,
							tmpPos == std::string::npos ? tmpPos : tmpPos - (pos+1));
						pos = tmpPos;

						if (tmp_val.empty()){
							std::cerr << "empty define supplied\n";
							continue;
						}

						LOG_PREPROC<<"adding define: "<< tmp_val<<'\n';
						defines_map.insert(std::make_pair(tmp_val,
							preproc_define(tmp_val)));
						if (tmp_val == "SKIP_CORE")
						{
							std::cerr << "'SKIP_CORE' defined.\n";
							skipCore = true;
						}
						else if (tmp_val == "NO_TERRAIN_GFX")
						{
							std::cerr << "'NO_TERRAIN_GFX' defined.\n";
							skipTerrainGFX = true;
						}
					}
					std::cerr << "added " << defines_map.size() << " defines.\n";
				}

				// preprocess core macros first if we don't skip the core
				if (skipCore == false)
				{
					std::cerr << "preprocessing common macros from 'data/core' ...\n";

					// process each folder explicitly to gain speed
					preprocess_resource(game_config::path + "/data/core/macros",&defines_map);
					if (skipTerrainGFX == false)
						preprocess_resource(game_config::path + "/data/core/terrain-graphics",&defines_map);

					std::cerr << "acquired " << (defines_map.size() - preproc.input_macros_.size())
						<< " 'data/core' defines.\n";
				}
				else
					std::cerr << "skipped 'data/core'\n";

				// preprocess resource
				std::cerr << "preprocessing specified resource: "
						  << resourceToProcess << " ...\n";
				preprocess_resource(resourceToProcess, &defines_map, true,true, targetDir);
				std::cerr << "acquired " << (defines_map.size() - preproc.input_macros_.size())
					      << " total defines.\n";

				if (preproc.output_macros_path_ != "false")
				{
					std::string outputPath = targetDir + "/_MACROS_.cfg";
					if (preproc.output_macros_path_ != "true")
						outputPath = preproc.output_macros_path_;

					std::cerr << "writing '" << outputPath << "' with "
							  << defines_map.size() << " defines.\n";

					scoped_ostream out = ostream_file(outputPath);
					if (!out->fail())
					{
						config_writer writer(*out,false);

						for(preproc_map::iterator itor = defines_map.begin();
							itor != defines_map.end(); ++itor)
						{
							(*itor).second.write(writer, (*itor).first);
						}
					}
					else
						std::cerr << "couldn't open the file.\n";
				}

				std::cerr << "preprocessing finished. Took "<< SDL_GetTicks() - startTime << " ticks.\n";
				return 0;
			}
			else{
				std::cerr << "Please specify a source file/folder and a target folder\n";
				return 2;
			}
		}
	}

	// Not the most intuitive solution, but I wanted to leave current semantics for now
	return -1;
}

/**
 * I would prefer to setup locale first so that early error
 * messages can get localized, but we need the game_controller
 * initialized to have get_intl_dir() to work.  Note: setlocale()
 * does not take GUI language setting into account.
 */
static void init_locale() {
	#ifdef _WIN32
	    std::setlocale(LC_ALL, "English");
	#else
		std::setlocale(LC_ALL, "C");
		std::setlocale(LC_MESSAGES, "");
	#endif
	const std::string& intl_dir = get_intl_dir();
	bindtextdomain (PACKAGE, intl_dir.c_str());
	bind_textdomain_codeset (PACKAGE, "UTF-8");
	bindtextdomain (PACKAGE "-lib", intl_dir.c_str());
	bind_textdomain_codeset (PACKAGE "-lib", "UTF-8");
	textdomain (PACKAGE);
}

/**
 * Setups the game environment and enters
 * the titlescreen or game loops.
 */
static int do_gameloop(int argc, char** argv)
{
	srand(time(NULL));

	int finished = process_command_args(argc, argv);
	if(finished != -1) {
		return finished;
	}

	//ensure recorder has an actually random seed instead of what it got during
	//static initialization (befire any srand() call)
	recorder.set_seed(rand());

	game_controller game(argc,argv);
	const int start_ticks = SDL_GetTicks();

	init_locale();

	bool res;

	// do initialize fonts before reading the game config, to have game
	// config error messages displayed. fonts will be re-initialized later
	// when the language is read from the game config.
	res = font::load_font_config();
	if(res == false) {
		std::cerr << "could not initialize fonts\n";
		return 1;
	}

	res = game.init_language();
	if(res == false) {
		std::cerr << "could not initialize the language\n";
		return 1;
	}

	res = game.init_video();
	if(res == false) {
		std::cerr << "could not initialize display\n";
		return 1;
	}

	const cursor::manager cursor_manager;
	cursor::set(cursor::WAIT);

	loadscreen::global_loadscreen_manager loadscreen_manager(game.disp().video());

	loadscreen::start_stage("init gui");
	gui2::init();
	const gui2::event::tmanager gui_event_manager;

	loadscreen::start_stage("load config");
	res = game.init_config();
	if(res == false) {
		std::cerr << "could not initialize game config\n";
		return 1;
	}
	loadscreen::start_stage("init fonts");

	res = font::load_font_config();
	if(res == false) {
		std::cerr << "could not re-initialize fonts for the current language\n";
		return 1;
	}

	loadscreen::start_stage("refresh addons");
	refresh_addon_version_info_cache();

#if defined(_X11) && !defined(__APPLE__)
	SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
#endif

	config tips_of_day;

	loadscreen::start_stage("titlescreen");

	LOG_CONFIG << "time elapsed: "<<  (SDL_GetTicks() - start_ticks) << " ms\n";

	for (;;)
	{
		game::exception::sticky = NULL;

		// reset the TC, since a game can modify it, and it may be used
		// by images in add-ons or campaigns dialogs
		image::set_team_colors();

		statistics::fresh_stats();

        if (!game.is_loading()) {
			const config &cfg = game.game_config().child("titlescreen_music");
			if (cfg) {
	            sound::play_music_repeatedly(game_config::title_music);
				foreach (const config &i, cfg.child_range("music")) {
					sound::play_music_config(i);
				}
				sound::commit_music_changes();
			} else {
				sound::empty_playlist();
				sound::stop_music();
			}
        }

		if(game.play_test() == false) {
			return 0;
		}

		if(game.play_multiplayer_mode() == false) {
			return 0;
		}

		if(game.play_screenshot_mode() == false) {
			return 0;
		}

		recorder.clear();

		//Start directly a campaign
		if(game.goto_campaign() == false){
			if (game.jump_to_campaign_id().empty())
				continue; //Go to main menu
			else
				return 1; //we got an error starting the campaign from command line
		}

		//Start directly a multiplayer
		//Eventually with a specified server
		if(game.goto_multiplayer() == false){
			continue; //Go to main menu
		}
#ifndef DISABLE_EDITOR
		if (game.goto_editor() == false) {
			return 0;
		}
#endif
		loadscreen_manager.reset();

		gui2::ttitle_screen::tresult res = game.is_loading()
				? gui2::ttitle_screen::LOAD_GAME
				: gui2::ttitle_screen::NOTHING;

		const preferences::display_manager disp_manager(&game.disp());

		const font::floating_label_context label_manager;

		cursor::set(cursor::NORMAL);
		if(res == gui2::ttitle_screen::NOTHING) {
			const hotkey::basic_handler key_handler(&game.disp());
			gui2::ttitle_screen dlg;
			dlg.show(game.disp().video());

			res = static_cast<gui2::ttitle_screen::tresult>(dlg.get_retval());
		}

		game_controller::RELOAD_GAME_DATA should_reload = game_controller::RELOAD_DATA;

		if(res == gui2::ttitle_screen::QUIT_GAME) {
			LOG_GENERAL << "quitting game...\n";
			return 0;
		} else if(res == gui2::ttitle_screen::LOAD_GAME) {
			if(game.load_game() == false) {
				game.clear_loaded_game();
				res = gui2::ttitle_screen::NOTHING;
				continue;
			}

			should_reload = game_controller::NO_RELOAD_DATA;
		} else if(res == gui2::ttitle_screen::TUTORIAL) {
			game.set_tutorial();
		} else if(res == gui2::ttitle_screen::NEW_CAMPAIGN) {
			if(game.new_campaign() == false) {
				continue;
			}
		} else if(res == gui2::ttitle_screen::MULTIPLAYER) {
			game_config::debug = false;
			if(game.play_multiplayer() == false) {
				continue;
			}
		} else if(res == gui2::ttitle_screen::CHANGE_LANGUAGE) {
			if (game.change_language()) {
				tips_of_day.clear();
				t_string::reset_translations();
				image::flush_cache();
			}
			continue;
		} else if(res == gui2::ttitle_screen::EDIT_PREFERENCES) {
			game.show_preferences();
			continue;
		} else if(res == gui2::ttitle_screen::SHOW_ABOUT) {
			about::show_about(game.disp());
			continue;
		} else if(res == gui2::ttitle_screen::SHOW_HELP) {
			help::help_manager help_manager(&game.game_config(), NULL);
			help::show_help(game.disp());
			continue;
		} else if(res == gui2::ttitle_screen::GET_ADDONS) {
			try {
				manage_addons(game.disp());
			} catch(config_changed_exception const&) {
				game.reload_changed_game_config();
			}
			continue;
		} else if(res == gui2::ttitle_screen::RELOAD_GAME_DATA) {
			loadscreen::global_loadscreen_manager loadscreen(game.disp().video());
			game.reload_changed_game_config();
			image::flush_cache();
			continue;
#ifndef DISABLE_EDITOR
		} else if(res == gui2::ttitle_screen::START_MAP_EDITOR) {
			///@todo editor can ask the game to quit completely
			if (game.start_editor() == editor::EXIT_QUIT_TO_DESKTOP) {
				return 0;
			}
			continue;
#endif
		}

		if (recorder.at_end()){
			game.launch_game(should_reload);
		}
		else{
			game.play_replay();
		}
	}

	return 0;
}

#ifndef DISABLE_POOL_ALLOC
extern "C" {
void init_custom_malloc();
}
#endif

int main(int argc, char** argv)
{
#ifndef DISABLE_POOL_ALLOC
	init_custom_malloc();
#endif
	if(SDL_Init(SDL_INIT_TIMER) < 0) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
		return(1);
	}

	try {
		std::cerr << "Battle for Wesnoth v" << game_config::revision << '\n';
		const time_t t = time(NULL);
		std::cerr << "Started on " << ctime(&t) << "\n";

		const std::string exe_dir = get_exe_dir();
		if(!exe_dir.empty() && file_exists(exe_dir + "/data/_main.cfg")) {
			std::cerr << "Automatically found a possible data directory at "
			          << exe_dir << '\n';
			game_config::path = exe_dir;
		}

		const int res = do_gameloop(argc,argv);
		safe_exit(res);
	} catch(CVideo::error&) {
		std::cerr << "Could not initialize video. Exiting.\n";
		return 1;
	} catch(font::manager::error&) {
		std::cerr << "Could not initialize fonts. Exiting.\n";
		return 1;
	} catch(config::error& e) {
		std::cerr << e.message << "\n";
		return 1;
	} catch(gui::button::error&) {
		std::cerr << "Could not create button: Image could not be found\n";
		return 1;
	} catch(CVideo::quit&) {
		//just means the game should quit
	} catch(end_level_exception&) {
		std::cerr << "caught end_level_exception (quitting)\n";
	} catch(twml_exception& e) {
		std::cerr << "WML exception:\nUser message: "
			<< e.user_message << "\nDev message: " << e.dev_message << '\n';
		return 1;
	} catch(game_logic::formula_error& e) {
		std::cerr << e.what()
			<< "\n\nGame will be aborted.\n";
		return 1;
	} catch(game::error &) {
		// A message has already been displayed.
		return 1;
	} catch(std::bad_alloc&) {
		std::cerr << "Ran out of memory. Aborted.\n";
		return ENOMEM;
	}

	return 0;
} // end main

