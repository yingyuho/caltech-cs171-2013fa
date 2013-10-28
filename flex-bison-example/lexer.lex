/* Comments go here */
%{
#include <iostream>
#include <list>
#include "data.h"
/* Include tokens and variables from the parser (NUM, OPEN, CLOSE, yylval) */
#include "parser.tab.hpp"
using namespace std;
%}
%option noyywrap

DIGIT [0-9]

%%

{DIGIT}+     { yylval.ival = atoi(yytext); return NUM; }
[ \t\r\n]+   /* Eat whitespace */
\{           return OPEN;
\}           return CLOSE;
.            { cerr << "Unexpected character: " << yytext << endl;
               yyterminate();
             }
%%

/* Extra code goes here */
