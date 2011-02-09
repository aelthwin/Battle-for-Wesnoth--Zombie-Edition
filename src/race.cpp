/* $Id: race.cpp 48153 2011-01-01 15:57:50Z mordante $ */
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

/**
 *  @file
 *  Generate race-specific unit-names.
 */

#include "global.hpp"

#include "race.hpp"

#include "log.hpp"
#include "random.hpp"
#include "simple_rng.hpp"

static const config &empty_traits() {
		static config cfg;
		return cfg;
}

static const config &empty_topics() {
  		static config cfg;
		return cfg;
}

static void add_prefixes(const wide_string& str, size_t length, markov_prefix_map& res)
{
	for(size_t i = 0; i <= str.size(); ++i) {
		const size_t start = i > length ? i - length : 0;
		const wide_string key(str.begin() + start, str.begin() + i);
		const wchar_t c = i != str.size() ? str[i] : 0;
		res[key].push_back(c);
	}
}

static markov_prefix_map markov_prefixes(const std::vector<std::string>& items, size_t length)
{
	markov_prefix_map res;

	for(std::vector<std::string>::const_iterator i = items.begin(); i != items.end(); ++i) {
		add_prefixes(utils::string_to_wstring(*i),length,res);
	}

	return res;
}

static wide_string markov_generate_name(const markov_prefix_map& prefixes,
	size_t chain_size, size_t max_len, rand_rng::simple_rng* rng)
{
	if(chain_size == 0)
		return wide_string();

	wide_string prefix, res;

	// Since this function is called in the name description in a MP game it
	// uses the local locale. The locale between players can be different and
	// thus the markov_prefix_map can be different. This resulted in
	// get_random() getting called a different number of times for the
	// generation in different locales (due to the bail out at 'if(c == 0)').
	//
	// This causes a problem since the random state is no longer in sync. The
	// following calls to get_random() return different results, which caused
	// traits to be different. To avoid that problem we call get_random()
	// the maximum number of times and store the result in a lookup table.
	std::vector<int> random(max_len);
	size_t j = 0;
	for(; j < max_len; ++j) {
		random[j] = rng ? rng->get_next_random() : get_random_nocheck();
	}

	j = 0;
	while(res.size() < max_len) {
		const markov_prefix_map::const_iterator i = prefixes.find(prefix);
		if(i == prefixes.end() || i->second.empty()) {
			return res;
		}

		const wchar_t c = i->second[random[j++]%i->second.size()];
		if(c == 0) {
			return res;
		}

		res.resize(res.size()+1);
		res[res.size()-1] = c;
		prefix.resize(prefix.size()+1);
		prefix[prefix.size()-1] = c;
		while(prefix.size() > chain_size) {
			prefix.erase(prefix.begin());
		}
	}

	// Getting here means that the maximum length was reached when
	// generating the name, hence the ending of the name has to be
	// made valid. Otherwise weird names like Unárierini- and
	// Thramboril-G may occur.

	// Strip characters from the end until the last prefix of the
	// name has end-of-string as a possible next character in the
	// markov prefix map. If no valid ending is found, use the
	// originally generated name.
	wide_string originalRes = res;
	int prefixLen;
	while(!res.empty()) {
		prefixLen = chain_size < res.size() ? chain_size : res.size();
		prefix = wide_string(res.end() - prefixLen, res.end());

		const markov_prefix_map::const_iterator i = prefixes.find(prefix);
		if (i == prefixes.end() || i->second.empty()) {
			return res;
		}
		if (std::find(i->second.begin(), i->second.end(), static_cast<wchar_t>(0))
				!= i->second.end()) {
			// This ending is valid.
			return res;
		}
		// The current ending is invalid, remove the last character
		// and retry.
		res.pop_back();
	}
	// No valid ending at all could be found. This generally should
	// not happen, unless the chain length is very long or the
	// maximum length is very small. Return the originally generated
	// name, it's not much we can do about it.
	return originalRes;
}

unit_race::unit_race() :
		id_(),
		plural_name_(),
		description_(),
		ntraits_(0),
		chain_size_(0),
		traits_(empty_traits().child_range("trait")),
		topics_(empty_topics().child_range("topic")),
		global_traits_(true)
{
		name_[MALE] = "";
		name_[FEMALE] = "";
}

unit_race::unit_race(const config& cfg) :
		id_(cfg["id"]),
		plural_name_(cfg["plural_name"].t_str()),
		description_(cfg["description"].t_str()),
		ntraits_(cfg["num_traits"]),
		chain_size_(cfg["markov_chain_size"]),
		traits_(cfg.child_range("trait")),
		topics_(cfg.child_range("topic")),
		global_traits_(!cfg["ignore_global_traits"].to_bool())

{
	if (id_.empty()) {
		lg::wml_error << "[race] '" << cfg["name"] << "' is missing an id field.";
	}
	if (plural_name_.empty()) {
		lg::wml_error << "[race] '" << cfg["name"] << "' is missing a plural_name field.";
		plural_name_ = (cfg["name"]);
	}
	// use "name" if "male_name" or "female_name" aren't available
	name_[MALE] = cfg["male_name"];
	if(name_[MALE].empty()) {
		name_[MALE] = (cfg["name"]);
	}
	name_[FEMALE] = cfg["female_name"];
	if(name_[FEMALE].empty()) {
		name_[FEMALE] = (cfg["name"]);
	}

	if(chain_size_ <= 0)
		chain_size_ = 2;

	//std::vector<std::string> names = ;
	next_[MALE] = markov_prefixes(utils::split(cfg["male_names"]), chain_size_);
	next_[FEMALE] = markov_prefixes(utils::split(cfg["female_names"]), chain_size_);
}

std::string unit_race::generate_name(
		unit_race::GENDER gender, rand_rng::simple_rng* rng) const
{
	return utils::wstring_to_string(
		markov_generate_name(next_[gender], chain_size_, 12, rng));
}

bool unit_race::uses_global_traits() const
{
	return global_traits_;
}

const config::const_child_itors &unit_race::additional_traits() const
{
	return traits_;
}

const config::const_child_itors &unit_race::additional_topics() const
{
  return topics_;
}

unsigned int unit_race::num_traits() const { return ntraits_; }

std::string const& gender_string(unit_race::GENDER gender) {
	static const std::string female_string = "female";
	static const std::string male_string = "male";
	switch(gender) {
	case unit_race::FEMALE:
		return female_string;
	default:
	case unit_race::MALE:
		return male_string;
	}
}

unit_race::GENDER string_gender(const std::string& str, unit_race::GENDER def) {
	if(str == gender_string(unit_race::MALE)) {
		return unit_race::MALE;
	} else if(str == gender_string(unit_race::FEMALE)) {
		return unit_race::FEMALE;
	}
	return def;
}
