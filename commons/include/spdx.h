#pragma once
/*
 * spdx definition file
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "annotation.h"
#include "spdx_config.h"
#include "utils.h"

#include <SPDXParserBaseListener.h>

#include <nlohmann/json.hpp>

#include <ctime>
#include <iomanip>
#include <stdexcept>

namespace spdx {

/**
 * SPDX lists in JSON format share same semantic but different labels.
 */
enum class LabelType {
  name,     // extended name
  id,       // compact id
  list,     // item list
  version,  // global DB version
  date,     // global DB release date
  details   // details url
};

/**
 * Error raised during JSON parse of the license and exception DBs
 */
struct StatusError : public std::logic_error {
  // inherit parent class constructors
  using std::logic_error::logic_error;
};

namespace license {

/**
 * this interface provides a JSON label for SPDX license DB.
 */
struct LabelSet {
  const std::string& operator [] (LabelType type) const;
};

}

namespace exception {

/**
 * this interface provides a JSON label for SPDX license exception DB.
 */
struct LabelSet {
  const std::string& operator [] (LabelType type) const;
};

}

/**
 * One parametric class both the license and exception DBs.
 *
 * @tparam LABELSET defines the LabelSet to be used.
 */
template <typename LABELSET>
class List {
public:
  class Item {
  public:
    const std::string &name() const {return _name;}
    const std::string &id() const {return _id;}
    const std::string &url() const {return _url;}
    /**
     * If the passed key is recognized sets the corresponding attribute to value.
     *
     * Returns true if the key is recognized, false otherwise.
     *
     * Currently recognized key types:
     *
     * - LabelType::id
     * - LabelType::name
     */
    bool set(const std::string_view &key, std::string &value) {
      if (key==label(LabelType::name)) {
        _name = std::move(value);
      } else if (key==label(LabelType::id)) {
        _id   = std::move(value);
      } else if (key==label(LabelType::details)) {
        _url  = std::move(value);
      }
      return true;
    }
  private:
    std::string _name;
    std::string _id;
    std::string _url;
  };

  using Map   = std::map< std::string,Item,std::less<> >;

  inline const std::string &version() const {return _version;}
  /**
   * returns the release date of the license list
   */
  inline const std::tm *date() const {return &_date;}
  /**
   * returns the number of licenses in the list
   */
  inline std::size_t size() const {return _map.size();}
  /**
   * Tries to add an Item to the list.
   *
   * Returns true if the Item id was free, false otherwise.
   */
  bool add(const Item &item) {
    auto [i,result] = _map.try_emplace(item.id(),item);
    return result;
  }
  /**
   * returns an Item from its id or nullptr in case of no match.
   */
  const Item *find(const std::string_view &id) const {
    const auto i = _map.find(id);
    return i!=_map.end() ? &i->second : nullptr;
  }
  /**
   * If the passed key is recognized sets the corresponding attribute to value.
   *
   * Returns true if the key is recognized, false otherwise.
   *
   * Currently recognized key types:
   *
   * - LabelType::date
   * - LabelType::version
   */
  inline bool set(const std::string_view &key, std::string &value) {
    if (key==label(LabelType::version)) {
      _version  = std::move(value);
    } else if (key==label(LabelType::date)) {
      std::stringstream(value) >> std::get_time(&_date, "%Y-%m-%d");
    }
    return true;
  }

  static const std::string &label(LabelType type) {
    static const LABELSET labs;
    return labs[type];
  }

private:
  std::string _version;
  std::tm     _date;
  Map         _map;
};

/**
 * SPDX license list
 */
using LicenseList   = List<license::LabelSet>;
/**
 * SPDX license exception list
 */
using ExceptionList = List<exception::LabelSet>;

/**
 * SAX event consumer collecting string representations of the passed values.
 *
 * NOTE: Inheriting from json::json_sax_t assures not to forget any required function.
 */
template <class T>
class SAXConsumer : public nlohmann::json::json_sax_t {
public:
  using json = nlohmann::json;

  explicit SAXConsumer(T &root) : _root(root) {}

  /*!
   * @brief a string value was read
   * @param[in] val  string value
   * @return whether parsing should proceed
   * @note It is safe to move the passed string value.
   */
  bool string(string_t& val) override {
    if (_status==Status::ROOT) {
      _root.set(_key,val);
      return true;
    } else if (_status==Status::OBJECT) {
      _item.set(_key,val);
      return true;
    } else
      // FIXME: should this throw an exception error?
      return false;
  }

  /*!
   * @brief the beginning of an object was read
   * @param[in] elements  number of object elements or -1 if unknown
   * @return whether parsing should proceed
   * @note binary formats may report the number of elements
   */
  bool start_object(std::size_t elements) override {
    switch (_status) {
      case Status::UNDEF:
        _status  = Status::ROOT;
        break;
      case Status::LIST:
        _status  = Status::OBJECT;
        break;
    }
    return true;
  }

  /*!
   * @brief the end of an object was read
   * @return whether parsing should proceed
   */
  bool end_object() override {
    switch (_status) {
      case Status::UNDEF:
        throw StatusError("Bad status");
        break;
      case Status::ROOT:
        _status  = Status::UNDEF;
        break;
      case Status::OBJECT:
        _root.add(_item);
        _status = Status::LIST;
        break;
    }
    return true;
  }

  /*!
    * @brief the beginning of an array was read
    * @param[in] elements  number of array elements or -1 if unknown
    * @return whether parsing should proceed
    * @note binary formats may report the number of elements
    */
  bool start_array(std::size_t elements) override {
    if (_status==Status::ROOT && _key==T::label(LabelType::list))
      _status  = Status::LIST;
    return true;
  }

  /*!
    * @brief the end of an array was read
    * @return whether parsing should proceed
    */
  bool end_array() override {
    if (_status==Status::LIST)
      _status  = Status::ROOT;
    return true;
  }

  /*!
   * @brief an object key was read
   * @param[in] val  object key
   * @return whether parsing should proceed
   * @note It is safe to move the passed string.
   */
  bool key(string_t& val) override {
    this->_key  = val;
    return true;
  }

  /*!
   * @brief a parse error occurred
   * @param[in] position    the position in the input where the error occurs
   * @param[in] last_token  the last read token
   * @param[in] ex          an exception object describing the error
   * @return whether parsing should proceed (must return false)
   */
  bool parse_error(std::size_t position, const std::string& last_token, const json::exception& ex) override {
    return false;
  }

  bool null() override {return true;}
  bool boolean(bool val) override {return true;}
  bool number_integer(number_integer_t val) override {return true;}
  bool number_unsigned(number_unsigned_t val) override {return true;}
  bool number_float(number_float_t val, const string_t& s) override {return true;}
  bool binary(binary_t& val) override {return true;}

private:
  enum class Status {
    UNDEF,
    ROOT,
    LIST,
    OBJECT
  };
  Status            _status = Status::UNDEF;

  std::string       _key;
  T&                _root;
  typename T::Item  _item;
};

template <typename T>
class db {
public:
  explicit db(const std::string &db_name=SPDX_LICENSES_JSON) : _consumer(_db) {
    nlohmann::json::sax_parse(std::ifstream(db_name),&_consumer);
  }
  inline size_t size() const {return _db.size();}
  inline const std::string &version() const {return _db.version();}
  inline const std::tm *date() const {return _db.date();}
  operator const T & () const {return _db;}

private:
  SAXConsumer<T>  _consumer;
  T               _db;
};

/**
 * spdx analysis data
 */
struct Data : public analitic::Data {
  Data() = default;
  // full license name
  std::string name;
  // license details url
  std::string url;
};

/**
 * This listener is triggered during the parse-tree walk.
 */
class Listener : public spdx::SPDXParserBaseListener, doc::Annotation::Setter {
public:

  Listener(const LicenseList &licenses, const ExceptionList &exceptions, doc::Annotation &anno)
    : _licenses(licenses),_exceptions(exceptions),_analitics(doc::Annotation::Setter::analitics(anno)) {}

  using License_and_beyondContext   = SPDXParser::License_and_beyondContext;
  using Compound_expressionContext  = SPDXParser::Compound_expressionContext;

  void exitCompound_expression(Compound_expressionContext *ctx) override;
  void exitLicense_and_beyond(License_and_beyondContext *ctx) override;

  const Analitics &analitics() const {return _analitics;}

private:
  const LicenseList&      _licenses;
  const ExceptionList&    _exceptions;
  // overall analysis results
  Analitics&              _analitics;
};

/**
 * Filter annotation text separating SPDX meta data from the rest.
 *
 * Returns a std::pair with the extracted spdx information in the 'first'
 * string, the stripped annotation in the 'second'.
 */
void analize(doc::Annotation &annotation);

}
