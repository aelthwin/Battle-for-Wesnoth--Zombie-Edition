/* $Id: addon_connect.cpp 48153 2011-01-01 15:57:50Z mordante $ */
/*
   Copyright (C) 2008 - 2011 by Mark de Wever <koraq@xs4all.nl>
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

#include "gui/dialogs/addon_connect.hpp"

#include "gui/widgets/button.hpp"
#include "gui/widgets/window.hpp"
#include "gui/widgets/settings.hpp"
#include "gui/widgets/text_box.hpp"

namespace gui2 {

/*WIKI
 * @page = GUIWindowDefinitionWML
 * @order = 2_addon_connect
 *
 * == Addon connect ==
 *
 * This shows the dialog for managing addons and connecting to the addon server.
 *
 * @begin{table}{dialog_widgets}
 * hostname & & text_box & m &
 *         This text contains the name of the server to connect to. $
 *
 * & 2 & button & o &
 *         This button closes the dialog and starts the addon manager. $
 *
 * & 3 & button & o &
 *         This button closes the dialog and starts the update routine. $
 *
 * @end{table}
 */

REGISTER_WINDOW(addon_connect)

void taddon_connect::pre_show(CVideo& /*video*/, twindow& window)
{
	ttext_box& host_widget =
			find_widget<ttext_box>(&window, "host_name", false);
	tbutton& update_cmd =
			find_widget<tbutton>(&window, "update_addons", false);
	update_cmd.set_active( allow_updates_ );
	tbutton& remove_cmd =
			find_widget<tbutton>(&window, "remove_addons", false);
	remove_cmd.set_active( allow_remove_ );

	host_widget.set_value(host_name_);
	window.keyboard_capture(&host_widget);
}

void taddon_connect::post_show(twindow& window)
{
	if(get_retval() == twindow::OK) {
		ttext_box& host_widget =
				find_widget<ttext_box>(&window, "host_name", false);

		host_widget.save_to_history();
		host_name_= host_widget.get_value();
	}
}

} // namespace gui2
