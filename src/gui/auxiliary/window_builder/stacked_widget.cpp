/* $Id: stacked_widget.cpp 48153 2011-01-01 15:57:50Z mordante $ */
/*
   Copyright (C) 2009 - 2011 by Mark de Wever <koraq@xs4all.nl>
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

#include "gui/auxiliary/window_builder/stacked_widget.hpp"

#include "config.hpp"
#include "foreach.hpp"
#include "gettext.hpp"
#include "gui/auxiliary/log.hpp"
#include "gui/auxiliary/widget_definition/stacked_widget.hpp"
#include "gui/widgets/stacked_widget.hpp"
#include "wml_exception.hpp"

namespace gui2 {

namespace implementation {

tbuilder_stacked_widget::tbuilder_stacked_widget(const config& cfg)
	: tbuilder_control(cfg)
	, stack()
{
	const config &s = cfg.child("stack");
	VALIDATE(s, _("No stack defined."));
	foreach(const config &layer, s.child_range("layer")) {
		stack.push_back(new tbuilder_grid(layer));
	}
}

twidget* tbuilder_stacked_widget::build() const
{
	tstacked_widget *widget = new tstacked_widget();

	init_control(widget);

	DBG_GUI_G << "Window builder: placed stacked widget '"
			<< id << "' with definition '"
			<< definition << "'.\n";

	boost::intrusive_ptr<const tstacked_widget_definition::tresolution> conf =
			boost::dynamic_pointer_cast
				<const tstacked_widget_definition::tresolution>(
						widget->config());
	assert(conf);

	widget->init_grid(conf->grid);

	widget->finalize(stack);

	return widget;
}

} // namespace implementation

} // namespace gui2

/*WIKI
 * @page = GUIToolkitWML
 * @order = 2_stacked_widget
 *
 * == Stacked widget ==
 *
 * A stacked widget is a set of widget stacked on top of each other. The
 * widgets are drawn in the layers, in the order defined in the the instance
 * config. By default the last drawn item is also the "active" layer for the
 * event handling.
 *
 * @begin{table}{config}
 * @end{table}
 */

