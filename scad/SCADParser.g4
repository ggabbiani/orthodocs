/*
 * OpenSCAD parser grammar
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

parser grammar SCADParser;

options {
  tokenVocab = SCADLexer;
}

pkg: statements? EOF;

statements: statement | statements statement;

statement:
	incl
	| use
	| assignment SEMI
	| named_function_definition
	| named_module_definition
	| module_instantiation
	;

assignment	: ID ('[' expr ']')* ASSIGN expr;

expr:
	TRUE
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
	| expr LEFT_BRACKET expr RIGHT_BRACKET
	| LEFT_PAREN expr RIGHT_PAREN
	| LEFT_BRACKET list_comprehension_elements RIGHT_BRACKET
	| let_clause expr
	| function_call
	| function_literal
;

named_module_definition:
	MODULE ID LEFT_PAREN parameters_opt RIGHT_PAREN (
		statement_block
		| statement
	);

statement_block: LEFT_BRACE statements? RIGHT_BRACE;

module_instantiation: ID LEFT_PAREN arguments_opt RIGHT_PAREN sons;

sons:
	SEMI
	| LEFT_BRACE module_instantiation* RIGHT_BRACE
	| module_instantiation;

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

list_comprehension_elements:
	let_clause list_comprehension_elements
	| for_clause list_comprehension_elements_or_expr
	| if_clause list_comprehension_elements_or_expr;

list_comprehension_elements_or_expr:
	list_comprehension_elements
	| expr;

let_clause: LET LEFT_PAREN assignments_opt RIGHT_PAREN;
for_clause: FOR LEFT_PAREN assignments RIGHT_PAREN;
if_clause: IF LEFT_PAREN expr RIGHT_PAREN;

assignments_opt: assignments?;

assignments: assignment | assignments COMMA assignment;

lookup		: ID;

range_expression:
  LEFT_BRACKET expr COLON expr RIGHT_BRACKET
  | LEFT_BRACKET expr COLON expr COLON expr RIGHT_BRACKET;

list_expression: LEFT_BRACKET expression_opt RIGHT_BRACKET;
expression_opt: comma_opt | expressions comma_opt;
comma_opt: COMMA?;
expressions: expr | expressions COMMA expr;
