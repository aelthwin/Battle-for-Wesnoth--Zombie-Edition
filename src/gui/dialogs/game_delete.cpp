/* $Id: game_delete.cpp 48153 2011-01-01 15:57:50Z mordante $ */
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

#include "gui/dialogs/field.hpp"
#include "gui/dialogs/game_delete.hpp"
#include "gui/widgets/settings.hpp"

namespace gui2 {

/*WIKI
 * @page = GUIWindowDefinitionWML
 * @order = 2_game_delete
 *
 * == Delete a savegame ==
 *
 * This shows the dialog to confirm deleting a savegame file.
 *
 * @begin{table}{dialog_widgets}
 *
 * dont_ask_again & & boolean_selector & m &
 *        A checkbox to not show this dialog again. $
 *
 * @end{table}
 */

REGISTER_WINDOW(game_delete)

tgame_delete::tgame_delete()
	: chk_dont_ask_again_(register_bool("dont_ask_again"))
	, dont_ask_again_(false)
{}

void tgame_delete::post_show(twindow& window)
{
	dont_ask_again_ = chk_dont_ask_again_->get_widget_value(window);
}

} // namespace gui2

