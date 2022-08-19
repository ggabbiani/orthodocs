/*
 * OpenSCAD grammar.
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

grammar SCAD;

pkg: annotation_opt stats? EOF;

stats: stat | stats stat;

stat
	: incl
	| use
	| assignment ';'
	| function_def
	| module_def
	| ('%'|'#'|'!'|'*')? module_inst
	| let_clause stat_or_block
	;

annotation_opt: annotation?;

annotation
	: BLOCK_ANNO
	| LINE_ANNO
	;

assignment	: annotation_opt ID '=' expr;

indexing	: '[' expr ']';

if_stat			: let_clause? IF '(' expr ')' stat_or_block else_stat?;
else_stat		: ELSE stat_or_block;
for_stat		: FOR 		'(' assignments ')' stat_or_block;
int_for_stat	: INT_FOR	'(' assignments ')' stat_or_block;
stat_or_block	: stat | stat_block;

expr
	: TRUE
	| FALSE
	| UNDEF
	| NUMBER
	| STRING
	| lookup
	| range_expr
	| sequence
	| expr '+'	expr
	| expr '-' 	expr
	| expr '*' 	expr
	| expr '/' 	expr
	| expr '%' 	expr
	| expr '>=' expr
	| expr '>' 	expr
	| expr '==' expr
	| expr '!=' expr
	| expr '<=' expr
	| expr '<' 	expr
	| expr '&&' expr
	| expr '||' expr
	| '!' expr
	| '+' expr
	| '-' expr
	| expr '?' expr ':' expr
	| expr indexing
	| expr DOT_INDEXING
	| '(' expr ')'
	| list_comprehension_elements
	| let_clause expr
	| function_call
	| function_literal
	| special_function_call expr?
	;

special_function_call
	: echo_function_call
	| assert_function_call
	;
echo_function_call	: ECHO 		'(' arguments_opt ')';
assert_function_call: ASSERT	'(' argument (',' argument)? ')';

module_def
	: annotation_opt MODULE ID '(' parameters_opt COMMA? ')' (stat_block|stat);

stat_block: '{' stats? '}';

module_inst
	: ECHO '(' arguments_opt ')' sons?
	| ASSERT '(' argument (COMMA argument)? ')' sons?
	| ID '(' arguments_opt ')' sons?
	| for_stat
	| int_for_stat
	| if_stat
	;

sons
	: ';'
	| '{' module_inst* '}'
	| module_inst
	;

function_def	: annotation_opt FUNCTION ID '(' parameters_opt COMMA? ')' '=' expr ';';
parameters_opt	: parameters?;
parameters		: parameter | parameters COMMA parameter;
parameter		: lookup | assignment;

function_literal: FUNCTION '(' arguments_opt ')' expr;

incl: INCLUDE FILE;
use	: USE FILE;

function_call: ID '(' arguments_opt ')';

arguments_opt: arguments?;

arguments: argument | arguments COMMA argument;
argument: expr | assignment;

/* list comprehension */

list_comprehension_elements
	: let_clause list_comprehension_elements
	| for_clause list_comprehension_elements_or_for_variants
	| if_clause list_comprehension_elements_or_expr
	| ifelse_clause list_comprehension_elements_or_expr
	;

list_comprehension_elements_or_expr
	: list_comprehension_elements
	| expr
	;

list_comprehension_elements_or_for_variants
	: list_comprehension_elements
	| seq_item
	;

let_clause
	: LET 	'(' assignments_opt ')';
for_clause
	: FOR 	'(' for_styles 		')';
if_clause
	: IF	'(' expr 			')';
ifelse_clause
	: if_clause list_comprehension_elements_or_expr ELSE;

for_styles
	: assignments
	| c_style
	;

c_style
	: assignments_opt ';' expr ';' assignments_opt;

// end of list comprehension

assignments_opt
	: assignments?;
assignments
	: assignment
	| assignments COMMA assignment
	;
lookup
	: annotation_opt ID;

range_expr
	: '[' expr COLON expr ']'
	| '[' expr COLON expr COLON expr ']'
	;

sequence: '[' optional_items ']';

optional_items
	: COMMA?
	| items ','?
	;

items
	: seq_item
	| items ',' seq_item
	;

seq_item: EACH? expr;

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
