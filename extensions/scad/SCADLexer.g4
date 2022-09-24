/*
 * OpenSCAD lexer grammar.
 *
 * Copyright © 2022 Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * This file is part of the 'AutoDox' (ADOX) project.
 *
 * ADOX is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ADOX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ADOX.  If not, see <http: //www.gnu.org/licenses/>.
 */

lexer grammar SCADLexer;

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
BLOCK_ANNO		: '/*!' .*? '*/';
LINE_ANNO		: '//!' ~[\r\n]*;

BLOCK_COMMENT	: '/*' .*? '*/' -> skip;
LINE_COMMENT	: '//' ~[\r\n]*	-> skip;

WS: [ \t]+ -> skip;
NL: ( '\r' '\n'? | '\n') -> skip;

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
