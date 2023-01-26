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

namespace analitic {

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
using Analitics = std::map<analitic::Data::Position,analitic::Data::Owner>;

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
     * add new analitic data
     */
    void add(Annotation &anno,analitic::Data *data) const {
      auto [ignored,success] = anno._analitics.try_emplace(data->position,data);
      assert(success);
    }
    Analitics &analitics(Annotation &anno) const {
      return anno._analitics;
    }
  };

  Annotation() = default;
  explicit Annotation(std::string &&s) : _data{std::move(s)} {}
  explicit Annotation(const std::string &s) : _data{s} {}

  // property getters
  bool empty() const {return data().empty();}
  const std::string &data() const {return _data;}
  const Analitics &analitics() const {return _analitics;}

private:
  std::string _data;
  Analitics   _analitics;
};

}
