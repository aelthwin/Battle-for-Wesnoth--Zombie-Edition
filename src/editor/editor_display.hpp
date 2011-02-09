/* $Id: editor_display.hpp 48153 2011-01-01 15:57:50Z mordante $ */
/*
   Copyright (C) 2008 - 2011 by Tomasz Sniatowski <kailoran@gmail.com>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#ifndef EDITOR_EDITOR_DISPLAY_HPP_INCLUDED
#define EDITOR_EDITOR_DISPLAY_HPP_INCLUDED

#include "editor_map.hpp"
#include "../display.hpp"

namespace editor {

class editor_display : public display
{
public:
	editor_display(CVideo& video, const editor_map& map, const config& theme_cfg,
			const config& level);

	bool in_editor() const { return true; }

	void add_brush_loc(const map_location& hex);
	void set_brush_locs(const std::set<map_location>& hexes);
	void clear_brush_locs();
	void remove_brush_loc(const map_location& hex);
	const editor_map& map() const { return static_cast<const editor_map&>(get_map()); }
	void rebuild_terrain(const map_location &loc);
	void set_toolbar_hint(const std::string& value) { toolbar_hint_ = value; }

protected:
	void pre_draw();
	/**
	* The editor uses different rules for terrain highlighting (e.g. selections)
	*/
	image::TYPE get_image_type(const map_location& loc);

	void draw_hex(const map_location& loc);

	const SDL_Rect& get_clip_rect();
	void draw_sidebar();

	std::set<map_location> brush_locations_;
	std::string toolbar_hint_;
};

} //end namespace editor
#endif
