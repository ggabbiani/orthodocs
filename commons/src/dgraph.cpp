/*
 * Document graph header
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "commons/dgraph.h"

namespace doc {

Graph::Graph() {

}

auto Graph::add(const Document *document) -> Id {
  return _alist.add_vertex(VertexData{document});
}

auto Graph::vertex(Id i) const -> const VertexData& {
  return _alist.vertex_data(i);
}

auto Graph::vertices() const -> IdSet {
  return _alist.vertex_ids();
}

} // namespace doc
