/* $Id: button.cpp 48153 2011-01-01 15:57:50Z mordante $ */
/*
   Copyright (C) 2007 - 2011 by Mark de Wever <koraq@xs4all.nl>
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

#include "gui/auxiliary/widget_definition/button.hpp"

#include "gui/auxiliary/log.hpp"

namespace gui2 {

tbutton_definition::tbutton_definition(const config& cfg)
	: tcontrol_definition(cfg)
{
	DBG_GUI_P << "Parsing button " << id << '\n';

	load_resolutions<tresolution>(cfg);
}

tbutton_definition::tresolution::tresolution(const config& cfg)
	: tresolution_definition_(cfg)
{
/*WIKI
 * @page = GUIWidgetDefinitionWML
 * @order = 1_button
 *
 * == Button ==
 *
 * @macro = button_description
 *
 * The following states exist:
 * * state_enabled, the button is enabled.
 * * state_disabled, the button is disabled.
 * * state_pressed, the left mouse button is down.
 * * state_focussed, the mouse is over the button.
 */

	// Note the order should be the same as the enum tstate in button.hpp.
	state.push_back(tstate_definition(cfg.child("state_enabled")));
	state.push_back(tstate_definition(cfg.child("state_disabled")));
	state.push_back(tstate_definition(cfg.child("state_pressed")));
	state.push_back(tstate_definition(cfg.child("state_focussed")));
}

} // namespace gui2

