/*
 * orthodocs unit tests
 *
 * Copyright © 2022 Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * ODOX is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ODOX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ODOX.  If not, see <http://www.gnu.org/licenses/>.
 */

#define BOOST_TEST_MODULE orthodocs unit tests
#include <boost/test/included/unit_test.hpp>

#include "scad/extension.h"

using namespace boost::unit_test;
using namespace std;

BOOST_AUTO_TEST_CASE( factories ) {
  language::Extension *language=nullptr;
  BOOST_REQUIRE_THROW(language=language::Extension::factory("unexistent_extension"),std::domain_error);
  BOOST_REQUIRE(language = language::Extension::factory("scad"));
  BOOST_REQUIRE(dynamic_cast<scad::Extension*>(language));
  writer::Extension::Dictionary dict;
  writer::Extension *writer=nullptr;
  BOOST_REQUIRE_THROW(writer=writer::Extension::factory("unexistent_extension",dict,language),std::domain_error);
  BOOST_REQUIRE(writer=writer::Extension::factory("markdown",dict,language));
  BOOST_REQUIRE(dynamic_cast<writer::Extension*>(writer));
}

BOOST_AUTO_TEST_CASE( utilities ) {
  string s{" \t I'm a string with spaces \r\n"};
  BOOST_REQUIRE(trim(s)==s.substr(3).substr(0,24));
}
