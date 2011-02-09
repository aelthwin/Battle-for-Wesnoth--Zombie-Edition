/* $Id: stub.hpp 34520 2009-04-05 03:00:11Z shadowmaster $ */
/*
   By Ignacio R. Morelle <shadowm2006@gmail.com>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This source code file is not copyrighted.
*/

#include <boost/current_function.hpp>
#define STUB() \
	std::cerr << "Ouch[stub]: " << BOOST_CURRENT_FUNCTION << " [at " << __FILE__ << ":" << __LINE__ << "]\n"
