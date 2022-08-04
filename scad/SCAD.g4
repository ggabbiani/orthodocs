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

pkg: statements? EOF;

statements: statement | statements statement;

statement:
	incl
	| use
	| assignment SEMI
	| named_function_definition
	| named_module_definition
	| ('%'| '#'| '!'| '*')? module_instantiation
	| special_function_call* SEMI
	;

assignment	: ID ASSIGN expr;

indexing	: LEFT_BRACKET expr RIGHT_BRACKET;

if_statement	: IF '(' expr ')' statement_or_block else_statement?;
else_statement	: ELSE statement_or_block;
for_statement	: 'for' '(' assignments ')' statement_or_block;
intersection_for_statement	: 'intersection_for' '(' assignments ')' statement_or_block;

statement_or_block: statement | statement_block;

expr
	: TRUE
	| FALSE
	| UNDEF
	| NUMBER
	| STRING
	| lookup
	| range_expression
	| list_expression
	| expr PLUS expr
	| expr MINUS expr
	| expr STAR expr
	| expr DIV expr
	| expr MOD expr
	| expr GE expr
	| expr GREATER expr
	| expr EQ expr
	| expr NE expr
	| expr LE expr
	| expr LESS expr
	| expr AND expr
	| expr OR expr
	| NOT expr
	| PLUS expr
	| MINUS expr
	| expr QUESTION expr COLON expr
	| expr indexing
	| expr DOT_INDEXING
	| LEFT_PAREN expr RIGHT_PAREN
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

echo_function_call: ECHO LEFT_PAREN arguments_opt RIGHT_PAREN;

assert_function_call: ASSERT LEFT_PAREN argument (COMMA argument)? RIGHT_PAREN;

named_module_definition:
	MODULE ID LEFT_PAREN parameters_opt RIGHT_PAREN (
		statement_block
		| statement
	);

statement_block: LEFT_BRACE statements? RIGHT_BRACE;

module_instantiation
	: ID LEFT_PAREN arguments_opt RIGHT_PAREN sons?
	| for_statement
	| intersection_for_statement
	| if_statement
	;

sons:
	SEMI
	| LEFT_BRACE module_instantiation* RIGHT_BRACE
	| module_instantiation
	;

named_function_definition:
	FUNCTION ID LEFT_PAREN parameters_opt RIGHT_PAREN ASSIGN expr SEMI;
parameters_opt: parameters?;
parameters: parameter | parameters COMMA parameter;
parameter: lookup | assignment;

function_literal: FUNCTION LEFT_PAREN arguments_opt RIGHT_PAREN expr;

incl: INCLUDE FILE;
use: USE FILE;

function_call: ID LEFT_PAREN arguments_opt RIGHT_PAREN;

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
	| expr
	| 'each' list_expression
	;

let_clause	: 'let' '(' assignments_opt ')';
for_clause	: 'for' '(' for_styles ')';
if_clause	: 'if' 	'(' expr 			')';
ifelse_clause: if_clause list_comprehension_elements_or_expr 'else';

for_styles
	: assignments
	| c_style
	;

c_style: assignments_opt ';' expr ';' assignments_opt;

// end of list comprehension

assignments_opt: assignments?;

assignments: assignment | assignments COMMA assignment;

lookup		: ID;

range_expression
	: LEFT_BRACKET expr COLON expr RIGHT_BRACKET
	| LEFT_BRACKET expr COLON expr COLON expr RIGHT_BRACKET
	;

list_expression: '[' expression_opt ']';

expression_opt
	: ','?
	| expressions ','?
	;

expressions
	: expr
	| expressions ',' expr
	;

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

// ID: SPECIAL? (ALPHA|D|UNDERSCORE)+;
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

// fragment PATH_COMPONENT: [a-zA-Z0-9_-.]+;
fragment PATH_COMPONENT: (ALPHA | D | MINUS | UNDERSCORE | DOT)+;
FILE: LESS PATH_COMPONENT (DIV PATH_COMPONENT)* GREATER;
