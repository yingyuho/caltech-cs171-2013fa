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
PointLight              { return PTLIGHT; }
location				{ return LOCATION; }
color					{ return COLOR; }
Separator               { return SEPARATOR; }
Transform                   { return TRANSFORM; }
translation                     { return TRANSLAT; }
rotation                        { return ROT; }
scaleFactor                     { return SFACTOR; }
Material					{ return MATERIAL; }
ambientColor					{ return ACOLOR; }
diffuseColor					{ return DCOLOR; }
specularColor					{ return SCOLOR; }
shininess						{ return SHININESS; }
Coordinate3                 { return COORD3; }
point                           { return POINT; }
Normal                 		{ return NORMAL; }
vector                          { return VECTOR; }
IndexedFaceSet              { return IFACESET; }
coordIndex                      { return COORDINDEX; }
normalIndex                     { return NORMINDEX; }
#.*$            ;// ignore comment
[ \t\r\n]+      ;// eat whitespace
.               { cerr << "Unexpected character: " << yytext << endl; return ERROR; }
%%