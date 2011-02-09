/* $Id: action.cpp 48153 2011-01-01 15:57:50Z mordante $ */
/*
 Copyright (C) 2010 - 2011 by Gabriel Morin <gabrielmorin (at) gmail (dot) com>
 Part of the Battle for Wesnoth Project http://www.wesnoth.org

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
 */

#include "action.hpp"

namespace wb {

std::ostream& operator<<(std::ostream& s, action_ptr action)
{
	assert(action);
	return action->print(s);
}

std::ostream& operator<<(std::ostream& s, action_const_ptr action)
{
	assert(action);
	return action->print(s);
}

std::ostream& action::print(std::ostream& s) const
{
	return s;
}

action::action(size_t team_index)
	: team_index_(team_index)
{
}

action::~action()
{
}

} // end namespace wb
