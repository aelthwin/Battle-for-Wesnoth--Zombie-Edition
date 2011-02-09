/* $Id: drawing.cpp 48153 2011-01-01 15:57:50Z mordante $ */
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

#include "gui/widgets/drawing.hpp"

#include "gui/auxiliary/widget_definition/drawing.hpp"
#include "gui/auxiliary/window_builder/drawing.hpp"
#include "gui/widgets/settings.hpp"

#include <boost/bind.hpp>

namespace gui2 {

REGISTER_WIDGET(drawing)

const std::string& tdrawing::get_control_type() const
{
	static const std::string type = "drawing";
	return type;
}

} // namespace gui2

