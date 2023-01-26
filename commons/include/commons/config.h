#pragma once
/*
 * Common configuration file: DO NOT CHANGE BY HAND, this file is generated automatically
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2023, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#define ODOX_XSTR(x)    ODOX_STR(x)
#define ODOX_STR(x)     #x

#define ODOX_VERSION_MAJOR  0
#define ODOX_VERSION_MINOR  8
#define ODOX_VERSION_PATCH  0
#define ODOX_VERSION        ((ODOX_VERSION_MAJOR << 16) | (ODOX_VERSION_MINOR << 8) | ODOX_VERSION_PATCH)
#define ODOX_VERSION_STR    ODOX_XSTR(ODOX_VERSION_MAJOR) "." ODOX_XSTR(ODOX_VERSION_MINOR) "." ODOX_XSTR(ODOX_VERSION_PATCH)

#define OPTION_LANGUAGE_SCAD
#define OPTION_WRITER_MARKDOWN
/* #undef OPTION_SCAD_VISITORS */
