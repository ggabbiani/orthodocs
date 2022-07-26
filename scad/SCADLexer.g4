/*
 * OpenSCAD lexer grammar
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

ASSERT				: 'assert';
ECHO				: 'echo';
LET					: 'let';
FOR					: 'for';
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

BLOCKCOMMENT: '/*' .*? '*/' -> skip;
LINECOMMENT: '//' ~[\r\n]* -> skip;

WS: [ \t]+ -> skip;
NL: ( '\r' '\n'? | '\n') -> skip;

// ID: NONDIGIT (NONDIGIT | DIGIT)*;
ID: SPECIAL? NONDIGIT (NONDIGIT | DIGIT)*;
NUMBER: FP | INT;

DOT_INDEXING	: DOT [xyz];

fragment ALPHA: [a-zA-Z];
fragment DIGIT: [0-9];
fragment NONDIGIT: [a-zA-Z_];
fragment INT: DIGIT+;

fragment FP: (FRACTIONAL EXPONENT?) | (DIGIT_SEQUENCE EXPONENT);
fragment FRACTIONAL
	: DIGIT_SEQUENCE? DOT DIGIT_SEQUENCE
	| DIGIT_SEQUENCE DOT
	;
fragment EXPONENT: [eE] SIGN? DIGIT_SEQUENCE;

fragment SIGN: [+-];

fragment DIGIT_SEQUENCE: DIGIT+;

fragment ESCAPE_SEQUENCE: SIMPLE_ESCAPE_SEQUENCE | UNICODE;

fragment SIMPLE_ESCAPE_SEQUENCE: '\\' ["nrt\\];

STRING: QUOTE CHAR_SEQUENCE? QUOTE;

fragment CHAR_SEQUENCE: CHAR+;

fragment CHAR: ~["\\\r\n] | ESCAPE_SEQUENCE;

fragment UNICODE: '\\u' HEX_DIGIT HEX_DIGIT HEX_DIGIT HEX_DIGIT;

fragment HEX_DIGIT: [0-9a-fA-F];

// PATH_COMPONENT: [a-zA-Z0-9_-]+;
fragment PATH_COMPONENT: (ALPHA | DIGIT | MINUS | UNDERSCORE | DOT)+;
FILE: LESS PATH_COMPONENT ('/' PATH_COMPONENT)* GREATER;
