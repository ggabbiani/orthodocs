/*
 * orthodocs unit tests
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/**
 * Static library usage variant as from [Usage variants - 1.81.0](https://www.boost.org/doc/libs/1_81_0/libs/test/doc/html/boost_test/usage_variants.html)
 */
#define BOOST_TEST_MODULE orthodocs unit tests
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <commons/config.h>
#include <scad/extension.h>

#include <cstdlib>
#include <filesystem>

using namespace boost::unit_test;
using namespace std;
namespace fs=std::filesystem;

BOOST_AUTO_TEST_CASE( factories ) {
  language::Extension *language=nullptr;
  BOOST_REQUIRE_THROW(language=language::Extension::factory("nonexistent_extension"),std::domain_error);
  BOOST_REQUIRE(language = language::Extension::factory("scad"));
  BOOST_REQUIRE(dynamic_cast<scad::Extension*>(language));
  writer::Extension::Dictionary dict;
  writer::Extension *writer=nullptr;
  BOOST_REQUIRE_THROW(writer=writer::Extension::factory("nonexistent_extension",dict,language),std::domain_error);
  BOOST_REQUIRE(writer=writer::Extension::factory("markdown",dict,language));
  BOOST_REQUIRE(dynamic_cast<writer::Extension*>(writer));
}

BOOST_AUTO_TEST_CASE( utilities ) {
  string s{" \t I'm a string with spaces \r\n"};
  BOOST_REQUIRE(trim(s)==s.substr(3).substr(0,24));
}

BOOST_AUTO_TEST_CASE( ut_datadir ) {
  auto result = Option::dataDir();
  if (const char* p = getenv(ODOX_DATADIR_ENV))
    BOOST_REQUIRE(result==fs::path(p));
  else
    BOOST_REQUIRE(result==fs::path(ODOX_INSTALL_FULL_DATADIR));
  cout << __PRETTY_FUNCTION__ << ": Option::dataDir()==" << result << endl;
}

BOOST_AUTO_TEST_CASE( ut_custom_datadir ) {
  fs::path user_defined("user/defined/datadir");
  Option::_data_dir = user_defined;
  auto result = Option::dataDir();
  BOOST_REQUIRE(result==user_defined);
  cout << __PRETTY_FUNCTION__ << ": Option::dataDir()==" << result << endl;
}
