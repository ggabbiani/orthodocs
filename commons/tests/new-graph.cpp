#include <filesystem>
#include <iostream>
#include <vector>

#include <debug/trace.h>

#include "graph.h"

using namespace std;
namespace fs = std::filesystem;

struct Document {
  struct Id {
    explicit Id(const Document &doc) : id(doc.path.string()) {
      TR_FUNC;
      TR_MSG(doc.path);
    }
    // Id(const Id &other) {
    //   TR_FUNC;
    //   TR_MSG("COPY");
    //   *this = other;
    // }
    // Id(Id &&other) : id(std::move(other.id)) {
    //   TR_FUNC;
    //   TR_MSG("MOVE");
    // }
    ~Id() {
      TR_FUNC;
    }
    const std::string &string() const {return id;}

    auto operator<=>(const Id& rhs) const = default;
    constexpr auto operator<=>(const std::string &rhs) const {
      return id <=> rhs;
    }

    std::string id;
  };
  fs::path path;
};

struct Dependency {
  struct Id {
    Id(const Document::Id &from, const Document::Id &to)
    : from{from},to{to}, id{from.string()+"=>"+to.string()} {
      TR_FUNC;
      TR_MSG(this->from.id,this->to.id);
    }
    Id(const Document &from, const Document &to) : Id{Document::Id{from},Document::Id{to}} {
      TR_FUNC;
      TR_MSG(this->from.id,this->to.id);
    }
    const std::string &string() const {return id;}

    auto operator<=>(const Id& rhs) const = default;
    constexpr auto operator<=>(const std::string_view &rhs) const {
      return id <=> rhs;
    }

    // Id & operator = (Id &&) noexcept;

    Document::Id  from;
    Document::Id  to;
    std::string   id;
  };
  // "include" or "use"
  string type;
};

int main(int argc, const char *argv[]) {
  TR_FUNC;
  try {
    vector<Document> docs{
      {"foundation/2d"},
      {"foundation/3d"},
      {"foundation/torus"}
    };
    vector<Document::Id>  doc_ids;

    vector<Dependency> deps{
      {"include"},
    };

    Graph<Document, Dependency> g;
    TR_MSG("Vertices: ", g.size());

    {
      TR_NEW("init");
      for(const auto &doc: docs)
        doc_ids.push_back(g.add(doc));
      TR_MSG("Vertices: ", g.size());
    }

    // auto eid = g.add(Document::Id{docs[1]},Document::Id{docs[0]},deps[0]);
    // TR_MSG("edge",eid.string(),"added");

    // {
    //   TR_NEW("eid2");
    //   auto eid2 = g.add(Dependency::Id(docs[2],docs[1]),Dependency("use"));
    //   TR_MSG("edge", eid2.string(), "added");
    // }
  } catch(const exception &error) {
    cerr << error.what() << endl;
  }
}
