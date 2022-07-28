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
	SEMI	# empty_l
	| incl	# include_l
	| use	# use_l
	| LEFT_BRACE inner_input+ RIGHT_BRACE # inner_input_l
	| module_instantiation	# module_instantiation_l
	| assignment			# assignment_l
	| MODULE ID LEFT_PAREN parameters? RIGHT_PAREN statement	# module_definition_l
	| FUNCTION ID LEFT_PAREN parameters? RIGHT_PAREN ASSIGN expr SEMI # function_definition_l
	;

incl: INCLUDE FILE;
use: USE FILE;

inner_input: statement | inner_input statement;

assignment	: ID ASSIGN expr SEMI;

module_instantiation:
	NOT module_instantiation
	| HASH module_instantiation
	| MOD module_instantiation
	| STAR module_instantiation
	| single_module_instantiation child_statement
	| ifelse_statement
	;

ifelse_statement:
	if_statement
	| if_statement ELSE child_statement
	;

if_statement: IF LEFT_PAREN expr RIGHT_PAREN child_statement;

child_statements:
	child_statement
	| child_statements child_statement
	| assignment
	| child_statements assignment
	;

child_statement:
	SEMI
	| LEFT_BRACE child_statements? RIGHT_BRACE
	| module_instantiation
	;

module_id:
	ID
	| FOR
	| LET
	| ASSERT
	| ECHO
	| EACH
	;

single_module_instantiation: module_id LEFT_PAREN arguments? RIGHT_PAREN;

expr:
	logic_or
	| FUNCTION LEFT_PAREN parameters? RIGHT_PAREN expr
	| logic_or QUESTION expr COLON expr
	| LET LEFT_PAREN arguments? RIGHT_PAREN expr
	| ASSERT LEFT_PAREN arguments? RIGHT_PAREN expr?
	| ECHO LEFT_PAREN arguments? RIGHT_PAREN expr?
	;

logic_or:
	logic_and
	| logic_or OR logic_and
	;

logic_and:
	equality
	| logic_and AND equality
	;

equality:
	comparison
	| equality EQ comparison
	| equality NE comparison
	;

comparison:
	addition
	| comparison GREATER addition
	| comparison GE addition
	| comparison LESS addition
	| comparison LE addition
	;

addition:
	multiplication
	| addition PLUS multiplication
	| addition MINUS multiplication
	;

multiplication:
	unary
	| multiplication STAR unary
	| multiplication DIV unary
	| multiplication MOD unary
	;

unary:
	exponent
	| PLUS unary
	| MINUS unary
	| NOT unary
	;

exponent:
	call
	| call CIRCUMFLEX unary
	;

call:
	primary
	| call LEFT_PAREN arguments? RIGHT_PAREN
	| call LEFT_BRACKET expr RIGHT_BRACKET
	| call DOT ID
	;

primary:
	TRUE
	| FALSE
	| UNDEF
	| NUMBER
	| STRING
	| ID
	| LEFT_PAREN expr RIGHT_PAREN
	| LEFT_BRACKET expr COLON expr RIGHT_BRACKET
	| LEFT_BRACKET expr COLON expr COLON expr RIGHT_BRACKET
	| LEFT_BRACKET RIGHT_BRACKET
	| LEFT_BRACKET vector_elements COMMA? RIGHT_BRACKET
	;

/*
 * The last set element may not be a "let"
 * (as that would instead be parsed as an expression)
 */
list_comprehension_elements:
	LET LEFT_PAREN arguments? RIGHT_PAREN list_comprehension_elements_p
	| EACH vector_element
	| FOR LEFT_PAREN arguments? RIGHT_PAREN vector_element
	| FOR LEFT_PAREN arguments? SEMI expr SEMI arguments? RIGHT_PAREN vector_element
	| IF LEFT_PAREN expr RIGHT_PAREN vector_element
	| IF LEFT_PAREN expr RIGHT_PAREN vector_element ELSE vector_element
	;

// list_comprehension_elements with optional parenthesis
list_comprehension_elements_p:
	list_comprehension_elements
	| LEFT_PAREN list_comprehension_elements RIGHT_PAREN
	;

vector_elements:
	vector_element
	| vector_elements COMMA vector_element
	;

vector_element:
	list_comprehension_elements_p
	| expr
	;

parameters: parameter_list COMMA?;

parameter_list:
	parameter
	| parameter_list COMMA parameter
	;

parameter:
	ID
	| ID ASSIGN expr
	;

arguments: argument_list COMMA?;

argument_list:
	argument
	| argument_list COMMA argument
	;

argument:
	expr
	| ID ASSIGN expr
	;

