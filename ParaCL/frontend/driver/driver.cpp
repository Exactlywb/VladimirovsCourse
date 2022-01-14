#include "driver.hpp"

#include <iostream>
#include <fstream>

#include "InputHandler.hpp"

int yyFlexLexer::yywrap () { return 1; }

int main (int argc, char **argv)
{
    InputHandler input;

    try {
        input.setParams (argc, argv);
    }
    catch (const std::runtime_error &err) {
        std::cout << "Input error: " << err.what () << std::endl;
        return 0;
    }
    
    std::streambuf *cinbuf = std::cin.rdbuf();
    std::cin.rdbuf (input.code_.rdbuf ());

    yy::FrontendDriver driver;
    
    try {

        driver.parse ();

    } catch (std::runtime_error& err) {

        std::cout << err.what () << std::endl;
        return 0;

    }

    std::cin.rdbuf(cinbuf);

    driver.interpret ();

    return 0;
}
