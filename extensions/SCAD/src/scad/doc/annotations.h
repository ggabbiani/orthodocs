#pragma once
/*
 * Annotations utils
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later */

#include <string>

/**
 * Modifies the passed annotation inserting the corresponding emoji
 * to each admonition found inside the annotation text.
 * The supported admonition are taken from
 * [Admonitions | Asciidoctor Docs](https://docs.asciidoctor.org/asciidoc/latest/blocks/admonitions/)
 *
 * See also [Using emoji](https://docs.github.com/en/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax#using-emoji)
 *
 * The currently supported admonitions are:
 *
 * - caution
 * - important
 * - note
 * - tip
 * - warning
 */
extern void mk_admonitions(std::string &annotation);
