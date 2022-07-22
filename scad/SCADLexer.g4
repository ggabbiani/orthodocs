lexer grammar SCADLexer;

COLON           	: ':';
SEMICOLON       	: ';';
LEFT_CURLY_BRACKET	: '{';
RIGHT_CURLY_BRACKET	: '}';
LEFT_PARENTHESIS  	: '(';
RIGHT_PARENTHESIS	: ')';
LEFT_SQUARE_BRACKET : '[';
RIGHT_SQUARE_BRACKET: ']';
COMMA           	: ',';
EQUAL           	: '=';
EXCLAMATION_MARK	: '!';
QUESTION_MARK    	: '?';
MINUS            	: '-';
PLUS             	: '+';
SLASH            	: '/';
PERCENT          	: '%';
GREATER_THEN        : '>';
LESS_THEN          	: '<';
STAR             	: '*';

LET					: 'let';
FOR					: 'for';
IF					: 'if';
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

fragment ALPHA: [a-zA-Z];
fragment DIGIT: [0-9];
fragment NONDIGIT: [a-zA-Z_];
fragment INT: SIGN? DIGIT+;

fragment FP: FRACTIONAL EXPONENT? | DIGIT_SEQUENCE EXPONENT;
fragment FRACTIONAL:
	SIGN? (
		DIGIT_SEQUENCE? '.' DIGIT_SEQUENCE
		| DIGIT_SEQUENCE '.'
	);
fragment EXPONENT: [eE] SIGN? DIGIT_SEQUENCE;

SIGN: [+-];
NUMBER: INT | FP;

DIGIT_SEQUENCE: DIGIT+;

// ID: NONDIGIT (NONDIGIT | DIGIT)*;
ID: '$'? [a-zA-Z0-9_]+;

PATH_COMPONENT: (ALPHA | DIGIT | MINUS | '_' | '.')+;
// PATH_COMPONENT: ([a-zA-Z0-9_-])+;
FILE: '<' PATH_COMPONENT ('/' PATH_COMPONENT)* '>';

fragment ESCAPE_SEQUENCE: SIMPLE_ESCAPE_SEQUENCE | UNICODE;

fragment SIMPLE_ESCAPE_SEQUENCE: '\\' ["nrt\\];

STRING: '"' CHAR_SEQUENCE? '"';

fragment CHAR_SEQUENCE: CHAR+;

fragment CHAR: ~["\\\r\n] | ESCAPE_SEQUENCE;

fragment UNICODE: '\\u' HEX_DIGIT HEX_DIGIT HEX_DIGIT HEX_DIGIT;

fragment HEX_DIGIT: [0-9a-fA-F];
