/* $Id: wml_exception.cpp 48250 2011-01-10 20:38:34Z mordante $ */
/*
   Copyright (C) 2007 - 2011 by Mark de Wever <koraq@xs4all.nl>
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
 *  @file
 *  Implementation for wml_exception.hpp.
 */

#define GETTEXT_DOMAIN "wesnoth-lib"

#include "global.hpp"
#include "wml_exception.hpp"

#include "display.hpp"
#include "gettext.hpp"
#include "gui/dialogs/message.hpp"
#include "formula_string_utils.hpp"
#include "log.hpp"

void wml_exception(
		  const char* cond
		, const char* file
		, int line
		, const char *function
		, const std::string& message
		, const std::string& dev_message)
{
	std::ostringstream sstr;
	sstr << "Condition '" << cond << "' failed at "
		<< file << ":" << line << " in function '" << function << "'.";

	if(!dev_message.empty()) {
		sstr << " Extra development information: " << dev_message;
	}

	throw twml_exception(message, sstr.str());
}

void twml_exception::show(display &disp)
{
	std::ostringstream sstr;

	// The extra spaces between the \n are needed, otherwise the dialog doesn't show
	// an empty line.
	sstr << _("An error due to possibly invalid WML occurred\nThe error message is :")
		<< "\n" << user_message << "\n \n"
		<< _("When reporting the bug please include the following error message :")
		<< "\n" << dev_message;

	gui2::show_error_message(disp.video(), sstr.str());
}

std::string missing_mandatory_wml_key(const std::string &section, const std::string &key,
		const std::string& primary_key, const std::string& primary_value)
{
	utils::string_map symbols;
	symbols["section"] = section;
	symbols["key"] = key;
	if(!primary_key.empty()) {
		assert(!primary_value.empty());

		symbols["primary_key"] = primary_key;
		symbols["primary_value"] = primary_value;

		return vgettext("In section '[$section|]' where '$primary_key| = "
			"$primary_value' the mandatory key '$key|' isn't set.", symbols);
	} else {
		return vgettext("In section '[$section|]' the "
			"mandatory key '$key|' isn't set.", symbols);
	}
}

std::string deprecate_wml_key_warning(
		  const std::string& key
		, const std::string& removal_version)
{
	assert(!key.empty());
	assert(!removal_version.empty());

	utils::string_map symbols;
	symbols["key"] = key;
	symbols["removal_version"] = removal_version;

	return vgettext("The key '$key' is deprecated and support "
			"will be removed in version $removal_version.", symbols);
}

std::string deprecated_renamed_wml_key_warning(
		  const std::string& deprecated_key
		, const std::string& key
		, const std::string& removal_version)
{
	assert(!deprecated_key.empty());
	assert(!key.empty());
	assert(!removal_version.empty());

	utils::string_map symbols;
	symbols["deprecated_key"] = deprecated_key;
	symbols["key"] = key;
	symbols["removal_version"] = removal_version;

	return vgettext(
			  "The key '$deprecated_key' has been renamed to '$key'. "
				"Support for '$deprecated_key' will be removed in version "
				"$removal_version."
			, symbols);
}

const config::attribute_value& get_renamed_config_attribute(
		  const config& cfg
		, const std::string& deprecated_key
		, const std::string& key
		, const std::string& removal_version)
{

	const config::attribute_value* result = cfg.get(key);
	if(result) {
		return *result;
	}

	result = cfg.get(deprecated_key);
	if(result) {
		lg::wml_error
			<< deprecated_renamed_wml_key_warning(
				  deprecated_key
				, key
				, removal_version)
			<< '\n';

		return *result;
	}

	static const config::attribute_value empty_attribute;
	return empty_attribute;
}

