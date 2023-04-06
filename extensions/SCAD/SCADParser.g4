/*
 * OpenSCAD parser grammar.
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

parser grammar SCADParser;

options {
    tokenVocab = SCADLexer;
}

pkg: stat* EOF;

stat
	: incl								# statIncl
	| use								# statUse
	| assignment SEMI					# statAssign
	| function_def SEMI					# statFuncDef
	| module_def						# statModDef
	| let_clause stat_or_block			# statLet
	| ('%'|'#'|'!'|'*')? module_inst	# statModInst
	| SEMI								# statEmpty
	;

assignment	: ID ASSIGN expr;

indexing	: '[' expr ']';

if_stat			: let_clause? IF '(' expr ')' stat_or_block (ELSE stat_or_block)?;
for_stat		: FOR 		'(' assignments ')' stat_or_block;
int_for_stat	: INT_FOR	'(' assignments ')' stat_or_block;
stat_or_block	: stat | block;

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
	| expr '^' 	expr
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
echo_function_call	: ECHO 		'(' arguments? ')';
assert_function_call: ASSERT	'(' argument (',' argument)? ')';

module_def
	: MODULE ID '(' parameter* ')' stat_or_block;

block: '{' stat* '}';

module_inst
	: ECHO '(' arguments? ')' sons?
	| ASSERT '(' argument (COMMA argument)? ')' sons?
	| ID '(' arguments? ')' sons?
	| for_stat
	| int_for_stat
	| if_stat
	;

sons
	: SEMI
	| stat_or_block
	;

function_def	: FUNCTION ID '(' parameter* ')' ASSIGN expr;
/*
 * The definition rule for formal parameters in module / function calls
 * **IS HUGLY BUT MANDATORY** in order to capture annotations in both
 * 'directions' (i.e. in both --orthodox and --unorthodox mode).
 *
 * See also [ANTLR 4 - How to access hidden comment channel from custom listener?](https://stackoverflow.com/questions/34730162/antlr-4-how-to-access-hidden-comment-channel-from-custom-listener)
 * for an interesting example of how to browse all the token in a buffered
 * stream.
 *
 * The real problem is the implementation of the
 * antlr4::BufferedTokenStream::getHiddenTokensTo(Left|Right) function, whose
 * exact behaviour is the following:
 *
 * Collect all hidden tokens (i.e. any off-default channel) to the right / left
 * of the token index passed as argument, until one of the following conditions
 * is met:
 *
 *	- a token is present on the DEFAULT_TOKEN_CHANNEL
 *  - we arrive to the EOF.
 */
parameter		: (lookup | assignment) COMMA?;

function_literal: FUNCTION '(' arguments? ')' expr;

incl: INCLUDE FILE;
use	: USE FILE;

function_call: ID '(' arguments? ')';

arguments: argument (COMMA argument)*;
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
	: LET 	'(' assignments? ')';
for_clause
	: FOR 	'(' for_styles 		')';
if_clause
	: IF	'(' expr 			')';
ifelse_clause
	: if_clause list_comprehension_elements_or_expr ELSE;

for_styles
	: assignments								# style_scad
	| assignments? SEMI expr SEMI assignments?	# style_c
	;

// end of list comprehension

assignments	:	assignment (COMMA assignment)*;
lookup		:	ID;

range_expr
	: '[' expr COLON expr ']'
	| '[' expr COLON expr COLON expr ']'
	;

sequence: '[' (items COMMA?)? ']';

items: seq_item (COMMA seq_item)*;

seq_item: EACH? expr;

