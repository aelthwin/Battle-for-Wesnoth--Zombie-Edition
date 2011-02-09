/* $Id: tooltip.hpp 48153 2011-01-01 15:57:50Z mordante $ */
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

#ifndef GUI_WIDGETS_TOOLTIP_HPP_INCLUDED
#define GUI_WIDGETS_TOOLTIP_HPP_INCLUDED

#include "gui/widgets/control.hpp"

namespace gui2 {

/**
 * A tooltip shows a 'floating' message.
 *
 * This is a small class which only has one state and that's active, so the
 * functions implemented are mostly dummies.
 *
 * @todo Allow wrapping for the tooltip.
 */
class ttooltip : public tcontrol
{
public:

	ttooltip() :
		tcontrol(1)
	{
	}

	/** Inherited from tcontrol. */
	void set_active(const bool) {}

	/** Inherited from tcontrol. */
	bool get_active() const { return true; }

	/** Inherited from tcontrol. */
	unsigned get_state() const { return 0; }

	/** Inherited from tcontrol. */
	bool disable_click_dismiss() const { return false; }

private:
	/** Inherited from tcontrol. */
	const std::string& get_control_type() const;
};

} // namespace gui2

#endif
