%{
#include <iostream>
// #include <cstdlib>
#include <list>
#include <string>
#include "KeyframeSeries.h"
#include "keyframe.tab.hpp"
#define LEXER_MSG() 
#define LEXER_MSG2() cerr << yytext
using namespace std;
%}
%option noyywrap

%%
#.*$            { LEXER_MSG(); }// ignore comment
[ \t\r\n]+      { LEXER_MSG(); }// eat whitespace
[-+]?([0-9]+\.?[0-9]*|\.[0-9]+)([eE][-+]?[0-9]+)?   { LEXER_MSG(); yylval.cstrVal = yytext; return NUMBER; }
Frame           { LEXER_MSG(); return FRAME; }
translation     { LEXER_MSG(); return TRANSLATION; }
rotation        { LEXER_MSG(); return ROTATION; }
scale           { LEXER_MSG(); return SCALE; }
.               { cerr << "Unexpected character: " << yytext << endl; return ERROR; }
%%