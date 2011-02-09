/* $Id: control.hpp 48153 2011-01-01 15:57:50Z mordante $ */
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

#ifndef GUI_AUXILIARY_WINDOW_BUILDER_CONTROL_HPP_INCLUDED
#define GUI_AUXILIARY_WINDOW_BUILDER_CONTROL_HPP_INCLUDED

#include "gui/auxiliary/window_builder.hpp"

namespace gui2 {

class tcontrol;

namespace implementation {

struct tbuilder_control
	: public tbuilder_widget
{
public:

	tbuilder_control(const config& cfg);

	void init_control(tcontrol* control) const;

	/** Parameters for the control. */
	std::string id;
	std::string definition;
	std::string linked_group;
	t_string label;
	t_string tooltip;
	t_string help;
	bool use_tooltip_on_label_overflow;
#ifndef LOW_MEM
	int debug_border_mode;
	unsigned debug_border_color;
#endif
};

} // namespace implementation

} // namespace gui2

#endif

