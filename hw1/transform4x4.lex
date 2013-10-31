%{
#include <iostream>
#include <cstdlib>
#include <list>
#include "matrix.h"
#include "transform4x4.tab.hpp"
%}
%option noyywrap

%%
[-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?      { yylval.fval = atof(yytext); return FLOAT; }
translation                                 { return TRANSTOK; }
rotation                                    { return ROTATTOK; }
scaleFactor                                 { return SCALETOK; }
\n                                          /* ignore end of line */;
[ \t]+                                      /* ignore whitespace */;
.               /*{ std::cerr << "Unexpected character: " << yytext << std::endl;
                yyterminate(); }*/
%%
