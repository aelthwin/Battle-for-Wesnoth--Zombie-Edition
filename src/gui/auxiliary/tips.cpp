/* $Id: tips.cpp 48153 2011-01-01 15:57:50Z mordante $ */
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

#include "gui/auxiliary/tips.hpp"

#include "config.hpp"
#include "foreach.hpp"
#include "game_preferences.hpp"
#include "serialization/string_utils.hpp"

namespace gui2 {

ttip::ttip(const t_string& text
		, const t_string& source
		, const std::string& unit_filter)
	: text_(text)
	, source_(source)
	, unit_filter_(utils::split(unit_filter))
{
}

namespace tips {

std::vector<ttip> load(const config& cfg)
{
	std::vector<ttip> result;

	foreach(const config &tip, cfg.child_range("tip")) {
		result.push_back(ttip(tip["text"]
				, tip["source"]
				, tip["encountered_units"]));
	}

	return result;
}

std::vector<ttip> shuffle(const std::vector<ttip>& tips)
{
	std::vector<ttip> result;

	const std::set<std::string>& units = preferences::encountered_units();

	foreach(const ttip& tip, tips) {
		if(tip.unit_filter_.empty()) {
			result.push_back(tip);
		} else {
			foreach(const std::string& unit, tip.unit_filter_) {
				if(units.find(unit) != units.end()) {
					result.push_back(tip);
					break;
				}
			}
		}
	}

	std::random_shuffle(result.begin(), result.end());
	return result;
}

} // namespace tips

} // namespace gui2
