%option noyywrap c++
%option yyclass="CoolLexer"

%{
#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>

#include "Parser.h"
#include "CoolLexer.h"

#undef YY_DECL
#define YY_DECL int CoolLexer::yylex()
%}

white_space       [ \t\f\r\v]*
digit0            [0-9]
digit1            [1-9]
alpha             [A-Za-z_]
alpha_num         ({alpha}|{digit0})
identifier        {alpha}{alpha_num}*

%x COMMENT
%x STRING

%%

"--".*              
"(*"                {BEGIN(COMMENT); MultiLineCommentStart();}
<COMMENT>{
    "(*"            {MultiLineCommentStart();}
    "*)"            {if (MultiLineCommentEnd()) BEGIN(INITIAL);}
    \n              {++lineno;}
    <<EOF>>         {Error("EOF in multi line comment");}
    .               
}
"*)"                {Error("Comment end without start");}

\"                  {BEGIN(STRING);}
<STRING>{
    \\b             {string_buf += "\b";}
    \\t             {string_buf += "\t";}
    \\n             {string_buf += "\n";}
    \\f             {string_buf += "\f";}
    \\\n            {}
    \\[^\n]         {string_buf += yytext[1];}
    \"              {BEGIN(INITIAL); return token::str_lit;}
    \n              {Error("Unescaped newline in string");}
    \0              {Error("Null character in string");}
    <<EOF>>         {Error("EOF in string");}
    [^\\\n\"]+      {string_buf += yytext;}
}

{white_space}       

class               {return token::key_class;}
else                {return token::key_else;}
false               {return token::key_false;}
fi                  {return token::key_fi;}
if                  {return token::key_if;}
in                  {return token::key_in;}
inherits            {return token::key_inherits;}
isvoid              {return token::key_isvoid;}
let                 {return token::key_let;}
loop                {return token::key_loop;}
pool                {return token::key_pool;}
then                {return token::key_then;}
while               {return token::key_while;}
case                {return token::key_case;}
esac                {return token::key_esac;}
new                 {return token::key_new;}
of                  {return token::key_of;}
not                 {return token::key_not;}
true                {return token::key_true;}

Object              {return token::class_Object;}
Int                 {return token::class_Int;}
String              {return token::class_String;}
Bool                {return token::class_Bool;}

{identifier}        {return token::id;}

"<-"                {return token::expr_assign;}

"+"                 {return token::op_plus;}
"-"                 {return token::op_minus;}
"*"                 {return token::op_mul;}
"/"                 {return token::op_div;}
"<"                 {return token::op_lt;}
"<="                {return token::op_le;}
"="                 {return token::op_eq;}

"{"                 {return token::brace_l;}
"}"                 {return token::brace_r;}
"("                 {return token::paren_l;}
")"                 {return token::paren_r;}

\n                  {lineno++;} 
.                   {return token::tok_unknown;}

%%

void CoolLexer::Error(const char* msg) const
{
    std::cerr << "Lexer error (line " << lineno << "): " << msg << ": lexeme '" << YYText() << "'\n";
    std::exit(YY_EXIT_FAILURE);
}
