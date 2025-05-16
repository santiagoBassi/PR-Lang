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
%destructor { releaseFactor($$); } <expression_factor>
%destructor { releaseExpressionArgs($$); } <expression_args>
%destructor { releaseCompositionDef($$); } <composition_def>
%destructor { releaseRecursiveDef($$); } <recursive_def>
%destructor { releaseBaseCase($$); } <base_case>
%destructor { releaseNextCase($$); } <next_case>

/** Terminals. */
%token <integer> INTEGER
%token <token> DEF
%token <token> COLON
%token <token> ARROW
%token <token> DOUBLE_ARROW
%token <token> INPUT_FROM_STDIN
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

/**
 * Precedence and associativity.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 */

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.

program: statements								    { $$ = ProgramSemanticAction(currentCompilerState(), $1); }
	;

statements: statement NEW_LINE statements           { $$ = StatementsSemanticAction($3, $1); }
          | statement NEW_LINE                      { $$ = StatementsSemanticAction(NULL, $1); }
          | statement                               { $$ = StatementsSemanticAction(NULL, $1); }
          ;

statement: definition                               { $$ = DefinitionStatementSemanticAction($1); }
         | expression                               { $$ = ExpressionStatementSemanticAction($1); }
         ;

definition: DEF ID OPEN_PARENTHESIS function_args CLOSE_PARENTHESIS COLON NEW_LINE definition_body { $$ = DefinitionSemanticAction($2,$4,$8); }
          | DEF ID OPEN_PARENTHESIS CLOSE_PARENTHESIS COLON NEW_LINE composition_def               { $$ = DefinitionNoArgsSemanticAction($2,$7); }
          ;

definition_body: composition_def { $$ = CompositionDefBodySemanticAction($1); }
               | recursive_def   { $$ = RecursiveDefBodySemanticAction($1); }
               ;

composition_def: DOUBLE_ARROW ID OPEN_PARENTHESIS function_args CLOSE_PARENTHESIS EQUALS expression { $$ = CompositionDefSemanticAction($2, $4, $7); }
               | DOUBLE_ARROW ID OPEN_PARENTHESIS CLOSE_PARENTHESIS EQUALS expression               { $$ = CompositionDefSemanticAction($2, NULL, $6); }
               ;

recursive_def: base_case NEW_LINE next_case { $$ = RecursiveDefSemanticAction($1, $3); }
             ; 

base_case: ARROW ID OPEN_PARENTHESIS function_args CLOSE_PARENTHESIS EQUALS expression { $$ = BaseCaseSemanticAction($2, $4, $7); }
         ;

next_case: ARROW ID OPEN_PARENTHESIS function_args ID INTEGER CLOSE_PARENTHESIS EQUALS expression { $$ = NextCaseSemanticAction($2, $4, $5, $6, $9); }
         ;

expression: function_expression { $$ = FunctionExpressionSemanticAction($1); }
          | expression_factor   { $$ = FactorExpressionSemanticAction($1); }
          ;

function_expression: ID OPEN_PARENTHESIS expression_args CLOSE_PARENTHESIS { $$ = FunctionSemanticAction($1, $3); }
                   | ID OPEN_PARENTHESIS CLOSE_PARENTHESIS                 { $$ = FunctionSemanticAction($1, NULL); }
                   ;

expression_args: expression COMMA_SEPARATOR expression_args         { $$ = ExpressionArgsSemanticAction($3, $1); }
               | expression                                         { $$ = ExpressionArgsSemanticAction(NULL, $1); }
               ;

expression_factor: ID                { $$ = IdFactorSemanticAction($1); }
                 | INTEGER           { $$ = IntegerFactorSemanticAction($1); }
                 | INPUT_FROM_STDIN  { $$ = InputFactorSemanticAction(); }
                 ;

function_args: ID COMMA_SEPARATOR function_args { $$ = FunctionArgsSemanticAction($1, $3); }
             | ID                               { $$ = FunctionArgsSemanticAction($1, NULL); }
             | INTEGER                          { $$ = FunctionArgsIntSemanticAction($1); }
             ;
%%
