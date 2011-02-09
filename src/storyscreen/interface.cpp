/* $Id: interface.cpp 48153 2011-01-01 15:57:50Z mordante $ */
/*
   Copyright (C) 2009 - 2011 by Ignacio R. Morelle <shadowm2006@gmail.com>
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
 * Storyscreen controller (wrapper interface).
 */

#include "global.hpp"
#include "foreach.hpp"
#include "variable.hpp"

#include "storyscreen/interface.hpp"
#include "storyscreen/controller.hpp"

#include "display.hpp"
#include "game_events.hpp"
#include "gettext.hpp"
#include "intro.hpp"
#include "language.hpp"
#include "log.hpp"
#include "sound.hpp"
#include "text.hpp"

static lg::log_domain log_engine("engine");
#define LOG_NG LOG_STREAM(info, log_engine)

// TODO: remove when completed
#include "stub.hpp"

namespace {

	int count_segments(const config::const_child_itors &story)
	{
		config::const_child_iterator itor = story.first;
		int count = 0;
		while(itor != story.second) {
			++itor;
			++count;
		}
		return count;
	}
} // end anonymous namespace

void show_story(display &disp, const std::string &scenario_name,
	const config::const_child_itors &story)
{
	const int total_segments = count_segments(story);
	int segment_count = 0;
	config::const_child_iterator itor = story.first;
	storyscreen::START_POSITION startpos = storyscreen::START_BEGINNING;
	while (itor != story.second)
	{
		storyscreen::controller ctl(disp, vconfig(*itor, true),
			scenario_name, segment_count, total_segments);
		storyscreen::STORY_RESULT result = ctl.show(startpos);

		switch(result) {
		case storyscreen::NEXT:
			if(itor != story.second) {
				++itor;
				++segment_count;
				startpos = storyscreen::START_BEGINNING;
			}
			break;
		case storyscreen::BACK:
			if(itor != story.first) {
				--itor;
				--segment_count;
				startpos = storyscreen::START_END;
			}
			break;
		case storyscreen::QUIT:
			return;
		}
	}
	return;
}

void show_endscreen(display& /*disp*/, const t_string& /*text*/, unsigned int /*duration*/)
{
	STUB();
	LOG_NG << "show_endscreen() invoked...\n";

	config story_cfg;

	// FIXME: stub!

	LOG_NG << "show_endscreen() completed...\n";
}
