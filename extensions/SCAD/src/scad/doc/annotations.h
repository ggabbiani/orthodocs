#pragma once
/*
 * Annotations utils
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

#include <string>

/**
 * Modifies the passed annotation inserting the corresponding emoji
 * to each admonition found inside the annotation text.
 * The supported admonition are taken from 
 * [Admonitions | Asciidoctor Docs](https://docs.asciidoctor.org/asciidoc/latest/blocks/admonitions/)
 * 
 * See also [Using emoji](https://docs.github.com/en/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax#using-emoji)
 */
extern void mk_admonitions(std::string &annotation);
