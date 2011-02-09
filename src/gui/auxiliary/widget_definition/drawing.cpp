/* $Id: drawing.cpp 48165 2011-01-02 10:58:02Z mordante $ */
/*
   Copyright (C) 2010 - 2011 by Mark de Wever <koraq@xs4all.nl>
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

#include "gui/auxiliary/widget_definition/drawing.hpp"

#include "gui/auxiliary/log.hpp"

namespace gui2 {

tdrawing_definition::tdrawing_definition(const config& cfg)
	: tcontrol_definition(cfg)
{
	DBG_GUI_P << "Parsing drawing " << id << '\n';

	load_resolutions<tresolution>(cfg);
}

tdrawing_definition::tresolution::tresolution(const config& cfg)
	: tresolution_definition_(cfg)
{
/*WIKI
 * @page = GUIWidgetDefinitionWML
 * @order = 1_drawing
 *
 * == Drawing ==
 *
 * @macro = drawing_description
 *
 * The definition of a drawing. The widget normally has no event interaction
 * so only one state is defined. This
 *
 * The following states exist:
 * * state_enabled, the drawing is enabled. The state is a dummy since the
 *     things really drawn are placed in the window instance.
 */
	/*
	 * Note the order should be the same as the enum tstate in drawing.hpp.
	 * Normally the [draw] section is in the config, but for this widget the
	 * original draw section is ignored, so send a dummy.
	 */
	static const config dummy("draw");
	state.push_back(tstate_definition(dummy));
}

} // namespace gui2

