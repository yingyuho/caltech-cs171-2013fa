%{
#include <iostream>
// #include <cstdlib>
#include <list>
#include <string>
#include "inventor.h"
#include "parser_param.h"
#include "inventor.tab.hpp"
#define LEXER_MSG() 
#define LEXER_MSG2() cerr << yytext
using namespace std;
%}
%option noyywrap

%%
#.*$            { LEXER_MSG(); }// ignore comment
[ \t\r\n]+      { LEXER_MSG(); }// eat whitespace
[-+]?([0-9]+\.?[0-9]*|\.[0-9]+)([eE][-+]?[0-9]+)?   { LEXER_MSG(); yylval.strVal = yytext; return NUMBER; }
\{              { LEXER_MSG(); return LBRACE; }
\}              { LEXER_MSG(); return RBRACE; }
\[              { LEXER_MSG(); return LBRACKET; }
\]              { LEXER_MSG(); return RBRACKET; }
,               { LEXER_MSG(); return COMMA; }
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
location                { return LOCATION; }
color                   { return COLOR; }
Separator               { LEXER_MSG(); return SEPARATOR; }
Texture2                    { LEXER_MSG(); return TEXTURE2; }
filename                        { LEXER_MSG(); return FILENAME; }
[^ \t\r\n]+\.(png|PNG)          { LEXER_MSG(); yylval.strP = new std::string(yytext); return PNGFILE; }
Transform                   { LEXER_MSG(); return TRANSFORM; }
translation                     { LEXER_MSG(); return TRANSLAT; }
rotation                        { LEXER_MSG(); return ROT; }
scaleFactor                     { LEXER_MSG(); return SFACTOR; }
Material                    { return MATERIAL; }
ambientColor                    { return ACOLOR; }
diffuseColor                    { return DCOLOR; }
specularColor                   { return SCOLOR; }
shininess                       { return SHININESS; }
Coordinate3                 { return COORD3; }
point                           { return POINT; }
TextureCoordinate2          { return TCOORD2; }
Normal                      { return NORMAL; }
vector                          { return VECTOR; }
IndexedFaceSet              { return IFACESET; }
coordIndex                      { return COORDINDEX; }
normalIndex                     { return NORMINDEX; }
textureCoordIndex               { return TCOORDINDEX; }
.               { cerr << "Unexpected character: " << yytext << endl; return ERROR; }
%%