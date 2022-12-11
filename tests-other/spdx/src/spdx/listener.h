#pragma once
/*
 * SPDX listener
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

#include <SPDXParserBaseListener.h>

#include <memory>

namespace spdx {

/**
 * This listener is triggered during the parse-tree walk. The resulting synthetic
 * document ownership is then passed to the calling environment through the
 * documentRelease() call.
 */
class Listener : public spdx::SPDXParserBaseListener {
public:
  using Parser    = spdx::SPDXParser;

  explicit Listener(const std::filesystem::path &pkg_source);

  void enterLicense_id(Parser::License_idContext * /*ctx*/) override;
  void exitLicense_id(Parser::License_idContext * /*ctx*/) override;

  void enterLicense_and_beyond(Parser::License_and_beyondContext * /*ctx*/) override;
  void exitLicense_and_beyond(Parser::License_and_beyondContext * /*ctx*/) override;

  void enterSimple_expression(Parser::Simple_expressionContext * /*ctx*/) override;
  void exitSimple_expression(Parser::Simple_expressionContext * /*ctx*/) override;

  void enterCompound_expression(Parser::Compound_expressionContext * /*ctx*/) override;
  void exitCompound_expression(Parser::Compound_expressionContext * /*ctx*/) override;

  void enterLicense_expression(Parser::License_expressionContext * /*ctx*/) override;
  void exitLicense_expression(Parser::License_expressionContext * /*ctx*/) override;
};

}
