#pragma once

/*
 * insert a brief description here
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

#include "orthodocs/document.h"

#include <iostream> 
#include <set>
#include <regex> 
#include <string> 

namespace orthodocs::doc {

/**
 * Simple cross-reference class.
 * 
 * The algorithm used is based on three elements:
 * 
 * 1. an external functor analizer for language dependent references inside 
 *    annotations; 
 * 2. one dictionary, containing the referred items;
 * 3. a reference writer for references substitution during document saving.
 */
class XRef {
public:
  struct Analysis {
    using Results = std::map<size_t,Analysis>;
    // token position as a delta from the start of the annotation's string
    ptrdiff_t   position;
    // resulting token length to be substituted when resolving the reference
    ptrdiff_t   length;
    // to be searched to in the dictionary
    std::string token;
  };

  /**
   * CASE SENSITIVE Item comparison functor.
   */
  struct DictLess {
    inline bool operator() (const Item *lhs, const Item *rhs) const {
      return  lhs->dictKey() < rhs->dictKey();
    }
  };

  using Annotation  = orthodocs::doc::Annotation;
  /**
   * Through the contained doc::Items is realized a map between two domains:
   * - language domain for the referred token (see DictLess and Item::dictKey());
   * - writer domain for the concrete reference (see writer::Extension::reference())
   */
  using Dictionary  = std::map< std::string, orthodocs::doc::Item* >;
  using Analyzer    = Analysis::Results (*) (const Annotation&);

  explicit XRef(Analysis::Results (*analyst)(const Annotation&)) : _analist(analyst) {}

  Analysis::Results analize(const Annotation &annotation) {
    return _analist(annotation);
  }

  /**
   * populated by language extension.
   */
  Dictionary   dictionary;

private:
  std::function<Analysis::Results (const Annotation&)> _analist;
};

} // namespace orthodocs::doc
