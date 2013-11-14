%{
#include <iostream>
// #include <cstdlib>
#include <list>
#include "inventor.h"
#include "parser_param.h"
#include "inventor.tab.hpp"
using namespace std;
%}
%option noyywrap

%%
[-+]?([0-9]+\.?[0-9]*|\.[0-9]+)([eE][-+]?[0-9]+)?   { yylval.strVal = yytext; return NUMBER; }
\{              { return LBRACE; }
\}              { return RBRACE; }
\[              { return LBRACKET; }
\]              { return RBRACKET; }
,               { return COMMA; }
PerspectiveCamera       { return PCAMERA; }
position                    { return POS; }
orientation                 { return ORIENT; }
nearDistance                { return NDIST; }
farDistance                 { return FDIST; }
left                        { return LEFT; }
right                       { return RIGHT; }
top                         { return TOP; }
bottom                      { return BOTTOM; }
Separator               { return SEPARATOR; }
Transform                   { return TRANSFORM; }
translation                     { return TRANSLAT; }
rotation                        { return ROT; }
scaleFactor                     { return SFACTOR; }
Coordinate3                 { return COORD3; }
point                           { return POINT; }
IndexedFaceSet              { return IFACESET; }
coordIndex                      { return COORDINDEX; }
#.*$            ;// ignore comment
[ \t\r\n]+      ;// eat whitespace
.               { cerr << "Unexpected character: " << yytext << endl; yyterminate(); }
%%