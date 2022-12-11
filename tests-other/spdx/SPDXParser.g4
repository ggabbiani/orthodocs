/*
 * SPDX parser grammar build from [Annex D: SPDX License Expressions](https://spdx.github.io/spdx-spec/v2-draft/SPDX-license-expressions/)
 *
 * Copyright 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

parser grammar SPDXParser;

options { 
    tokenVocab = SPDXLexer;
}

// all     : license? EOF;
// license : LICENSE_TAG license_expression;

all     : license_expression? EOF;

license_expression  
    : simple_expression
    | compound_expression
    ;

simple_expression   
    : license_and_beyond
    | license_id
    | LICENSE_REF
    ;

compound_expression
    : simple_expression                                 # exprSimple
    | simple_expression     op=WITH ID                  # exprWithException
    | compound_expression   op=AND  compound_expression # exprAndExpr
    | compound_expression   op=OR   compound_expression # exprOrExpr
    | LEFT_PAREN compound_expression RIGHT_PAREN        # parenthesisedExpr
    ;

license_id          : id=ID;
license_and_beyond  : id=ID op=PLUS;
