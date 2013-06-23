%include {
#include <stdlib.h>
#include <string.h>
#include "lexglobal.h"
}
%token_type{int}
%nonassoc PRECEDENCE_KEYWORD PRECEDENCE_IDENTIFIER.

program ::= source_elements.



identifier ::= IDENTIFIER_NAME. [PRECEDENCE_IDENTIFIER]



literal ::= NULL_LITERAL.
literal ::= BOOLEAN_LITERAL.
literal ::= NumericLiteral.
literal ::= StringLiteral.

primary_expression ::=  THIS.
primary_expression ::=  identifier.
primary_expression ::=  literal.
primary_expression ::=  array_literal.
primary_expression ::=  object_literal.
primary_expression ::=  PAR_OPEN expression PAR_CLOSE.

elision_opt ::= .
elision_opt ::= elision.

array_literal ::= BRACKET_OPEN elision_opt BRACKET_CLOSE
	        |BRACKET_OPEN element_list BRACKET_CLOSE
		|BRACKET_OPEN element_list COMMA elision_opt BRACKET_CLOSE.

assignment_operator ::= EQUAL.
assignment_operator ::= ASSIGNMENT_OPERATOR_NOEQUAL.

elision ::= COMMA.
elision ::= elision COMMA.

element_list ::= elision_opt assignment_expression.
element_list ::= element_list COMMA elision_opt  COMMA assignment_expression.

object_literal ::= CURLY_BRACE_OPEN CURLY_BRACE_CLOSE
		  |CURLY_BRACE_OPEN property_name_and_value_list CURLY_BRACE_CLOSE.

property_name_and_value_list ::= property_name COLON assignment_expression.
property_name_and_value_list ::= property_name_and_value_list COMMA property_name COLON assignment_expression.

property_name ::= identifier.
property_name ::= StringLiteral
		|NumericLiteral.


/*

temp phony decls

*/

function_expression ::= .
/*------------------*/
member_expression ::= primary_expression.
member_expression ::= function_expression.
member_expression ::= member_expression BRACKET_OPEN expression BRACKET_CLOSE.
member_expression ::= member_expression DOT identifier.
member_expression ::= NEW member_expression arguments.

new_expression ::= member_expression.
new_expression ::= NEW new_expression.

call_expression ::= member_expression arguments.
call_expression ::= call_expression arguments.
call_expression ::= call_expression BRACKET_OPEN expression BRACKET_CLOSE.
call_expression ::= call_expression DOT identifier.

arguments ::= PAR_OPEN PAR_CLOSE
	     |PAR_OPEN arguments_list PAR_CLOSE.

arguments_list ::= assignment_expression.
arguments_list ::= arguments_list COMMA assignment_expression.

lefthandside_expression ::= new_expression.
lefthandside_expression ::= call_expression.

postfix_expression ::= lefthandside_expression.
postfix_expression ::= lefthandside_expression NOLF_PLUSPLUS.
postfix_expression ::= lefthandside_expression NOLF_MINUSMINUS.

unary_expression ::= postfix_expression.
unary_expression ::= DELETE|VOID|TYPEOF|PLUSPLUS|MINUSMINUS|MINUS|TILDE|EXCLAMATION unary_expression.

multiplicative_expression ::= unary_expression.
multiplicative_expression ::= multiplicative_expression MULTIPLY|DIVIDE|PERCENT unary_expression.

additive_expression ::= multiplicative_expression.
additive_expression ::= additive_expression PLUS|MINUS multiplicative_expression.

shift_expression ::= additive_expression.
shift_expression ::= shift_expression SHIFT_LEFT|SHIFT_RIGHT|DOUBLESHIFT_RIGHT additive_expression.

relational_expression ::= shift_expression.
relational_expression ::= relational_expression LESS|GREATER|LESSEQUAL|GREATEREQUAL|INSTANCEOF|IN shift_expression.

relational_expression_noin ::= shift_expression.
relational_expression_noin ::= relational_expression LESS|GREATER|LESSEQUAL|GREATEREQUAL|INSTANCEOF shift_expression.

equality_expression ::= relational_expression.
equality_expression ::= equality_expression EQUAL_EQUAL|NOT_EQUAL|TRIPLE_EQUAL|NOT_DOUBLEEQUAL relational_expression.

equality_expression_noin ::= relational_expression_noin.
equality_expression_noin ::= equality_expression_noin EQUAL_EQUAL|NOT_EQUAL|TRIPLE_EQUAL|NOT_DOUBLEEQUAL relational_expression_noin.

bitwise_and_expression ::= equality_expression.
bitwise_and_expression ::= bitwise_and_expression AND equality_expression.

bitwise_and_expression_noin ::= equality_expression_noin.
bitwise_and_expression_noin ::= bitwise_and_expression_noin AND equality_expression_noin.

bitwise_xor_expression ::= bitwise_and_expression.
bitwise_xor_expression ::= bitwise_xor_expression XOR bitwise_and_expression.


bitwise_xor_expression_noin ::= bitwise_and_expression_noin.
bitwise_xor_expression_noin ::= bitwise_xor_expression_noin XOR bitwise_and_expression_noin.

bitwise_or_expression ::= bitwise_xor_expression.
bitwise_or_expression ::= bitwise_or_expression OR bitwise_xor_expression.


bitwise_or_expression_noin ::= bitwise_xor_expression_noin.
bitwise_or_expression_noin ::= bitwise_or_expression_noin OR bitwise_xor_expression_noin.

logical_and_expression ::= bitwise_or_expression.
logical_and_expression ::= logical_and_expression AND_AND bitwise_or_expression.

logical_and_expression_noin ::= bitwise_or_expression_noin.
logical_and_expression_noin ::= logical_and_expression_noin AND_AND bitwise_or_expression_noin.
		       

logical_or_expression ::= logical_and_expression.
logical_or_expression ::= logical_or_expression OR_OR logical_and_expression.

logical_or_expression_noin ::= logical_and_expression_noin.
logical_or_expression_noin ::= logical_or_expression_noin OR_OR logical_and_expression_noin.

conditional_expression ::= logical_or_expression.
conditional_expression ::= logical_or_expression QUESTIONMARK assignment_expression COLON assignment_expression.

conditional_expression_noin ::= logical_or_expression_noin.
conditional_expression_noin ::= logical_or_expression_noin QUESTIONMARK assignment_expression_noin COLON assignment_expression_noin.

assignment_expression ::= conditional_expression.
assignment_expression ::= lefthandside_expression assignment_operator assignment_expression.

assignment_expression_noin ::= conditional_expression_noin.
assignment_expression_noin ::= lefthandside_expression assignment_operator assignment_expression_noin.
		      
expression ::= assignment_expression.
expression ::= expression COMMA assignment_expression.
expression_opt ::= expression.

expression_noin_opt ::= expression.

statement ::= block.
statement ::= variable_statement.
statement ::= empty_statement.
statement ::= expression_statement.
statement ::= if_statement.
statement ::= iteration_statement.
statement ::= continue_statement.
statement ::= break_statement.
statement ::= return_statement.
statement ::= with_statement.
statement ::= labelled_statement.
statement ::= throw_statement.
statement ::= try_statement.

block ::= CURLY_BRACE_OPEN statement_list_opt CURLY_BRACE_CLOSE.

statement_list_opt ::= .
statement_list_opt ::= statement_list.

statement_list ::= statement.
statement_list ::= statement_list statement.

variable_statement ::=  VAR variable_declaration_list SEMICOLON.

variable_declaration_list ::= variable_declaration.		   
variable_declaration_list ::= variable_declaration_list COMMA variable_declaration.

variable_declaration_list_noin ::= variable_declaration_noin.
variable_declaration_list_noin ::= variable_declaration_list_noin COMMA variable_declaration_noin.

variable_declaration ::= identifier initialiser_opt.

variable_declaration_noin ::= identifier initialiser_noin_opt.

initialiser_opt ::= .

initialiser_noin_opt ::= .
initialiser_noin_opt ::= initaliser_noin.

initaliser_noin ::= EQUAL assignment_expression_noin.

empty_statement ::= SEMICOLON.

expression_statement ::= expression SEMICOLON.
		     /*TODO:IMPLEMENT THIS RULE: lookahead not-contains {, function*/
		  
if_statement ::= IF PAR_OPEN expression PAR_CLOSE ELSE statement.
if_statement ::= IF PAR_OPEN expression PAR_CLOSE statement.


iteration_statement ::= DO statement WHILE PAR_OPEN expression PAR_CLOSE SEMICOLON.
iteration_statement ::= WHILE PAR_OPEN expression PAR_CLOSE statement.
iteration_statement ::= FOR PAR_OPEN expression_noin_opt SEMICOLON expression_opt SEMICOLON expression_opt PAR_CLOSE statement.
iteration_statement ::= FOR PAR_OPEN VAR variable_declaration_list_noin SEMICOLON expression_opt SEMICOLON expression_opt PAR_CLOSE statement.
iteration_statement ::= FOR PAR_OPEN lefthandside_expression IN expression PAR_CLOSE statement.
iteration_statement ::= FOR PAR_OPEN VAR variable_declaration_noin IN expression PAR_CLOSE statement.

continue_statement ::= CONTINUE_NOLF identifier_opt SEMICOLON.

break_statement ::= BREAK_NOLF identifier_opt SEMICOLON.

return_statement ::= RETURN_NOLF expression_opt SEMICOLON.

with_statement ::= WITH PAR_OPEN expression PAR_CLOSE statement.






labelled_statement ::= identifier COLON statement.

throw_statement ::= THROW_NOLF expression SEMICOLON.

try_statement ::= TRY block catch.
try_statement ::= TRY block finally.
try_statement ::= TRY block catch finally.

catch ::= CATCH PAR_OPEN identifier PAR_CLOSE block.

finally ::= FINALLY block.

identifier_opt ::= .
identifier_opt ::= identifier.

function_declaration ::= FUNCTION identifier PAR_OPEN formal_parameter_list_opt PAR_CLOSE CURLY_BRACE_OPEN function_body CURLY_BRACE_CLOSE.

function_expression ::= FUNCTION identifier_opt PAR_OPEN formal_parameter_list_opt PAR_CLOSE CURLY_BRACE_OPEN function_body CURLY_BRACE_CLOSE.

formal_parameter_list_opt ::= .
formal_parameter_list_opt ::= formal_parameter_list.

formal_parameter_list ::= identifier.
formal_parameter_list ::= formal_parameter_list COMMA identifier.

function_body ::= source_elements.


source_elements ::= source_element.
source_elements ::= source_elements source_element.

source_element ::= statement.
source_element ::= function_declaration.


