/* $Id: mp_connect.cpp 48153 2011-01-01 15:57:50Z mordante $ */
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

#include "gui/dialogs/mp_connect.hpp"

#include "foreach.hpp"
#include "game_preferences.hpp"
#include "gui/dialogs/field.hpp"
#include "gui/widgets/button.hpp"
#ifdef GUI2_EXPERIMENTAL_LISTBOX
#include "gui/widgets/list.hpp"
#else
#include "gui/widgets/listbox.hpp"
#endif
#include "gui/widgets/label.hpp"
#include "gui/widgets/password_box.hpp"
#include "gui/widgets/settings.hpp"
#include "gui/widgets/scroll_label.hpp"
#include "gui/widgets/toggle_button.hpp"

#include <boost/bind.hpp>

namespace gui2 {

namespace {

/*WIKI
 * @page = GUIWindowDefinitionWML
 * @order = 2_mp_server_list
 *
 * == Multiplayer server list ==
 *
 * This shows the dialog with a list of predefined multiplayer servers.
 *
 * @begin{table}{dialog_widgets}
 *
 * server_list & & listbox & m &
 *         Listbox with the predefined servers to connect to. $
 *
 * -name & & control & o &
 *         Widget which shows the name of the server. $
 *
 * -address & & control & m &
 *         The address/host_name of the server. $
 *
 * @end{table}
 */

class tmp_server_list : public tdialog
{
public:
	tmp_server_list() :
		host_name_()
	{}

	const std::string& host_name() const { return host_name_; }

private:
	std::string host_name_;

	/** Inherited from tdialog, implemented by REGISTER_WINDOW. */
	virtual const std::string& window_id() const;

	/** Inherited from tdialog. */
	void pre_show(CVideo& video, twindow& window);

	/** Inherited from tdialog. */
	void post_show(twindow& window);
};

REGISTER_WINDOW(mp_server_list)

void tmp_server_list::pre_show(CVideo& /*video*/, twindow& window)
{
	tlistbox& list = find_widget<tlistbox>(&window, "server_list", false);

	window.keyboard_capture(&list);

	const std::vector<game_config::server_info>&
		pref_servers = preferences::server_list();

	foreach(const game_config::server_info& server, pref_servers) {

		std::map<std::string, string_map> data;
		string_map item;

		item["label"] = server.name;
		data.insert(std::make_pair("name", item));

		item["label"] = server.address;
		data.insert(std::make_pair("address", item));

		list.add_row(data);
	}
}

void tmp_server_list::post_show(twindow& window)
{
	if(get_retval() == twindow::OK) {

		const tlistbox& list = find_widget<const tlistbox>(
				&window, "server_list", false);

		const tgrid* row = list.get_row_grid(list.get_selected_row());
		assert(row);

		host_name_ = find_widget<const tcontrol>(row, "address", false).label();
	}
}

} // namespace

/*WIKI
 * @page = GUIWindowDefinitionWML
 * @order = 2_mp_connect
 *
 * == Multiplayer connect ==
 *
 * This shows the dialog to the MP server to connect to.
 *
 * @begin{table}{dialog_widgets}
 *
 * start_table & & text_box & m &
 *         The name of the server to connect to. $
 *
 * list & & button & o &
 *         Shows a dialog with a list of predefined servers to connect to. $
 *
 * @end{table}
 */

REGISTER_WINDOW(mp_connect)

tmp_connect::tmp_connect() :
	video_(0),
	host_name_(register_text("host_name", false,
		preferences::network_host,
		preferences::set_network_host))
{
}

void tmp_connect::pre_show(CVideo& video, twindow& window)
{
	assert(!video_);
	assert(host_name_);
	video_ = &video;

	window.keyboard_capture(host_name_->widget(window));

	// Set view list callback button.
	if(tbutton* button = find_widget<tbutton>(&window, "list", false, false)) {

		connect_signal_mouse_left_click(*button, boost::bind(
				  &tmp_connect::show_server_list
				, this
				, boost::ref(window)));
	}

}

void tmp_connect::post_show(twindow& /*window*/)
{
	video_ = 0;
}

void tmp_connect::show_server_list(twindow& window)
{
	assert(video_);
	assert(host_name_);

	tmp_server_list dlg;
	dlg.show(*video_);

	if(dlg.get_retval() == twindow::OK) {
		host_name_->set_widget_value(window, dlg.host_name());
	}
}

/*WIKI
 * @page = GUIWindowDefinitionWML
 * @order = 2_mp_login
 *
 * == Multiplayer connect ==
 *
 * This shows the dialog to log in to the MP server
 *
 * @begin{table}{dialog_widgets}
 *
 * user_name & & text_box & m &
 *         The login user name. $
 *
 * password & & text_box & m &
 *         The password. $
 *
 * password_reminder & & button & o &
 *         Request a password reminder. $
 *
 * change_username & & button & o &
 *         Use a different username. $
 *
 * login_label & & button & o &
 *         Displays the information received from the server. $
 *
 * @end{table}
 */

REGISTER_WINDOW(mp_login)

tmp_login::tmp_login(const t_string& label,	const bool focus_password) :
		label_(label), focus_password_(focus_password) { }

void tmp_login::pre_show(CVideo& /*video*/, twindow& window)
{
	ttext_box* username =
			find_widget<ttext_box>(&window, "user_name", false, true);
	username->set_value(preferences::login());

	tpassword_box* password =
			find_widget<tpassword_box>(&window, "password", false, true);
	password->set_value(preferences::password());

	window.keyboard_capture(focus_password_ ? password : username);

	if(tbutton* button = find_widget<tbutton>(
			&window, "password_reminder", false, false)) {

		button->set_retval(1);
	}

	if(tbutton* button = find_widget<tbutton>(
			&window, "change_username", false, false)) {

		button->set_retval(2);
	}

	// Needs to be a scroll_label since the text can get long and a normal
	// label can't wrap (at the moment).
	tcontrol* label =
		dynamic_cast<tscroll_label*>(window.find("login_label", false));
	if(label) label->set_label(label_);

	if(ttoggle_button* button = find_widget<ttoggle_button>(
			&window, "remember_password", false, false)) {

		button->set_value(preferences::remember_password());
	}
}

void tmp_login::post_show(twindow& window)
{
	if(get_retval() == twindow::OK) {
		if(ttoggle_button* button = find_widget<ttoggle_button>(
				&window, "remember_password", false, false)) {

			preferences::set_remember_password(button->get_value());
		}

		preferences::set_login(find_widget<ttext_box>(
				&window, "user_name", false).get_value());

		preferences::set_password(find_widget<tpassword_box>(
				&window, "password", false).get_real_value());
	}
}

} // namespace gui2

