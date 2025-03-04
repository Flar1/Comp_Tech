#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include "CoolLexer.h"
#include "Parser.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "usage: coolc <source>\n";
        std::exit(EXIT_FAILURE);
    }
    std::ifstream ifs(argv[1]);
    if (ifs.fail())
    {
        std::cerr << "Error opening file `" << argv[1] << "`\n";
        std::exit(EXIT_FAILURE);
    }

    CoolLexer *lexer = new CoolLexer(ifs, std::cout);
    for (int token = lexer->yylex(); token; token = lexer->yylex())
    {
        if (token == token::str_lit)
        {
            std::cout << "Token: " << token << " \"" << lexer->get_string_buffer() << "\" "
                      << "line:" << lexer->get_lineno() << '\n';
            lexer->clear_string_buffer();
        }
        else
        {
            std::cout << "Token: " << token << " '" << lexer->YYText() << "' "
                      << "line:" << lexer->get_lineno() << '\n';
        }
    }

    delete lexer;
    return 0;
}
