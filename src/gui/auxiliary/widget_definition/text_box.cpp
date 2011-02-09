/* $Id: text_box.cpp 48153 2011-01-01 15:57:50Z mordante $ */
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

#include "gui/auxiliary/widget_definition/text_box.hpp"

#include "gui/auxiliary/log.hpp"

namespace gui2 {

ttext_box_definition::ttext_box_definition(const config& cfg)
	: tcontrol_definition(cfg)
{
	DBG_GUI_P << "Parsing text_box " << id << '\n';

	load_resolutions<tresolution>(cfg);
}

ttext_box_definition::tresolution::tresolution(const config& cfg)
	: tresolution_definition_(cfg)
	, text_x_offset(cfg["text_x_offset"])
	, text_y_offset(cfg["text_y_offset"])
{
/*WIKI
 * @page = GUIWidgetDefinitionWML
 * @order = 1_text_box
 *
 * == Text box ==
 *
 * The definition of a text box.
 *
 * The resolution for a text box also contains the following keys:
 * @begin{table}{config}
 *     text_x_offset & f_unsigned & "" & The x offset of the text in the text
 *                                     box. This is needed for the code to
 *                                     determine where in the text the mouse
 *                                     clicks, so it can set the cursor
 *                                     properly. $
 *     text_y_offset & f_unsigned & "" & The y offset of the text in the text
 *                                     box. $
 * @end{table}
 *
 * The following states exist:
 * * state_enabled, the text box is enabled.
 * * state_disabled, the text box is disabled.
 * * state_focussed, the text box has the focus of the keyboard.
 */
	// Note the order should be the same as the enum tstate in text_box.hpp.
	state.push_back(tstate_definition(cfg.child("state_enabled")));
	state.push_back(tstate_definition(cfg.child("state_disabled")));
	state.push_back(tstate_definition(cfg.child("state_focussed")));
}

} // namespace gui2

