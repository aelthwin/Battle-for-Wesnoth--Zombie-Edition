/* $Id: game_load.hpp 48153 2011-01-01 15:57:50Z mordante $ */
/*
   Copyright (C) 2008 - 2011 by Jörg Hinrichs <joerg.hinrichs@alice-dsl.de>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#ifndef GUI_DIALOGS_LOAD_GAME_HPP_INCLUDED
#define GUI_DIALOGS_LOAD_GAME_HPP_INCLUDED

#include "gui/dialogs/dialog.hpp"
#include "gui/widgets/listbox.hpp"
#include "gui/widgets/text.hpp"
#include "savegame.hpp"
#include "tstring.hpp"

namespace gui2 {

class tgame_load : public tdialog
{
public:
	tgame_load(const config& cache_config);

	const std::string& filename() const { return filename_; }
	bool show_replay() const { return show_replay_; }
	bool cancel_orders() const { return cancel_orders_; }

protected:
	/** Inherited from tdialog. */
	void pre_show(CVideo& video, twindow& window);

	/** Inherited from tdialog. */
	void post_show(twindow& window);

private:

	/** Inherited from tdialog, implemented by REGISTER_WINDOW. */
	virtual const std::string& window_id() const;

	bool filter_text_changed(ttext_* textbox, const std::string& text);
	void list_item_clicked(twindow& window);
	void delete_button_callback(twindow& window);

	void display_savegame(twindow& window);
	void evaluate_summary_string(std::stringstream& str, const config& cfg_summary);
	void fill_game_list(twindow& window, std::vector<savegame::save_info>& games);

	tfield_text* txtFilter_;
	tfield_bool* chk_show_replay_;
	tfield_bool* chk_cancel_orders_;

	std::string filename_;
	bool show_replay_;
	bool cancel_orders_;

	std::vector<savegame::save_info> games_;
	const config& cache_config_;

	std::vector<std::string> last_words_;
};

}

#endif

