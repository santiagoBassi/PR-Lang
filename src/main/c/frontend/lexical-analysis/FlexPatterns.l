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


"//"				                { BEGIN(COMMENT); }
<COMMENT>"\n"				        { BEGIN(INITIAL); }
<COMMENT>.				            {}


"def"                               {}
"("									{}
")"									{}
"#include"                          {}
":"                                 {}
"->"                                {}
"="                                 {}
","                                 {}
"input"                             {}
"suc"                               {}
"\n"                                {}

[[:digit:]]+						{}
[[:alnum:]]+\.rp                    {}
[[:alpha:]][[:alnum:]]*             {}

([[:^digit:]]{-}["\.[:^graph:]])([[:graph:]]{-}["\.[:^graph:]])*            {}

[[:space:]]+						{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
.									{ return UnknownLexemeAction(createLexicalAnalyzerContext()); }


%%

#include "FlexExport.h"
