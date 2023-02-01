#pragma once
/**
 * insert a brief description here
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include <commons/utils.h>

#include <cassert>
#include <map>
#include <memory>
#include <string>

namespace analytic {

struct Data {
  using Position  = std::ptrdiff_t;
  using Size      = size_t;
  using Owner     = std::unique_ptr<Data>;

  Data(Position pos, Size len) : position{pos}, length{len} {}
  Data() = default;
  virtual ~Data() = default;
  // source token position in annotation text
  Position  position;
  // source token length
  Size      length;
};

}

/**
 * analysis results data ordered by character position
 */
using Analytics = std::map<analytic::Data::Position,analytic::Data::Owner>;

namespace doc {

class Annotation {
public:
  /**
   * Objects derived from this class can modify Annotation properties
   */
  struct Setter {
    virtual ~Setter() = default;
    /**
     * modifies the Annotation's data
     */
    void set(Annotation &anno,std::string_view s) const {
      anno._data = s;
    }
    /**
     * add new analytic data
     */
    void add(Annotation &anno,analytic::Data *data) const {
      auto [ignored,success] = anno._analytics.try_emplace(data->position,data);
      assert(success);
    }
    Analytics &analytics(Annotation &anno) const {
      return anno._analytics;
    }
  };

  Annotation() = default;
  explicit Annotation(std::string &&s) : _data{std::move(s)} {}
  explicit Annotation(const std::string &s) : _data{s} {}

  // property getters
  bool empty() const {return data().empty();}
  const std::string &data() const {return _data;}
  const Analytics &analytics() const {return _analytics;}

private:
  std::string _data;
  Analytics   _analytics;
};

}
