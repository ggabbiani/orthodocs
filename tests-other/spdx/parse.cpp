#include <SPDXParserBaseListener.h>

#include <CLI/CLI.hpp>
#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs=std::filesystem;
using namespace std;

namespace spdx {

/**
 * This listener is triggered during the parse-tree walk.
 */
class Listener : public spdx::SPDXParserBaseListener {
public:
  using AllContext                = SPDXParser::AllContext;
  using License_expressionContext = SPDXParser::License_expressionContext;
  using Simple_expressionContext  = SPDXParser::Simple_expressionContext;
  using ParenthesisedExprContext  = SPDXParser::ParenthesisedExprContext;
  using ExprSimpleContext         = SPDXParser::ExprSimpleContext;
  using ExprOrExprContext         = SPDXParser::ExprOrExprContext;
  using ExprWithExceptionContext  = SPDXParser::ExprWithExceptionContext;
  using License_idContext         = SPDXParser::License_idContext;
  using License_and_beyondContext = SPDXParser::License_and_beyondContext;

  // void enterAll(AllContext * /*ctx*/) override { }
  // void exitAll(AllContext * /*ctx*/) override { }

  // void enterLicense_expression(License_expressionContext * /*ctx*/) override { }
  // void exitLicense_expression(License_expressionContext * /*ctx*/) override { }

  // void enterSimple_expression(Simple_expressionContext * /*ctx*/) override { }
  // void exitSimple_expression(Simple_expressionContext * /*ctx*/) override { }

  // void enterParenthesisedExpr(ParenthesisedExprContext * /*ctx*/) override { }
  // void exitParenthesisedExpr(ParenthesisedExprContext * /*ctx*/) override { }

  // void enterExprSimple(ExprSimpleContext * /*ctx*/) override { }
  // void exitExprSimple(ExprSimpleContext * /*ctx*/) override { }

  // void enterExprOrExpr(ExprOrExprContext * /*ctx*/) override { }
  // void exitExprOrExpr(ExprOrExprContext * /*ctx*/) override { }

  void enterExprWithException(ExprWithExceptionContext * /*ctx*/) override { }
  void exitExprWithException(ExprWithExceptionContext * /*ctx*/) override { }

  // void enterExprAndExpr(SPDXParser::ExprAndExprContext * /*ctx*/) override { }
  // void exitExprAndExpr(SPDXParser::ExprAndExprContext * /*ctx*/) override { }

  void enterLicense_id(License_idContext * /*ctx*/) override { }
  void exitLicense_id(License_idContext * /*ctx*/) override { }

  void enterLicense_and_beyond(License_and_beyondContext * /*ctx*/) override { }
  void exitLicense_and_beyond(License_and_beyondContext * /*ctx*/) override { }


  // void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  // void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  // void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  // void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }
};

// unique_ptr<::Document> Extension::parse(const fs::path &source) const {
//   // change to source root
//   cwd source_root(Option::sroot());
//   ifstream          is(source);
//   ANTLRInputStream  in(is);
//   SCADLexer         lexer(&in);

//   CommonTokenStream tokens(&lexer);
//   SCADParser        parser(&tokens);

//   // error management - FIXME: set once before this call
//   parser.removeErrorListeners();
//   parser.addErrorListener(&handler);
//   // source parse listener
//   Listener  listener(source);
//   // parse tree depth-first traverse
//   tree::ParseTreeWalker  walker;
//   // parsing
//   tree::ParseTree       *tree = parser.pkg();

//   // creation of the document
//   walker.walk(&listener,tree);
//   return listener.releaseDocument();
// }

class LicenseList {
public:
  using path = std::filesystem::path;
  using json = nlohmann::json;

  explicit LicenseList(const path &in) : _list(json::parse(std::ifstream(in))) {}

  std::string version() const {
    return _list["licenseListVersion"].get<std::string>();
  }

  const json &find(const std::string &id) const {
    return _list[id];
  }

private:
  json _list;
};

class LicenseExceptions {
public:
  using path = std::filesystem::path;
  using json = nlohmann::json;

  explicit LicenseExceptions(const path &in) : _list(json::parse(std::ifstream(in))) {}

  std::string version() const {
    return _list["licenseListVersion"].get<std::string>();
  }

  const json &find(const std::string &id) const {
    return _list[id];
  }

private:
  json _list;
};

}

namespace exception {

struct MissingArgument : public std::runtime_error {
  explicit MissingArgument(const std::string &argument) : runtime_error(std::string("Expecting <")+argument+">.") {}
};

struct FileNotFound : public std::runtime_error {
  explicit FileNotFound(const std::filesystem::path &fname) : runtime_error(std::string("File not found ")+fname.string()+".") {}
};

}

class License {
public:
  using json = nlohmann::json;
  License(const std::filesystem::path &lic_f,const std::filesystem::path &xcp_f) 
    : _list(json::parse(ifstream(lic_f)))
    , _exceptions(json::parse(ifstream(xcp_f))) {
  }
  const json &list() const {return _list;}
  const json &exceptions() const {return _exceptions;}
private:
  json  _list;
  json  _exceptions;
};

int main(int argc, const char *argv[]) {
  CLI::App  app{"spdx test"};
  auto      result = EXIT_SUCCESS;

  try {
    fs::path  license_f;
    fs::path  exceptions_f;

    app.add_option("licenses",license_f,"Path of the JSON file containing the SPDX license list")
      ->required()
      ->check(CLI::ExistingFile);
    app.add_option("exceptions",exceptions_f,"Path of the JSON file containing the SPDX exception list")
      ->required()
      ->check(CLI::ExistingFile);
    app.parse(argc,argv);
    
    License license(license_f,exceptions_f);
    cout  << "SPDX license list version " << license.list()["licenseListVersion"] << '\n'
          << "successfully read " << license.list()["licenses"].size() << " licenses." << endl;

    for(auto lic: license.list()["licenses"]) {
      cout << "license id " << lic["licenseId"] << " (" << lic["name"] << ")\n";
    }
    cout << endl;
  } catch (const CLI::Error &error) {
    result  = app.exit(error);
  } catch(const std::exception &error) {
    cerr << error.what() << endl;
    result  = EXIT_FAILURE;
  }
  return result;
}
