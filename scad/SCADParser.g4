parser grammar SCADParser;

options {
  tokenVocab = SCADLexer;
}

pkg: statements? EOF;

statements: statement | statements statement;

statement:
	incl
	| use
	| assignment SEMICOLON
	| named_function_definition
	| named_module_definition
	| module_instantiation;

statement_block: LEFT_CURLY_BRACKET statements? RIGHT_CURLY_BRACKET;

named_module_definition:
	MODULE ID LEFT_PARENTHESIS parameters_opt RIGHT_PARENTHESIS (
		statement_block
		| statement
	);

module_instantiation: ID LEFT_PARENTHESIS arguments_opt RIGHT_PARENTHESIS sons;

sons:
	SEMICOLON
	| LEFT_CURLY_BRACKET module_instantiation* RIGHT_CURLY_BRACKET
	| module_instantiation;

named_function_definition:
	FUNCTION ID LEFT_PARENTHESIS parameters_opt RIGHT_PARENTHESIS EQUAL expr SEMICOLON;
parameters_opt: parameters?;
parameters: parameter | parameters COMMA parameter;
parameter: lookup | assignment;

incl: INCLUDE FILE;
use: USE FILE;

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
	| expr SLASH expr
	| expr PERCENT expr
	| expr GE expr
	| expr GREATER_THEN expr
	| expr EQ expr
	| expr NE expr
	| expr LE expr
	| expr LESS_THEN expr
	| expr AND expr
	| expr OR expr
	| EXCLAMATION_MARK expr
	| PLUS expr
	| MINUS expr
	| expr QUESTION_MARK expr COLON expr
	| expr LEFT_SQUARE_BRACKET expr RIGHT_SQUARE_BRACKET
	| LEFT_PARENTHESIS expr RIGHT_PARENTHESIS
	| LEFT_SQUARE_BRACKET list_comprehension_elements RIGHT_SQUARE_BRACKET
	| let_clause expr
	| function_call;

function_call: ID LEFT_PARENTHESIS arguments_opt RIGHT_PARENTHESIS;

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

let_clause: LET LEFT_PARENTHESIS assignments_opt RIGHT_PARENTHESIS;
for_clause: FOR LEFT_PARENTHESIS assignments RIGHT_PARENTHESIS;
if_clause: IF LEFT_PARENTHESIS expr RIGHT_PARENTHESIS;

assignments_opt: assignments?;

assignments: assignment | assignments COMMA assignment;

assignment: ID EQUAL expr;
lookup: ID;
range_expression:
	LEFT_SQUARE_BRACKET expr COLON expr RIGHT_SQUARE_BRACKET
	| LEFT_SQUARE_BRACKET expr COLON expr COLON expr RIGHT_SQUARE_BRACKET;

list_expression: LEFT_SQUARE_BRACKET expression_opt RIGHT_SQUARE_BRACKET;
expression_opt: comma_opt | expressions comma_opt;
comma_opt: COMMA?;
expressions: expr | expressions COMMA expr;
