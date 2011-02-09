/* $Id: mp_create_game_set_password.cpp 48153 2011-01-01 15:57:50Z mordante $ */
/*
   Copyright (C) 2010 - 2011 by Ignacio Riquelme Morelle <shadowm2006@gmail.com>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#define GETTEXT_DOMAIN "wesnoth-lib"

#include "gui/dialogs/mp_create_game_set_password.hpp"

#include "gui/dialogs/field.hpp"
#include "gui/widgets/settings.hpp"
#include "gui/widgets/window.hpp"

namespace gui2 {

/*WIKI
 * @page = GUIWindowDefinitionWML
 * @order = 2_mp_create_game_set_password
 *
 * == Create Game: Set Password ==
 *
 * Dialog for setting a join password for MP games.
 *
 * @begin{table}{dialog_widgets}
 *
 * password & & text_box & m &
 *         Input field for the game password. $
 *
 * @end{table}
 */

REGISTER_WINDOW(mp_create_game_set_password)

tmp_create_game_set_password::tmp_create_game_set_password(const std::string& password)
	: password_(password)
	, password_field_(register_text("password", false))
{
}

void tmp_create_game_set_password::pre_show(CVideo& /*video*/, twindow& window)
{
	assert(password_field_);
	password_field_->set_widget_value(window, password_);
	window.keyboard_capture(password_field_->widget(window));
}

void tmp_create_game_set_password::post_show(twindow& window)
{
	if(get_retval() != twindow::OK) {
		return;
	}

	password_ = password_field_->get_widget_value(window);
}

}
