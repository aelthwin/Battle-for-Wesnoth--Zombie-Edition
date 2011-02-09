/* $Id: combo.hpp 48153 2011-01-01 15:57:50Z mordante $ */
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
#ifndef COMBO_H_INCLUDED
#define COMBO_H_INCLUDED

#include "button.hpp"

class display;

namespace gui {

class combo : public button
{
public:
	combo(display& disp, const std::vector<std::string>& items);

	void set_selected(int val);
	void set_items(const std::vector<std::string>& items);
	int selected() const;
	bool changed();

protected:
	virtual void process_event();

	void make_drop_down_menu();
private:
	void set_selected_internal(int val);
	std::vector<std::string> items_;
	int selected_, oldSelected_;
	display* disp_;
	static const std::string empty_combo_label;
	static const int font_size;
	static const int horizontal_padding;
	static const int vertical_padding;
}; //end class combo

}

#endif
