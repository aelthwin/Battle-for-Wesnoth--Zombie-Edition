/* $Id: variable.hpp 48153 2011-01-01 15:57:50Z mordante $ */
/*
   Copyright (C) 2003 by David White <dave@whitevine.net>
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
#ifndef VARIABLE_H_INCLUDED
#define VARIABLE_H_INCLUDED

#include "config.hpp"

class game_state;
class unit_map;

/**
 * A variable-expanding proxy for the config class. This class roughly behaves
 * as a constant config object, but automatically expands variables.
 *
 */
class vconfig
{
private:
	vconfig();
	vconfig(const config* cfg, const config* cache_key);
public:
	vconfig(const vconfig& v);
	explicit vconfig(const config &cfg, bool is_volatile=false);
	~vconfig();

	static vconfig empty_vconfig(); // Valid to dereference. Contains nothing
	static vconfig unconstructed_vconfig(); // Must not be dereferenced

	vconfig& operator=(const vconfig& cfg);

	bool null() const { return cfg_ == NULL; }
	bool is_volatile() const { return cache_key_ != NULL; }
	const config& get_config() const { return *cfg_; }
	const config get_parsed_config() const;

	typedef std::vector<vconfig> child_list;
	child_list get_children(const std::string& key) const;
	vconfig child(const std::string& key) const;
	bool has_child(const std::string& key) const;

	/**
	 * Note: vconfig::operator[] returns const, and this should not be changed
	 * because vconfig is often used as a drop-in replacement for config, and
	 * this const will properly warn you if you try to assign vcfg["key"]=val;
	 *
	 * Note: The following construction is unsave:
	 * const std::string& temp = vcfg["foo"];
	 * This bind temp to a member of a temporary t_string. The lifetime of the
	 * temporary is not extended by this reference binding and the temporary's
	 * lifetime ends which causes UB. Instead use:
	 * const std::string temp = vcfg["foo"];
	 */
	const config::attribute_value operator[](const std::string &key) const
	{ return expand(key); }
	config::attribute_value expand(const std::string&) const; /** < Synonym for operator[] */
	bool has_attribute(const std::string& key) const { return cfg_->has_attribute(key); }
	bool empty() const { return (null() || cfg_->empty()); }

	struct all_children_iterator
	{
		struct pointer_proxy;

		typedef std::pair<std::string, vconfig> value_type;
		typedef std::forward_iterator_tag iterator_category;
		typedef int difference_type;
		typedef const pointer_proxy pointer;
		typedef const value_type reference;
		typedef config::all_children_iterator Itor;
		explicit all_children_iterator(const Itor &i, const config *cache_key = NULL);

		all_children_iterator& operator++();
		all_children_iterator  operator++(int);

		reference operator*() const;
		pointer operator->() const;

		std::string get_key() const;
		vconfig get_child() const;
		void disable_insertion() { inner_index_ = -1; }

		bool operator==(const all_children_iterator &i) const;
		bool operator!=(const all_children_iterator &i) const
		{ return !operator==(i); }

	private:
		Itor i_;
		int inner_index_;
		const config* cache_key_;
	};

	struct recursion_error : public config::error {
		recursion_error(const std::string& msg) : error(msg) {}
	};

	/** In-order iteration over all children. */
	all_children_iterator ordered_begin() const;
	all_children_iterator ordered_end() const;

private:
	const config* cfg_;
	const config* cache_key_;
};

struct vconfig::all_children_iterator::pointer_proxy
{
	value_type p;
	const value_type *operator->() const { return &p; }
};

namespace variable
{

/**
 * Used to clear the cache for variables.
 */
class manager
{
public:
	~manager();
};

}



class scoped_wml_variable
{
public:
	scoped_wml_variable(const std::string& var_name);
	virtual ~scoped_wml_variable();
	const std::string& name() const { return var_name_; }
	virtual void activate() = 0;
	config &store(const config &var_value = config());
	bool activated() const { return activated_; }
private:
	config previous_val_;
	const std::string var_name_;
	bool activated_;
};

class scoped_weapon_info : public scoped_wml_variable
{
public:
	scoped_weapon_info(const std::string& var_name, const config &data)
		: scoped_wml_variable(var_name), data_(data) {}
	void activate();
private:
	config const &data_;
};

class scoped_xy_unit : public scoped_wml_variable
{
public:
	scoped_xy_unit(const std::string& var_name, const int x, const int y, const unit_map& umap)
		: scoped_wml_variable(var_name), x_(x), y_(y), umap_(umap) {}
	void activate();
private:
	const int x_, y_;
	const unit_map& umap_;
};

class scoped_recall_unit : public scoped_wml_variable
{
public:
	scoped_recall_unit(const std::string& var_name, const std::string& player,
		unsigned int recall_index) : scoped_wml_variable(var_name), player_(player),
		recall_index_(recall_index) {}
	void activate();
private:
	const std::string player_;
	unsigned int recall_index_;
};

/** Information on a WML variable. */
struct variable_info
{
	typedef config::child_itors array_range;

	/**
	 * TYPE: the correct variable type should be decided by the user of the info structure
	 * Note: an Array can also be considered a Container, since index 0 will be used by default
	 */
	enum TYPE { TYPE_SCALAR,    //a Scalar variable resolves to a t_string attribute of *vars
	            TYPE_ARRAY,     //an Array variable is a series of Containers
	            TYPE_CONTAINER, //a Container is a specific index of an Array (contains Scalars)
	            TYPE_UNSPECIFIED };

	variable_info(const std::string& varname, bool force_valid=true,
		TYPE validation_type=TYPE_UNSPECIFIED);

	TYPE vartype; //default is TYPE_UNSPECIFIED
	bool is_valid;
	std::string key; //the name of the internal attribute or child
	bool explicit_index; //true if query ended in [...] specifier
	size_t index; //the index of the child
	config *vars; //the containing node in game_state::variables

	/**
	 * Results: after deciding the desired type, these methods can retrieve the result
	 * Note: first you should force_valid or check is_valid, otherwise these may fail
	 */
	config::attribute_value &as_scalar();
	config& as_container();
	array_range as_array(); //range may be empty
};

#endif
