/* $Id: button.hpp 48153 2011-01-01 15:57:50Z mordante $ */
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
#ifndef BUTTON_H_INCLUDED
#define BUTTON_H_INCLUDED

#include "widget.hpp"

#include "../exceptions.hpp"

namespace gui {

class button : public widget
{
public:
	struct error : public game::error {
        error()
            : game::error("GUI1 button error")
            {}
    };

	enum TYPE { TYPE_PRESS, TYPE_CHECK, TYPE_TURBO, TYPE_IMAGE };

	enum SPACE_CONSUMPTION { DEFAULT_SPACE, MINIMUM_SPACE };

	button(CVideo& video, const std::string& label, TYPE type=TYPE_PRESS,
	       std::string button_image="", SPACE_CONSUMPTION spacing=DEFAULT_SPACE,
		   const bool auto_join=true);

	/** Default implementation, but defined out-of-line for efficiency reasons. */
	virtual ~button();
	void set_check(bool check);
	void set_active(bool active);
	bool checked() const;

	void set_label(const std::string& val);

	bool pressed();
	bool hit(int x, int y) const;
	virtual void enable(bool new_val=true);
	void release();

protected:
	virtual void handle_event(const SDL_Event& event);
	virtual void mouse_motion(const SDL_MouseMotionEvent& event);
	virtual void mouse_down(const SDL_MouseButtonEvent& event);
	virtual void mouse_up(const SDL_MouseButtonEvent& event);
	virtual void draw_contents();

	TYPE type_;

private:

	void calculate_size();

	std::string label_;
	surface image_, pressedImage_, activeImage_, pressedActiveImage_;
	SDL_Rect textRect_;

	bool button_;

	enum STATE { UNINIT, NORMAL, ACTIVE, PRESSED, PRESSED_ACTIVE };
	STATE state_;

	bool pressed_;

	SPACE_CONSUMPTION spacing_;

	int base_height_, base_width_;

}; //end class button

}

#endif
