/* $Id: binary_or_text.cpp 48153 2011-01-01 15:57:50Z mordante $ */
/*
   Copyright (C) 2003 by David White <dave@whitevine.net>
   Copyright (C) 2005 - 2011 by Guillaume Melquiond <guillaume.melquiond@gmail.com>
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
 * Read/Write file in binary (compressed) or text-format (uncompressed).
 */

#include "global.hpp"

#include "binary_or_text.hpp"
#include "config.hpp"
#include "log.hpp"
#include "wesconfig.h"
#include "serialization/parser.hpp"


#include <boost/iostreams/filter/gzip.hpp>

static lg::log_domain log_config("config");
#define ERR_CF LOG_STREAM(err, log_config)

config_writer::config_writer(
	std::ostream &out, bool compress, int level) :
		filter_(),
		out_ptr_(compress ? &filter_ : &out), //ternary indirection creates a temporary
		out_(*out_ptr_), //now MSVC will allow binding to the reference member
		compress_(compress),
		level_(0),
		textdomain_(PACKAGE)
{
	if(compress_) {
		if (level >=0)
			filter_.push(boost::iostreams::gzip_compressor(boost::iostreams::gzip_params(level)));
		else
			filter_.push(boost::iostreams::gzip_compressor(boost::iostreams::gzip_params()));

		filter_.push(out);
	}
}

config_writer::~config_writer()
{
}

void config_writer::write(const config &cfg)
{
	::write(out_, cfg, level_);
}

void config_writer::write_child(const std::string &key, const config &cfg)
{
	open_child(key);
	::write(out_, cfg, level_);
	close_child(key);
}

void config_writer::write_key_val(const std::string &key, const std::string &value)
{
	config::attribute_value v;
	v = value;
	::write_key_val(out_, key, v, level_, textdomain_);
}

void config_writer::open_child(const std::string &key)
{
	::write_open_child(out_, key, level_++);
}

void config_writer::close_child(const std::string &key)
{
	::write_close_child(out_, key, --level_);
}

bool config_writer::good() const
{
	return out_.good();
}

