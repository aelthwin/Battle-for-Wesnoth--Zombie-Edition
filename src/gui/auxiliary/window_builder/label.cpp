/* $Id: label.cpp 48153 2011-01-01 15:57:50Z mordante $ */
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

#include "gui/auxiliary/window_builder/label.hpp"

#include "config.hpp"
#include "gui/auxiliary/log.hpp"
#include "gui/widgets/label.hpp"

namespace gui2 {

namespace implementation {

tbuilder_label::tbuilder_label(const config& cfg)
	: tbuilder_control(cfg)
	, wrap(cfg["wrap"].to_bool())
	, text_alignment(decode_text_alignment(cfg["text_alignment"]))
{
}

twidget* tbuilder_label::build() const
{
	tlabel* widget = new tlabel();

	init_control(widget);

	widget->set_can_wrap(wrap);
	widget->set_text_alignment(text_alignment);

	DBG_GUI_G << "Window builder: placed label '"
		<< id << "' with definition '"
		<< definition << "'.\n";

	return widget;
}

} // namespace implementation

} // namespace gui2

/*WIKI_MACRO
 * @start_macro = label_description
 *
 *        A label displays a text, the text can be wrapped but no scrollbars
 *        are provided.
 * @end_macro
 */

/*WIKI
 * @page = GUIWidgetInstanceWML
 * @order = 2_label
 *
 * == Label ==
 *
 * @macro = label_description
 *
 * List with the label specific variables:
 * @begin{table}{config}
 *     wrap & bool & false &        Is wrapping enabled for the label. $
 *     text_alignment & h_align & "left" &
 *                                How is the text aligned in the label. $
 * @end{table}
 */

