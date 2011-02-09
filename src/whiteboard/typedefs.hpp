/* $Id: typedefs.hpp 48153 2011-01-01 15:57:50Z mordante $ */
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
 * Contains typedefs for the whiteboard.
 */


#ifndef WB_TYPEDEFS_HPP_
#define WB_TYPEDEFS_HPP_

#include "log.hpp"
static lg::log_domain log_whiteboard("whiteboard");
#define ERR_WB LOG_STREAM_INDENT(err, log_whiteboard)
#define WRN_WB LOG_STREAM_INDENT(warn, log_whiteboard)
#define LOG_WB LOG_STREAM_INDENT(info, log_whiteboard)
#define DBG_WB LOG_STREAM_INDENT(debug, log_whiteboard)

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <deque>
#include <ostream> //used for << operators

class arrow;
struct map_location; //not used in the typedefs, saves a few forward declarations
class unit;
class unit_map; //not used in the typedefs, saves a few forward declarations

namespace pathfind {
	struct plain_route;
	struct marked_route;
}

namespace wb {

class action;
class move;
class attack;
class recall;
class recruit;
class side_actions;

typedef boost::shared_ptr<arrow> arrow_ptr;
typedef boost::shared_ptr<unit> fake_unit_ptr;

typedef boost::shared_ptr<action> action_ptr;
typedef boost::shared_ptr<action const> action_const_ptr;
typedef boost::weak_ptr<action> weak_action_ptr;
typedef std::deque<action_ptr> action_queue;
typedef boost::shared_ptr<side_actions> side_actions_ptr;

typedef boost::shared_ptr<move> move_ptr;
typedef boost::shared_ptr<move const> move_const_ptr;
typedef boost::shared_ptr<attack> attack_ptr;
typedef boost::shared_ptr<attack const> attack_const_ptr;
typedef boost::shared_ptr<recruit> recruit_ptr;
typedef boost::shared_ptr<recruit const> recruit_const_ptr;
typedef boost::shared_ptr<recall> recall_ptr;
typedef boost::shared_ptr<recall const> recall_const_ptr;

} // end namespace wb

#endif /* WB_TYPEDEFS_HPP_ */
