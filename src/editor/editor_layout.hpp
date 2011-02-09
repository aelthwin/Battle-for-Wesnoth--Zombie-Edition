/* $Id: editor_layout.hpp 48153 2011-01-01 15:57:50Z mordante $ */
/*
  Copyright (C) 2003 - 2011 by David White <dave@whitevine.net>
  Part of the Battle for Wesnoth Project http://www.wesnoth.org/

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY.

  See the COPYING file for more details.
*/

/** @file */

#ifndef EDITOR_LAYOUT_H_INCLUDED
#define EDITOR_LAYOUT_H_INCLUDED

#include "global.hpp"

#include <cstddef>

class display;

namespace editor {

/** Size specifications for the map editor. */
struct size_specs {
	/**
	 * Initialize the values to dummie-values that will avoid floating point
	 * errors if calculations are made before the sizes are adjusted through
	 * adjust_sizes().
	 */
	size_specs();
	size_t terrain_size;
	size_t terrain_padding;
	size_t terrain_space;
	size_t terrain_width;
	size_t palette_x;
	size_t palette_y;
	size_t palette_h;
	size_t palette_w;
	size_t brush_x;
	size_t brush_y;
	size_t brush_padding;
};

/** Adjust the internal size specifications to fit the display. */
void adjust_sizes(const display &disp, size_specs &sizes);

}

#endif // EDITOR_LAYOUT_H_INCLUDED
