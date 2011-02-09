/* $Id: test_util.cpp 48153 2011-01-01 15:57:50Z mordante $ */
/*
   Copyright (C) 2007 - 2011 by Karol Nowak <grywacz@gmail.com>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#define GETTEXT_DOMAIN "wesnoth-test"

#include <boost/test/unit_test.hpp>

#include "util.hpp"

BOOST_AUTO_TEST_SUITE( util )

BOOST_AUTO_TEST_CASE( test_lexical_cast )
{
	/* First check if lexical_cast returns correct results for correct args */
	int result = lexical_cast<int, const std::string&>(std::string("1"));
	BOOST_CHECK( result == 1 );

	int result2 = lexical_cast<int, const char*>("2");
	BOOST_CHECK( result2 == 2 );

	/* Check that an exception is thrown when an invalid argument is passed */
	try {
		lexical_cast<int, const std::string&>(std::string("iddqd"));

		/* A bad_lexical_cast should have been thrown already */
		BOOST_CHECK( false );
	}
	catch( const bad_lexical_cast &e) {
		// Don't do anything, we succeeded.
	}

	try {
		lexical_cast<int, const char*>("idkfa");

		/* A bad_lexical_cast should have been thrown already */
		BOOST_CHECK( false );
	}
	catch( const bad_lexical_cast &e) {
		// Don't do anything, we succeeded.
	}
}

BOOST_AUTO_TEST_CASE( test_lexical_cast_default )
{
	/* First check if it works with correct values */
	int result = lexical_cast_default<int, const std::string&>(std::string("1"));
	BOOST_CHECK( result == 1 );

	int result2 = lexical_cast_default<int, const char*>("2");
	BOOST_CHECK( result2 == 2 );

	double result3 = lexical_cast_default<double, const std::string&>(std::string("0.5"));
	BOOST_CHECK( result3 >= 0.499 && result3 <= 0.511 );

	/* Check if default is returned when argument is empty/invalid */
	int result4 = lexical_cast_default<int, const std::string&>(std::string(), 4);
	BOOST_CHECK( result4 == 4 );

	int result5 = lexical_cast_default<int, const char*>("", 5);
	BOOST_CHECK( result5 == 5 );

	double result6 = lexical_cast_default<double, const std::string&>(std::string(), 0.5);
	BOOST_CHECK( result6 >= 0.499 && result6 <= 0.511 );
}

/* vim: set ts=4 sw=4: */

BOOST_AUTO_TEST_SUITE_END()
