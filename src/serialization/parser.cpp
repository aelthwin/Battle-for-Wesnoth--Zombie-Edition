/* $Id: parser.cpp 48153 2011-01-01 15:57:50Z mordante $ */
/*
   Copyright (C) 2003 by David White <dave@whitevine.net>
   Copyright (C) 2005 by Guillaume Melquiond <guillaume.melquiond@gmail.com>
   Copyright (C) 2005 - 2011 by Philippe Plantier <ayin@anathas.org>
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
 * Read/Write & analyze WML- and config-files.
 */


#include "serialization/parser.hpp"

#include "config.hpp"
#include "log.hpp"
#include "gettext.hpp"
#include "loadscreen.hpp"
#include "wesconfig.h"
#include "serialization/preprocessor.hpp"
#include "serialization/tokenizer.hpp"
#include "serialization/string_utils.hpp"
#include "foreach.hpp"

#include <stack>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/variant.hpp>

static lg::log_domain log_config("config");
#define ERR_CF LOG_STREAM(err, log_config)
#define WRN_CF LOG_STREAM(warn, log_config)
#define LOG_CF LOG_STREAM(info, log_config)

static const size_t max_recursion_levels = 1000;

namespace {

class parser
{
	parser();
	parser(const parser&);
	parser& operator=(const parser&);
public:
	parser(config& cfg, std::istream& in);
	~parser();
	void operator()();

private:
	void parse_element();
	void parse_variable();
	std::string lineno_string(utils::string_map &map, std::string const &lineno,
		const char *error_string);
	void error(const std::string& message);

	config& cfg_;
	tokenizer *tok_;

	struct element {
		element(config *cfg, std::string const &name,
			int start_line = 0, const std::string &file = "") :
			cfg(cfg), name(name), start_line(start_line), file(file)
		{}

		config* cfg;
		std::string name;
		int start_line;
		std::string file;
	};

	std::stack<element> elements;
};

parser::parser(config &cfg, std::istream &in) :
		cfg_(cfg),
		tok_(new tokenizer(in)),
		elements()
{
}


parser::~parser()
{
	delete tok_;
}

void parser::operator()()
{
	cfg_.clear();
	elements.push(element(&cfg_, ""));

	do {
		tok_->next_token();

		switch(tok_->current_token().type) {
		case token::LF:
			continue;
		case '[':
			parse_element();
			break;
		case token::STRING:
			parse_variable();
			break;
		default:
			if (static_cast<unsigned char>(tok_->current_token().value[0]) == 0xEF &&
			    static_cast<unsigned char>(tok_->next_token().value[0])    == 0xBB &&
			    static_cast<unsigned char>(tok_->next_token().value[0])    == 0xBF)
			{
				ERR_CF << "Skipping over a utf8 BOM\n";
			} else {
				error(_("Unexpected characters at line start"));
			}
			break;
		case token::END:
			break;
		}
		loadscreen::increment_progress();
	} while (tok_->current_token().type != token::END);

	// The main element should be there. If it is not, this is a parser error.
	assert(!elements.empty());

	if(elements.size() != 1) {
		utils::string_map i18n_symbols;
		i18n_symbols["tag"] = elements.top().name;
		std::stringstream ss;
		ss << elements.top().start_line << " " << elements.top().file;
		error(lineno_string(i18n_symbols, ss.str(),
				N_("Missing closing tag for tag $tag at $pos")));
	}
}

void parser::parse_element()
{
	tok_->next_token();
	std::string elname;
	config* current_element = NULL;

	switch(tok_->current_token().type) {
	case token::STRING: // [element]
		elname = tok_->current_token().value;
		if (tok_->next_token().type != ']')
			error(_("Unterminated [element] tag"));

		// Add the element
		current_element = &(elements.top().cfg->add_child(elname));
		elements.push(element(current_element, elname, tok_->get_start_line(), tok_->get_file()));
		break;

	case '+': // [+element]
		if (tok_->next_token().type != token::STRING)
			error(_("Invalid tag name"));
		elname = tok_->current_token().value;
		if (tok_->next_token().type != ']')
			error(_("Unterminated [+element] tag"));

		// Find the last child of the current element whose name is
		// element
		if (config &c = elements.top().cfg->child(elname, -1)) {
			current_element = &c;
		} else {
			current_element = &elements.top().cfg->add_child(elname);
		}
		elements.push(element(current_element, elname, tok_->get_start_line(), tok_->get_file()));
		break;

	case '/': // [/element]
		if(tok_->next_token().type != token::STRING)
			error(_("Invalid closing tag name"));
		elname = tok_->current_token().value;
		if(tok_->next_token().type != ']')
			error(_("Unterminated closing tag"));
		if(elements.size() <= 1)
			error(_("Unexpected closing tag"));
		if(elname != elements.top().name) {
			utils::string_map i18n_symbols;
			i18n_symbols["tag1"] = elements.top().name;
			i18n_symbols["tag2"] = elname;
			std::stringstream ss;
			ss << elements.top().start_line << " " << elements.top().file;
			error(lineno_string(i18n_symbols, ss.str(),
					N_("Found invalid closing tag $tag2 for tag $tag1 (opened at $pos)")));
		}

		elements.pop();
		break;
	default:
		error(_("Invalid tag name"));
	}
}

void parser::parse_variable()
{
	config& cfg = *elements.top().cfg;
	std::vector<std::string> variables;
	variables.push_back("");

	while (tok_->current_token().type != '=') {
		switch(tok_->current_token().type) {
		case token::STRING:
			if(!variables.back().empty())
				variables.back() += ' ';
			variables.back() += tok_->current_token().value;
			break;
		case ',':
			if(variables.back().empty()) {
				error(_("Empty variable name"));
			} else {
				variables.push_back("");
			}
			break;
		default:
			error(_("Unexpected characters after variable name (expected , or =)"));
			break;
		}
		tok_->next_token();
	}
	if(variables.back().empty())
		error(_("Empty variable name"));

	t_string_base buffer;

	std::vector<std::string>::const_iterator curvar = variables.begin();

	bool ignore_next_newlines = false, previous_string = false;
	while(1) {
		tok_->next_token();
		assert(curvar != variables.end());

		switch (tok_->current_token().type) {
		case ',':
			if ((curvar+1) != variables.end()) {
				if (buffer.translatable())
					cfg[*curvar] = t_string(buffer);
				else
					cfg[*curvar] = buffer.value();
				buffer = t_string_base();
				++curvar;
			} else {
				buffer += ",";
			}
			break;
		case '_':
			tok_->next_token();
			switch (tok_->current_token().type) {
			case token::UNTERMINATED_QSTRING:
				error(_("Unterminated quoted string"));
				break;
			case token::QSTRING:
				buffer += t_string_base(tok_->current_token().value, tok_->textdomain());
				break;
			default:
				buffer += "_";
				buffer += tok_->current_token().value;
				break;
			case token::END:
			case token::LF:
				buffer += "_";
				goto finish;
			}
			break;
		case '+':
			ignore_next_newlines = true;
			continue;
		case token::STRING:
			if (previous_string) buffer += " ";
			//nobreak
		default:
			buffer += tok_->current_token().value;
			break;
		case token::QSTRING:
			buffer += tok_->current_token().value;
			break;
		case token::UNTERMINATED_QSTRING:
			error(_("Unterminated quoted string"));
			break;
		case token::LF:
			if (ignore_next_newlines) continue;
			//nobreak
		case token::END:
			goto finish;
		}

		previous_string = tok_->current_token().type == token::STRING;
		ignore_next_newlines = false;
	}

	finish:
	if (buffer.translatable())
		cfg[*curvar] = t_string(buffer);
	else
		cfg[*curvar] = buffer.value();
	while (++curvar != variables.end()) {
		cfg[*curvar] = "";
	}
}

/**
 * This function is crap. Don't use it on a string_map with prefixes.
 */
std::string parser::lineno_string(utils::string_map &i18n_symbols,
	std::string const &lineno, const char *error_string)
{
	i18n_symbols["pos"] = ::lineno_string(lineno);
	std::string result = _(error_string);
	foreach(utils::string_map::value_type& var, i18n_symbols)
		boost::algorithm::replace_all(result, std::string("$") + var.first, std::string(var.second));
	return result;
}

void parser::error(const std::string& error_type)
{
	utils::string_map i18n_symbols;
	i18n_symbols["error"] = error_type;
	i18n_symbols["value"] = tok_->current_token().value;
	std::stringstream ss;
	ss << tok_->get_start_line() << " " << tok_->get_file();
#ifdef DEBUG
	i18n_symbols["previous_value"] = tok_->previous_token().value;
	throw config::error(
		lineno_string(i18n_symbols, ss.str(),
		              N_("$error, value '$value', previous '$previous_value' at $pos")));
#else
	throw config::error(
		lineno_string(i18n_symbols, ss.str(),
		              N_("$error, value '$value' at $pos")));
#endif
}

} // end anon namespace

void read(config &cfg, std::istream &in)
{
	parser(cfg, in)();
}

void read(config &cfg, std::string &in)
{
	std::istringstream ss(in);
	parser(cfg, ss)();
}

void read_gz(config &cfg, std::istream &file)
{
	//an empty gzip file seems to confuse boost on msvc
	//so return early if this is the case
	if (file.peek() == EOF) {
		return;
	}
	boost::iostreams::filtering_stream<boost::iostreams::input> filter;
	filter.push(boost::iostreams::gzip_decompressor());
	filter.push(file);

	parser(cfg, filter)();
}

static std::string escaped_string(const std::string &value)
{
	std::string res = value;
	std::string::size_type pos = 0;
	while ((pos = res.find('"', pos)) != std::string::npos) {
		res.insert(pos + 1, 1, '"');
		pos += 2;
	}
	return res;
}

struct write_key_val_visitor : boost::static_visitor<void>
{
	std::ostream &out_;
	unsigned level_;
	std::string &textdomain_;
	const std::string &key_;

	write_key_val_visitor(std::ostream &out, unsigned level,
		std::string &textdomain, const std::string &key)
		: out_(out), level_(level), textdomain_(textdomain), key_(key)
	{}

	void operator()(boost::blank const &) const
	{ out_ << "\"\""; }
	void operator()(bool b) const
	{ out_ << (b ? "yes" : "no"); }
	void operator()(double d) const
	{ int i = d; if (d == i) out_ << i; else out_ << d; }
	void operator()(std::string const &s) const
	{ out_ << '"' << escaped_string(s) << '"'; }
	void operator()(t_string const &s) const;
};

/**
 * Writes all the parts of a translatable string.
 * @note If the first part is translatable and in the wrong textdomain,
 *       the textdomain change has to happen before the attribute name.
 *       That is the reason for not outputting the key beforehand and
 *       letting this function do it.
 */
void write_key_val_visitor::operator()(t_string const &value) const
{
	bool first = true;

	for (t_string::walker w(value); !w.eos(); w.next())
	{
		std::string part(w.begin(), w.end());

		if (!first)
			out_ << " +\n";

		if (w.translatable() && w.textdomain() != textdomain_) {
			textdomain_ = w.textdomain();
			out_ << "#textdomain " << textdomain_ << '\n';
		}

		for (unsigned i = 0; i < level_; ++i) out_ << '\t';

		if (first)
			out_ << key_ << '=';
		else
			out_ << '\t';

		if (w.translatable())
			out_ << '_';

		out_ << '"' << escaped_string(part) << '"';
		first = false;
	}
}

void write_key_val(std::ostream &out, const std::string &key,
	const config::attribute_value &value, unsigned level,
	std::string& textdomain)
{
	if (!boost::get<t_string const>(&value.value)) {
		for (unsigned i = 0; i < level; ++i) out << '\t';
		out << key << '=';
	}
	boost::apply_visitor(write_key_val_visitor(out, level, textdomain, key), value.value);
	out << '\n';
}

void write_open_child(std::ostream &out, const std::string &child, unsigned int level)
{
	out << std::string(level, '\t') << '[' << child << "]\n";
}

void write_close_child(std::ostream &out, const std::string &child, unsigned int level)
{
	out << std::string(level, '\t') << "[/" << child << "]\n";
}

static void write_internal(config const &cfg, std::ostream &out, std::string& textdomain, size_t tab = 0)
{
	if (tab > max_recursion_levels)
		throw config::error("Too many recursion levels in config write");

	foreach (const config::attribute &i, cfg.attribute_range()) {
		write_key_val(out, i.first, i.second, tab, textdomain);
	}

	foreach (const config::any_child &item, cfg.all_children_range())
	{
		write_open_child(out, item.key, tab);
		write_internal(item.cfg, out, textdomain, tab + 1);
		write_close_child(out, item.key, tab);
	}
}

void write(std::ostream &out, config const &cfg, unsigned int level)
{
	std::string textdomain = PACKAGE;
	write_internal(cfg, out, textdomain, level);
}

