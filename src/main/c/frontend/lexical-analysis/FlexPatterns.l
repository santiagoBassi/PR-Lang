%{

#include "FlexActions.h"

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

/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */

%%


"//"				                { BEGIN(COMMENT); BeginCommentLexemeAction(createLexicalAnalyzerContext()); }
<COMMENT>"\n"				        { BEGIN(INITIAL); EndCommentLexemeAction(createLexicalAnalyzerContext()); }
<COMMENT>.				            { IgnoredLexemeAction(createLexicalAnalyzerContext()); }


"def"                               { return KeywordLexemeAction(createLexicalAnalyzerContext(), DEF); }
"("									{ return ParenthesisLexemeAction(createLexicalAnalyzerContext(), OPEN_PARENTHESIS); }
")"									{ return ParenthesisLexemeAction(createLexicalAnalyzerContext(), CLOSE_PARENTHESIS); }
"#include"                          { return KeywordLexemeAction(createLexicalAnalyzerContext(), INCLUDE); }
":"                                 { return FunctionDecoratorLexemeAction(createLexicalAnalyzerContext(), COLON); }
"->"                                { return FunctionDecoratorLexemeAction(createLexicalAnalyzerContext(), ARROW); }
"="                                 { return EqualsLexemeAction(createLexicalAnalyzerContext()); }
","                                 { return CommaSeparatorLexemeAction(createLexicalAnalyzerContext()); }
"input"                             { return KeywordLexemeAction(createLexicalAnalyzerContext(), INPUT); }
"\n"                                { return NewLineLexemeAction(createLexicalAnalyzerContext()); }

[[:digit:]]+						{ return IntegerLexemeAction(createLexicalAnalyzerContext()); }
[[:alnum:]]+\.rp                    { return FileNameLexemeAction(createLexicalAnalyzerContext()); }
[[:alpha:]][[:alnum:]]*             { return ParameterLexemeAction(createLexicalAnalyzerContext()); }

([[:^digit:]]{-}["\.[:^graph:]])([[:graph:]]{-}["\.[:^graph:]])*            { return FunctionNameLexemeAction(createLexicalAnalyzerContext()); }

[[:space:]]+						{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
.									{ return UnknownLexemeAction(createLexicalAnalyzerContext()); }

%%

#include "FlexExport.h"
