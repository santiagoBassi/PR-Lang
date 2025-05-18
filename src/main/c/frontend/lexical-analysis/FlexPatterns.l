%{

#include "FlexActions.h"
#define PATH_MAX 500

%}

/**
 * Enable start condition manipulation functions.
 */
%option stack

/**
 * Flex contexts (a.k.a. start conditions).
 *
 * @see https://westes.github.io/flex/manual/Start-Conditions.html
 */
%x COMMENT
%x INCLUDE_FILE

/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */

%%


"//"				                { BEGIN(COMMENT); BeginCommentLexemeAction(createLexicalAnalyzerContext()); }
<COMMENT>(\r?\n)+				    { BEGIN(INITIAL); EndCommentLexemeAction(createLexicalAnalyzerContext()); }
<COMMENT>.				            { IgnoredLexemeAction(createLexicalAnalyzerContext()); }


"def"                               { return KeywordLexemeAction(createLexicalAnalyzerContext(), DEF); }
"("									{ return ParenthesisLexemeAction(createLexicalAnalyzerContext(), OPEN_PARENTHESIS); }
")"									{ return ParenthesisLexemeAction(createLexicalAnalyzerContext(), CLOSE_PARENTHESIS); }
":"                                 { return FunctionDecoratorLexemeAction(createLexicalAnalyzerContext(), COLON); }
"->"                                { return FunctionDecoratorLexemeAction(createLexicalAnalyzerContext(), ARROW); }
"=>"                                { return FunctionDecoratorLexemeAction(createLexicalAnalyzerContext(), DOUBLE_ARROW); }
"="                                 { return EqualsLexemeAction(createLexicalAnalyzerContext()); }
","                                 { return CommaSeparatorLexemeAction(createLexicalAnalyzerContext()); }
"input"                             { return KeywordLexemeAction(createLexicalAnalyzerContext(), INPUT_FROM_STDIN); }
(\r?\n)+                            { return NewLineLexemeAction(createLexicalAnalyzerContext()); }

"#include"                           { BEGIN(INCLUDE_FILE); }
<INCLUDE_FILE>[[:alnum:]_\/\-]+\.rp  { 
                                        yyin = fopen(yytext, "r");
                                        if (yyin == NULL) {
                                            yyterminate();
                                        }

										yypush_buffer_state(yy_create_buffer(yyin, YY_BUF_SIZE));
                                        BEGIN(INITIAL);
                                    }
<INCLUDE_FILE>[[:space:]]+		    { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<INCLUDE_FILE>.					    { return UnknownLexemeAction(createLexicalAnalyzerContext()); }
<<EOF>>                             {
                                        yypop_buffer_state();
                                        if (!YY_CURRENT_BUFFER) {
											yyterminate();
										}
                                    }

[[:digit:]]+						{ return IntegerLexemeAction(createLexicalAnalyzerContext()); }

([^[:digit:]\r\n\(\), ])([^\r\n\(\), ])*  { return IdLexemeAction(createLexicalAnalyzerContext()); }

[[:space:]]+						{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
.									{ return UnknownLexemeAction(createLexicalAnalyzerContext()); }

%%

#include "FlexExport.h"
