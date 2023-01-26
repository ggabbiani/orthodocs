/*
 * OpenSCAD lexer grammar.
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

lexer grammar SCADLexer;

channels {
	// SPACES,
	COMMENTS
}

DOT					: '.';
COLON           	: ':';
SEMI       			: ';';
LEFT_BRACE			: '{';
RIGHT_BRACE			: '}';
LEFT_PAREN  		: '(';
RIGHT_PAREN			: ')';
LEFT_BRACKET 		: '[';
RIGHT_BRACKET		: ']';
COMMA           	: ',';
ASSIGN           	: '=';
NOT					: '!';
QUESTION    		: '?';
HASH 				: '#';
QUOTE				: '"';
MINUS            	: '-';
PLUS             	: '+';
DIV            		: '/';
MOD          		: '%';
GREATER        		: '>';
LESS          		: '<';
STAR             	: '*';
UNDERSCORE			: '_';
SPECIAL				: '$';
CIRCUMFLEX			: '^';

ASSERT				: 'assert';
EACH				: 'each';
ECHO				: 'echo';
LET					: 'let';
FOR					: 'for';
INT_FOR				: 'intersection_for';
IF					: 'if';
ELSE				: 'else';
FUNCTION			: 'function';
MODULE				: 'module';
INCLUDE				: 'include';
USE     			: 'use';
TRUE    			: 'true';
FALSE   			: 'false';
UNDEF   			: 'undef';

GE					: '>=';
EQ					: '==';
NE					: '!=';
LE					: '<=';
AND					: '&&';
OR					: '||';

MODIFIER		: '%'| '#'| '!'| '*';

BLOCK_COMMENT	: '/*' .*? '*/' 	-> channel(COMMENTS);
LINE_COMMENT	: '//' ~[\r\n]*		-> channel(COMMENTS);

WS: [ \t]+ 							-> skip;
NL: ( '\r' '\n'? | '\n') 			-> skip;

ID: SPECIAL? [a-zA-Z0-9_]+;

NUMBER
	: D+ E?
	| D* DOT D+ E?
	| D+ DOT D* E?
	;

DOT_INDEXING	: DOT [xyz];

fragment ALPHA: [a-zA-Z];
fragment NONDIGIT: [a-zA-Z_];

fragment D: [0-9];
fragment E: [Ee][+-]? D+;
fragment H: [0-9a-fA-F];

fragment ESCAPE_SEQUENCE: SIMPLE_ESCAPE_SEQUENCE | UNICODE;

fragment SIMPLE_ESCAPE_SEQUENCE: '\\' ["nrt\\];

STRING: QUOTE CHAR_SEQUENCE? QUOTE;

fragment CHAR_SEQUENCE: CHAR+;

fragment CHAR: ~["\\\r\n] | ESCAPE_SEQUENCE;

fragment UNICODE: '\\u' H H H H;

fragment PATH_COMPONENT: (ALPHA | D | MINUS | UNDERSCORE | DOT)+;
FILE: LESS PATH_COMPONENT (DIV PATH_COMPONENT)* GREATER;

// these definition MUST be kept at bottom because unused
CHILDREN:	'children';
PROJECTION:	'projection';
RESIZE:		'resize';
