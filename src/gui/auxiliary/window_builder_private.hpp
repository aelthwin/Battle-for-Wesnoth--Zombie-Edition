/* $Id: window_builder_private.hpp 48153 2011-01-01 15:57:50Z mordante $ */
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

/**
 * @file
 * This file contains all classes used privately in window_builder.cpp and
 * should only be included by window_builder.cpp.
 */

#ifndef GUI_AUXILIARY_WINDOW_BUILDER_PRIVATE_HPP_INCLUDED
#define GUI_AUXILIARY_WINDOW_BUILDER_PRIVATE_HPP_INCLUDED

#include "gui/auxiliary/window_builder.hpp"

#include "config.hpp"

namespace gui2 {

/**
 * A temporary helper class.
 *
 * @todo refactor with the grid builder.
 */
struct tbuilder_gridcell
	: public tbuilder_widget
{
	explicit tbuilder_gridcell(const config& cfg);

	/** The flags for the cell. */
	unsigned flags;

	/** The bordersize for the cell. */
	unsigned border_size;

	/** The widgets for the cell. */
	tbuilder_widget_ptr widget;

	/** We're a dummy the building is done on construction. */
	twidget* build () const { return NULL; }
};

} // namespace gui2

#endif

