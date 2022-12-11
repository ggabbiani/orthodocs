/*
 * SPDX lexer grammar build from [Annex D: SPDX License Expressions](https://spdx.github.io/spdx-spec/v2-draft/SPDX-license-expressions/)
 *
 * Copyright 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

lexer grammar SPDXLexer;

// rules in default mode

LICENSE_TAG : 'SPDX-License-Identifier' COLON -> more, pushMode(MODE_SPDX);
ANYTHING    : .+? -> skip;

mode MODE_SPDX;

DOT					: '.';
COLON           	: ':';
LEFT_PAREN  		: '(';
RIGHT_PAREN			: ')';
MINUS            	: '-';
PLUS             	: '+';

// as stated in [spdx-tutorial/README.md](https://github.com/david-a-wheeler/spdx-tutorial/blob/master/README.md)
// 'SPDX expressions are case-insensitive, but by convention the operation names should be capitalized.'
WITH    options { caseInsensitive=true; }   : 'with';
AND     options { caseInsensitive=true; }   : 'and';
OR      options { caseInsensitive=true; }   : 'or';

fragment ALPHA: [a-zA-Z];
fragment DIGIT: [0-9];
fragment ALNUM: ALPHA | DIGIT;
fragment SPACE: [ \t];

ID                  : (ALNUM | MINUS | DOT)+;
LICENSE_REF         : ('DocumentRef' MINUS ID ':')? 'LicenseRef' MINUS ID;

WS: SPACE+ -> skip;
NL: ( '\r' '\n'? | '\n') -> skip, popMode;
