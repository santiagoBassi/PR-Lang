%{

#include "BisonActions.h"

%}

// You touch this, and you die.
%define api.value.union.name SemanticValue

%union {
	/** Terminals. */

	int integer;
	Token token;
    char* string;

	/** Non-terminals. */
	Program* program;
    Statements* statements;
    Statement* statement;
    Definition* definition;
    FunctionArgs* function_args;
    DefinitionBody* definition_body;
    CompositionDef* composition_def;
    RecursiveDef* recursive_def;
    BaseCase* base_case;
    NextCase* next_case;
    
    Expression* expression;
    FunctionExpression* function_expression;
    ExpressionArgs* expression_args;
    Factor* expression_factor;
    
    Expression* evaluation;
    FunctionExpression* function_evaluation;
    ExpressionArgs* evaluation_args;
    Factor* evaluation_factor;
}

/**
 * Destructors. This functions are executed after the parsing ends, so if the
 * AST must be used in the following phases of the compiler you shouldn't used
 * this approach for the AST root node ("program" non-terminal, in this
 * grammar), or it will drop the entire tree even if the parse succeeds.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Destructor-Decl.html
 */
%destructor { releaseStatements($$); } <statements>
%destructor { releaseStatement($$); } <statement>
%destructor { releaseFunctionArgs($$); } <function_args>
%destructor { releaseDefinition($$); } <definition>
%destructor { releaseDefinitionBody($$); } <definition_body>
%destructor { releaseExpression($$); } <expression>
%destructor { releaseFunctionExpression($$); } <function_expression>
%destructor { releaseExpressionFactor($$); } <expression_factor>
%destructor { releaseExpressionArgs($$); } <expression_args>
%destructor { releaseCompositionDef($$); } <composition_def>
%destructor { releaseRecursiveDef($$); } <recursive_def>
%destructor { releaseBaseCase($$); } <base_case>
%destructor { releaseNextCase($$); } <next_case>
%destructor { releaseExpression($$); } <evaluation>
%destructor { releaseFunctionExpression($$); } <function_evaluation>
%destructor { releaseExpressionFactor($$); } <evaluation_factor>
%destructor { releaseExpressionArgs($$); } <evaluation_args>

/** Terminals. */
%token <integer> INTEGER
%token <token> DEF
%token <token> COLON
%token <token> ARROW
%token <token> INPUT
%token <token> OPEN_PARENTHESIS
%token <token> CLOSE_PARENTHESIS
%token <token> COMMA_SEPARATOR
%token <token> EQUALS
%token <token> NEW_LINE
%token <string> ID

%token <token> UNKNOWN

/** Non-terminals. */
%type <program> program
%type <statements> statements
%type <statement> statement
%type <definition> definition
%type <function_args> function_args
%type <definition_body> definition_body
%type <composition_def> composition_def
%type <recursive_def> recursive_def
%type <base_case> base_case
%type <next_case> next_case

%type <expression> expression
%type <function_expression> function_expression
%type <expression_args> expression_args
%type <expression_factor> expression_factor

%type <evaluation> evaluation
%type <function_evaluation> function_evaluation
%type <evaluation_args> evaluation_args
%type <evaluation_factor> evaluation_factor

/**
 * Precedence and associativity.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 */
%left ADD SUB
%left MUL DIV

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.

program: statements								    { $$ = ProgramSemanticAction(currentCompilerState(), $1); }
	;

statements: statement NEW_LINE statements
          | statement
          ;

statement: definition
         | evaluation
         ;

definition: DEF ID OPEN_PARENTHESIS function_args CLOSE_PARENTHESIS COLON NEW_LINE definition_body
          | DEF ID OPEN_PARENTHESIS CLOSE_PARENTHESIS COLON NEW_LINE composition_def
          ;

definition_body: composition_def { $$ = CompositionDefBodySemanticAction($1); }
               | recursive_def   { $$ = RecursiveDefBodySemanticAction($1); }
               ;

composition_def: ARROW ID OPEN_PARENTHESIS function_args CLOSE_PARENTHESIS EQUALS expression { $$ = CompositionDefSemanticAction($2, $4, $7); }
               | ARROW ID OPEN_PARENTHESIS CLOSE_PARENTHESIS EQUALS expression               { $$ = CompositionDefSemanticAction($2, NULL, $7); }
               ;

recursive_def: base_case NEW_LINE next_case { $$ = RecursiveDefSemanticAction($1, $3); }
             ; 

base_case: ARROW ID OPEN_PARENTHESIS function_args COMMA_SEPARATOR INTEGER CLOSE_PARENTHESIS EQUALS expression { $$ = BaseCaseSemanticAction($2, $4, $6, $9); }
         | ARROW ID OPEN_PARENTHESIS INTEGER CLOSE_PARENTHESIS EQUALS expression                               { $$ = BaseCaseSemanticAction($2, NULL, $4, $7); }
         ;

next_case: ARROW ID OPEN_PARENTHESIS function_args ID INTEGER CLOSE_PARENTHESIS EQUALS expression { $$ = NextCaseSemanticAction($2, $4, $5, $6, $9); }
         ;

expression: function_expression
          | expression_factor
          ;

function_expression: ID OPEN_PARENTHESIS expression_args CLOSE_PARENTHESIS
                   | ID OPEN_PARENTHESIS CLOSE_PARENTHESIS
                   ;

expression_args: expression COMMA_SEPARATOR expression_args
               | expression
               ;

expression_factor: ID
                 | INTEGER
                 ;

function_args: ID COMMA_SEPARATOR function_args
             | ID
             ;

evaluation: function_evaluation
          | evaluation_factor
          ;

function_evaluation: ID OPEN_PARENTHESIS evaluation_args CLOSE_PARENTHESIS
                   | ID OPEN_PARENTHESIS CLOSE_PARENTHESIS 
                   ;

evaluation_args: evaluation COMMA_SEPARATOR evaluation_args
               | evaluation
               ;

evaluation_factor: INTEGER
                 | INPUT
                 ;
%%
